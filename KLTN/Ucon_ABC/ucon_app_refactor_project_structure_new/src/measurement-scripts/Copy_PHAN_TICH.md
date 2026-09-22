# PHÂN TÍCH KẾT QUẢ UCON CORE SERVICE

## Thông tin thành viên

| MSSV | Họ và tên |
|---|---|
| 22127256 | Ngô Triệu Mẫn |
| 22127117 | Lý Liên Hoa |

---

# PHẦN I. TỔNG QUAN

## 1. Bối cảnh và mục tiêu

Khác với các mô hình điều khiển truy cập truyền thống, hệ thống UCON sẽ kiểm tra quyền sử dụng của chủ thể trước và trong khi truy cập. Khi phát hiện có sự thay đổi trong thuộc tính chính sách, hệ thống sẽ đánh giá lại và thu hồi các phiên không còn hợp lệ.

Hiện tại, tốc độ thu hồi quyền phụ thuộc vào chu kỳ quét (interval) của poller (bộ thăm dò) và scheduler (bộ lập lịch). Nếu chu kỳ ngắn thì hệ thống sẽ phản hồi nhanh hơn, nhưng có thể tiêu tốn nhiều tài nguyên hơn. Vì vậy, phần đo đạc tập trung trả lời bốn câu hỏi:

1. Khi giảm thời gian quét có giúp thu hồi phiên nhanh hơn không?
2. Khi giảm thời gian quét có làm tăng số lần kiểm tra KHÔNG tìm thấy sự kiện không?
3. Khi một sự kiện ảnh hưởng đến nhiều phiên, thời gian xử lý có tăng theo số phiên không?
4. Thời gian thu hồi phiên của cơ chế dựa trên thời gian (timer-based) khác như thế nào so với cơ chế dựa trên sự kiện (event-driven)?

Mục tiêu chính là tìm mức cấu hình cân bằng giữa tốc độ thu hồi quyền và chi phí xử lý nền, thay vì chỉ chọn khoảng thời gian nhỏ nhất.

## 2. Hai cơ chế đánh giá lại chính sách

Mã nguồn triển khai hai cơ chế chính:

| Cơ chế | Luồng xử lý |
|---|---|
| **Event-driven** | Trạng thái gói thuê thay đổi → cơ sở dữ liệu tạo sự kiện `ATTRIBUTE_CHANGED` → poller lấy sự kiện → UCON Core đánh giá lại chính sách → phiên không hợp lệ bị chuyển sang `REVOKED`. |
| **Timer-based** | Gói thuê đến hạn → Scheduler tạo sự kiện `TIMER_DUE` → Poller lấy sự kiện → UCON Core đánh giá lại chính sách → phiên hết hạn bị chuyển sang `REVOKED`. |

Cơ chế event-driven chỉ phụ thuộc vào chu kỳ của poller, còn cơ chế time-baser phải phụ thuộc thêm độ trễ của scheduler. Vì vậy cần phân tích, đánh giá hai cơ chế này một cách độc lập.

## 3. Các phép đo được triển khai

Theo cấu hình mặc định trong các tệp lệnh chạy, mã nguồn hiện có ba kịch bản chính và một phép đo riêng khi không có sự kiện:

| Phép đo | Cấu hình thay đổi | Mục đích |
|---|---|---|
| **Kịch bản 1: Một phiên và một sự kiện từ cơ sở dữ liệu** | Thay đổi chu kỳ của poller: `250`, `500`, `1000`, `2000`, `5000` mili giây (ms) | Đo độ trễ phản hồi cơ bản của hệ thống. |
| **Phụ trợ: Thăm dò khi không có sự kiện (idle polling)** | Thay đổi chu kì của poller theo năm mức của kịch bản 1 | Thống kê số lần không tìm thấy sự kiện. |
| **Kịch bản 2: Một sự kiện ảnh hưởng nhiều phiên** | Giữ chu kỳ poller `1000` ms; thay đổi số phiên: `1`, `10`, `50`, `100` | Đánh giá chi phí xử lý khi số phiên bị ảnh hưởng tăng. |
| **Kịch bản 3: Gói thuê hết hạn theo thời gian** | Giữ chu kỳ poller `1000` ms; thay đổi khoảng thời gian của scheduler: `250`, `500`, `1000`, `2000`, `5000` ms | Đánh giá độ trễ khi phải đi qua scheduler và poller. |

**Lưu ý về quá trình đo**
- Trong bản demo này, hệ thống luôn có quá trình warm-up (khởi động) trước khi đo chính thức để ổn định hệ thống và sẽ không được ghi vào tệp kết quả chính thức. Quá trình warm-up quan trọng vì ở những chu kỳ poller đầu tiên, hệ thống thường mất thêm thời gian khởi tạo cache, dễ làm nhiễu kết quả đo đạc
- Mỗi mẫu (sample), mỗi lần đo chính thức có trường `success`; nếu thất bại, nguyên nhân được lưu trong trường `error`.

## 4. Dữ liệu và nhóm chỉ số

Dữ liệu đo được lấy từ các mốc thời gian do hệ thống ghi lại:

- `occurred_at`: Thời điểm sự kiện được tạo;
- `picked_at`: Thời điểm poller lấy sự kiện;
- `processed_at`: Thời điểm sự kiện được xử lý xong;
- `revoked_at`: Thời điểm phiên bị thu hồi;
- `dependency_due_at`: Mốc thời gian gói thuê hết hạn (dùng cho kịch bản 3).

Từ các mốc này, báo cáo phân tích hai nhóm chỉ số:

- **Độ trễ phản ứng:** thời gian phát hiện sự kiện, xử lý sự kiện, đánh giá lại chính sách và thu hồi phiên.
- **Chi phí kiểm tra nền:** số lần poller chạy, số lần kiểm tra không tìm thấy sự kiện và số sự kiện trung bình được xử lý trong mỗi lần kiểm tra.

Việc tách hai nhóm chỉ số này giúp xác định một phiên sử dụng bị thu hồi chậm do phải chờ lần kiểm tra tiếp theo của poller, scheduler hay do quá trình xử lý bên trong hệ thống mô phỏng UCON Core mất nhiều thời gian.

