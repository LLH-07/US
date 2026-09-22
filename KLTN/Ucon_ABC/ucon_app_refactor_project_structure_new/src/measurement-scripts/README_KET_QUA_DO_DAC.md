# README – Chú thích các file kết quả đo đạc UCON Core

Tài liệu này mô tả ý nghĩa, đơn vị và công thức của các trường xuất hiện trong file kết quả của các phép đo hiện có:

1. **Kịch bản 1 – Một ACTIVE session, một DB event**
2. **Idle polling – Poller chạy khi không có event**
3. **Kịch bản 2 – Một DB event ảnh hưởng nhiều ACTIVE session**
4. **Kịch bản 3 – Rental hết hạn theo timer**

Runner hiện tại chưa tạo file kết quả cho Kịch bản 4 về số chunk đọc thêm sau khi policy mất hiệu lực.

---

## 1. Quy ước chung

### 1.1. Đơn vị

- Các trường có hậu tố `_ms` được tính bằng **millisecond**.
- Các trường thời gian như `event_occurred_at` được ghi theo định dạng ngày giờ ISO, với độ chính xác đến microsecond.
- Các metric số thực được làm tròn đến 3 chữ số thập phân khi ghi ra CSV.

### 1.2. Các mốc thời gian chính

| Mốc | Ý nghĩa |
|---|---|
| `dependency_due_at` | Thời điểm timer dependency đến hạn, thường trùng với `user_rentals.expires_at`. |
| `occurred_at` / `event_occurred_at` | Thời điểm trigger event được tạo trong `ucon_trigger_events`. |
| `picked_at` / `event_picked_at` | Thời điểm trigger-event poller lấy event để xử lý. |
| `processed_at` / `event_processed_at` | Thời điểm UCON Core xử lý xong event và đánh dấu event đã xử lý. |
| `revoked_at` / `session_revoked_at` | Thời điểm usage session được chuyển sang trạng thái `REVOKED`. |

Thứ tự kỳ vọng đối với DB event:

```text
occurred_at <= picked_at <= processed_at
occurred_at <= revoked_at
```

Thứ tự kỳ vọng đối với timer event:

```text
dependency_due_at <= event_occurred_at
                  <= event_picked_at
                  <= event_processed_at

dependency_due_at <= session_revoked_at
```

`session_revoked_at` có thể sớm hơn `event_processed_at`, vì session được revoke trong lúc xử lý event, còn `processed_at` chỉ được ghi sau khi toàn bộ thao tác xử lý event hoàn tất.

### 1.3. Trường nhận diện và trạng thái

| Trường | Ý nghĩa |
|---|---|
| `scenario` | Tên loại phép đo. |
| `iteration` | Số thứ tự mẫu chính thức trong file hiện tại. Warm-up không được ghi vào file kết quả. |
| `session_id` | Khóa chính của usage session trong bảng `usage_sessions`. |
| `trigger_event_id` | Khóa chính của event trong bảng `ucon_trigger_events`. |
| `success` | `True` khi iteration hoàn tất và dữ liệu vượt qua các kiểm tra của runner; `False` khi iteration lỗi. |
| `error` | Nội dung lỗi nếu `success=False`; để trống nếu thành công. |
| `final_status` | Trạng thái cuối của session, kỳ vọng là `REVOKED` trong Kịch bản 2 và 3. |

Khi một iteration thất bại, các metric chưa thu thập được có thể để trống, còn nguyên nhân được ghi ở cột `error`.

---

# 2. Kịch bản 1 – Một session, một DB event

## 2.1. File kết quả

Mẫu tên file:

```text
db_event_baseline_<poller_interval>ms_<timestamp>.csv
```

Ví dụ:

```text
db_event_baseline_1000ms_20260708_091500.csv
```

Mỗi dòng là kết quả của một lần:

```text
Tạo 1 session ACTIVE
→ đổi user_rentals.status từ ACTIVE sang CANCELLED
→ DB trigger tạo ATTRIBUTE_CHANGED event
→ poller lấy event
→ UCON Core tái đánh giá ongoing policy
→ session bị REVOKED
```

## 2.2. Các trường

