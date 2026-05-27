**4. Bài thực hành 4: Điều khiển Servo cơ bản sử dụng bộ PCA9685**

**a. Mục tiêu**

- Đáp ứng CĐR 2.1 và 2.2 môn Lập trình nhúng (ĐVT 2.9.7); CĐR 2.1 và 2.2 môn Kiến trúc và giao thức trong IoT (ĐVT 2.10.7); CĐR 2.1 môn Các hệ thống và giải pháp IoT tiên tiến (ĐVT 2.11.7); CĐR 2.1 môn IoT và ứng dụng (ĐCN 2.3.5), cụ thể:

+ Mô phỏng được nguyên lý hoạt động của hệ thống nhúng điển hình sử dụng giao tiếp I2C.

+ Xây dựng được hệ thống nhúng điều khiển servo đa kênh thông qua module PCA9685.

+ Viết được chương trình điều khiển servo, vận dụng giao thức I2C vào xây dựng hệ thống IoT.

+ Nhận biết và gọi tên đúng các thành phần chính của bộ KIT Robot Arm 6DOF (ESP32, PCA9685, Cánh tay Robot).

+ Kết nối đúng bus I2C giữa ESP32 và PCA9685, kết nối đúng 6 servo vào các kênh tương ứng.

+ Biên dịch, nạp firmware trên PlatformIO và kiểm tra hoạt động thông qua Serial Monitor.

+ Sử dụng các lệnh M, G, T, H, I, S, A, W để điều khiển từng khớp servo đến góc mong muốn.

+ Hiểu được giới hạn góc của từng khớp và tác dụng của việc clamp góc bảo vệ cơ cấu.

**b.Các bước tiến hành**

Bài thực hành 4 được triển khai qua ba giai đoạn tiệm tiến. Mỗi giai đoạn xây dựng trên kết quả của giai đoạn trước, giúp người học nắm vững từng lớp kiến thức trước khi tiến lên mức độ phức tạp hơn.

**Bảng 4.1.** Phân bổ nội dung cụ thể trong từng giai đoạn của bài thực hành 4

| **Giai đoạn**                | **Nội dung chính**             | **Kết quả đạt được**                             |
|------------------------------|--------------------------------|--------------------------------------------------|
| GĐ1: Kết nối và kiểm tra I2C | ESP32 + PCA9685 + I2C scan     | Xác nhận giao tiếp I2C, PCA9685 tại địa chỉ 0x40 |
| GĐ2: Điều khiển servo cơ bản | Lệnh M, G, T, H qua Serial     | Điều khiển từng khớp, clamp góc, di chuyển mượt  |
| GĐ3: Điều khiển nâng cao     | Lệnh S, A, W + chuỗi hành động | Thay đổi tốc độ, di chuyển đồng thời 6 khớp      |

**Giai đoạn 1: Kết nối phần cứng và kiểm tra I2C**

Giai đoạn này tập trung vào việc đấu nối phần cứng ESP32 với module PCA9685 và kiểm tra giao tiếp I2C. Đây là bước nền tảng giúp người học làm quen với bus I2C, module mở rộng PWM và cách xác nhận thiết bị hoạt động đúng.

**Bước 1: Khởi tạo dự án**

- Mở VS Code, nhấn biểu tượng PlatformIO → Open → New Project.

- Điền thông tin trong các ô textbox: Name = RobotArm\_GD1, Board = Espressif ESP32 Dev Module, Framework = Arduino. Nhấn Finish. Mở file platformio.ini, cấu hình như sau:

[env:esp32dev]

platform = espressif32

board = esp32dev

framework = arduino

upload\_speed = 115200

monitor\_speed = 115200

upload\_port = COM9 ; doi thanh COM port thuc te

lib\_deps =

adafruit/Adafruit PWM Servo Driver Library

**Bước 2: Xác định modul, linh kiện cần dùng**

- Board ESP32 DevKit V1 (01 chiếc)

- Module PCA9685 — IC điều khiển PWM 16 kênh qua I2C, địa chỉ mặc định 0x40 (01 chiếc)

- Cánh tay Robot 6DOF gồm 6 động cơ Servo (01 bộ)

- Nguồn ngoài **6V / 15A** trở lên (01 bộ) — lưu ý: mỗi servo MG995/MG996 có thể kéo đến 2,5A khi khóa cứng; 6 servo tổng cộng lên đến ~15A, tuyệt đối không dùng nguồn 3A

- Dây nối Dupont cái-cái (04 sợi), cáp Micro-USB (01 sợi)

<!-- image -->

**Hình 2.4.1.** Danh sách các module cần sử dụng trong giai đoạn 1

**Bước 3: Kết nối các modul, linh kiện**

- Thực hiện đấu nối các linh kiện theo bảng kết nối:

+ Nối chân SDA của PCA9685 → GPIO 21 của ESP32 (Dữ liệu I2C).

+ Nối chân SCL của PCA9685 → GPIO 22 của ESP32 (Xung nhịp I2C).

+ Nối chân GND của PCA9685 → chân GND của ESP32 (Mass chung).

+ Nối chân VCC của PCA9685 → chân 3V3 của ESP32 (Nguồn logic cho IC PCA9685).

+ Cắm 6 servo vào kênh 0–5 trên PCA9685 (dây nâu/đen hướng ra ngoài mép board — đây là chân GND).

+ Nối nguồn ngoài **6V/15A** vào domino V+ và GND trên PCA9685 (nguồn riêng cho servo, không dùng nguồn USB ⚠️ dòng đỉnh có thể lên đến ~15A khi cả 6 servo hoạt động đồng thời).

**Bảng 4.2.** Bảng mô tả 6 khớp Servo của cánh tay Robot

| **Khớp**   | **Tên**        | **Chức năng**          | **Góc min**   | **Góc max**   | **Góc Home**   |
|------------|----------------|------------------------|---------------|---------------|----------------|
| J0         | Base (Đế)      | Xoay toàn bộ cánh tay  | 0°            | 180°          | 90°            |
| J1         | Shoulder (Vai) | Nâng/hạ cánh tay trên  | 70°           | 150°          | 70°            |
| J2         | Elbow (Khuỷu)  | Gập/duỗi cánh tay dưới | 0°            | 150°          | 90°            |
| J3         | Wrist Pitch    | Ngửa/úp bàn kẹp        | 0°            | 180°          | 90°            |
| J4         | Wrist Roll     | Xoay bàn kẹp           | 0°            | 180°          | 90°            |
| J5         | Gripper (Kẹp)  | Mở/đóng kẹp            | 60°           | 120°          | 90°            |

- Cơ sở lý thuyết về giao tiếp I2C và module PCA9685:

+ I2C (Inter-Integrated Circuit) là bus truyền thông nối tiếp đồng bộ sử dụng 2 dây: SDA (dữ liệu) và SCL (xung nhịp). ESP32 đóng vai trò Master, PCA9685 là Slave tại địa chỉ 0x40.

+ PCA9685 là IC điều xung PWM 16 kênh, độ phân giải 12 bit (giá trị 0–4095), tần số mặc định 50 Hz (chu kỳ 20 ms) phù hợp với servo RC tiêu chuẩn.

+ Quy đổi góc → xung PWM: servo hoạt động trong dải xung 0,5 ms (0°) đến 2,5 ms (180°). Với tần số 50 Hz (chu kỳ 20 ms) và độ phân giải 12 bit: SERVOMIN = (0,5/20) × 4096 ≈ 102; SERVOMAX = (2,5/20) × 4096 ≈ 512. Công thức chuyển đổi: pulse = map(angle, 0, 180, 102, 512).

<!-- image -->

**Hình 2.4.2.** Sơ đồ kết nối ESP32 với PCA9685 và Robot Arm 6DOF

**Bước 4: Viết chương trình**

***-*** Tạo file main.cpp trong thư mục src/ để quét bus I2C và xác nhận PCA9685 hoạt động:

#include &lt;Wire.h&gt;

void setup() {

Serial.begin(115200);

delay(1000);

Serial.println("=== I2C Scanner ===");

Serial.println("Dang quet bus I2C...");

Wire.begin(21, 22);

}

void loop() {

int device\_count = 0;

for (uint8\_t addr = 1; addr &lt; 127; addr++) {

Wire.beginTransmission(addr);

uint8\_t error = Wire.endTransmission();

if (error == 0) {

Serial.print("I2C device found at 0x");

if (addr &lt; 16) Serial.print("0");

Serial.println(addr, HEX);

device\_count++;

}

}

if (device\_count == 0) {

Serial.println("Khong tim thay thiet bi I2C nao!");

} else {

Serial.print("Tim thay ");

Serial.print(device\_count);

Serial.println(" thiet bi.");

}

Serial.println("--- Quet lai sau 5 giay ---");

delay(5000);

}

**Bước 5: Vận hành và kiểm thử**

Biên dịch chương trình bằng Ctrl+Alt+B và nạp vào ESP32 bằng Ctrl+Alt+U. Mở Serial Monitor (nhấn biểu tượng ổ cắm trên thanh trạng thái, baud rate 115200). Kết quả mong đợi: "=== I2C Scanner ===" → "I2C device found at 0x40" → "Tim thay 1 thiet bi." Chương trình quét lại mỗi 5 giây. Nếu không phát hiện thiết bị, kiểm tra lại dây SDA (GPIO 21) và SCL (GPIO 22), đảm bảo GND chung giữa ESP32 và PCA9685.

**Bước 6: Rút ra nhận xét, kết luận**

- Học viên tự đánh giá nhận xét kết quả đã đạt được dựa theo danh sách bên dưới và giảng viên dùng danh sách này để đánh giá kết quả thực hành:

+ Serial Monitor hiển thị "I2C device found at 0x40" — xác nhận PCA9685 giao tiếp I2C bình thường.