---
# PHẦN II. TIẾN HÀNH ĐO ĐẠC VÀ PHÂN TÍCH KẾT QUẢ
## 1. Phạm vi phân tích
Bộ dữ liệu bao gồm:
- 500 mẫu cho kịch bản 1, tương ứng mỗi 100 mẫu cho mỗi chu kỳ poller
- 5 phép đo ở năm mức chu kỳ của poller, mỗi mức được theo dõi khoảng 300s
- 400 mẫu sự kiện cho kịch bản 2
- 500 mẫu cho kịch bản 3, mỗi 100 mẫu cho mỗi chu kỳ scheduler

Hiện tại, sau khi đo đạc, toàn bộ mẫu đều có `success=True`. Các phiên cần thu hồi đều kết thúc ở trạng thái `REVOKED`. Các quan hệ giữa độ trễ phát hiện, xử lý và toàn trình cũng khớp với công thức trong mã nguồn. Do đó, dữ liệu đủ hợp lệ để phân tích.

Các thống kê được sử dụng trong phần phân tích gồm:

$$
\operatorname{mean}(x)
=
\frac{\sum_{i=1}^{n}x_i}{n}
$$

Phân vị 95% (P95) là giá trị mà khoảng 95% số mẫu quan sát không vượt quá. Giá trị lớn nhất được dùng để xem xét trường hợp chậm nhất đã xuất hiện trong dữ liệu.


## 2. Kịch bản 1: Một phiên và một sự kiện từ cơ sở dữ liệu
### 2.1. File kết quả

Mẫu tên file:

```text
db_event_baseline_<poller_interval>ms_<timestamp>.csv
```

Ví dụ:

```text
db_event_baseline_1000ms_20260708_091500.csv
```

Mỗi dòng là kết quả của một lần đo:
```text
Tạo 1 phiên sử dụng ở trạng thái `ACTIVE`
→ đổi `user_rentals.status` từ `ACTIVE` sang `CANCELLED`
→ DB trigger tạo sự kiện `ATTRIBUTE_CHANGED`
→ poller lấy sự kiện
→ UCON Core đánh giá lại chính sách đang áp dụng
→ phiên sử dụng bị thu hồi và chuyển sang `REVOKED`
```


### 2.2. Một số phép tính liên quan
**Chu kỳ của poller:**

$$
\texttt{poller\_interval\_ms}
=
\texttt{ucon.trigger-poller.interval-ms}
$$

**Thời gian sự kiện phải chờ cho đến khi poller phát hiện:**

$$
\texttt{event\_detection\_latency\_ms}
=
\left(
\texttt{picked\_at}
-
\texttt{occurred\_at}
\right)
\times 1000
$$

**Thời gian UCON Core xử lý sự kiện sau khi poller lấy sự kiện:**

$$
\texttt{event\_processing\_latency\_ms}
=
\left(
\texttt{processed\_at}
-
\texttt{picked\_at}
\right)
\times 1000
$$

**Tổng thời gian từ lúc sự kiện được tạo đến lúc xử lý xong:**

$$
\texttt{event\_end\_to\_end\_latency\_ms}
=
\left(
\texttt{processed\_at}
-
\texttt{occurred\_at}
\right)
\times 1000
$$

**Thời gian Bộ xử lý ngữ cảnh (Context Handler – CH) đánh giá lại chính sách đang áp dụng:**

$$
\texttt{reevaluation\_duration\_ms}
=
\texttt{session\_events.details\_json.durationMs}
$$

**Thời gian từ lúc sự kiện được tạo đến khi phiên bị thu hồi:**

$$
\texttt{revoke\_latency\_ms}
=
\left(
\texttt{usage\_sessions.revoked\_at}
-
\texttt{occurred\_at}
\right)
\times 1000
$$

### 2.3. Quan hệ giữa các chỉ số
Tổng thời gian xử lý một sự kiện gồm hai phần: **Thời gian sự kiện chờ poller phát hiện** và **thời gian UCON Core xử lý sự kiện** sau đó. Vì vậy:

$$
\texttt{event\_end\_to\_end\_latency\_ms}
=
\texttt{event\_detection\_latency\_ms}
+
\texttt{event\_processing\_latency\_ms}
$$

Trong quá trình xử lý, phiên có thể được chuyển sang trạng thái `REVOKED` trước khi sự kiện được ghi nhận là xử lý hoàn tất tại thời điểm `processed_at`. Do đó, thời gian thu hồi phiên thường nhỏ hơn hoặc bằng tổng thời gian xử lý sự kiện
$$
\texttt{revoke\_latency\_ms}
\le
\texttt{event\_end\_to\_end\_latency\_ms}
$$
Cụ thể hơn:
- `revoke_latency_ms` được ghi nhận tại thời điểm bị thu hồi
- `event_end_to_end_latency_ms` được ghi nhận khi toàn bộ quá trình xử lý sự kiện hoàn tất

Đối với mỗi mức chu kỳ poller, các **giá trị trung bình** và **P95** được tính trên toàn bộ 100 mẫu chính thức. Các chỉ số chính gồm:

- `event_detection_latency_ms`;
- `event_processing_latency_ms`;
- `event_end_to_end_latency_ms`;
- `reevaluation_duration_ms`;
- `revoke_latency_ms`.


### 2.4 Phân tích kết quả
#### **Bảng thống kê**

Một số quy ước trong bảng:
- `TB` là giá trị trung bình
- `P95` là mức 95% mẫu không vượt quá.
- Các cột liên quan đến thời gian đều có đơn vị là ms, trừ khi tên cột ghi rõ đơn vị khác 

