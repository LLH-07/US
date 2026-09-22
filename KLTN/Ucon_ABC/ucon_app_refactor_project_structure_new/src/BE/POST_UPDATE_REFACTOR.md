# Refactor luồng postUpdate: Context Handler → PIP

## Mục tiêu

Tách hoàn toàn hai khái niệm trong mã nguồn:

- **UCON obligation**: hành động bắt buộc do obligation subject thực hiện và được `ObligationManager` quản lý.
- **Attribute update**: cập nhật thuộc tính do hệ thống thực hiện ở các pha PRE/ONGOING/POST; `ContextHandler` điều phối và gọi `PipService` áp dụng cập nhật.

Luồng postUpdate sau khi refactor:

```text
ContextHandler
  → đánh giá POST policy qua PDP
  → nhận AttributeUpdateInstruction
  → gọi PipService.applyAttributeUpdate(...)
  → PIP phân giải AttributeUpdateDefinition
  → AttributeUpdateHandler.apply(...)
  → cập nhật nguồn thuộc tính
```

`ObligationManager` không tham gia luồng trên. Nó chỉ còn xử lý các obligation thật sự, ví dụ bằng chứng đồng ý điều khoản trong PRE.

## Lưu ý về XACML

AuthzForce trả các hành động bắt buộc dưới dạng XACML PEP action. Vì XACML 3.0 dùng phần tử `ObligationExpression` để mang mandatory PEP action, policy XML vẫn phải dùng phần tử này về mặt kỹ thuật.

Tuy nhiên, ID và kiểu nội bộ đã được đổi thành:

- `urn:ucon:instruction:attribute-update`
- `urn:ucon:instruction:attribute-update:action-code`
- `AttributeUpdateInstruction`

Do đó, phần tử XACML này không được chuyển đến `ObligationManager` và không còn bị mô hình hóa như UCON subject obligation.

## Các đổi tên chính

| Cũ | Mới |
|---|---|
| `EnforcementInstruction` | `AttributeUpdateInstruction` |
| `PolicyDecision.instructions` | `PolicyDecision.attributeUpdateInstructions` |
| `extractInstructions()` | `extractAttributeUpdateInstructions()` |
| `executePostUpdates()` | `processPostAttributeUpdates()` |
| `executeInstructions()` | `applyAttributeUpdates()` |
| `executeUpdateAction()` | `applyAttributeUpdate()` |
| `AttributeUpdateActionCommand` | `AttributeUpdateRequest` |
| `ResolvedAttributeUpdateCommand` | `ResolvedAttributeUpdate` |
| `PipUpdateAction` | `AttributeUpdateDefinition` |
| `PipUpdateActionRepository` | `AttributeUpdateDefinitionRepository` |
| `AttributeUpdateHandler.execute()` | `AttributeUpdateHandler.apply()` |
| `markPostHandled()` | `markPostProcessingCompleted()` |

## Phạm vi postUpdate đang bật

`handleEndAccess()` đã gọi:

```java
processPostAttributeUpdates(sessionId, "USER_END", null);
```

Vì vậy, khi người dùng kết thúc phiên bình thường:

1. Session chuyển sang `ENDED` và phase `POST`.
2. CH đánh giá POST policy đã được bind với session.
3. PDP trả `AttributeUpdateInstruction`.
4. CH yêu cầu PIP cập nhật `book_volumes.read_count`.
5. Session được đánh dấu hoàn tất post processing.

Khi session bị `REVOKED`, mã hiện tại **không tăng `read_count`**, phù hợp mô tả policy “after normal end access”. CH chỉ hoàn tất post processing cho phiên bị revoke. Nếu sau này cần postUpdate riêng khi revoke, nên thêm termination cause vào POST evaluation context và viết rule/action riêng.

## Các event log liên quan

Sau một lần kết thúc bình thường thành công, `session_events` cần có:

- `END_ACCESS`
- `POST_POLICY_EVALUATED`
- `ATTRIBUTE_UPDATE`
- `POST_PROCESSING_COMPLETED`

## Kiểm thử đề xuất

### 1. Ghi nhận giá trị trước khi đọc

```sql
SELECT id, read_count
FROM book_volumes
WHERE id = <volume_id>;
```

### 2. Tạo và kích hoạt session

- Gọi `POST /api/pep/read/try`.
- Hoàn thành pre-obligation nếu policy yêu cầu.
- Gọi ít nhất một chunk qua `GET /api/pep/read/sessions/{sessionId}/chunks/{chunkNo}` để session chuyển sang `ACTIVE`.

### 3. Kết thúc bình thường

```text
POST /api/pep/read/sessions/{sessionId}/end
```

### 4. Kiểm tra kết quả

```sql
SELECT id, status, phase, ended_at, post_handled_at
FROM usage_sessions
WHERE id = <session_id>;

SELECT event_type, phase, decision, reason_code, event_at
FROM session_events
WHERE usage_session_id = <session_id>
ORDER BY id;

SELECT id, read_count
FROM book_volumes
WHERE id = <volume_id>;
```

Kỳ vọng:

- `usage_sessions.status = 'ENDED'`
- `usage_sessions.phase = 'POST'`
- `post_handled_at IS NOT NULL`
- `read_count` tăng đúng 1

### 5. Kiểm tra idempotency

Gọi lại endpoint end với cùng `sessionId`. Session không còn `ACTIVE`, nên không được áp dụng update lần thứ hai.

## Kiểm tra đã thực hiện trên bản refactor

- Policy XML đã được parse thành công.
- Không còn tham chiếu đến các tên cũ hoặc URN `urn:ucon:obligation:update-action`.
- Đã kiểm tra tên public Java type khớp tên file.
- Đã chạy kiểm tra cú pháp Java ở mức parser của `javac`; môi trường tạo bản refactor không có Maven/dependency cache nên chưa chạy được `mvn test` hoặc `mvn package` đầy đủ.