+ Đấu nối đúng 4 dây I2C (SDA, SCL, VCC, GND) và nguồn ngoài cho servo.

+ Giải thích được tại sao servo cần nguồn ngoài riêng và không dùng nguồn USB từ ESP32.

**Câu hỏi tự đánh giá**

1. Bus I2C sử dụng bao nhiêu dây? Vai trò của từng dây là gì?

2. Tại sao PCA9685 có địa chỉ mặc định 0x40? Làm thế nào để thay đổi địa chỉ nếu dùng nhiều module PCA9685?

3. Nếu quét I2C không tìm thấy thiết bị nào, nguyên nhân có thể là gì?

**Giai đoạn 2: Điều khiển servo cơ bản qua Serial**

Giai đoạn này nạp firmware điều khiển Robot Arm hoàn chỉnh và thực hành các lệnh cơ bản qua Serial Monitor. Người học sẽ hiểu cách firmware xử lý lệnh, chuyển đổi góc thành xung PWM và cơ chế clamp góc bảo vệ cơ cấu.

**Bước 1: Khởi tạo dự án**

Mở VS Code, tạo project PlatformIO mới đặt tên RobotArm\_GD2 (hoặc tiếp tục sử dụng project từ Giai đoạn 1). Mở file platformio.ini, cấu hình:

[env:esp32dev]

platform = espressif32

board = esp32dev

framework = arduino

upload\_speed = 115200

monitor\_speed = 115200

upload\_port = COM9 ; doi thanh COM port thuc te

lib\_deps =

adafruit/Adafruit PWM Servo Driver Library

**Bước 2: Xác định modul, linh kiện cần dùng**

Sử dụng toàn bộ linh kiện từ Giai đoạn 1.

**Bước 3: Kết nối các modul, linh kiện**

Giữ nguyên kết nối phần cứng từ Giai đoạn 1.

**Bước 4: Viết chương trình**

> **Ghi chú về cấu trúc mã nguồn:** Tài liệu thực hành này trình bày code theo dạng **đơn file** (single-file) để sinh viên dễ đọc và tự đánh máy. Mã nguồn mẫu đầy đủ trong repository GitHub được tổ chức theo **kiến trúc module** (5 file riêng biệt: config.h, pca9685\_driver.h/cpp, servo\_ctrl.h/cpp, cmd\_parser.h/cpp, main.cpp) — đây là cấu trúc chuẩn trong thực tế. Hai cách tổ chức này có chức năng **hoàn toàn tương đương nhau**.

- Cấu trúc module đầy đủ (tham khảo trong mã nguồn mẫu trên GitHub) bao gồm:

+ config.h → src/config.h (khai báo cấu hình các khớp: góc min, max, home, tốc độ, tham số PWM)

+ pca9685\_driver.h và pca9685\_driver.cpp → src/ (module giao tiếp I2C với PCA9685, chuyển đổi góc → xung PWM)

+ servo\_ctrl.h và servo\_ctrl.cpp → src/ (module quản lý 6 servo: clamp góc, di chuyển mượt, trạng thái)

+ cmd\_parser.h và cmd\_parser.cpp → src/ (module phân tích lệnh Serial: M, G, T, H, I)

+ main.cpp → src/main.cpp (file chính: khởi tạo và vòng lặp đọc lệnh Serial)

- Cấu trúc firmware hoạt động như sau: khi ESP32 khởi động, hàm setup() gọi Wire.begin(SDA\_PIN, SCL\_PIN) để khởi tạo I2C (SDA\_PIN = 21, SCL\_PIN = 22), sau đó thiết lập tần số PWM 50 Hz cho PCA9685, và đưa 6 servo về vị trí Home. Hàm loop() đọc lệnh từ Serial, phân tích và gọi hàm điều khiển servo tương ứng.

- Trong bài thực hành này, sinh viên tạo **một file duy nhất** src/main.cpp với toàn bộ nội dung bên dưới (tổng hợp từ kiến trúc module thành 1 file đơn):

#include &lt;Wire.h&gt;

#include &lt;Adafruit\_PWMServoDriver.h&gt;

// === GIAI DOAN 2: DIEU KHIEN SERVO CO BAN QUA SERIAL ===

// ---- Hang so PWM ----

// Servo hoat dong o tan so 50Hz (chu ky 20ms)

// Xung 0.5ms tuong ung 0 do: (0.5/20)*4096 = 102

// Xung 2.5ms tuong ung 180 do: (2.5/20)*4096 = 512

#define SERVOMIN 102

#define SERVOMAX 512

#define PWM\_FREQ 50

#define NUM\_JOINTS 6

// ---- Cau hinh tung khop robot ----

// minAngle, maxAngle: gioi han co hoc cua khop

// homeAngle: vi tri mac dinh an toan

struct JointConfig {

const char* name;

int channel;

int minAngle;

int maxAngle;

int homeAngle;

};

JointConfig joints[NUM\_JOINTS] = {

{"Base",         0, 0,   180, 90},   // J0: Xoay toan bo canh tay

{"Shoulder",     1, 70,  150, 70},   // J1: Nang/ha canh tay tren

{"Elbow",        2, 0,   150, 90},   // J2: Gap/duoi canh tay duoi

{"Wrist Pitch",  3, 0,   180, 90},   // J3: Ngua/up ban kep

{"Wrist Roll",   4, 0,   180, 90},   // J4: Xoay ban kep

{"Gripper",      5, 60,  120, 90}    // J5: Mo/dong kep

};

// ---- Bien trang thai ----

int currentAngles[NUM\_JOINTS];   // Goc hien tai cua 6 khop

int jointSpeed = 5;              // Toc do mac dinh (1-10)

// ---- Doi tuong PCA9685 ----

Adafruit\_PWMServoDriver pca = Adafruit\_PWMServoDriver();

// ------------------------------------------

// Ham: angleToPulse

// Chuyen doi goc (0-180) sang gia tri xung PWM 12 bit

// Su dung ham map() de noi suy tuyen tinh

// ------------------------------------------

int angleToPulse(int angle) {

return map(angle, 0, 180, SERVOMIN, SERVOMAX);

}

// ------------------------------------------

// Ham: clampAngle

// Gioi han goc trong pham vi [min, max] cua khop

// Bao ve co cau co khi, tranh va cham va hu hong

// ------------------------------------------

int clampAngle(int joint, int angle) {

if (angle &lt; joints[joint].minAngle)

return joints[joint].minAngle;

if (angle &gt; joints[joint].maxAngle)

return joints[joint].maxAngle;

return angle;

}

// ------------------------------------------

// Ham: moveServo

// Di chuyen 1 khop den goc dich voi hieu ung muot

// Thay vi nhay truc tiep, chia thanh nhieu buoc 1 do

// Moi buoc delay ty le nghich voi toc do

// ------------------------------------------

void moveServo(int joint, int targetAngle) {

targetAngle = clampAngle(joint, targetAngle);

int current = currentAngles[joint];

int step = (targetAngle &gt; current) ? 1 : -1;

int delayMs = 20 / jointSpeed;  // Toc do 1-&gt;chậm, 10-&gt;nhanh

if (delayMs &lt; 1) delayMs = 1;

while (current != targetAngle) {

current += step;

pca.setPWM(joints[joint].channel, 0, angleToPulse(current));

delay(delayMs);

}

currentAngles[joint] = targetAngle;

}

// ------------------------------------------

// Ham: homeAll

// Dua tat ca 6 khop ve vi tri Home

// ------------------------------------------

void homeAll() {

for (int i = 0; i &lt; NUM\_JOINTS; i++) {

moveServo(i, joints[i].homeAngle);

}

}

// ------------------------------------------

// Ham: homeOne

// Dua 1 khop ve vi tri Home

// ------------------------------------------

void homeOne(int joint) {

moveServo(joint, joints[joint].homeAngle);

}

// ------------------------------------------

// Ham: printStatus

// In trang thai goc hien tai cua 6 khop

// Dinh dang: STA:a0,a1,a2,a3,a4,a5

// ------------------------------------------

void printStatus() {

Serial.print("STA:");

for (int i = 0; i &lt; NUM\_JOINTS; i++) {

Serial.print(currentAngles[i]);

if (i &lt; NUM\_JOINTS - 1) Serial.print(",");

}

Serial.println();

}

// ------------------------------------------

// Ham: printInfo

// In thong tin cau hinh tung khop

// ------------------------------------------

void printInfo() {

for (int i = 0; i &lt; NUM\_JOINTS; i++) {

Serial.print("J"); Serial.print(i);

Serial.print(" "); Serial.print(joints[i].name);

Serial.print(" | min="); Serial.print(joints[i].minAngle);

Serial.print(" max="); Serial.print(joints[i].maxAngle);

Serial.print(" home="); Serial.print(joints[i].homeAngle);

Serial.print(" cur="); Serial.println(currentAngles[i]);

}

}

// ------------------------------------------

// Ham: parseCommand

// Phan tich lenh tu Serial Monitor va thuc thi

// Cac lenh ho tro: M, G, T, H, I

// ------------------------------------------