| Trường | Ý nghĩa | Công thức / nguồn |
|---|---|---|
| `scenario` | Tên kịch bản. | Luôn là `db_event_baseline`. |
| `poller_interval_ms` | Khoảng nghỉ cấu hình của trigger-event poller. | Giá trị `ucon.trigger-poller.interval-ms` khi Backend chạy. |
| `iteration` | Số thứ tự mẫu chính thức. | Bắt đầu từ 1 trong mỗi batch/file. |
| `session_id` | Session bị ảnh hưởng bởi DB event. | `session_events.usage_session_id`. |
| `trigger_event_id` | DB trigger event tương ứng. | `ucon_trigger_events.id`. |
| `event_detection_latency_ms` | Thời gian event chờ poller phát hiện. | `(picked_at - occurred_at) × 1000`. |
| `event_processing_latency_ms` | Thời gian UCON Core xử lý event sau khi poller lấy event. | `(processed_at - picked_at) × 1000`. |
| `event_end_to_end_latency_ms` | Tổng thời gian từ lúc event được tạo đến lúc xử lý xong. | `(processed_at - occurred_at) × 1000`. |
| `reevaluation_duration_ms` | Thời gian Context Handler tái đánh giá ongoing policy cho session. | `session_events.details_json.durationMs` của event `EVENT_REEVALUATE`. |
| `revoke_latency_ms` | Thời gian từ lúc DB event được tạo đến lúc session bị revoke. | `(usage_sessions.revoked_at - occurred_at) × 1000`. |
| `success` | Iteration hợp lệ hay không. | `True` hoặc `False`. |
| `error` | Lỗi của iteration. | Chuỗi rỗng khi thành công. |

## 2.3. Quan hệ giữa các metric

Theo định nghĩa:

```text
event_end_to_end_latency_ms
= event_detection_latency_ms
+ event_processing_latency_ms
```

Có thể có sai khác rất nhỏ ở chữ số cuối do làm tròn.

`revoke_latency_ms` không bắt buộc bằng `event_end_to_end_latency_ms`. Session thường được revoke trước khi event được đánh dấu `processed_at`, nên:

```text
revoke_latency_ms <= event_end_to_end_latency_ms
```

trong phần lớn trường hợp hợp lệ.

## 2.4. Cách tổng hợp khi phân tích

Đối với mỗi `poller_interval_ms`, nên tính trên toàn bộ mẫu chính thức:

```text
mean(metric) = tổng các giá trị / số mẫu
p95(metric)  = phân vị 95% của toàn bộ mẫu
```

Các metric chính thường dùng:

- `event_detection_latency_ms`
- `event_processing_latency_ms`
- `event_end_to_end_latency_ms`
- `reevaluation_duration_ms`
- `revoke_latency_ms`

---

# 3. Idle polling – Không có event

## 3.1. File kết quả

Mẫu tên file:

```text
idle_polling_<poller_interval>ms_<timestamp>.json
```

Ví dụ:

```text
idle_polling_1000ms_20260708_101500.json
```

Phép đo reset bộ đếm poller, chờ trong một khoảng thời gian khi không có pending event, rồi lấy snapshot.

## 3.2. Các trường

| Trường | Ý nghĩa | Công thức / nguồn |
|---|---|---|
| `scenario` | Tên phép đo. | Luôn là `idle_polling`. |
| `pollerIntervalMs` | Poller interval đang được cấu hình. | Giá trị truyền vào runner. |
| `elapsedMs` | Thời gian thực tế từ lúc reset metric đến lúc lấy snapshot. | `elapsedNanos / 1,000,000`. |
| `pollRuns` | Tổng số lần hàm poller đã chạy. | Tăng 1 sau mỗi lần `pollEvents()`. |
| `emptyPolls` | Số lần poll không lấy được event nào. | Tăng 1 khi `eventCount == 0`. |
| `eventsProcessed` | Tổng số event được lấy và xử lý trong thời gian đo. | Tổng `eventCount` của tất cả lần poll. |
| `pollerRunsPerMinute` | Số lần poll trung bình mỗi phút thực tế. | `pollRuns / elapsedMinutes`. |
| `emptyPollsPerMinute` | Số empty poll trung bình mỗi phút. | `emptyPolls / elapsedMinutes`. |
| `eventsProcessedPerPoll` | Số event trung bình được xử lý trong mỗi lần poll. | `eventsProcessed / pollRuns`. |