| Chu kỳ | Số mẫu | Phát hiện TB | Phát hiện P95 | Xử lý TB | Xử lý P95 | Toàn trình TB | Toàn trình P95 | Tái đánh giá TB | Tái đánh giá P95 | Thu hồi TB | Thu hồi P95 | Hoàn tất TB | Hoàn tất P95 |
|---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|
| 250 | 100 | 128,0 | 240,4 | 17,7 | 20,4 | 145,7 | 257,4 | 6,39 | 7,91 | 137,5 | 249,5 | 8,22 | 9,57 |
| 500 | 100 | 264,8 | 487,9 | 17,5 | 20,5 | 282,3 | 505,7 | 6,43 | 7,61 | 274,2 | 497,2 | 8,05 | 9,70 |
| 1000 | 100 | 470,3 | 975,1 | 17,4 | 20,5 | 487,8 | 993,1 | 6,30 | 7,59 | 479,5 | 984,5 | 8,26 | 9,87 |
| 2000 | 100 | 1021,8 | 1967,3 | 17,9 | 20,2 | 1039,7 | 1984,2 | 6,39 | 7,71 | 1031,3 | 1976,3 | 8,45 | 10,06 |
| 5000 | 100 | 2695,4 | 4745,8 | 17,3 | 20,2 | 2712,7 | 4763,4 | 6,07 | 7,52 | 2704,3 | 4754,9 | 8,32 | 10,46 |

<figure align="center">
  <img src="./ucon_measurement_analysis_postupdate/charts/01_db_event_detection_latency.png" alt="Hình 1. Độ trễ phát hiện sự kiện theo chu kỳ poller" />
  <figcaption><i>Hình 1. Độ trễ phát hiện sự kiện theo chu kỳ poller</i></figcaption>
</figure>

Biều đồ đầu tiên cho thấy độ trễ phát hiện sự kiện tăng rõ rệt khi chu kỳ của poller tăng. Khi chu kỳ tăng từ `250 ms` lên `5000 ms` Thời gian phát hiện trung bình tăng từ `128,0 ms` lên `2695,4 ms` và thời gian phát hiện của P95 tăng từ `240,4 ms` lên `4745,8 ms`. 

Để ý kỹ hơn, ta sẽ để ý thời gian phát hiện trung bình thường nằm quanh khoảng nửa của chu kỳ. Nguyên nhân có thể là trong khoảng giữa của hai chu kỳ có thể sẽ có một sự kiện xuất hiện. Nếu sự kiện xuất hiện gần lần quét tiếp theo thì được phát hiện nhanh; nếu xuất hiện ngay sau một lần quét thì phải chờ lâu hơn. Khi tính trung bình nhiều trường hợp, thời gian chờ thường xấp xỉ một nửa chu kỳ.

P95 lại gần bằng với chu kỳ của poller. Điều này có nghĩa là 95 mẫu trong 100 mẫu có thời gian phát hiện không vượt quá giá trị P95. Ở các trường hợp chậm, sự kiện có thể phải chờ gần hết một chu kỳ mới được phát hiện, thường do sự kiện xuất hiện ngay sau khi poller vừa quét xong.

**Nhận xét**: Chu kỳ poller ảnh hưởng trực tiếp đến tốc độ phát hiện sự kiện. Chu kỳ càng ngắn thì sự kiện được phát hiện càng nhanh.

<figure align="center">
  <img src="./ucon_measurement_analysis_postupdate/charts/02_db_event_end_to_end_latency.png"
       alt="Độ trễ toàn trình và độ trễ thu hồi theo chu kỳ poller" />
  <figcaption><i>Hình 2. Độ trễ toàn trình và độ trễ thu hồi theo chu kỳ poller</i></figcaption>
</figure>

Đối với biểu đồ số 2, P95 của độ trễ toàn tiến trình và của thu hồi gần như trùng nhau ở tất cả các mức của chu kỳ. Nguyên nhân là phiên được chuyển sang trạng thái `REVOKED` ngay trong quá trình xử lý sự kiện, trước khi sự kiện được đánh dấu là xử lý hoàn tất tại thời điểm `processed_at`. Chính vì vậy, độ trễ thu hồi thường nhỏ hơn độ trễ toàn trình một khoảng nhỏ, khoảng `8–9 ms`.

Hiểu một cách đơn giản, phần lớn thời gian của toàn bộ quá trình là thời gian chờ poller phát hiện ra sự kiện. Sau khi sự kiện đã được tìm thấy, hệ thống chỉ cần một khoảng thời gian tương đối nhỏ để thu hồi phiên và hoàn tất việc xử lý.

**Nhận xét**: Chu kỳ poller càng dài thì phiên càng bị thu hồi chậm. Do đó, giảm chu kỳ poller sẽ giúp giảm cả độ trễ thu hồi và độ trễ toàn trình.

## 3. Phép đo phụ trợ: Thăm dò khi không có sự kiện
### 3.1. File kết quả

Mẫu tên file:

```text
idle_polling_<poller_interval>ms_<timestamp>.json
```

Ví dụ:

```text
idle_polling_1000ms_20260708_101500.json
```

Phép đo đặt lại bộ đếm của poller, chờ trong khoảng thời gian không có sự kiện đang chờ xử lý, sau đó lấy số liệu tại thời điểm kết thúc.

### 3.2. Một số phép tính liên quan

Thời gian đo thực tế theo phút:

$$
\texttt{elapsedMinutes}
=
\frac{\texttt{elapsedMs}}{60000}
$$

Số lần poller chạy trung bình mỗi phút:

$$
\texttt{pollerRunsPerMinute}
=
\frac{\texttt{pollRuns}}{\texttt{elapsedMinutes}}
$$

Số lần kiểm tra không tìm thấy sự kiện mỗi phút:

$$
\texttt{emptyPollsPerMinute}
=
\frac{\texttt{emptyPolls}}{\texttt{elapsedMinutes}}
$$

Số sự kiện trung bình được xử lý trong mỗi lần thăm dò:

$$
\texttt{eventsProcessedPerPoll}
=
\frac{\texttt{eventsProcessed}}{\texttt{pollRuns}}
$$

Tần suất lý thuyết nếu bỏ qua thời gian thực thi mỗi lần thăm dò:

$$
\texttt{poller\_runs\_per\_minute\_theoretical}
=
\frac{60000}{\texttt{pollerIntervalMs}}
$$

