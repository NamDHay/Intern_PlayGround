# **MODBUS**
## **I. MODBUS là gì?**

- `MODBUS` là một chuẩn giao thức truyền thông công nghiệp được phát hành và phát triển bởi MODICON vào năm 1979, và chính thức thuộc về Schneider Electrics vào năm 1996. `MODBUS` đã nhanh chóng trở thành trở thành tiêu chuẩn truyền thông trong các ngành công nghiệp tự động hóa bởi tính ổn định, dễ dàng, thuận tiện và đặc biệt hơn nữa là MIỄN PHÍ.
- Modbus là một giao thức mở; vì nó xác định các quy tắc để tổ chức và giải thích dữ liệu. Nhưng vẫn đơn giản là một cấu trúc nhắn tin, độc lập với lớp vật lý bên dưới. Vì nó dễ hiểu, có sẵn miễn phí và có thể truy cập được cho mọi người. Do đó nó đã trở thành một giao thức rất phổ biến; được sử dụng rộng rãi trong các ngành sản xuất công nghiệp.

## **II. Nguyên tắc hoạt động của MODBUS**

- `MODBUS` hoạt động theo nguyên tắc `Master – Slave`, thiết bị yêu cầu thông tin được gọi là Modbus Master và thiết bị cung cấp thông tin là Modbus Slaves. Nói một cách dễ hiểu, nó là một phương pháp được sử dụng để truyền thông tin qua đường dây nối tiếp giữa các thiết bị điện tử.
- Trong mạng Modbus tiêu chuẩn, có một Master và tối đa 247 Slave, mỗi Slave có một địa chỉ Slave duy nhất từ 1 đến 247. Master cũng có thể ghi thông tin vào các Slave.

## **III. Phân loại chuẩn MODBUS**

Modbus có một số biến thể tùy thuộc vào loại cổng nối tiếp hoặc thậm chí Ethernet: Modbus RTU, Modbus TCP/IP, Modbus RS485, Modbus ASCII và Modbus UDP/IP. Ngoài ra, có một Modbus Poll là một trình giả lập được các nhà phát triển sử dụng để xác định tính hiệu quả của giao thức Modbus.

Hiện nay, MODBUS được biết đến và sử dụng phổ biến trong công nghiệp gồm 3 chuẩn: `MODBUS RTU`, `MODBUS TCP` và `MODBUS ASCII`.

### **1. Modbus RTU**

- `Modbus RTU` được coi là phương tiện truyền thông phổ biến nhất trong công nghệ tự động hóa công nghiệp. Nó là phương pháp được sử dụng nhiều nhất để liên kết các thành phần; thiết bị, mạng, hệ thống điện tử công nghiệp.
- Dữ liệu được mã hóa theo hệ nhị phân, và chỉ cần một byte truyền thông cho một byte dữ liệu. Đây là giao thức truyền thông lí tưởng đối với `RS232` hay `RS485`, tốc độ từ 1200 đến 115000 baud. Tốc độ phổ biến nhất là từ 9600 đến 19200 baud. `MODBUS RTU` là giao thức truyền thông công nghiệp được sử dụng rộng rãi nhất.

### **2. MODBUS TCP/IP**

- Nó đơn giản là giao thức Modbus RTU và giao diện TCP chạy trên Ethernet.
- TCP / IP đề cập đến Giao thức điều khiển truyền và Giao thức Internet, cung cấp phương tiện truyền cho tin nhắn Modbus TCP / IP.
- `MODBUS TCP` là MODBUS qua `Ethernet` (RJ45). Với `MODBUS TCP`, dữ liệu MODBUS được tóm lược đơn giản trong một gói `TCP/IP`. Nói một cách đơn giản, đây như là một thông điệp của Modbus RTU được truyền bằng trình bao bọc `TCP/IP` và được gửi qua mạng thay vì các đường nối tiếp. Máy chủ không có `Slave ID` vì nó sử dụng địa chỉ IP.

**Trong đó:**