Trong đó:

```text
elapsedMinutes = elapsedMs / 60,000
```

Tương đương với cách tính trực tiếp trong Backend:

```text
elapsedMinutes = elapsedNanos / 60,000,000,000
```

## 3.3. Điều kiện kỳ vọng của idle polling

Khi phép đo thật sự không có event:

```text
eventsProcessed = 0
emptyPolls = pollRuns
eventsProcessedPerPoll = 0
```

Tần suất lý thuyết, nếu bỏ qua thời gian chạy câu query và các chi phí khác:

```text
poller_runs_per_minute_theoretical
= 60,000 / pollerIntervalMs
```

Ví dụ:

```text
interval 1000 ms → khoảng 60 lần/phút
interval 500 ms  → khoảng 120 lần/phút
```

Do poller đang dùng `fixedDelay`, một chu kỳ thực tế bao gồm cả thời gian chạy lần poll trước và khoảng delay cấu hình. Vì vậy, `pollerRunsPerMinute` thực tế có thể thấp hơn giá trị lý thuyết.

---

# 4. Kịch bản 2 – Nhiều session cùng bị ảnh hưởng

Kịch bản 2 tạo hai loại file:

1. **Summary:** một dòng cho mỗi event/iteration.
2. **Details:** một dòng cho mỗi session trong event đó.

## 4.1. File summary

Mẫu tên file:

```text
db_event_multi_session_summary_<poller_interval>ms_<session_count>sessions_<timestamp>.csv
```

Ví dụ:

```text
db_event_multi_session_summary_1000ms_50sessions_20260728_110536.csv
```

### Các trường summary

| Trường | Ý nghĩa | Công thức / nguồn |
|---|---|---|
| `scenario` | Tên kịch bản. | Luôn là `db_event_multi_session`. |
| `poller_interval_ms` | Trigger-event poller interval. | Giá trị Backend đang dùng. |
| `requested_session_count` | Số ACTIVE session runner yêu cầu tạo. | Ví dụ `1`, `10`, `50`, `100`. |
| `iteration` | Số thứ tự mẫu chính thức. | Bắt đầu từ 1 trong mỗi batch/file. |
| `trigger_event_id` | DB event duy nhất làm các session mất quyền. | `ucon_trigger_events.id`. |
| `affected_session_count` | Số session có bản ghi `EVENT_REEVALUATE` gắn với event. | `COUNT(detail rows)`. |
| `revoked_session_count` | Số session có trạng thái cuối `REVOKED`. | `COUNT(final_status = 'REVOKED')`. |
| `event_detection_latency_ms` | Event chờ poller phát hiện. | `(picked_at - occurred_at) × 1000`. |
| `event_processing_latency_ms` | Tổng thời gian UCON Core xử lý một event ảnh hưởng nhiều session. | `(processed_at - picked_at) × 1000`. |
| `event_end_to_end_latency_ms` | Tổng thời gian từ khi event được tạo đến khi xử lý xong. | `(processed_at - occurred_at) × 1000`. |
| `reevaluation_total_ms` | Tổng thời gian reevaluation của tất cả session. | `Σ reevaluation_duration_i`. |
| `reevaluation_mean_ms` | Thời gian reevaluation trung bình trên một session. | `Σ reevaluation_duration_i / N`. |
| `reevaluation_p95_ms` | Phân vị 95% thời gian reevaluation giữa các session trong iteration hiện tại. | `P95({reevaluation_duration_i})`. |
| `reevaluation_max_ms` | Reevaluation chậm nhất trong iteration. | `MAX(reevaluation_duration_i)`. |
| `revoke_mean_ms` | Revoke latency trung bình giữa các session. | `Σ revoke_latency_i / N`. |
| `revoke_p95_ms` | Phân vị 95% revoke latency giữa các session trong iteration. | `P95({revoke_latency_i})`. |
| `revoke_max_ms` | Thời gian từ event đến session bị revoke cuối cùng. | `MAX(revoke_latency_i)`. |
| `success` | Iteration có hoàn tất đúng hay không. | `True` hoặc `False`. |
| `error` | Nội dung lỗi. | Rỗng khi thành công. |

Với:

```text
N = affected_session_count

reevaluation_duration_i
= thời gian tái đánh giá policy của session i

revoke_latency_i
= session_i.revoked_at - event.occurred_at
```