### 3.3. Quan hệ giữa các chỉ số

Khi phép đo thật sự không có sự kiện:

$$
\texttt{eventsProcessed}
=
0
$$

$$
\texttt{emptyPolls}
=
\texttt{pollRuns}
$$

$$
\texttt{eventsProcessedPerPoll}
=
0
$$

Do poller sử dụng `fixedDelay`, một chu kỳ thực tế bao gồm cả thời gian chạy lần thăm dò trước và khoảng nghỉ được cấu hình. Vì vậy:

$$
\texttt{pollerRunsPerMinute}
\le
\texttt{poller\_runs\_per\_minute\_theoretical}
$$

### 3.4. Phân tích kết quả

| Chu kỳ (ms) | Lần quét/phút | Quét rỗng/phút | Lý thuyết/phút | Chu kỳ thực (ms) | Tỷ lệ đạt (%) |
|---:|---:|---:|---:|---:|---:|
| 250 | 233,59 | 233,59 | 240,0 | 256,86 | 97,33 |
| 500 | 117,59 | 117,59 | 120,0 | 510,24 | 97,99 |
| 1000 | 59,40 | 59,40 | 60,0 | 1010,16 | 98,99 |
| 2000 | 29,80 | 29,80 | 30,0 | 2013,56 | 99,33 |
| 5000 | 12,00 | 12,00 | 12,0 | 5000,35 | 99,99 |

<figure align="center">
  <img src="./ucon_measurement_analysis_postupdate/charts/03_idle_polling_overhead.png"
       alt="Chi phí thăm dò khi không có sự kiện" />
  <figcaption><i>Hình 3. Chi phí thăm dò khi không có sự kiện</i></figcaption>
</figure>

Về tổng quan, biểu đồ sô 3 cho thấy số lần thăm dò thực tế thấp hơn lý thuyết vì trên thực tế mỗi lần thăm dò cần một khoảng thời gian để truy vấn và hoàn tất xử lý. Do sử dụng cơ chế độ trễ cố định, chu kỳ thực tế bằng thời gian xử lý cộng với chu kỳ cấu hình nên số lần chạy trong một phút sẽ có phần giảm nhẹ.

**Nhận xét**: Mặc dù, khi chu kỳ của poller càng ngắn thì sẽ càng giúp cho hệ thống kiểm tra sự kiện thường xuyên hơn nhưng lại làm tăng số lần thăm dò rỗng hoặc không có sự kiện. Ngược lại, chu kỳ dài giúp giảm chi phí kiểm tra nền nhưng có thể làm sự kiện được phát hiện chậm hơn.

Do đó, ở biểu đồ số 4 sẽ giúp ta xem xét và chọn ra điểm cân bằng chu kỳ trong bộ dữ liệu này, không có nghĩa sẽ là chu kỳ tối ưu cho mọi hệ thống. Cụ thể hơn, biểu đồ thể hiện sự đánh đổi giữ tốc độ thu hồi phiền và chi phí thăm dò khi không có sự kiện.
- Đường màu xanh là độ trễ thu hồi P95 đã được chuẩn hóa (khoảng từ 0 - 1). Khi chu kỳ càng lớn thì phiên bị thu hồi càng chậm.
- Ngược lại, số lần lăm dò không có sự kiện của mỗi phút được chuẩn hóa (khoảng từ 0 - 1), được thể hiện qua đường màu cam, lại đi xuống khi chu kỳ poller tăng lên nên chi phí kiểm tra nền giảm.
- Và hai đường này cắt nhau tại thời điểm khoảng `1141 ms` - là độ trễ thu hồi và chi phí thăm dò tương đối cân bằng. Trong các chu kỳ poller đang xét thì chu kỳ `1000 ms` nằm gần giao điểm này nhất, do đó, chu kỳ `1000 ms` được chọn làm mức cân bằng đại diện. 

<figure align="center">
  <img src="./ucon_measurement_analysis_postupdate/charts/04_representative_interval_tradeoff.png"
       alt="Điểm cân bằng giữa độ trễ thu hồi và chi phí thăm dò" />
  <figcaption><i>Hình 4. Điểm cân bằng giữa độ trễ thu hồi và chi phí thăm dò</i></figcaption>
</figure>

#### Bảng chuẩn hóa điểm cân bằng

| Chu kỳ (ms) | Phát hiện P95 | Thu hồi P95 | Quét rỗng/phút | Thu hồi chuẩn hóa | Quét rỗng chuẩn hóa | Độ lệch | Điểm cân bằng |
|---:|---:|---:|---:|---:|---:|---:|---:|
| 250 | 240,4 | 249,5 | 233,59 | 0,0525 | 1,0000 | 0,9475 | 1,0525 |
| 500 | 487,9 | 497,2 | 117,59 | 0,1046 | 0,5034 | 0,3988 | 0,6080 |
| 1000 | 975,1 | 984,5 | 59,40 | 0,2070 | 0,2543 | 0,0472 | 0,4613 |
| 2000 | 1967,3 | 1976,3 | 29,80 | 0,4156 | 0,1276 | 0,2881 | 0,5432 |
| 5000 | 4745,8 | 4754,9 | 12,00 | 1,0000 | 0,0514 | 0,9486 | 1,0514 |

## 4. Kịch bản 2: Một sự kiện ảnh hưởng đến nhiều phiên
### 4.1. File kết quả

Kịch bản 2 tạo hai loại tệp.

Tệp tổng hợp, mỗi dòng tương ứng với một sự kiện:

```text
db_event_multi_session_summary_<poller_interval>ms_<session_count>sessions_<timestamp>.csv
```

Ví dụ:

```text
db_event_multi_session_summary_1000ms_50sessions_20260728_110536.csv
```

Tệp chi tiết, mỗi dòng tương ứng với một phiên bị ảnh hưởng:

```text
db_event_multi_session_details_<poller_interval>ms_<session_count>sessions_<timestamp>.csv
```

Ví dụ:

```text
db_event_multi_session_details_1000ms_50sessions_20260728_110536.csv
```

Mỗi lần đo thực hiện luồng:

```text
Tạo N phiên `ACTIVE` trên cùng một gói thuê
→ đổi `user_rentals.status` từ `ACTIVE` sang `CANCELLED`
→ cơ sở dữ liệu tạo một sự kiện `ATTRIBUTE_CHANGED`
→ poller lấy sự kiện
→ UCON Core đánh giá lại chính sách của N phiên
→ các phiên không còn hợp lệ chuyển sang `REVOKED`
```

### 4.2. Một số phép tính liên quan

Số phiên thực tế được đánh giá lại:

$$
\texttt{affected\_session\_count}
=
\operatorname{COUNT}
\left(
\text{các dòng đánh giá lại của cùng sự kiện}
\right)
$$

Số phiên đã bị thu hồi:

$$
\texttt{revoked\_session\_count}
=
\operatorname{COUNT}
\left(
\texttt{final\_status}=\texttt{REVOKED}
\right)
$$

Độ trễ phát hiện, xử lý và toàn trình của sự kiện được tính giống Kịch bản 1:

$$
\texttt{event\_detection\_latency\_ms}
=
\left(
\texttt{picked\_at}
-
\texttt{occurred\_at}
\right)
\times 1000
$$

$$
\texttt{event\_processing\_latency\_ms}
=
\left(
\texttt{processed\_at}
-
\texttt{picked\_at}
\right)
\times 1000
$$

$$
\texttt{event\_end\_to\_end\_latency\_ms}
=
\left(
\texttt{processed\_at}
-
\texttt{occurred\_at}
\right)
\times 1000
$$

Với \($N$\) là số phiên bị ảnh hưởng và \($r_i$\) là thời gian đánh giá lại của phiên thứ \($i$\):

$$
\texttt{reevaluation\_total\_ms}
=
\sum_{i=1}^{N} r_i
$$

$$
\texttt{reevaluation\_mean\_ms}
=
\frac{\sum_{i=1}^{N}r_i}{N}
$$

$$
\texttt{reevaluation\_max\_ms}
=
\max
\left(
r_1,r_2,\ldots,r_N
\right)
$$

Với \($v_i$\) là thời gian từ lúc sự kiện được tạo đến khi phiên thứ \($i$\) bị thu hồi:

$$
v_i
=
\left(
\texttt{session}_i\texttt{.revoked\_at}
-
\texttt{event.occurred\_at}
\right)
\times 1000
$$

$$
\texttt{revoke\_mean\_ms}
=
\frac{\sum_{i=1}^{N}v_i}{N}
$$

$$
\texttt{revoke\_max\_ms}
=
\max
\left(
v_1,v_2,\ldots,v_N
\right)
$$

Chi phí xử lý ngoài tổng thời gian đánh giá lại:

$$
\texttt{non\_reevaluation\_overhead\_ms}
=
\texttt{event\_processing\_latency\_ms}
-
\texttt{reevaluation\_total\_ms}
$$

Độ chênh giữa phiên bị thu hồi muộn nhất và mức thu hồi trung bình:

$$
\texttt{revoke\_spread\_ms}
=
\texttt{revoke\_max\_ms}
-
\texttt{revoke\_mean\_ms}
$$

### 4.3. Quan hệ giữa các chỉ số

Một lần đo chỉ được xem là thành công khi:

$$
\texttt{affected\_session\_count}
=
\texttt{requested\_session\_count}
$$

$$
\texttt{revoked\_session\_count}
=
\texttt{requested\_session\_count}
$$

Các giá trị tổng hợp được tính từ dữ liệu chi tiết của cùng một sự kiện:

$$
\texttt{reevaluation\_total\_ms}
=
\operatorname{SUM}
\left(
\texttt{details.reevaluation\_duration\_ms}
\right)
$$

$$
\texttt{revoke\_mean\_ms}
=
\operatorname{MEAN}
\left(
\texttt{details.revoke\_latency\_ms}
\right)
$$


### 4.4. Phân tích kết quả

| Số phiên | Số mẫu | Phát hiện TB | Xử lý TB | Xử lý P95 | Xử lý lớn nhất | Tổng tái đánh giá TB | Tổng tái đánh giá P95 | Tái đánh giá/phiên | Chi phí ngoài tái đánh giá | Toàn trình P95 | Thu hồi P95 TB | Thu hồi lớn nhất TB | Độ chênh TB | Độ chênh P95 |
|---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|
| 1 | 100 | 515,5 | 18,3 | 21,2 | 24,4 | 6,6 | 7,8 | 6,617 | 11,7 | 923,2 | 525,3 | 525,3 | 0,0 | 0,0 |
| 10 | 100 | 466,7 | 107,7 | 118,7 | 123,9 | 41,2 | 46,1 | 4,122 | 66,5 | 1071,0 | 566,0 | 569,9 | 42,6 | 47,2 |
| 50 | 100 | 545,8 | 414,1 | 436,2 | 452,7 | 158,3 | 168,2 | 3,167 | 255,7 | 1349,6 | 937,2 | 955,8 | 188,0 | 202,2 |
| 100 | 100 | 491,0 | 789,5 | 834,3 | 1010,3 | 300,1 | 323,4 | 3,001 | 489,4 | 1761,1 | 1239,3 | 1276,5 | 374,3 | 393,7 |

Từ biểu đồ số 5 bên dưới, ta có thể dễ dàng thấy được khi số phiên hoạt động được xét tăng lên thì thời gian xử lý cũng tăng lên và thậm chí là tăng mạnh. Khi chỉ có 1 phiên, P95 của thời gian xử lý chỉ khoảng `21,2 ms` và đạt `834,3 ms` khi phải xử lý 100 phiên.

Tương tự, thời gian tổng thời tái đánh giá cũng tăng theo, vì khi một sự kiện ảnh hưởng đến cùng lúc nhiều phiên đang hoạt động, điều này đồng nghĩa với việc hệ thống phải kiểm tra và tái đánh giá chính sách cho từng phiên.