void parseCommand(String input) {

input.trim();

if (input.length() == 0) return;

char cmd = input.charAt(0);  // Ky tu dau tien la lenh

if (cmd == 'M' || cmd == 'm') {

// Lenh M &lt;id&gt; &lt;angle&gt;: di chuyen 1 khop

int sp1 = input.indexOf(' ');

int sp2 = input.indexOf(' ', sp1 + 1);

if (sp1 &lt; 0 || sp2 &lt; 0) {

Serial.println("ERR:MISSING\_PARAM");

return;

}

int id = input.substring(sp1 + 1, sp2).toInt();

int angle = input.substring(sp2 + 1).toInt();

if (id &lt; 0 || id &gt;= NUM\_JOINTS) {

Serial.println("ERR:INVALID\_JOINT");

return;

}

moveServo(id, angle);

Serial.println("OK");

} else if (cmd == 'G' || cmd == 'g') {

// Lenh G &lt;id&gt;: doc goc hien tai

int sp1 = input.indexOf(' ');

if (sp1 &lt; 0) {

Serial.println("ERR:MISSING\_PARAM");

return;

}

int id = input.substring(sp1 + 1).toInt();

if (id &lt; 0 || id &gt;= NUM\_JOINTS) {

Serial.println("ERR:INVALID\_JOINT");

return;

}

Serial.print("VAL:");

Serial.print(id);

Serial.print(":");

Serial.println(currentAngles[id]);

} else if (cmd == 'T' || cmd == 't') {

// Lenh T: in trang thai toan bo

printStatus();

} else if (cmd == 'H' || cmd == 'h') {

// Lenh H [id]: ve Home (1 khop hoac tat ca)

int sp1 = input.indexOf(' ');

if (sp1 &lt; 0) {

homeAll();

} else {

int id = input.substring(sp1 + 1).toInt();

if (id &lt; 0 || id &gt;= NUM\_JOINTS) {

Serial.println("ERR:INVALID\_JOINT");

return;

}

homeOne(id);

}

Serial.println("OK");

} else if (cmd == 'I' || cmd == 'i') {

// Lenh I: in thong tin cau hinh

printInfo();

} else {

Serial.println("ERR:UNKNOWN\_CMD");

}

}

// ---- Setup ----

void setup() {

Serial.begin(115200);

Wire.begin(21, 22);  // SDA=21, SCL=22

// Khoi tao PCA9685

pca.begin();

pca.setPWMFreq(PWM\_FREQ);  // 50Hz cho servo

delay(10);

Serial.println("ARM INIT");

// Dua 6 servo ve vi tri Home

for (int i = 0; i &lt; NUM\_JOINTS; i++) {

currentAngles[i] = joints[i].homeAngle;

pca.setPWM(joints[i].channel, 0,

angleToPulse(joints[i].homeAngle));

delay(100);  // Cho tung servo on dinh

}

Serial.println("ARM READY");

Serial.println("Nhap lenh: M G T H I");

}

// ---- Loop ----

void loop() {

// Doc lenh tu Serial Monitor

if (Serial.available()) {

String input = Serial.readStringUntil('\n');

input.trim();

if (input.length() &gt; 0) {

parseCommand(input);

}

}

}

- Cơ chế di chuyển mượt (smooth move): thay vì nhảy trực tiếp từ góc hiện tại đến góc đích, firmware chia quãng đường thành nhiều bước nhỏ, mỗi bước di chuyển 1° và chờ một khoảng delay tỷ lệ nghịch với tốc độ đã cài đặt. Điều này giúp servo chuyển động êm ái và giảm tải cơ học lên các khớp.

- Cơ chế clamp góc: trước khi gửi xung PWM, firmware kiểm tra góc yêu cầu có nằm trong phạm vi [min, max] của khớp đó hay không. Nếu vượt quá, góc sẽ bị giới hạn (clamp) về giá trị biên gần nhất. Đây là cơ chế bảo vệ cơ cấu cơ khí, tránh va chạm và hư hỏng.

**Bước 5: Vận hành và kiểm thử**

- Biên dịch và nạp firmware (Ctrl+Alt+U). Mở Serial Monitor (115200 baud), nhấn nút Reset trên ESP32. Kết quả mong đợi: "ARM INIT" rồi "ARM READY". Tiến hành kiểm thử các lệnh cơ bản:

+ Gõ G 0 → mong đợi phản hồi VAL:0:90 (khớp J0 đang ở 90°).

+ Gõ M 0 45 → đế robot xoay sang phải, chuyển động chậm và mượt. Serial phản hồi OK.

+ Gõ M 0 135 → đế robot xoay sang trái.

+ Gõ H 0 → khớp J0 về Home (90°).

- Thực hành tương tự với các khớp J1–J5.

+ Gõ M 5 60 (mở kẹp), M 5 120 (đóng kẹp), M 5 90 (vừa phải).

+ Gõ T → xem trạng thái toàn bộ 6 khớp, mong đợi STA:90,70,90,90,90,90 (nếu đã về Home).

+ Gõ H → đưa tất cả khớp về Home.

+ Gõ I → xem thông tin cấu hình từng khớp (góc min, max, Home, tốc độ).

**Bước 6: Rút ra nhận xét, kết luận**

- Học viên tự đánh giá nhận xét kết quả đã đạt được dựa theo danh sách bên dưới và giảng viên dùng danh sách này để đánh giá kết quả thực hành:

+ Firmware biên dịch thành công trong PlatformIO, tất cả thư viện trong lib\_deps được cài đặt tự động không có lỗi.

+ Serial Monitor hiển thị "ARM INIT" → "ARM READY" sau khi nhấn Reset.

+ Lệnh M 0 45 làm đế robot xoay đúng hướng, chuyển động mượt (không nhảy đột ngột).

+ Lệnh G 0 trả về đúng giá trị góc hiện tại sau khi di chuyển.

+ Lệnh T trả về đúng trạng thái 6 khớp dưới dạng STA:a0,a1,a2,a3,a4,a5.

+ Khi gõ M 1 200 (vượt max 150°), servo chỉ di chuyển đến 150° — xác nhận cơ chế clamp hoạt động.

+ Giải thích được tại sao firmware dùng giá trị SERVOMIN = 102 và SERVOMAX = 512 để chuyển đổi góc sang xung PWM.

**Câu hỏi tự đánh giá**

1. Tại sao servo cần tần số PWM 50 Hz? Nếu đặt tần số 100 Hz, servo có hoạt động đúng không?

2. Giải thích công thức pulse = map(angle, 0, 180, 102, 512). Giá trị 102 và 512 có ý nghĩa gì?

3. Nếu không có cơ chế clamp góc, điều gì có thể xảy ra khi gửi lệnh M 1 200 cho khớp Shoulder?

**Giai đoạn 3: Điều khiển nâng cao — tốc độ, đồng thời và chuỗi hành động**

Giai đoạn này mở rộng khả năng điều khiển với lệnh thay đổi tốc độ (S), di chuyển đồng thời 6 khớp (A) và cơ chế chờ hoàn thành (W). Người học sẽ lập trình chuỗi hành động tự động gắp — di chuyển — thả vật thể.

**Bước 1: Khởi tạo dự án**

Tiếp tục sử dụng project từ Giai đoạn 2 (firmware đã hỗ trợ sẵn các lệnh nâng cao).

**Bước 2: Xác định modul, linh kiện cần dùng**

Sử dụng toàn bộ linh kiện từ Giai đoạn 2.

**Bước 3: Kết nối các modul, linh kiện**

Giữ nguyên kết nối phần cứng từ Giai đoạn 2.

**Bước 4: Viết chương trình**

> **Ghi chú:** Giai đoạn 3 trình bày dạng **đơn file** tổng hợp, mã nguồn mẫu GitHub dùng kiến trúc module (xem Giai đoạn 2). Hai cách có chức năng tương đương nhau.

***-*** Mã nguồn giai đoạn 3 mở rộng từ giai đoạn 2, bổ sung 3 lệnh mới: S (tốc độ), A (di chuyển đồng thời 6 khớp), W (chờ hoàn thành). Sinh viên thay thế nội dung file src/main.cpp bằng nội dung bên dưới:

#include &lt;Wire.h&gt;

#include &lt;Adafruit\_PWMServoDriver.h&gt;

// === GIAI DOAN 3: DIEU KHIEN NANG CAO ===

// Mo rong tu GD2: them lenh S (toc do), A (di chuyen dong thoi),

// W (cho hoan thanh)

// ---- Hang so PWM ----

#define SERVOMIN 102

#define SERVOMAX 512

#define PWM\_FREQ 50

#define NUM\_JOINTS 6

// ---- Cau hinh tung khop robot ----

struct JointConfig {

const char* name;

int channel;

int minAngle;

int maxAngle;

int homeAngle;

};

JointConfig joints[NUM\_JOINTS] = {

{"Base",         0, 0,   180, 90},

{"Shoulder",     1, 70,  150, 70},

{"Elbow",        2, 0,   150, 90},

{"Wrist Pitch",  3, 0,   180, 90},

{"Wrist Roll",   4, 0,   180, 90},

{"Gripper",      5, 60,  120, 90}

};

// ---- Bien trang thai ----

int currentAngles[NUM\_JOINTS];

int jointSpeed[NUM\_JOINTS];  // Toc do rieng tung khop (1-10)

bool isMoving = false;       // Co di chuyen

Adafruit\_PWMServoDriver pca = Adafruit\_PWMServoDriver();

int angleToPulse(int angle) {

return map(angle, 0, 180, SERVOMIN, SERVOMAX);

}

int clampAngle(int joint, int angle) {

if (angle &lt; joints[joint].minAngle)

return joints[joint].minAngle;

if (angle &gt; joints[joint].maxAngle)

return joints[joint].maxAngle;

return angle;

}

// ------------------------------------------

// Ham: moveServo (1 khop, muot)

// ------------------------------------------

void moveServo(int joint, int targetAngle) {

isMoving = true;

targetAngle = clampAngle(joint, targetAngle);

int current = currentAngles[joint];

int step = (targetAngle &gt; current) ? 1 : -1;

int delayMs = 20 / jointSpeed[joint];

if (delayMs &lt; 1) delayMs = 1;

while (current != targetAngle) {

current += step;

pca.setPWM(joints[joint].channel, 0, angleToPulse(current));

delay(delayMs);

}

currentAngles[joint] = targetAngle;

isMoving = false;

}

// ------------------------------------------

// Ham: moveAll

// Di chuyen DONG THOI 6 khop den goc dich

// Thuat toan: tim so buoc lon nhat, lap tung buoc,