Runner yêu cầu đối với iteration thành công:

```text
affected_session_count = requested_session_count
revoked_session_count  = requested_session_count
```

### Cách tính p95 trong file summary

Runner dùng phân vị liên tục có nội suy tuyến tính:

```text
Sắp xếp các giá trị tăng dần: x[0], x[1], ..., x[n-1]
position = (n - 1) × 0.95
lower    = floor(position)
upper    = ceil(position)
```

Nếu `lower = upper`:

```text
p95 = x[lower]
```

Nếu khác nhau:

```text
fraction = position - lower
p95 = x[lower] + (x[upper] - x[lower]) × fraction
```

Lưu ý: `reevaluation_p95_ms` và `revoke_p95_ms` trong summary là **p95 giữa các session của một iteration**, không phải p95 giữa toàn bộ iteration trong cả thí nghiệm.

Khi viết báo cáo theo từng mức session, vẫn cần tổng hợp tiếp qua 100 iteration, ví dụ:

```text
processing_mean_at_50_sessions
= mean(event_processing_latency_ms của 100 iteration)

processing_p95_at_50_sessions
= p95(event_processing_latency_ms của 100 iteration)
```

## 4.2. File details

Mẫu tên file:

```text
db_event_multi_session_details_<poller_interval>ms_<session_count>sessions_<timestamp>.csv
```

Ví dụ:

```text
db_event_multi_session_details_1000ms_50sessions_20260728_110536.csv
```

### Các trường details

| Trường | Ý nghĩa | Công thức / nguồn |
|---|---|---|
| `scenario` | Tên kịch bản. | `db_event_multi_session`. |
| `poller_interval_ms` | Poller interval. | Giá trị Backend đang dùng. |
| `requested_session_count` | Số session được yêu cầu trong iteration. | Giá trị đầu vào. |
| `iteration` | Iteration chứa session này. | Dùng để ghép với summary. |
| `trigger_event_id` | Event làm session được tái đánh giá. | Dùng để ghép với summary và DB. |
| `session_sequence` | Thứ tự session trong danh sách detail của event. | Được đánh số theo thứ tự bản ghi `session_events.id`. |
| `session_id` | Usage session cụ thể. | `usage_sessions.id`. |
| `reevaluation_duration_ms` | Thời gian tái đánh giá policy của session. | `session_events.details_json.durationMs`. |
| `revoke_latency_ms` | Thời gian từ lúc event được tạo đến lúc session bị revoke. | `(session.revoked_at - event.occurred_at) × 1000`. |
| `final_status` | Trạng thái cuối của session. | Kỳ vọng `REVOKED`. |

`session_sequence` phản ánh thứ tự bản ghi reevaluation được ghi trong `session_events`, không phải thứ tự tăng dần của `session_id`.

## 4.3. Quan hệ giữa summary và details

Có thể ghép hai file bằng:

```text
iteration + trigger_event_id
```

Mỗi dòng summary tương ứng với:

```text
requested_session_count dòng details
```

Các giá trị summary được tính từ details của cùng `trigger_event_id`:

```text
reevaluation_total_ms = SUM(details.reevaluation_duration_ms)
reevaluation_mean_ms  = MEAN(details.reevaluation_duration_ms)
reevaluation_p95_ms   = P95(details.reevaluation_duration_ms)
reevaluation_max_ms   = MAX(details.reevaluation_duration_ms)

revoke_mean_ms = MEAN(details.revoke_latency_ms)
revoke_p95_ms  = P95(details.revoke_latency_ms)
revoke_max_ms  = MAX(details.revoke_latency_ms)
```

---

# 5. Kịch bản 3 – Rental hết hạn theo timer

## 5.1. File kết quả

Mẫu tên file:

```text
timer_expiry_poller<poller_interval>ms_scheduler<scheduler_interval>ms_<timestamp>.csv
```

Ví dụ:

```text
timer_expiry_poller1000ms_scheduler500ms_20260728_143500.csv
```

Mỗi dòng mô tả một lần:

```text
Tạo rental có expires_at gần
→ tạo session ACTIVE
→ dependency next_check_at đến hạn
→ timer scheduler tạo TIMER_DUE event
→ trigger-event poller lấy event
→ UCON Core tái đánh giá policy
→ session bị REVOKED
```