Ngoài thời gian xử lý và tái đánh giá, hệ thống còn tốn thời gian cho các công việc khác như tìm những phiên hoạt động bị ảnh hưởng, cập nhật lại trạng thái các phiên đóm ghi dữ liệu và hoàn tất tiến trình. Phần chi phí cho các hành động cũng tăng tăng rõ rệt khi tăng từ `11, 7 ms` lên `489,4 ms` khi xử lý 100 phiên bị ảnh hưởng.

**Nhận xét**: Qua đó, ta có thể thấy được khi một sự kiện xảy ra và nó ảnh hưởng tới càng nhiều phiên thì chi phí xử lý càng lớn. Kết quả cho thấy, khi từ 10 phiên trở lên cùng bị ảnh hưởng bởi một sự kiện, thời gian xử lý, tái đánh giá và các công việc khác tăng gần như tuyến tính theo số phiên do hệ thống phải thực hiện thêm công việc cho từng phiên bị ảnh hưởng. 




<figure align="center">
  <img src="./ucon_measurement_analysis_postupdate/charts/05_multi_session_scaling.png"
       alt="Thời gian xử lý khi số phiên bị ảnh hưởng tăng" />
  <figcaption><i>Hình 5. Thời gian xử lý khi số phiên bị ảnh hưởng tăng</i></figcaption>
</figure>

<figure align="center">
  <img src="./ucon_measurement_analysis_postupdate/charts/06_multi_session_revoke_spread.png"
       alt="Độ chênh thời gian thu hồi giữa các phiên" />
  <figcaption><i>Hình 6. Độ chênh thời gian thu hồi giữa các phiên</i></figcaption>
</figure>

Biểu đồ số 6 này thể hiện sự chênh lệch giữa phiên bị thu hồi chậm nhất và thời gian thu hồi trung bình của các phiên trong một lần đo. Khi số phiên tăng lên, sẽ có nhiều phiên không được thu hồi tại cùng một thời điểm. Những phiên được xử lý trước sẽ bị thu hồi sớm hơn so với những phiên được xử lý sau, làm cho khoảng cách giữa các phiên ngày càng lớn, đặc biệt là phiên xử lý đầu tiên và phiên xử lý cuối cùng.

## 5. Kịch bản 3: Gói thuê hết hạn theo thời gian

### 5.1. File kết quả

Mẫu tên file:

```text
timer_expiry_poller<poller_interval>ms_scheduler<scheduler_interval>ms_<timestamp>.csv
```

Ví dụ:

```text
timer_expiry_poller1000ms_scheduler500ms_20260728_143500.csv
```

Mỗi dòng là kết quả của một lần đo:

```text
Tạo gói thuê có `expires_at` gần thời điểm đo
→ tạo phiên `ACTIVE`
→ dependency `next_check_at` đến hạn
→ scheduler tạo sự kiện` TIMER_DUE`
→ poller lấy sự kiện
→ UCON Core đánh giá lại chính sách
→ phiên hết hạn chuyển sang `REVOKED`
```

### 5.2. Một số phép tính liên quan

Chu kỳ poller và scheduler:

$$
\texttt{poller\_interval\_ms}
=
\texttt{ucon.trigger-poller.interval-ms}
$$

$$
\texttt{timer\_scheduler\_interval\_ms}
=
\texttt{ucon.timer-scheduler.interval-ms}
$$

Độ lệch ngẫu nhiên được thêm để tránh việc thời điểm hết hạn luôn trùng với chu kỳ scheduler:

$$
\texttt{random\_phase\_ms}
\sim
\operatorname{Uniform}
\left(
0,
\texttt{timer\_scheduler\_interval\_ms}
\right)
$$

Thời gian từ lúc chuẩn bị phép đo đến thời điểm hết hạn:

$$
\texttt{expiry\_delay\_seconds}
=
\texttt{expiry\_lead\_seconds}
+
\frac{\texttt{random\_phase\_ms}}{1000}
$$

Độ trễ của scheduler:

$$
\texttt{timer\_detection\_latency\_ms}
=
\left(
\texttt{event\_occurred\_at}
-
\texttt{dependency\_due\_at}
\right)
\times 1000
$$

Độ trễ của poller sau khi sự kiện `TIMER_DUE` được tạo:

$$
\texttt{timer\_event\_detection\_latency\_ms}
=
\left(
\texttt{event\_picked\_at}
-
\texttt{event\_occurred\_at}
\right)
\times 1000
$$

Thời gian UCON Core xử lý sự kiện:

$$
\texttt{timer\_event\_processing\_latency\_ms}
=
\left(
\texttt{event\_processed\_at}
-
\texttt{event\_picked\_at}
\right)
\times 1000
$$

Tổng thời gian từ lúc sự kiện `TIMER_DUE` được tạo đến khi xử lý xong:

$$
\texttt{timer\_event\_end\_to\_end\_latency\_ms}
=
\left(
\texttt{event\_processed\_at}
-
\texttt{event\_occurred\_at}
\right)
\times 1000
$$

Thời gian đánh giá lại chính sách:

$$
\texttt{reevaluation\_duration\_ms}
=
\texttt{session\_events.details\_json.durationMs}
$$

Tổng thời gian từ lúc gói thuê hết hạn đến khi phiên bị thu hồi:

$$
\texttt{timer\_total\_revoke\_latency\_ms}
=
\left(
\texttt{session\_revoked\_at}
-
\texttt{dependency\_due\_at}
\right)
\times 1000
$$

### 5.3. Quan hệ giữa các chỉ số

Theo định nghĩa:

$$
\texttt{timer\_event\_end\_to\_end\_latency\_ms}
=
\texttt{timer\_event\_detection\_latency\_ms}
+
\texttt{timer\_event\_processing\_latency\_ms}
$$

Tổng thời gian từ lúc điều kiện đến hạn đến khi sự kiện được xử lý hoàn tất:

$$
\begin{aligned}
\texttt{event\_processed\_at}
-
\texttt{dependency\_due\_at}
={}&
\texttt{timer\_detection\_latency\_ms}
\\
&+
\texttt{timer\_event\_detection\_latency\_ms}
\\
&+
\texttt{timer\_event\_processing\_latency\_ms}
\end{aligned}
$$

