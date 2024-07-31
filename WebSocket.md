# **I. WebSocket là gì?**
- `WebSocket` là một giao thức truyền tải dữ liệu hai chiều (`full-duplex`) trên web, cho phép truyền tải dữ liệu trong thời gian thực giữa trình duyệt web và máy chủ. `WebSocket` cho phép một kết nối duy trì giữa máy khách và máy chủ, vì vậy các thông tin có thể được gửi đi và nhận lại một cách hiệu quả và nhanh chóng mà không cần phải thiết lập kết nối mới mỗi khi truyền tải thông tin.

- `WebSocket` thường được sử dụng để phát triển các ứng dụng web thời gian thực như trò chơi trực tuyến, ứng dụng trò chuyện và các ứng dụng cập nhật dữ liệu trực tiếp. `WebSocket` được thiết kế để thay thế các giải pháp truyền tải dữ liệu trực tiếp khác như Ajax hoặc Comet, vì nó có hiệu suất cao hơn và tiết kiệm năng lượng hơn.

- Cấu trúc của `WebSocket`
  
<div style='text-align: center;'>

![Untitled](/Pictute/WebSocketDiagram.png)

</div>

Giao thức chuẩn thông thường của WebSocket là `ws://` , giao thức secure là `wss://` . Chuẩn giao tiếp là String và hỗ trợ buffered arrays và blobs.

# **II. Phân loại WebSocket**

## **1. Stream Socket**
Dựa trên giao thức `TCP( Tranmission Control Protocol)`, stream socket thiết lập giao tiếp 2 chiều theo mô hình `client` và `server`. Được gọi là socket hướng kết nối.

<div style='text-align: center;'>

![Untitled](/Pictute/StreamSocket.png)

</div>

Giao thức này đảm bảo dữ liệu được truyền đến nơi nhận một cách đáng tin cậy, đúng tuần tự nhờ vào cơ chế quản lý luồng lưu thông trên mạng và cơ chế chống tắc nghẽn.

Đặc điểm :
 - Có một đường kết nối (địa chỉ IP) giữa 2 tiến trình.
 - Một trong hai tiến trình kia phải đợi tiến trình này yêu cầu kết nối.
 - Mô hình client /sever thì sever lắng nghe và chấp nhận từ client.
 - Mỗi thông điệp gửi phải có xác nhận trả về.
 - Các gói tin chuyển đi tuần tự.

## **2. Datagram Socket**
Dựa trên giao thức `UDP( User Datagram Protocol)` việc truyền dữ liệu không yêu cầu có sự thiết lập kết nối giữa 2 process. Tức là nó cung cấp `connection-less point` cho việc gửi và nhận packets. Gọi là socket không hướng kết nối.

<div style='text-align: center;'>

![Untitled](/Pictute/DatagramSocket.png)

</div>

Do không yêu cầu thiết lập kết nối, không phải có những cơ chế phức tạp. Nên tốc độ giao thức khá nhanh, thuận tiện cho các ứng dụng truyền dữ liệu nhanh như chat, game online…

Đặc điểm :
 - Hai tiến trình liên lạc với nhau không kết nối trực tiếp
 - Thông điệp gửi đi phải kèm theo thông điệp người nhận
 - Thông điệp có thể gửi nhiều lần
 - Người gửi không chắc chắn thông điệp đến tay người nhận.
 - Thông điệp gửi sau có thể đến trước và ngược lại.
 - Để có thể thực hiện các cuộc giao tiếp, một trong 2 quá trình phải công bố port của socket mà mình đang sử dụng.
# **III. Các sự kiện chính trong WebSocket**
`WebSocket` cho phép các ứng dụng web tạo kết nối thời gian thực, có khả năng truyền dữ liệu hai chiều liên tục giữa máy chủ và trình duyệt. Các sự kiện chính của `WebSocket` bao gồm:

- `Mở kết nối (Open)`: Khi một kết nối `WebSocket` được thiết lập giữa trình duyệt và máy chủ, sự kiện `open` được kích hoạt.
- `Nhận dữ liệu (Message)`: Khi trình duyệt hoặc máy chủ nhận được dữ liệu mới, sự kiện `message` được kích hoạt.
- `Gửi dữ liệu (Send)`: Khi trình duyệt hoặc máy chủ gửi dữ liệu, sự kiện `send` được kích hoạt.
- `Đóng kết nối (Close)`: Khi kết nối `WebSocket` bị đóng, sự kiện `close` được kích hoạt.
- `Lỗi (Error)`: Nếu có lỗi trong quá trình thiết lập hoặc sử dụng kết nối `WebSocket`, sự kiện `error` được kích hoạt.
  
Các sự kiện này đều cung cấp các thông tin cần thiết cho ứng dụng để xử lý các tình huống khác nhau khi sử dụng WebSocket.
# **IV. Công dụng của WebSocket**
`WebSocket` là một giao thức mạng cho phép truyền tải dữ liệu theo thời gian thực giữa các ứng dụng web và máy chủ một cách hiệu quả. Các công dụng của `WebSocket` bao gồm:

 - Thực hiện các ứng dụng web thời gian thực: Với sự hỗ trợ của `WebSocket`, các ứng dụng web có thể truyền tải và nhận dữ liệu giữa `client` và `server` một cách nhanh chóng, liên tục và liên tục mà không cần phải làm mới trang hay gửi yêu cầu mới.
 - Giảm thiểu tài nguyên hệ thống: So với việc sử dụng `HTTP`, `WebSocket` giúp giảm thiểu tài nguyên hệ thống vì kết nối giữa `client` và `server` sẽ được giữ mở trong toàn bộ phiên làm việc.
 - Tăng tính tương tác: Với khả năng truyền tải dữ liệu liên tục và trực tiếp giữa `client` và `server`, các ứng dụng web có thể tăng tính tương tác và cung cấp trải nghiệm người dùng tốt hơn.
 - Thực hiện các ứng dụng đa truy cập: `WebSocket` giúp các ứng dụng web đa truy cập trở nên dễ dàng hơn bởi vì nhiều kết nối có thể được thiết lập giữa `client` và server một cách đồng thời.
 - Tích hợp với các ứng dụng truyền tải dữ liệu lớn: Với khả năng truyền tải dữ liệu lớn, các ứng dụng truyền tải dữ liệu lớn có thể sử dụng `WebSocket` để truyền tải dữ liệu một cách nhanh chóng và hiệu quả hơn.