## 5.2. Các trường cấu hình

| Trường | Ý nghĩa | Công thức / nguồn |
|---|---|---|
| `scenario` | Tên kịch bản. | Luôn là `timer_expiry`. |
| `poller_interval_ms` | Interval của trigger-event poller. | Thường cố định ở `1000 ms`. |
| `timer_scheduler_interval_ms` | Interval của timer dependency scheduler. | Biến chính được thay đổi trong Kịch bản 3. |
| `expiry_lead_seconds` | Khoảng thời gian tối thiểu từ lúc chuẩn bị rental đến expiry. | Giá trị `--expiry-lead-seconds`, mặc định 3 giây. |
| `random_phase_ms` | Độ trễ ngẫu nhiên cộng thêm để tránh đồng bộ expiry với chu kỳ scheduler. | Lấy ngẫu nhiên đều trong `[0, timer_scheduler_interval_ms)`. |
| `iteration` | Số thứ tự mẫu chính thức. | Warm-up không được ghi. |

Thời điểm expiry được tạo theo:

```text
expiry_delay_seconds
= expiry_lead_seconds + random_phase_ms / 1000

user_rentals.expires_at
= database_clock + expiry_delay_seconds
```

Ký hiệu phân phối:

```text
random_phase_ms ~ Uniform(0, timer_scheduler_interval_ms)
```

## 5.3. Các trường nhận diện và timestamp

| Trường | Ý nghĩa | Nguồn |
|---|---|---|
| `session_id` | Session rental được đo. | `usage_sessions.id`. |
| `trigger_event_id` | TIMER_DUE event tương ứng. | `ucon_trigger_events.id`. |
| `dependency_due_at` | Mốc timer dependency đến hạn. | `usage_session_dependencies.next_check_at`; phải trùng với rental `expires_at`. |
| `event_occurred_at` | Thời điểm scheduler tạo TIMER_DUE event. | `ucon_trigger_events.occurred_at`. |
| `event_picked_at` | Thời điểm trigger-event poller lấy TIMER_DUE event. | `ucon_trigger_events.picked_at`. |
| `event_processed_at` | Thời điểm xử lý event hoàn tất. | `ucon_trigger_events.processed_at`. |
| `session_revoked_at` | Thời điểm session chuyển sang `REVOKED`. | `usage_sessions.revoked_at`. |

## 5.4. Các metric timer

| Trường | Ý nghĩa | Công thức |
|---|---|---|
| `timer_detection_latency_ms` | Độ trễ của timer scheduler: dependency đã đến hạn bao lâu thì scheduler mới tạo TIMER_DUE event. | `(event_occurred_at - dependency_due_at) × 1000`. |
| `timer_event_detection_latency_ms` | Độ trễ của trigger-event poller sau khi TIMER_DUE đã được tạo. | `(event_picked_at - event_occurred_at) × 1000`. |
| `timer_event_processing_latency_ms` | Thời gian UCON Core xử lý TIMER_DUE event. | `(event_processed_at - event_picked_at) × 1000`. |
| `timer_event_end_to_end_latency_ms` | Tổng thời gian xử lý kể từ lúc TIMER_DUE event được tạo. | `(event_processed_at - event_occurred_at) × 1000`. |
| `reevaluation_duration_ms` | Thời gian Context Handler tái đánh giá ongoing policy cho session. | `session_events.details_json.durationMs`. |
| `timer_total_revoke_latency_ms` | Tổng thời gian từ đúng lúc rental hết hạn đến lúc session bị revoke. | `(session_revoked_at - dependency_due_at) × 1000`. |
| `final_status` | Trạng thái cuối. | Kỳ vọng `REVOKED`. |
| `success` | Iteration hợp lệ. | `True` hoặc `False`. |
| `error` | Lỗi nếu iteration thất bại. | Chuỗi rỗng khi thành công. |

## 5.5. Quan hệ giữa các metric timer

Theo định nghĩa:

```text
timer_event_end_to_end_latency_ms
= timer_event_detection_latency_ms
+ timer_event_processing_latency_ms
```

Tổng đường đi đến lúc event xử lý xong là:

```text
event_processed_at - dependency_due_at
= timer_detection_latency_ms
+ timer_event_detection_latency_ms
+ timer_event_processing_latency_ms
```