`timer_total_revoke_latency_ms` không bắt buộc bằng tổng ba chỉ số trên vì nó kết thúc tại `session_revoked_at`, còn tổng trên kết thúc tại `event_processed_at`.

Cách biểu diễn sát với thời điểm thu hồi hơn:

$$
\begin{aligned}
\texttt{timer\_total\_revoke\_latency\_ms}
={}&
\texttt{timer\_detection\_latency\_ms}
\\
&+
\texttt{timer\_event\_detection\_latency\_ms}
\\
&+
\left(
\texttt{session\_revoked\_at}
-
\texttt{event\_picked\_at}
\right)
\times 1000
\end{aligned}
$$

Khoảng thời gian từ lúc phiên được ghi nhận thu hồi đến khi sự kiện xử lý hoàn tất:

$$
\begin{aligned}
\texttt{post\_revoke\_finalization\_ms}
={}&
\texttt{timer\_detection\_latency\_ms}
\\
&+
\texttt{timer\_event\_end\_to\_end\_latency\_ms}
\\
&-
\texttt{timer\_total\_revoke\_latency\_ms}
\end{aligned}
$$

Tương đương:

$$
\texttt{post\_revoke\_finalization\_ms}
=
\left(
\texttt{event\_processed\_at}
-
\texttt{session\_revoked\_at}
\right)
\times 1000
$$

### 5.4. Phân tích kết quả

| Chu kỳ lập lịch (ms) | Số mẫu | Phát hiện hạn TB | Phát hiện hạn P95 | Lấy sự kiện TB | Lấy sự kiện P95 | Xử lý TB | Xử lý P95 | Tổng thu hồi TB | Tổng thu hồi P95 | Tái đánh giá TB | Tái đánh giá P95 | Hoàn tất TB | Hoàn tất P95 |
|---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|
| 250 | 100 | 137,2 | 248,8 | 746,7 | 909,2 | 18,4 | 21,1 | 893,9 | 994,4 | 6,69 | 7,92 | 8,43 | 10,04 |
| 500 | 100 | 282,6 | 486,2 | 457,5 | 832,8 | 18,6 | 21,1 | 750,1 | 992,0 | 6,74 | 7,92 | 8,53 | 10,13 |
| 1000 | 100 | 452,5 | 946,0 | 1009,4 | 1014,1 | 18,2 | 21,2 | 1471,3 | 1967,2 | 6,50 | 7,68 | 8,69 | 10,47 |
| 2000 | 100 | 1030,7 | 1963,6 | 471,5 | 923,4 | 18,6 | 21,5 | 1512,2 | 2390,3 | 6,69 | 7,80 | 8,69 | 10,74 |
| 5000 | 100 | 2180,6 | 4710,9 | 476,0 | 932,7 | 18,3 | 20,8 | 2666,3 | 5286,7 | 6,58 | 7,64 | 8,60 | 10,18 |
<figure align="center">
  <img src="./ucon_measurement_analysis_postupdate/charts/07_timer_latency_by_scheduler.png"
       alt="Độ trễ thu hồi theo chu kỳ scheduler" />
  <figcaption><i>Hình 7. Độ trễ thu hồi theo chu kỳ scheduler</i></figcaption>
</figure>


<figure align="center">
  <img src="./ucon_measurement_analysis_postupdate/charts/08_timer_phase_alignment.png"
       alt="Hiện tượng trùng pha giữa scheduler và poller" />
  <figcaption><i>Hình 8. Hiện tượng trùng pha giữa scheduler và poller</i></figcaption>
</figure>

Biểu đồ số 7 cho ta thấy được tổng quan độ trễ của cơ chế timer-based, khi chu kỳ của scheduler tăng thì thời gian để phát hiện ra sự kiện hết hạn gói thuê cũng tăng theo. P95 của độ trễ phá hiện tăng từ `248,8 ms` ở chu kỳ `250 ms` tăng lên thành `4710,9 ms` ở chu kỳ `5000 ms`. Do đó, P95 của tổng thời gian thu hồi phiên cũng tăng mạnh từ khoảng `994,4 ms` lên `5286,7 ms`. Vì sau khi phiên hết quyền sử dụng, hệ thống phải chờ chu kỳ scheduler kiểm tra mới phát hiện là phiên đã hết hạn gói thuê và tiếp tục chờ poller lấy được sự kiện này ra để xử lý và thu hiện phiên. (Biết chu kỳ poller ở kịch bản là `1000 ms`)

Biểu đồ số 8 sẽ giải thích rõ hơn hiện tượng này. Tại thời điểm chu kỳ của scheduler là `1000 ms`, thời gian poller phát hiện ra sự kiện trung bình khoảng `1009,4 ms` và P95 khoảng `1014,1 ms`, tức là gần bằng một chu kỳ thăm dò đầy đủ. Điều này có nghĩa là ở mức chu kỳ này, trong quá trình chạy, scheduler có thể đã phát hiện ra sự kiện ngay sau khi poller quét xong, đồng nghĩa với việc, sự kiện này phải chờ gần hết chu kỳ poller mới được lấy ra và xử lý. Chính vì vậy, mà tổng thu hồi ở chu kỳ scheduler `1000 ms` tiến gần đến `2000 ms`. Ở các chu kỳ bộ lập lịch `2000 ms` và `5000 ms`, thời gian poller phát hiện sự kiện trung bình chỉ khoảng `472–476 ms`, gần một nửa chu kỳ thăm dò `1000 ms`. Tuy nhiên, P95 vẫn đạt khoảng `923–933 ms`, cho thấy ở các trường hợp chậm, sự kiện có thể phải chờ gần đến lần quét tiếp theo mới được phát hiện.

**Nhận xét**: Độ trễ thu hồi của cơ chế timer phụ thuộc đồng thời vào chu kỳ scheduler và chu kỳ poller. Đặc biệt, khi hai chu kỳ cùng bằng `1000 ms`, sự trùng giá trị chu kỳ giữa hai tiến trình có thể khiến sự kiện phải chờ gần toàn bộ một chu kỳ thăm dò.