// moi buoc tinh goc trung gian cho tung khop

// bang noi suy tuyen tinh

// ------------------------------------------

void moveAll(int targets[NUM\_JOINTS]) {

isMoving = true;

// Clamp va tinh khoang cach

int startAngles[NUM\_JOINTS];

int maxSteps = 0;

for (int i = 0; i &lt; NUM\_JOINTS; i++) {

targets[i] = clampAngle(i, targets[i]);

startAngles[i] = currentAngles[i];

int diff = abs(targets[i] - startAngles[i]);

if (diff &gt; maxSteps) maxSteps = diff;

}

if (maxSteps == 0) {

isMoving = false;

return;

}

// Di chuyen dong thoi theo tung buoc

for (int step = 1; step &lt;= maxSteps; step++) {

for (int i = 0; i &lt; NUM\_JOINTS; i++) {

// Noi suy tuyen tinh: goc = start + (target-start)*step/maxSteps

int angle = startAngles[i] +

(long)(targets[i] - startAngles[i]) * step / maxSteps;

pca.setPWM(joints[i].channel, 0, angleToPulse(angle));

currentAngles[i] = angle;

}

delay(15);  // 15ms moi buoc ~ 60 buoc/giay

}

// Dam bao goc cuoi chinh xac

for (int i = 0; i &lt; NUM\_JOINTS; i++) {

currentAngles[i] = targets[i];

pca.setPWM(joints[i].channel, 0,

angleToPulse(targets[i]));

}

isMoving = false;

}

void homeAll() {

int targets[NUM\_JOINTS];

for (int i = 0; i &lt; NUM\_JOINTS; i++)

targets[i] = joints[i].homeAngle;

moveAll(targets);

}

void homeOne(int joint) {

moveServo(joint, joints[joint].homeAngle);

}

void printStatus() {

Serial.print("STA:");

for (int i = 0; i &lt; NUM\_JOINTS; i++) {

Serial.print(currentAngles[i]);

if (i &lt; NUM\_JOINTS - 1) Serial.print(",");

}

Serial.println();

}

void printInfo() {

for (int i = 0; i &lt; NUM\_JOINTS; i++) {

Serial.print("J"); Serial.print(i);

Serial.print(" "); Serial.print(joints[i].name);

Serial.print(" | min="); Serial.print(joints[i].minAngle);

Serial.print(" max="); Serial.print(joints[i].maxAngle);

Serial.print(" home="); Serial.print(joints[i].homeAngle);

Serial.print(" spd="); Serial.print(jointSpeed[i]);

Serial.print(" cur="); Serial.println(currentAngles[i]);

}

}

// ------------------------------------------

// Ham: parseCommand

// Mo rong tu GD2: them lenh S, A, W

// ------------------------------------------

void parseCommand(String input) {

input.trim();

if (input.length() == 0) return;

char cmd = input.charAt(0);

if (cmd == 'M' || cmd == 'm') {

int sp1 = input.indexOf(' ');

int sp2 = input.indexOf(' ', sp1 + 1);

if (sp1 &lt; 0 || sp2 &lt; 0) {

Serial.println("ERR:MISSING\_PARAM"); return;

}

int id = input.substring(sp1+1, sp2).toInt();

int angle = input.substring(sp2+1).toInt();

if (id &lt; 0 || id &gt;= NUM\_JOINTS) {

Serial.println("ERR:INVALID\_JOINT"); return;

}

moveServo(id, angle);

Serial.println("OK");

} else if (cmd == 'G' || cmd == 'g') {

int sp1 = input.indexOf(' ');

if (sp1 &lt; 0) {

Serial.println("ERR:MISSING\_PARAM"); return;

}

int id = input.substring(sp1+1).toInt();

if (id &lt; 0 || id &gt;= NUM\_JOINTS) {

Serial.println("ERR:INVALID\_JOINT"); return;

}

Serial.print("VAL:"); Serial.print(id);

Serial.print(":"); Serial.println(currentAngles[id]);

} else if (cmd == 'T' || cmd == 't') {

printStatus();

} else if (cmd == 'H' || cmd == 'h') {

int sp1 = input.indexOf(' ');

if (sp1 &lt; 0) {

homeAll();

} else {

int id = input.substring(sp1+1).toInt();

if (id &lt; 0 || id &gt;= NUM\_JOINTS) {

Serial.println("ERR:INVALID\_JOINT"); return;

}

homeOne(id);

}

Serial.println("OK");

} else if (cmd == 'I' || cmd == 'i') {

printInfo();

} else if (cmd == 'S' || cmd == 's') {

// Lenh S &lt;id&gt; &lt;speed&gt;: thay doi toc do 1 khop

// Toc do tu 1 (cham nhat) den 10 (nhanh nhat)

int sp1 = input.indexOf(' ');

int sp2 = input.indexOf(' ', sp1 + 1);

if (sp1 &lt; 0 || sp2 &lt; 0) {

Serial.println("ERR:MISSING\_PARAM"); return;

}

int id = input.substring(sp1+1, sp2).toInt();

int spd = input.substring(sp2+1).toInt();

if (id &lt; 0 || id &gt;= NUM\_JOINTS) {

Serial.println("ERR:INVALID\_JOINT"); return;

}

// Clamp toc do trong khoang 1-10

if (spd &lt; 1) spd = 1;

if (spd &gt; 10) spd = 10;

jointSpeed[id] = spd;

Serial.println("OK");

} else if (cmd == 'A' || cmd == 'a') {

// Lenh A &lt;a0&gt; &lt;a1&gt; &lt;a2&gt; &lt;a3&gt; &lt;a4&gt; &lt;a5&gt;

// Di chuyen DONG THOI 6 khop

int targets[NUM\_JOINTS];

int pos = 1;  // Bo qua ky tu 'A'

for (int i = 0; i &lt; NUM\_JOINTS; i++) {

int sp = input.indexOf(' ', pos);

if (sp &lt; 0 &amp;&amp; i &lt; NUM\_JOINTS - 1) {

Serial.println("ERR:NEED\_6\_ANGLES"); return;

}

if (i &lt; NUM\_JOINTS - 1) {

targets[i] = input.substring(sp+1,

input.indexOf(' ', sp+1)).toInt();

pos = sp + 1;

} else {

targets[i] = input.substring(

input.lastIndexOf(' ')+1).toInt();

}

}

moveAll(targets);

Serial.println("OK");

} else if (cmd == 'W' || cmd == 'w') {

// Lenh W: cho den khi khong con di chuyen

// Trong firmware nay, cac ham move da chay dong bo

// nen W chi can phan hoi DONE

while (isMoving) { delay(10); }

Serial.println("DONE");

} else {

Serial.println("ERR:UNKNOWN\_CMD");

}

}

void setup() {

Serial.begin(115200);

Wire.begin(21, 22);

pca.begin();

pca.setPWMFreq(PWM\_FREQ);

delay(10);

Serial.println("ARM INIT");

// Khoi tao toc do mac dinh va vi tri Home

for (int i = 0; i &lt; NUM\_JOINTS; i++) {

jointSpeed[i] = 5;

currentAngles[i] = joints[i].homeAngle;

pca.setPWM(joints[i].channel, 0,

angleToPulse(joints[i].homeAngle));

delay(100);

}

Serial.println("ARM READY");

Serial.println("Nhap lenh: M G T H I S A W");

}

void loop() {

if (Serial.available()) {

String input = Serial.readStringUntil('\n');

input.trim();

if (input.length() &gt; 0) {

parseCommand(input);

}

}

}

- Firmware đã hỗ trợ sẵn các lệnh nâng cao. Các lệnh điều khiển bổ sung:

+ Lệnh S (Speed): S &lt;id&gt; &lt;tốc\_độ&gt; — thay đổi tốc độ di chuyển của khớp. Tốc độ từ 1 (chậm nhất) đến 10 (nhanh nhất). Mặc định: 5. Ví dụ: S 0 3 (khớp J0 chạy chậm hơn), S 0 8 (khớp J0 chạy nhanh hơn).

+ Lệnh A (All): A &lt;a0&gt; &lt;a1&gt; &lt;a2&gt; &lt;a3&gt; &lt;a4&gt; &lt;a5&gt; — di chuyển đồng thời 6 khớp đến các góc chỉ định. Ví dụ: A 90 120 130 90 90 60.

+ Lệnh W (Wait): W — chờ cho đến khi tất cả servo hoàn thành di chuyển. Phản hồi DONE khi xong.

- Thực hành chuỗi hành động gắp vật thể qua Serial Monitor:

+ Gõ lần lượt các lệnh sau (mỗi lệnh gõ xong nhấn Enter, chờ phản hồi rồi gõ lệnh tiếp):

H (Về Home)

W (Chờ hoàn thành)

S 0 3 (Giảm tốc độ J0 cho êm)

M 5 60 (Mở kẹp)

W

A 90 120 130 90 90 60 (Tiếp cận vị trí A)

W

A 90 140 140 90 90 60 (Hạ xuống lấy vật)

W

M 5 110 (Gắp vật)

W

A 90 100 90 90 90 110 (Nâng lên)

W

M 0 45 (Xoay đến vị trí B)

W

A 45 130 130 90 90 110 (Hạ xuống thả vật)

W

M 5 60 (Thả vật)

W

H (Về Home)

**Bước 5: Vận hành và kiểm thử**

- Thực hiện kiểm thử theo quy trình:

+ Gõ S 0 1, sau đó M 0 45 — quan sát đế xoay rất chậm. Gõ S 0 8, sau đó M 0 135 — quan sát đế xoay nhanh hơn rõ rệt.

+ Gõ A 45 100 120 90 90 60 → quan sát cả 6 khớp di chuyển đồng thời đến vị trí đích.

+ Gõ W → chờ cho đến khi nhận phản hồi DONE.