`TCP` (Transmission Control Protocol) là giao thức điều khiển đường truyền và `IP` (Internet Protocol) là giao thức Internet. Các giao thức này được sử dụng cùng nhau và là giao thức truyền tải cho internet. Khi thông tin modbus được gửi bằng các giao thức này, dữ liệu được chuyển tới TCP nơi thông tin bổ sung được đính kèm và cấp cho IP. IP sau đó đặt dữ liệu trong một gói (hoặc gói dữ liệu) và truyền nó.

TCP phải được thiết lập kết nối trước khi truyền dữ liệu, vì nó là giao thức dựa trên kết nối. `Master` (hoặc `Client` trong Modbus TCP) thiết lập kết nối với `Slave` (hoặc `Server`). `Server` chờ một kết nối đến từ `Client`. Sau khi kết nối được thiết lập, `Server` sẽ phản hồi các truy vấn từ `Client` cho đến khi `Client` ngắt kết nối.

## **IV. Địa chỉ dữ liệu và thanh ghi theo chuẩn MODBUS**

![Untitled](/Pictute/ModbusDataFrame.png)

Thông tin dữ liệu được lưu trữ trong thiết bị Slave được chia trong 4 khoảng giá trị khác nhau. Hai khoảng lưu trữ các giá trị rời rạc on/off (coils) và hai khoảng lưu trữ giá trị số (register – thanh ghi). Mỗi coils và register đều có khoảng biến chỉ đọc (read-only) và biến đọc và ghi (read-write).

- Mỗi khoảng có 9999 biến giá trị
- Mỗi coil hoặc contact là 1 bit và được gán một địa chỉ dữ liệu trong khoảng từ 0000 đến 270E
- Mỗi register là 1 word = 16 bít = 2 bytes và cũng được gán một địa chỉ dữ liệu từ 0000 đến 270E

| Coil/Register Numbers | Data Addresses | Type       | Table Name                      |
| --------------------- | -------------- | ---------- | ------------------------------- |
| 1-9999                | 0000 to 270E   | Read-Write | Discrete Output Coils           |
| 10001-19999           | 0000 to 270E   | Read-Only  | Discrete Input Contacts         |
| 30001-39999           | 0000 to 270E   | Read-Only  | Analog Input Registers          |
| 40001-49999           | 0000 to 270E   | Read-Write | Analog Output Holding Registers |

Coil/Register Numbers có thể được coi như tên vị trị vì chúng không xuất hiện trong các thông điệp thực tế. `Data Address` được sử dụng trong các thông điệp truyền tải (truy xuất dữ liệu).

Ví dụ: Holding Register có số là 40001, có `Data Address` là 0000. Sự khác biệt giữa hai giá trị này là độ lệch. Mỗi bảng có một độ lệch khác nhau. 1, 10001, 30001 và 40001.

**Function code**

Byte thứ hai được `Master` gửi đi là `Function code`. Con số này cho `Slave` biết được rằng, địa chỉ nào cần truy cập để đọc hay ghi giá trị.

| Function Code | Action         | Table Name                      |
| ------------- | -------------- | ------------------------------- |
| 01 (01 hex)   | Read           | Discrete Output Coils           |
| 05 (05 hex)   | Write single   | Discrete Output Coil            |
| 15 (0F hex)   | Write multiple | Discrete Output Coils           |
| 02 (02 hex)   | Read           | Discrete Input Contacts         |
| 04 (04 hex)   | Read           | Analog Input Registers          |
| 03 (03 hex)   | Read           | Analog Output Holding Registers |
| 06 (06 hex)   | Write single   | Analog Output Holding Register  |
| 16 (10 hex)   | Write multiple | Analog Output Holding Registers |

## **V. Sự khác biệt giữa RTU và TCP**