#### **Khoảng thời gian hoàn tất sau khi thu hồi phiên**

| Chỉ số | Giá trị (ms) |
|---|---:|
| Trung bình | 8,586 |
| Trung vị | 8,558 |
| P95 | 10,245 |
| Nhỏ nhất | 4,533 |
| Lớn nhất | 18,360 |


<figure align="center">
  <img src="./ucon_measurement_analysis_postupdate/charts/09_post_revoke_finalization.png"
       alt="Khoảng thời gian từ lúc phiên bị thu hồi đến khi sự kiện được xử lý xong" />
  <figcaption><i>Hình 9. Khoảng thời gian từ lúc phiên bị thu hồi đến khi sự kiện được xử lý xong</i></figcaption>
</figure>

Sau khi phiên được chuyển sang trạng thái `REVOKED`, hệ thông UCON Core cần phải thực hiện thêm một số bước để hoàn thành quá trình xử lý sự kiện. Khoảng thời gian này là khoảng thời gian từ `revoked_at` đến `processed_at`.

Theo biểu đồ số 9 ta có thể thấy được chu kỳ của scheduler ảnh hưởng đến thời gian phát hiện ra phiến đã đến hạn của gói thuê, nhưng gần như không ảnh hưởng nhiều đên thời gian xử lý còn lại sau khi phiên đã bị thu hồi.


## 6. Ảnh hưởng của `postUpdate` đến thời gian xử lý
<figure align="center">
  <img src="./ucon_measurement_analysis_postupdate/charts/10_before_after_baseline_processing.png"
       alt="Thời gian xử lý cơ bản trước và sau khi bật postUpdate" />
  <figcaption><i>Hình 10. Thời gian xử lý cơ bản trước và sau khi bật postUpdate</i></figcaption>
</figure>

<figure align="center">
  <img src="./ucon_measurement_analysis_postupdate/charts/11_before_after_multi_processing.png"
       alt="Thời gian xử lý nhiều phiên trước và sau khi bật postUpdate" />
  <figcaption><i>Hình 11. Thời gian xử lý nhiều phiên trước và sau khi bật postUpdate</i></figcaption>
</figure>

<figure align="center">
  <img src="./ucon_measurement_analysis_postupdate/charts/12_before_after_finalization.png"
       alt="Khoảng hoàn tất sau thu hồi trước và sau khi bật postUpdate" />
  <figcaption><i>Hình 12. Khoảng hoàn tất sau thu hồi trước và sau khi bật postUpdate</i></figcaption>
</figure>

Từ ba biểu đồ trên cho thấy thời gian xử lý sự kiện đều tăng sau khi bật `postUpdate`. Trong trường hợp một phiên, thời gian xử lý tăng khoảng `5–6 ms` và tương đối ổn định ở các chu kỳ poller. Khi số lượng phiên bị ảnh hưởng tăng, mức chênh lệch trở nên rõ hơn: với 100 phiên, thời gian xử lý trung bình tăng từ `503,3 ms` lên `789,5 ms`.

Đồng thời, khoảng thời gian từ khi phiên chuyển sang `REVOKED` đến khi sự kiện được xử lý hoàn tất tăng từ khoảng `2,71 ms` lên `8,59 ms`. Điều này cho thấy các bước thực hiện sau khi thu hồi phiên tạo thêm chi phí xử lý, đặc biệt khi một sự kiện ảnh hưởng đến nhiều phiên.

**Nhận xét**: `postUpdate` làm tăng thời gian xử lý và mức ảnh hưởng tăng theo số phiên. Tuy nhiên, khoảng thời gian tăng thêm còn bao gồm các bước như cập nhật trạng thái, ghi nhật ký và hoàn tất giao dịch, nên không thể xem toàn bộ phần chênh lệch là thời gian thực thi riêng của `postUpdate`.

# PHẦN III. NHẬN ĐỊNH CHUNG
Từ các kết quả đo và phân tích, ta có thể kết luận rằng chu kỳ của poller ảnh hưởng trực tiếp, phần lớn đến tốc độ phát hiện sự kiện và thu hồi phiên. Chu kỳ càng ngắn giúp hệ thống phản ứng nhanh hơn nhưng lại làm tăng số lần thăm dò không có sự kiện; ngược lại, chu kỳ dài giảm chi phí nền nhưng làm độ trễ tăng. Trong phạm vi các cấu hình đã đo, `1000 ms` là mức cân bằng đại diện, không phải cấu hình tối ưu cho mọi hệ thống.

Bên cạnh đó, khi số phiên bị ảnh hưởng tăng, thời gian xử lý và độ chênh thời gian thu hồi giữa các phiên cũng tăng gần như tuyến tính. Điều này cho thấy các sự kiện tác động đến nhiều phiên sẽ tạo áp lực xử lý lớn hơn và có thể làm một số phiên bị thu hồi muộn hơn so với các phiên khác.

Đối với cơ chế hết hạn theo thời gian, tổng độ trễ phụ thuộc vào cả scheduler và poller. Đặc biệt, khi hai chu kỳ cùng bằng `1000 ms`, hiện tượng trùng pha có thể làm sự kiện phải chờ gần trọn một chu kỳ mới được phát hiện.

Cuối cùng, khi có `postUpdate` sẽ làm tăng thời gian xử lý, nhất là khi số phiên bị ảnh hưởng lớn. Tuy nhiên, phần chênh lệch này còn bao gồm các bước cập nhật trạng thái, ghi dữ liệu và hoàn tất giao dịch, nên không thể xem toàn bộ là chi phí riêng của `postUpdate`.

**Kết luận chung**: UCON Core hoạt động đúng theo thiết kế, nhưng hiệu năng phụ thuộc rõ rệt vào chu kỳ tác vụ nền, số phiên bị ảnh hưởng và các bước xử lý bổ sung. Vì vậy, cấu hình thực tế cần được chọn dựa trên sự cân bằng giữa yêu cầu thu hồi nhanh và chi phí xử lý mà hệ thống có thể chấp nhận.