+ Thực hiện chuỗi hành động gắp vật thể đã mô tả ở Bước 4. Quan sát toàn bộ quá trình robot thực hiện: tiếp cận → hạ → gắp → nâng → xoay → hạ → thả → về Home.

+ Thử gõ lệnh sai: M 9 90 (khớp không tồn tại), M (thiếu tham số), A 90 90 (thiếu tham số) — quan sát phản hồi ERR.

**Bước 6: Rút ra nhận xét, kết luận**

- Học viên tự đánh giá nhận xét kết quả đã đạt được dựa theo danh sách bên dưới và giảng viên dùng danh sách này để đánh giá kết quả thực hành:

+ Lệnh S thay đổi tốc độ rõ rệt: S 0 1 chậm, S 0 8 nhanh.

+ Lệnh A di chuyển đồng thời 6 khớp, robot đạt đúng tư thế yêu cầu.

+ Lệnh W chờ đúng thời điểm hoàn thành, phản hồi DONE.

+ Chuỗi hành động gắp vật thể thực hiện đúng trình tự: tiếp cận → gắp → nâng → xoay → thả → Home.

+ Lệnh sai trả về ERR kèm mô tả lỗi, firmware không bị treo.

+ Giải thích được sự khác biệt giữa lệnh M (di chuyển 1 khớp) và lệnh A (di chuyển đồng thời 6 khớp).

**Câu hỏi tự đánh giá**

1.Tại sao cần lệnh W (Wait) giữa các lệnh? Nếu bỏ W, điều gì xảy ra khi gửi liên tiếp M 0 45 rồi M 0 135?

2.Lệnh A di chuyển 6 khớp đồng thời hay tuần tự? Giải thích cơ chế hoạt động bên trong firmware.

3.Trong chuỗi hành động gắp vật, tại sao phải hạ cánh tay trước rồi mới đóng kẹp, thay vì đóng kẹp trước rồi mới hạ?

**Bài tập mở rộng**

1.Thử di chuyển từng khớp J0–J5 đến góc min và max, mô tả chuyển động thực tế của từng khớp.

2.Thay đổi tốc độ từ 1 đến 10, đo thời gian di chuyển từ 0° đến 180° cho khớp J0, vẽ đồ thị thời gian theo tốc độ.

3.Thiết kế chuỗi hành động mới: robot gắp vật từ vị trí A, xếp chồng lên vật tại vị trí B.

4.Tìm hiểu datasheet PCA9685, giải thích cách IC tạo xung PWM 12 bit ở tần số 50 Hz.

**5. Bài thực hành 5: Điều khiển Servo thông qua hệ thống GUI**

**a. Mục tiêu**

- Đáp ứng CĐR 2.2 môn Lập trình nhúng (ĐVT 2.9.7); CĐR 2.2 môn Kiến trúc và giao thức trong IoT (ĐVT 2.10.7); CĐR 2.1 môn Các hệ thống và giải pháp IoT tiên tiến (ĐVT 2.11.7); CĐR 2.1 môn IoT và ứng dụng (ĐCN 2.3.5), cụ thể:

+ Xây dựng được hệ thống nhúng kết hợp phần mềm điều khiển trên máy tính, hoàn chỉnh từ firmware đến giao diện người dùng.

+ Lập trình được ứng dụng giao tiếp Serial giữa Python và ESP32 trong dự án IoT.

+ Hiểu nguyên lý giao tiếp Serial giữa Python trên máy tính và firmware trên ESP32.

+ Cài đặt thành công Python 3.8 trở lên, môi trường ảo (venv) và các thư viện cần thiết.

+ Viết script Python kết nối Serial, gửi lệnh điều khiển robot, nhận và hiển thị phản hồi.

+ Viết script Python tự động gửi chuỗi lệnh gắp — di chuyển — thả vật thể.

+ Xây dựng giao diện GUI chuyên nghiệp với thanh trượt cho 6 khớp, đồng bộ trạng thái từ phần cứng.

**b. Các bước tiến hành**

- Bài thực hành 5 được triển khai qua ba giai đoạn tiệm tiến.

**Bảng 5.1.** Phân bổ nội dung cụ thể trong từng giai đoạn của bài thực hành 5

| **Giai đoạn**                 | **Nội dung chính**                  | **Kết quả đạt được**                         |
|-------------------------------|-------------------------------------|----------------------------------------------|
| GĐ1: Kết nối Serial cơ bản    | Python + pyserial + script đơn giản | Gửi lệnh, nhận phản hồi, điều khiển robot    |
| GĐ2: Chuỗi hành động tự động  | Script tự động gắp-di chuyển-thả    | Lập trình chuỗi hành động pick-and-place     |
| GĐ3: Giao diện GUI hoàn chỉnh | Tkinter GUI + đồng bộ trạng thái    | GUI chuyên nghiệp điều khiển robot trực quan |

**Giai đoạn 1: Kết nối Serial cơ bản với Python**

Giai đoạn này tập trung vào việc cài đặt Python, thư viện pyserial và viết script đơn giản gửi lệnh điều khiển robot qua Serial. Đây là bước nền tảng giúp người học hiểu cơ chế giao tiếp giữa máy tính và vi điều khiển.

**Bước 1: Khởi tạo dự án**