# **V. Ưu và nhược điểm của WebSocket**
`WebSocket` cung cấp một cách kết nối liên tục, toàn diện hơn để truyền tải dữ liệu giữa hai bên, so với các phương pháp truyền tải dữ liệu truyền thống như `HTTP`. Dưới đây là một số ưu và nhược điểm của `WebSocket`:

`Ưu điểm:`
 - Thời gian đáp ứng nhanh hơn: Vì `WebSocket` cho phép duy trì kết nối mở liên tục giữa máy khách và máy chủ, việc truyền tải dữ liệu được thực hiện nhanh hơn so với `HTTP`.
 - Kích thước dữ liệu nhỏ hơn: `WebSocket` giảm thiểu kích thước gói tin và số lần giao tiếp yêu cầu/phản hồi so với `HTTP`. Do đó, kích thước dữ liệu được truyền tải sẽ nhỏ hơn, làm giảm lưu lượng mạng.
 - Công nghệ phù hợp cho ứng dụng web thời gian thực: `WebSocket` là một công nghệ phù hợp cho các ứng dụng web thời gian thực như trò chuyện trực tuyến, trò chơi trực tuyến, cập nhật dữ liệu thời gian thực và các ứng dụng tương tác người dùng khác.
 - Không cần phải gửi yêu cầu mới để nhận dữ liệu mới: Khi sử dụng `HTTP`, trình duyệt phải gửi yêu cầu mới để nhận dữ liệu mới từ máy chủ. Tuy nhiên, với `WebSocket`, máy chủ có thể gửi dữ liệu mới cho trình duyệt mà không cần phải yêu cầu mới.

`Nhược điểm:`
 - Tốn nhiều tài nguyên: `WebSocket` sử dụng kết nối mở liên tục giữa trình duyệt và máy chủ, vì vậy nó sẽ tốn nhiều tài nguyên hơn so với `HTTP`.
 - Không được hỗ trợ bởi một số trình duyệt cũ: Một số phiên bản cũ của các trình duyệt không hỗ trợ `WebSocket`, điều này có thể gây khó khăn cho các nhà phát triển khi triển khai ứng dụng của họ.
 - Thiếu tính năng bảo mật: Mặc dù `WebSocket` cung cấp các tính năng bảo mật cơ bản, nhưng nó vẫn có thể bị tấn công
# **VI. Phân biệt WebSocket và HTTP**

|                           | HTTP                                                                                                                                                                                                                                                                                 | WebSocket                                                                                                                                                                                                                                                                                                      |
| ------------------------- | ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------ | -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| Cơ chế truyền tải dữ liệu | Là giao thức request-response, nghĩa là khi một client gửi một yêu cầu tới server thì server sẽ trả về một phản hồi duy nhất và ngắn gọn. Sau đó, kết nối giữa client và server sẽ đóng lại. Điều này có nghĩa là để gửi dữ liệu mới, client sẽ phải gửi một yêu cầu mới đến server. | Là giao thức full-duplex, nghĩa là cả client và server có thể gửi và nhận dữ liệu đồng thời mà không cần phải gửi yêu cầu và phản hồi lần lượt như HTTP. Kết nối giữa client và server sẽ được giữ mở trong toàn bộ phiên làm việc, cho phép gửi và nhận dữ liệu trực tiếp mà không cần thiết lập lại kết nối. |
| Thiết lập kết nối         | Kết nối được thiết lập bằng cách gửi một yêu cầu từ client đến server. Sau khi server trả về phản hồi, kết nối giữa client và server sẽ đóng lại.                                                                                                                                    | Kết nối được thiết lập bằng cách sử dụng một giao thức riêng (WebSocket handshake protocol) để thiết lập kết nối giữa client và server. Sau khi kết nối được thiết lập, client và server sẽ gửi dữ liệu đến nhau mà không cần gửi yêu cầu và phản hồi.                                                         |
| Định dạng dữ liệu         | Thông thường sử dụng định dạng dữ liệu như HTML, JSON hoặc XML để truyền tải dữ liệu giữa client và server.                                                                                                                                                                          | Không giới hạn định dạng dữ liệu được sử dụng để truyền tải dữ liệu giữa client và server.                                                                                                                                                                                                                     |
| Độ tin cậy                | Tính tin cậy của HTTP phụ thuộc vào giao thức được sử dụng để gửi dữ liệu (ví dụ như TCP hoặc UDP). Nếu kết nối mạng gặp sự cố hoặc bị gián đoạn, việc truyền tải dữ liệu qua HTTP có thể bị mất hoặc bị lỗi.                                                                        | WebSocket được thiết kế để giảm thiểu tối đa tình trạng mất kết nối hoặc gián đoạn                                                                                                                                                                                                                             |