- Một tiêu đề 7 byte mới được gọi là `MBAP Header` (Modbus Application Header) được thêm vào đầu thư. Tiêu đề này có dữ liệu sau:
- `Mã định danh (Transaction ID)`: 2 byte do Client đặt để nhận dạng từng yêu cầu duy nhất. Các byte này được lặp lại bởi Server vì các phản hồi của nó có thể không được nhận theo thứ tự như các yêu cầu.
- `Định dạng giao thức (Protocol ID)`: 2 byte do Client đặt, luôn luôn = 00 00
- `Độ dài (Length)`: 2 byte xác định số byte trong thông điệp cần theo dõi.
- `Định dạng đơn vị (UnitlD)`: 1 byte được đặt bởi Client và được Server lặp lại để xác định một Slave từ xa được kết nối trên đường truyền nối tiếp hoặc trên các bus khác.

![Untitled](/Pictute/ModbusDataFrame.png)

**Yêu cầu tương đương với ví dụ về Modbus RTU này:**

11 03 006B 0003 7687

**Trong đó:**

- 11: Địa chỉ SlaveID (17 = 11 hex)
- 03: Function code (đọc thanh ghi giữ đầu ra tương tự (Analog Output Holding Registers))
- 006B: địa chỉ dữ liệu của thanh ghi đầu tiên được yêu cầu. (40108-40001 = 107 = 6B hex)
- 0003: tổng số thanh ghi được yêu cầu. (đọc 3 thanh ghi 40108 đến 40110)
- 7687: CRC (kiểm tra dự phòng theo chu kỳ) để kiểm tra lỗi.

**Modbus TCP tương đương sẽ là:**

0001 0000 0006 11 03 006B 0003

**Trong đó:**

- 0001: mã định danh
- 0000: định dạng giao thức
- 0006: độ dài tin nhắn (6 byte)
- 11: định dạng đơn vị (17 = 11 hex)
- 03: Function code (đọc thanh ghi giữ đầu ra tương tự (Analog Output Holding Registers))
- 006B: địa chỉ dữ liệu của thanh ghi đầu tiên được yêu cầu. (40108-40001 = 107 = 6B hex)
- 0003: tổng số thanh ghi được yêu cầu. (đọc 3 thanh ghi 40108 đến 40110)

## **VI. Code**

`Thư viện MODBUS (RTU + TCP)` : https://github.com/emelianov/modbus-esp8266. Thư viện này có thể chạy trên cả ESP32 lẫn ESP8266. Code mẫu trong bài viết này dùng ESP32 giao tiếp RS485 ở chế độ Master và TCP/IP ở chế độ Client thông qua cổng Ethernet.

### **1. MODBUS RTU**
Trước khi bắt đầu ta phải thêm class của thư viện
```cpp
ModbusRTU mbRTU;
```
#### **a. Master Init**

```cpp
void MODBUS_RTU::MasterInit(HardwareSerial *port, unsigned long baud)
{
    modbusRTU.config.port = port;
    modbusRTU.config.baud = baud;
    port->begin(baud, SERIAL_8N1);
    mbRTU.begin(port);
    mbRTU.master();
}
```
- `port` : Trỏ tới cổng truyền thông muốn sử dụng ( với Arduino có thể là Serial/Serial1/Serial2 hoặc là SoftSerial).
- `baud` : Baudrate của cổng tuyền thông
- `SERIAL_8N1` : định dạng truyền 8bit, không parity bit và 1 stop
#### **b. Read/Write Multiple Data**
**ĐỌC**
```cpp
bool read_Multiple_Data(byte ID, uint16_t *value, long startAddress, size_t length);
```
- `ID` : ID của thiết bị slave cần đọc
- `value` : con trỏ đến vùng nhớ cần để chứa dữ liệu đọc lên từ slave
- `startAddress` : địa chỉ đọc bắt đầu (địa chỉ của slave).
- `length` : độ dài chuỗi dữ liệu cần đọc

*VD*
```cpp
while (modbusRTU.read_Multiple_Data(mbParam.slave[i].ID.toInt(),
                                    (uint16_t *)&mbParam.slave[i].Data,
                                    mbParam.slave[i].ReadAddress.startAddress,
                                    lenght) != true)
    ;
```
**GHI**
```cpp
bool write_Multiple_Data(byte ID, uint16_t *value, long startAddress, size_t length);
```
- `ID` : ID của thiết bị slave cần đọc
- `value` : con trỏ đến vùng nhớ cần để chứa dữ liệu ghi vào slave
- `startAddress` : địa chỉ đọc bắt đầu (địa chỉ của slave).
- `length` : độ dài chuỗi dữ liệu cần đọc