- Kiểm tra Python đã cài chưa: mở Terminal (VS Code: Ctrl+`) hoặc CMD, gõ python --version. Nếu chưa có, tải tại [https://www.python.org/downloads/](https://www.python.org/downloads/) (nhớ tích "Add Python to PATH" khi cài đặt).

- Tạo thư mục dự án và cài đặt môi trường ảo:

*mkdir robot\_python*

*cd robot\_python*

*python -m venv .venv*

- Kích hoạt môi trường ảo:

+ Windows: .venv\\Scripts\\activate;

+ macOS/Linux: source .venv/bin/activate;

- Cài thư viện pyserial: pip install pyserial.

**Bước 2: Xác định modul, linh kiện cần dùng**

Sử dụng hệ thống phần cứng đã kết nối từ Bài thực hành 4 (ESP32 + PCA9685 + Robot Arm 6DOF). Đảm bảo firmware Giai đoạn 3 của Bài 4 đã được nạp và ESP32 đang kết nối với máy tính qua cáp USB.

**Bước 3: Kết nối các modul, linh kiện**

Giữ nguyên kết nối phần cứng từ Bài thực hành 4. Lưu ý: đóng Serial Monitor trong VS Code trước khi chạy script Python, vì cổng COM chỉ cho phép một ứng dụng truy cập tại một thời điểm.

**Bước 4: Viết chương trình**

**-** Tạo file robot\_basic.py trong thư mục robot\_python:

import serial

import time

PORT = 'COM9' # Thay COM port thuc te cua ESP32

BAUD = 115200

robot = serial.Serial(PORT, BAUD, timeout=2)

time.sleep(2)

print(f"Da ket noi voi {PORT} @ {BAUD} baud")

def send(cmd):

robot.write((cmd + '\n').encode())

response = robot.readline().decode().strip()

print(f' &gt;&gt; {cmd:25s} | &lt;&lt; {response}')

return response

print("\n--- BAT DAU DEMO ---")

send('H')

send('W')

send('M 0 45')

send('W')

send('T')

send('M 0 135')

send('W')

send('H')

send('W')

robot.close()

print("\n--- KET THUC DEMO ---")

print('Done.')

- Cơ sở lý thuyết về giao tiếp Serial: Python sử dụng thư viện pyserial để mở cổng COM, gửi chuỗi lệnh dưới dạng byte (encode UTF-8) và đọc phản hồi. Hàm time.sleep(2) cần thiết vì khi mở cổng Serial, ESP32 tự động reset và cần khoảng 2 giây để khởi động firmware.

**Bước 5: Vận hành và kiểm thử**

- Đóng Serial Monitor trong VS Code (nếu đang mở).

- Mở Terminal, kích hoạt môi trường ảo, chạy: python robot\_basic.py

- Quan sát kết quả in ra Terminal: mỗi lệnh hiển thị phản hồi từ firmware (OK, DONE, STA:...).

- Quan sát robot: đế xoay sang phải (45°), rồi sang trái (135°), rồi về Home.

- Thử thay đổi giá trị góc trong script, chạy lại và quan sát.

**Bước 6: Rút ra nhận xét, kết luận**

- Người học tự đánh giá nhận xét kết quả đã đạt được dựa theo danh sách bên dưới và giảng viên dùng danh sách này để đánh giá kết quả thực hành:

+ Script Python kết nối Serial thành công, không báo lỗi COM port.

+ Mỗi lệnh gửi đi đều nhận được phản hồi đúng từ firmware.

+ Robot di chuyển đúng theo lệnh: J0 xoay 45° → 135° → Home.

+ Giải thích được tại sao cần time.sleep(2) sau khi mở cổng Serial.

**Câu hỏi tự đánh giá**

1. Tại sao phải đóng Serial Monitor trước khi chạy script Python? Điều gì xảy ra nếu cả hai cùng truy cập cổng COM?

2. Hàm robot.readline() đọc dữ liệu cho đến ký tự nào? Nếu firmware không gửi ký tự kết thúc dòng, hàm sẽ hoạt động thế nào?

3. Tại sao lệnh gửi đi cần thêm '\\n' ở cuối?

**Giai đoạn 2: Chuỗi hành động tự động pick-and-place**

Giai đoạn này lập trình chuỗi hành động tự động để robot gắp vật thể từ vị trí A, di chuyển và thả tại vị trí B.

**Bước 1: Khởi tạo dự án**

Tiếp tục sử dụng thư mục robot\_python và môi trường ảo từ Giai đoạn 1.

**Bước 2: Xác định modul, linh kiện cần dùng**

Sử dụng hệ thống phần cứng từ Giai đoạn 1.

**Bước 3: Kết nối các modul, linh kiện**

Giữ nguyên kết nối phần cứng từ Giai đoạn 1.

**Bước 4: Viết chương trình**

Sinh viên tạo file robot\_sequence.py trong thư mục robot\_python.

import serial

import time

PORT = 'COM9'

BAUD = 115200

robot = serial.Serial(PORT, BAUD, timeout=2)

time.sleep(2)

print(f"Da ket noi voi {PORT} @ {BAUD} baud")

def send(cmd):

robot.write((cmd + '\n').encode())

response = robot.readline().decode().strip()

print(f' &gt;&gt; {cmd:30s} | &lt;&lt; {response}')

return response

sequence = [

('H', 'Ve Home'),

('S 0 3', 'Giam toc do J0'),

('M 5 60', 'Mo kep'),

('A 90 120 130 90 90 60', 'Tiep can vi tri A'),

('A 90 140 140 90 90 60', 'Ha xuong'),

('M 5 110', 'Gap vat'),

('A 90 100 90 90 90 110', 'Nang len'),

('M 0 45', 'Xoay den vi tri B'),

('A 45 130 130 90 90 110', 'Ha xuong'),

('M 5 60', 'Tha vat'),

('H', 'Ve Home'),

]

print("\n=== BAT DAU PICK-AND-PLACE ===\n")

for step\_index, (cmd, description) in enumerate(sequence, start=1):

print(f"[Buoc {step\_index:2d}/{len(sequence)}] {description}")

send(cmd)

send('W')

print()

robot.close()

print("=== HOAN TAT PICK-AND-PLACE ===")

print("Done.")

- Cấu trúc dữ liệu: mỗi phần tử trong danh sách sequence là một tuple gồm (lệnh, mô tả). Vòng lặp for sử dụng enumerate() để đánh số bước từ 1, gửi từng lệnh và chờ hoàn thành (W) trước khi gửi lệnh tiếp theo, đảm bảo robot thực hiện tuần tự.

**Bước 5: Vận hành và kiểm thử**

- Đặt một vật nhỏ (ví dụ khối gỗ hoặc hộp nhựa) tại vị trí phía trước robot (vị trí A).

- Chạy: python robot\_sequence.py

- Quan sát robot thực hiện toàn bộ chuỗi hành động: mở kẹp → tiếp cận → hạ → gắp → nâng → xoay → hạ → thả → Home.

- Nếu vật bị lệch hoặc rơi, điều chỉnh các giá trị góc trong sequence cho phù hợp với vị trí thực tế.

**Bước 6: Rút ra nhận xét, kết luận**

- Học viên tự đánh giá nhận xét kết quả đã đạt được dựa theo danh sách bên dưới và giảng viên dùng danh sách này để đánh giá kết quả thực hành:

- Script chạy hoàn chỉnh, robot thực hiện đúng toàn bộ chuỗi hành động pick-and-place.

- Vật được gắp, di chuyển và thả đúng vị trí (sai lệch cho phép ±1 cm).

- Mỗi bước in ra Terminal đúng mô tả và phản hồi firmware.

- Giải thích được tại sao mỗi lệnh cần gọi send('W') để chờ hoàn thành.

**Câu hỏi tự đánh giá**

1. Nếu muốn thêm bước robot xoay cổ tay 90° trước khi thả vật, cần thêm lệnh gì vào sequence?
2. Tại sao phải đặt tốc độ chậm (S 0 3) cho khớp đế khi gắp vật? Nếu dùng tốc độ cao, điều gì có thể xảy ra?

**Giai đoạn 3: Giao diện GUI hoàn chỉnh**

Giai đoạn này xây dựng giao diện đồ họa (GUI) chuyên nghiệp sử dụng Python Tkinter, cho phép điều khiển robot trực quan bằng thanh trượt và nút nhấn, đồng bộ trạng thái từ phần cứng.

**Bước 1: Khởi tạo dự án**

Tạo thư mục dự án GUI cho Robot Arm. Mở VS Code, tạo thư mục robot\_gui. Cài đặt môi trường ảo và thư viện:

*cd robot\_gui*

*python -m venv .venv*

*.venv\Scripts\activate (Windows)*

*pip install -r requirements.txt*

**Bước 2: Xác định modul, linh kiện cần dùng**

Sử dụng hệ thống phần cứng từ Bài thực hành 4.

**Bước 3: Kết nối các modul, linh kiện**

Giữ nguyên kết nối phần cứng. Đảm bảo firmware Robot Arm (Bài 4, GĐ3) đã nạp vào ESP32.

**Bước 4: Viết chương trình**

- Phần mềm GUI được tổ chức theo kiến trúc module. Tạo lần lượt các file sau trong thư mục robot\_gui:

+ main.py — file chính, khởi tạo cửa sổ Tkinter và các panel điều khiển (class RobotGUI).

+ serial\_comm.py — module giao tiếp Serial: class RobotSerial quản lý kết nối, gửi lệnh, nhận phản hồi, đọc trạng thái.

+ config.py — tham số cấu hình: danh sách khớp (tên, kênh, min, max, home), baudrate, timeout, màu sắc giao diện.

+ requirements.txt — khai báo thư viện cần cài đặt (pyserial&gt;=3.5).

- Giao diện GUI bao gồm các thành phần:

+ 6 thanh trượt (Slider) tương ứng 6 khớp robot, mỗi thanh có nhãn tên khớp, giá trị góc hiện tại và phạm vi min–max.

+ Nút HOME: đưa tất cả khớp về vị trí mặc định và đồng bộ thanh trượt.

+ Nút TRẠNG THÁI: gửi lệnh T, hiển thị trạng thái 6 khớp.

+ Nút DỪNG KHẨN CẤP: dừng mọi chuyển động ngay lập tức.

+ Vùng hiển thị Log: ghi lại toàn bộ lệnh gửi đi và phản hồi nhận về.

+ Lựa chọn cổng COM và nút Connect/Disconnect.

+ Khi kéo thanh trượt, GUI gửi lệnh M &lt;id&gt; &lt;góc&gt; theo thời gian thực. Khi nhấn HOME, GUI gửi lệnh H và đọc T để đồng bộ giá trị thanh trượt. Khi nhấn Connect, GUI gửi lệnh T để đồng bộ trạng thái ban đầu từ firmware.

**Bước 5: Vận hành và kiểm thử**

- Đóng Serial Monitor trong VS Code.

- Chạy: python main.py

- Trên giao diện, chọn cổng COM tương ứng với ESP32, nhấn Connect. Các thanh trượt tự động cập nhật giá trị hiện tại từ firmware.

- Kéo thanh trượt J0 → quan sát đế robot xoay theo. Giá trị góc cập nhật trên giao diện.

- Điều khiển lần lượt từng khớp J1–J5 qua các thanh trượt.

- Nhấn HOME → tất cả khớp về vị trí mặc định, thanh trượt đồng bộ.

- Nhấn TRẠNG THÁI → vùng Log hiển thị STA:a0,a1,a2,a3,a4,a5.

- Khi robot đang di chuyển, nhấn DỪNG KHẨN CẤP → robot dừng ngay lập tức.

**Bước 6: Rút ra nhận xét, kết luận**

- Học viên tự đánh giá nhận xét kết quả đã đạt được dựa theo danh sách bên dưới và giảng viên dùng danh sách này để đánh giá kết quả thực hành:

+ GUI mở thành công, giao diện hiển thị đầy đủ 6 thanh trượt, các nút chức năng và vùng Log.

+ Kết nối COM thành công, thanh trượt tự đồng bộ giá trị từ firmware khi Connect.

+ Kéo thanh trượt, robot phản hồi đúng — góc trên GUI khớp với góc thực tế.

+ Nút HOME đưa robot về Home và đồng bộ toàn bộ thanh trượt.

+ Nút DỪNG KHẨN CẤP dừng robot ngay lập tức khi đang di chuyển.

+ Giải thích được luồng dữ liệu: GUI → Serial → ESP32 → PCA9685 → Servo.

+ Giải thích được tại sao GUI gửi lệnh T khi vừa Connect để đồng bộ trạng thái.

**Câu hỏi tự đánh giá**

1. Khi kéo thanh trượt rất nhanh, GUI gửi nhiều lệnh M liên tiếp. Điều này có thể gây ra vấn đề gì trên firmware? Cách giải quyết?

2. Tại sao nút DỪNG KHẨN CẤP cần thiết? Nếu không có, điều gì xảy ra khi robot va chạm vật cản?

3. Nếu muốn thêm tính năng Record/Play (ghi và phát lại chuỗi động tác), cần lưu trữ những thông tin gì?

**Bài tập mở rộng**

1.Thêm nút Record/Play vào GUI để ghi lại và phát lại chuỗi động tác.

2.Lưu/Tải chuỗi hành động vào file JSON để chia sẻ giữa các nhóm.

3.Tìm hiểu thư viện Flask (Python) để tạo trang web điều khiển robot qua LAN.

4.Thêm chế độ Dark Mode cho GUI bằng cách thay đổi bảng màu.

**6. Bài thực hành 6: Điều khiển Servo thông qua hệ thống IoT**

**a. Mục tiêu**

- Đáp ứng CĐR 2.1 và 2.2 môn Mạng IoT (ĐVT 2.8.7); CĐR 2.1 và 2.2 môn Kiến trúc và giao thức trong IoT (ĐVT 2.10.7); CĐR 2.1 và 2.2 môn Các hệ thống và giải pháp IoT tiên tiến (ĐVT 2.11.7); CĐR 2.1 và 2.2 môn IoT và ứng dụng (ĐCN 2.3.5), cụ thể:

+ Thực hiện được việc thiết kế và triển khai mô hình IoT điều khiển robot từ xa hoàn chỉnh.

+ Làm chủ được việc xử lý và quản lý dữ liệu điều khiển trong hệ thống IoT.

+ Lập trình được dự án IoT hoàn chỉnh từ firmware điều khiển đến truyền thông với đám mây qua BLYNK.

+ Viết được chương trình điều khiển vận dụng giao thức WiFi vào xây dựng hệ thống IoT.

+ Hiểu nguyên lý điều khiển thiết bị từ xa qua nền tảng BLYNK IoT Platform.

+ Tích hợp firmware điều khiển servo với kết nối WiFi và giao thức BLYNK.

+ Thiết kế Web Dashboard và ứng dụng BLYNK Mobile để điều khiển 6 khớp robot từ xa.

+ Xây dựng hệ thống giám sát trạng thái robot theo thời gian thực trên cloud.

+ Triển khai cơ chế điều khiển kết hợp: cục bộ (Serial) và từ xa (IoT) đồng thời.

**b. Các bước tiến hành**

Bài thực hành 6 được triển khai qua ba giai đoạn tiệm tiến.

**Bảng 6.1.** Phân bổ nội dung cụ thể trong từng giai đoạn của bài thực hành 6

| **Giai đoạn**                  | **Nội dung chính**                  | **Kết quả đạt được**                   |
|--------------------------------|-------------------------------------|----------------------------------------|
| GĐ1: Kết nối BLYNK cơ bản      | WiFi + BLYNK + 1 servo              | Điều khiển 1 servo từ xa qua Dashboard |
| GĐ2: Điều khiển 6 khớp qua IoT | Slider V0–V5 + Home V6 + Status V7  | Điều khiển toàn bộ robot từ xa         |
| GĐ3: Hoàn thiện hệ thống IoT   | BLYNK Mobile + đồng thời Serial/IoT | Điều khiển kết hợp cục bộ và từ xa     |

**Giai đoạn 1: Kết nối BLYNK cơ bản với 1 servo**

Giai đoạn này tập trung vào việc kết nối ESP32 với nền tảng BLYNK IoT Platform và điều khiển 1 servo (khớp J0) từ xa qua Web Dashboard. Đây là bước nền tảng giúp người học hiểu luồng dữ liệu từ cloud đến thiết bị vật lý.

**Bước 1: Khởi tạo dự án**

- Thực hiện theo thứ tự các bước sau trên [https://blynk.cloud](https://blynk.cloud/) :

- Đăng nhập tài khoản BLYNK. Vào Developer Zone → My Templates → New Template.

- Điền: Template Name = Robot Arm IoT, Hardware = ESP32, Connection Type = WiFi. Nhấn Done.

- Vào tab Datastreams, tạo 1 Virtual Pin: V0 (J0 Angle, Integer, min 0, max 180).

- Vào tab Web Dashboard, thêm 1 widget Slider gán V0, đặt tên "J0 Base".

- Vào My Devices → New Device → From Template → chọn Robot Arm IoT. Hệ thống sinh ra Auth Token (32 ký tự). Sao chép và lưu lại.

- Mở VS Code, tạo project PlatformIO mới đặt tên RobotArm\_IoT\_GD1. Mở file platformio.ini:

[env:esp32dev]

platform = espressif32

board = esp32dev

framework = arduino

upload\_speed = 115200

monitor\_speed = 115200

upload\_port = COM9 ; doi thanh COM port thuc te

lib\_deps =

adafruit/Adafruit PWM Servo Driver Library

blynkkk/Blynk

**Bước 2: Xác định modul, linh kiện cần dùng**

Sử dụng hệ thống phần cứng đã kết nối từ Bài thực hành 4 (ESP32 + PCA9685 + Robot Arm 6DOF).

**Bước 3: Kết nối các modul, linh kiện**

Giữ nguyên kết nối phần cứng từ Bài thực hành 4. Đảm bảo ESP32 trong vùng phủ sóng WiFi.

**Bước 4: Viết chương trình**

- Nhập chương trình sau vào file src/main.cpp:

#define BLYNK\_PRINT Serial

#define BLYNK\_TEMPLATE\_ID "YOUR\_TEMPLATE\_ID"

#define BLYNK\_TEMPLATE\_NAME "Robot Arm IoT"

#define BLYNK\_AUTH\_TOKEN "YOUR\_TOKEN"

#include &lt;WiFi.h&gt;

#include &lt;BlynkSimpleEsp32.h&gt;

#include &lt;Wire.h&gt;

#include &lt;Adafruit\_PWMServoDriver.h&gt;

char ssid[] = "TEN\_WIFI\_CUA\_BAN";

char pass[] = "MAT\_KHAU\_WIFI";

Adafruit\_PWMServoDriver pca = Adafruit\_PWMServoDriver(0x40);

#define SERVOMIN 102

#define SERVOMAX 512

int angleToPulse(int angle) {

return map(angle, 0, 180, SERVOMIN, SERVOMAX);

}

void moveServo(uint8\_t ch, int angle) {

int pulse = angleToPulse(angle);

pca.setPWM(ch, 0, pulse);

Serial.print("Servo CH"); Serial.print(ch);

Serial.print(" -&gt; "); Serial.print(angle);

Serial.println(" do");

}

BLYNK\_WRITE(V0) {

int angle = param.asInt();

moveServo(0, angle);

}

void setup() {

Serial.begin(115200);

Serial.println("=== BAI 6 - GD1: Blynk co ban ===");

Wire.begin(21, 22);

pca.begin();

pca.setPWMFreq(50);

moveServo(0, 90);

Blynk.begin(BLYNK\_AUTH\_TOKEN, ssid, pass);

Serial.println("Blynk da ket noi thanh cong!");

}

void loop() {

Blynk.run();

}

- Thay YOUR\_TEMPLATE\_ID bằng Template ID từ BLYNK (dạng TMPLxxxxxx). Thay YOUR\_TOKEN bằng 32 ký tự Auth Token. Thay TEN\_WIFI\_CUA\_BAN và MAT\_KHAU\_WIFI bằng thông tin WiFi thực tế.

- Cơ sở lý thuyết: macro BLYNK\_WRITE(V0) được BLYNK SDK gọi tự động mỗi khi người dùng kéo Slider V0 trên Dashboard. Giá trị mới được truyền qua param.asInt(). Luồng dữ liệu: Dashboard → BLYNK Cloud → WiFi → ESP32 → PCA9685 → Servo.

**Bước 5: Vận hành và kiểm thử**

- Biên dịch và nạp chương trình (Ctrl+Alt+U). Mở Serial Monitor (115200 baud).

- Kết quả mong đợi trên Serial: "Connecting to WiFi..." → "WiFi connected" → "Connected to Blynk".

- Mở BLYNK Web Dashboard, kéo Slider J0 → quan sát đế robot xoay theo.

- Kéo Slider đến 0° → robot quay hết phải. Kéo đến 180° → quay hết trái.

**Bước 6: Rút ra nhận xét, kết luận**

- Học viên tự đánh giá nhận xét kết quả đã đạt được dựa theo danh sách bên dưới và giảng viên dùng danh sách này để đánh giá kết quả thực hành:

- Serial Monitor hiển thị "Connected to Blynk" — xác nhận kết nối WiFi và BLYNK thành công.

- Kéo Slider V0 trên Dashboard, khớp J0 xoay đúng góc yêu cầu.

- Giải thích được luồng dữ liệu: Dashboard → Cloud → WiFi → ESP32 → PCA9685 → Servo.

**Câu hỏi tự đánh giá**

1. Tại sao #define BLYNK\_TEMPLATE\_ID phải đặt trước #include &lt;BlynkSimpleEsp32.h&gt;?

2. Macro BLYNK\_WRITE(V0) được gọi bởi ai? Khi nào nó được kích hoạt?

3. Nếu mất WiFi giữa chừng, servo có giữ nguyên vị trí cuối cùng không? Tại sao?

**Giai đoạn 2: Điều khiển toàn bộ 6 khớp qua IoT**

Giai đoạn này mở rộng điều khiển cho toàn bộ 6 khớp robot, thêm nút Home và hiển thị trạng thái trên BLYNK Dashboard.

**Bước 1: Khởi tạo dự án**

–  Trên BLYNK Web, mở lại Template "Robot Arm IoT". Vào tab Datastreams, thêm các Virtual Pin:

+ V0 (J0 Angle): Integer, 0–180, điều khiển góc khớp J0 (Base).

+ V1 (J1 Angle): Integer, 70–150, điều khiển góc khớp J1 (Shoulder).

+ V2 (J2 Angle): Integer, 0–150, điều khiển góc khớp J2 (Elbow).

+ V3 (J3 Angle): Integer, 0–180, điều khiển góc khớp J3 (Wrist Pitch).

+ V4 (J4 Angle): Integer, 0–180, điều khiển góc khớp J4 (Wrist Roll).

+ V5 (J5 Angle): Integer, 60–120, điều khiển góc khớp J5 (Gripper).

+ V6 (Home): Integer, 0/1, nút đưa robot về Home.

+ V7 (Status): String, hiển thị trạng thái hoạt động.

–  Trên Web Dashboard, tạo Slider cho từng khớp (V0–V5) với phạm vi tương ứng, Button cho Home (V6), Label cho Status (V7).

**Bước 2: Xác định modul, linh kiện cần dùng**

Sử dụng hệ thống phần cứng từ Giai đoạn 1.

**Bước 3: Kết nối các modul, linh kiện**

Giữ nguyên kết nối phần cứng từ Giai đoạn 1.

**Bước 4: Viết chương trình**

–  Cập nhật file src/main.cpp:

#define BLYNK\_PRINT Serial

#define BLYNK\_TEMPLATE\_ID "YOUR\_TEMPLATE\_ID"

#define BLYNK\_TEMPLATE\_NAME "Robot Arm IoT"

#define BLYNK\_AUTH\_TOKEN "YOUR\_TOKEN"

#include &lt;WiFi.h&gt;

#include &lt;BlynkSimpleEsp32.h&gt;

#include &lt;Wire.h&gt;

#include &lt;Adafruit\_PWMServoDriver.h&gt;

char ssid[] = "TEN\_WIFI\_CUA\_BAN";

char pass[] = "MAT\_KHAU\_WIFI";

Adafruit\_PWMServoDriver pca = Adafruit\_PWMServoDriver(0x40);

#define SERVOMIN 102

#define SERVOMAX 512

int home\_angles[6] = {90, 70, 90, 90, 90, 90};

int current\_angles[6] = {90, 70, 90, 90, 90, 90};

BlynkTimer timer;

int angleToPulse(int angle) {

return map(angle, 0, 180, SERVOMIN, SERVOMAX);

}

void moveServo(uint8\_t ch, int angle) {

int pulse = angleToPulse(angle);

pca.setPWM(ch, 0, pulse);

Serial.print("J"); Serial.print(ch);

Serial.print("="); Serial.println(angle);

}

BLYNK\_WRITE(V0) { int a = param.asInt(); moveServo(0, a); current\_angles[0] = a; }

BLYNK\_WRITE(V1) { int a = param.asInt(); moveServo(1, a); current\_angles[1] = a; }

BLYNK\_WRITE(V2) { int a = param.asInt(); moveServo(2, a); current\_angles[2] = a; }

BLYNK\_WRITE(V3) { int a = param.asInt(); moveServo(3, a); current\_angles[3] = a; }

BLYNK\_WRITE(V4) { int a = param.asInt(); moveServo(4, a); current\_angles[4] = a; }

BLYNK\_WRITE(V5) { int a = param.asInt(); moveServo(5, a); current\_angles[5] = a; }

BLYNK\_WRITE(V6) {

if (param.asInt() == 1) {

for (int i = 0; i &lt; 6; i++) {

moveServo(i, home\_angles[i]);

current\_angles[i] = home\_angles[i];

}

Blynk.virtualWrite(V0, home\_angles[0]);

Blynk.virtualWrite(V1, home\_angles[1]);

Blynk.virtualWrite(V2, home\_angles[2]);

Blynk.virtualWrite(V3, home\_angles[3]);

Blynk.virtualWrite(V4, home\_angles[4]);

Blynk.virtualWrite(V5, home\_angles[5]);

Blynk.virtualWrite(V7, "HOME");

Serial.println("&gt;&gt;&gt; HOME position &lt;&lt;&lt;");

}

}

void sendStatus() {

String status\_str = "STA:";

for (int i = 0; i &lt; 6; i++) {

status\_str += String(current\_angles[i]);

if (i &lt; 5) status\_str += ",";

}

Blynk.virtualWrite(V7, status\_str);

}

void setup() {

Serial.begin(115200);

Serial.println("=== BAI 6 - GD2: 6 khop + Home + Status ===");

Wire.begin(21, 22);

pca.begin();

pca.setPWMFreq(50);

for (int i = 0; i &lt; 6; i++) {

moveServo(i, home\_angles[i]);

current\_angles[i] = home\_angles[i];

}

Blynk.begin(BLYNK\_AUTH\_TOKEN, ssid, pass);

timer.setInterval(3000L, sendStatus);

Serial.println("Blynk da ket noi - san sang dieu khien!");

}

void loop() {

Blynk.run();

timer.run();

}

–  BlynkTimer gọi hàm sendStatus() mỗi 3 giây để cập nhật trạng thái robot lên Label V7 trên Dashboard. Biến status\_str lưu chuỗi trạng thái dạng "STA:a0,a1,a2,a3,a4,a5". Đây là cơ chế push dữ liệu từ thiết bị lên cloud, giúp người dùng giám sát trạng thái theo thời gian thực.

**Bước 5: Vận hành và kiểm thử**

–  Biên dịch và nạp chương trình. Kiểm thử trên Web Dashboard:

+ Kéo Slider J0 → quan sát đế robot xoay theo.

+ Điều khiển lần lượt từng khớp J1–J5 qua các Slider.

+ Nhấn nút Home → tất cả khớp về vị trí mặc định, các Slider tự đồng bộ.

+ Quan sát Label Status cập nhật STA:a0,a1,a2,a3,a4,a5 mỗi 3 giây.

**Bước 6: Rút ra nhận xét, kết luận**

–  Học viên tự đánh giá nhận xét kết quả đã đạt được dựa theo danh sách bên dưới và giảng viên dùng danh sách này để đánh giá kết quả thực hành:

**+** Web Dashboard hiển thị 6 Slider, 1 Button Home, 1 Label Status đúng cấu hình.

**+** Kéo từng Slider, khớp robot tương ứng di chuyển đúng góc.

**+** Nhấn Home, tất cả khớp về vị trí mặc định và Slider đồng bộ tự động.

**+** Label Status cập nhật trạng thái đúng mỗi 3 giây.

**+** Giải thích được tại sao macro BLYNK\_WRITE(V6) cần gọi Blynk.virtualWrite(i, home\_angles[i]) để đồng bộ Slider.

**Câu hỏi tự đánh giá:**

1. BlynkTimer khác delay() và millis() ở chỗ nào? Tại sao phải gọi timer.run() liên tục?

2. Tại sao BLYNK\_WRITE(V6) cần gửi lại giá trị home\_angles lên Dashboard? Nếu bỏ dòng Blynk.virtualWrite, điều gì xảy ra?

3. Nếu nhiều người cùng điều khiển robot qua BLYNK, xung đột sẽ xảy ra thế nào?

**Giai đoạn 3: Hoàn thiện hệ thống IoT**

Giai đoạn cuối nâng cấp hệ thống: cài đặt BLYNK Mobile, kiểm thử điều khiển từ xa qua điện thoại và kiểm tra cơ chế tự động reconnect khi mất/phục hồi WiFi.

**Bước 1: Khởi tạo dự án**

–  Cài đặt ứng dụng BLYNK IoT trên điện thoại (iOS: App Store, Android: Google Play).

–  Đăng nhập cùng tài khoản BLYNK đã tạo. Hệ thống tự đồng bộ Template và Device.

–  Trên ứng dụng mobile, mở Device "Robot Arm IoT". Dashboard mobile hiển thị các widget tương tự Web Dashboard.

**Bước 2: Xác định modul, linh kiện cần dùng**

Sử dụng hệ thống phần cứng từ Giai đoạn 2.

**Bước 3: Kết nối các modul, linh kiện**

Giữ nguyên kết nối phần cứng.

**Bước 4: Viết chương trình**

Tiếp tục sử dụng firmware từ Giai đoạn 2 (không cần thay đổi code — BLYNK SDK tự hỗ trợ cả Web và Mobile).

**Bước 5: Vận hành và kiểm thử**

–  Kiểm thử điều khiển từ xa qua BLYNK Mobile:

+ Trên điện thoại, kéo Slider J0 → quan sát đế robot xoay theo.

+ Điều khiển lần lượt các khớp, nhấn Home.

+ So sánh độ trễ giữa Web Dashboard và Mobile — cả hai phải phản hồi tương đương.

–  Kiểm thử cơ chế reconnect:

+ Tắt WiFi router trong 10 giây → Serial Monitor hiển thị mất kết nối.

+ Bật lại WiFi → ESP32 tự kết nối lại BLYNK, Serial hiển thị "Reconnected".

+ Sau khi reconnect, thử điều khiển lại qua Dashboard — robot phản hồi bình thường.

–  Kiểm thử điều khiển đồng thời:

+ Mở cả Web Dashboard và Mobile App cùng lúc.

+ Kéo Slider J0 trên Web → Mobile tự cập nhật giá trị, và ngược lại.

**Bước 6: Rút ra nhận xét, kết luận**

–  Học viên tự đánh giá nhận xét kết quả đã đạt được dựa theo danh sách bên dưới và giảng viên dùng danh sách này để đánh giá kết quả thực hành:

**+** BLYNK Mobile hiển thị đúng giao diện điều khiển, các Slider và nút hoạt động bình thường.

**+** Điều khiển robot từ điện thoại phản hồi tương đương Web Dashboard (độ trễ không quá 1 giây trên cùng mạng LAN).

**+** Khi mất WiFi và phục hồi, ESP32 tự kết nối lại BLYNK mà không cần reset tay.

**+** Web Dashboard và Mobile App đồng bộ trạng thái với nhau theo thời gian thực.

**+** Giải thích được tại sao BLYNK SDK tự hỗ trợ reconnect mà không cần viết thêm code.

**+** Giải thích được tại sao cùng một firmware có thể điều khiển từ cả Web và Mobile.

**Câu hỏi tự đánh giá:**

1. So sánh ưu nhược điểm của điều khiển robot qua Serial (Bài 4), GUI (Bài 5) và IoT (Bài 6).

2. Nếu muốn điều khiển robot từ bất kỳ đâu trên Internet (không cùng mạng LAN), BLYNK có hỗ trợ không? Giải thích cơ chế.

3. Trong ứng dụng thực tế, tại sao cần cơ chế điều khiển kết hợp cả cục bộ (Serial) và từ xa (IoT)?

**Bài tập mở rộng**

1. Thêm tính năng ghi/phát chuỗi hành động từ xa qua BLYNK (dùng Datastream lưu chuỗi lệnh).

2. Tích hợp camera (ESP32-CAM) để giám sát hình ảnh robot từ xa qua BLYNK Video Stream.

3. Xây dựng hệ thống MQTT (thay thế BLYNK) để điều khiển robot từ bất kỳ đâu trên Internet.

4. Triển khai điều khiển đồng thời từ cả Serial lẫn IoT với cơ chế ưu tiên: lệnh Serial được ưu tiên cao hơn, IoT chỉ hoạt động khi không có lệnh Serial trong 5 giây.