Tuy nhiên:

```text
timer_total_revoke_latency_ms
```

không bắt buộc bằng tổng ba metric trên, bởi vì nó kết thúc ở `session_revoked_at`, trong khi tổng trên kết thúc ở `event_processed_at`.

Có thể biểu diễn chính xác hơn:

```text
timer_total_revoke_latency_ms
= timer_detection_latency_ms
+ timer_event_detection_latency_ms
+ (session_revoked_at - event_picked_at)
```

Trong đó phần cuối là thời gian từ khi poller lấy event đến khi session thực sự bị revoke.

## 5.6. Cách tổng hợp khi phân tích

Đối với mỗi `timer_scheduler_interval_ms`, nên tính mean và p95 trên toàn bộ iteration chính thức của:

- `timer_detection_latency_ms`
- `timer_event_detection_latency_ms`
- `timer_event_processing_latency_ms`
- `timer_event_end_to_end_latency_ms`
- `reevaluation_duration_ms`
- `timer_total_revoke_latency_ms`

Kỳ vọng:

- `timer_detection_latency_ms` tăng theo scheduler interval.
- `timer_event_detection_latency_ms` tương đối ổn định khi poller interval được giữ cố định.
- `timer_event_processing_latency_ms` và `reevaluation_duration_ms` tương đối ổn định.
- `timer_total_revoke_latency_ms` tăng khi scheduler interval tăng.

---

# 6. Ý nghĩa của mean, p95 và max

## 6.1. Mean

```text
mean = tổng các giá trị / số lượng giá trị
```

Mean phản ánh mức trung bình của toàn bộ mẫu nhưng có thể bị ảnh hưởng bởi outlier.

## 6.2. P95

P95 là giá trị mà khoảng 95% quan sát không vượt quá. Nó phù hợp để mô tả phần đuôi chậm của hệ thống.

Ví dụ:

```text
revoke p95 = 980 ms
```

có nghĩa là khoảng 95% mẫu có revoke latency không quá 980 ms.

## 6.3. Max

```text
max = giá trị lớn nhất trong tập dữ liệu
```

Max hữu ích để kiểm tra outlier hoặc trường hợp xấu nhất quan sát được, nhưng không nên dùng một mình để kết luận hiệu năng điển hình.

---

# 7. Kiểm tra nhanh tính hợp lệ của dữ liệu

## 7.1. Kịch bản 1

```text
success = True
session_id không rỗng
trigger_event_id không rỗng
các metric latency >= 0
```

Kiểm tra gần đúng:

```text
event_end_to_end_latency_ms
≈ event_detection_latency_ms + event_processing_latency_ms
```

## 7.2. Idle polling

```text
eventsProcessed = 0
emptyPolls = pollRuns
eventsProcessedPerPoll = 0
```

## 7.3. Kịch bản 2

```text
success = True
affected_session_count = requested_session_count
revoked_session_count = requested_session_count
số dòng detail của mỗi iteration = requested_session_count
mọi final_status = REVOKED
```

## 7.4. Kịch bản 3

```text
success = True
final_status = REVOKED
mọi latency >= 0
```

Kiểm tra thứ tự timestamp:

```text
dependency_due_at <= event_occurred_at
                  <= event_picked_at
                  <= event_processed_at

dependency_due_at <= session_revoked_at
```

Kiểm tra gần đúng:

```text
timer_event_end_to_end_latency_ms
≈ timer_event_detection_latency_ms
+ timer_event_processing_latency_ms
```

---

# 8. Lưu ý khi gộp nhiều file kết quả

Các batch khác nhau tạo file CSV khác nhau và `iteration` có thể bắt đầu lại từ 1. Vì vậy, khi gộp file không nên dùng riêng `iteration` làm khóa duy nhất.

Có thể bổ sung một cột nguồn trước khi gộp:

```text
source_file
```

Hoặc dùng tổ hợp:

```text
source_file + iteration
```

Đối với Kịch bản 2, `trigger_event_id` cũng có thể dùng để liên kết summary và details vì đây là ID duy nhất trong database của lần chạy đó.

Không đưa warm-up vào kết quả phân tích chính thức, vì runner chỉ ghi các iteration thuộc pha `MEASURE`.