*VD*
```cpp
while (modbusRTU.write_Multiple_Data(mbParam.slave[i].ID.toInt(),
                                    (uint16_t *)&mbParam.slave[i].Data,
                                    mbParam.slave[i].ReadAddress.startAddress,
                                    lenght) != true)
    ;
```
### **2. MODBUS TCP**
Trước khi bắt đầu ta phải thêm class của thư viện
```cpp
ModbusIP mbTCP;
```

#### **a. ETHERNET**

Các DEFINE cần thiết cho `ETHERNET` 
```cpp
#define ETH_ADDR 1
#define ETH_POWER_PIN -1 // Do not use it, it can cause conflict during the software reset.
#define ETH_POWER_PIN_ALTERNATIVE 14
#define ETH_MDC_PIN 23
#define ETH_MDIO_PIN 18
#define ETH_TYPE ETH_PHY_LAN8720
#define ETH_CLK_MODE ETH_CLOCK_GPIO0_IN
#define BUZZ 12
```
Hàm khởi tạo `ETHERNET`
```cpp
void MODBUS_TCP::EthernetInit()
{
    Serial.println("Connect Lan...");
    pinMode(ETH_POWER_PIN_ALTERNATIVE, OUTPUT);
    digitalWrite(ETH_POWER_PIN_ALTERNATIVE, HIGH);
    WiFi.onEvent(WiFiEvent);
    ETH.begin(ETH_ADDR, ETH_POWER_PIN, ETH_MDC_PIN, ETH_MDIO_PIN, ETH_TYPE, ETH_CLK_MODE);
    ETH.config(str2IP(modbusTCP.ethernet.ipAdress),
               str2IP(modbusTCP.ethernet.gateway),
               str2IP(modbusTCP.ethernet.subnet),
               str2IP(modbusTCP.ethernet.primaryDNS));
}
```
- `ipAdress` : Địa chỉ IP đăng kí của thiết bị client
- `gateway` : Địa chỉ của gateway
- `subnet` : Địa chỉ subnet
- `primaryDNS` : Địa chỉ DNS
#### **b. Client Init**
```cpp
void MODBUS_TCP::ClientInit()
{
    mbTCP.client();
}
```
#### **c. Read/Write Multiple Data**
**ĐỌC**
```cpp
bool read_Multiple_Data(IPAddress ID, uint16_t *value, long startAddress, size_t length);
```
- `ID` : ID của thiết bị slave cần đọc
- `value` : con trỏ đến vùng nhớ cần để chứa dữ liệu đọc lên từ slave
- `startAddress` : địa chỉ đọc bắt đầu (địa chỉ của slave).
- `length` : độ dài chuỗi dữ liệu cần đọc

*VD*
```cpp
while (modbusTCP.read_Multiple_Data(modbusTCP.str2IP(mbParam.slave[i].ID),
                                (uint16_t *)&mbParam.slave[i].Data,
                                mbParam.slave[i].ReadAddress.startAddress,
                                lenght) != true)
;
```
**GHI**
```cpp
bool write_Multiple_Data(IPAddress ID, uint16_t *value, long startAddress, size_t length);
```
- `ID` : ID của thiết bị slave cần đọc
- `value` : con trỏ đến vùng nhớ cần để chứa dữ liệu ghi vào slave
- `startAddress` : địa chỉ đọc bắt đầu (địa chỉ của slave).
- `length` : độ dài chuỗi dữ liệu cần đọc

*VD*
```cpp
while (modbusTCP.write_Multiple_Data(modbusTCP.str2IP(mbParam.slave[i].ID),
                                    (uint16_t *)&mbParam.slave[i].Data,
                                    mbParam.slave[i].ReadAddress.startAddress,
                                    lenght) != true)
    ;
```