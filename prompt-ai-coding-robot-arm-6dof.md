# PROMPT CHO AI CODING: SOẠN MÃ NGUỒN MẪU — BÀI THỰC HÀNH 4, 5, 6 (ROBOT ARM 6DOF)

---

## BỐI CẢNH DỰ ÁN

Bạn là kỹ sư phần mềm nhúng, được giao nhiệm vụ soạn thảo **toàn bộ mã nguồn mẫu** cho giáo trình thực hành đại học. Mã nguồn sẽ được **in trong sách**, sinh viên sẽ **tự đánh máy** từ sách vào VS Code + PlatformIO để học tập. Không sử dụng GitHub clone — mọi file đều phải có nội dung đầy đủ, rõ ràng, dễ đọc, dễ đánh máy.

**Nền tảng phần cứng:**
- ESP32 DevKit V1 (ESP-WROOM-32S)
- Module PCA9685 — IC điều khiển PWM 16 kênh qua I2C, địa chỉ 0x40
- Cánh tay Robot 6DOF (6 servo MG995/MG996, khung hợp kim nhôm)
- Nguồn ngoài 6V / 15A cho servo (lưu ý: mỗi servo MG995/MG996 có thể kéo đến 2.5A khi khóa cứng — 6 servo tổng cộng lên đến ~15A)
- Kết nối: SDA → GPIO 21, SCL → GPIO 22, GND chung, VCC → 3V3

**Thông số 6 khớp Robot:**

| Khớp | Tên | Chức năng | Góc min | Góc max | Góc Home | Kênh PCA9685 |
|------|-----|-----------|---------|---------|----------|--------------|
| J0 | Base (Đế) | Xoay toàn bộ cánh tay | 0° | 180° | 90° | 0 |
| J1 | Shoulder (Vai) | Nâng/hạ cánh tay trên | 70° | 150° | 70° | 1 |
| J2 | Elbow (Khuỷu) | Gập/duỗi cánh tay dưới | 0° | 150° | 90° | 2 |
| J3 | Wrist Pitch | Ngửa/úp bàn kẹp | 0° | 180° | 90° | 3 |
| J4 | Wrist Roll | Xoay bàn kẹp | 0° | 180° | 90° | 4 |
| J5 | Gripper (Kẹp) | Mở/đóng kẹp | 60° | 120° | 90° | 5 |

**Thông số kỹ thuật PWM:**
- Tần số PWM: 50 Hz (chu kỳ 20 ms)
- Độ phân giải PCA9685: 12 bit (0–4095)
- SERVOMIN = 102 (tương ứng xung 0.5 ms → 0°)
- SERVOMAX = 512 (tương ứng xung 2.5 ms → 180°)
- Công thức: pulse = map(angle, 0, 180, 102, 512)

---

## YÊU CẦU CHUNG VỀ MÃ NGUỒN

### Quy tắc viết code cho giáo trình in sách:
1. **Comment tiếng Việt KHÔNG DẤU** (vì font in sách có thể không hỗ trợ Unicode trong code block). Ví dụ: `// Khoi tao I2C`, `// Di chuyen servo den goc mong muon`.
2. **Comment chi tiết giải thích từng khối chức năng** — sinh viên đọc comment để hiểu logic trước khi đánh máy.
3. **Mỗi file bắt đầu bằng header comment** ghi rõ: Tên bài thực hành, Giai đoạn, Tên file, Mô tả ngắn.
4. **Độ dài mỗi file tối đa 120 dòng** (phù hợp in 2-3 trang sách). Nếu dài hơn thì tách thành nhiều file module.
5. **Tên biến, hàm bằng tiếng Anh**, viết theo camelCase hoặc snake_case nhất quán.
6. **Không dùng thư viện ngoài không cần thiết** — chỉ dùng: Wire.h, Adafruit_PWMServoDriver.h, WiFi.h, BlynkSimpleEsp32.h, Serial (có sẵn).
7. **Mã nguồn phải biên dịch và chạy được thực tế** trên ESP32 + PCA9685 + 6 servo.
8. **Trong mỗi file .cpp/.h phải có dòng ghi chú giai đoạn** dạng: `// === GIAI DOAN X: Ten giai doan ===`

### Cấu trúc thư mục đầu ra:
```
robot-arm-source-code/
├── bai-thuc-hanh-4/
│   ├── giai-doan-1/
│   │   ├── platformio.ini
│   │   └── src/
│   │       └── main.cpp                  (I2C scan)
│   ├── giai-doan-2/
│   │   ├── platformio.ini
│   │   └── src/
│   │       ├── config.h                  (cau hinh 6 khop)
│   │       ├── pca9685_driver.h          (header module PCA9685)
│   │       ├── pca9685_driver.cpp        (giao tiep I2C, chuyen goc -> PWM)
│   │       ├── servo_ctrl.h              (header dieu khien servo)
│   │       ├── servo_ctrl.cpp            (clamp goc, smooth move, trang thai)
│   │       ├── cmd_parser.h              (header phan tich lenh Serial)
│   │       ├── cmd_parser.cpp            (xu ly lenh M, G, T, H, I)
│   │       └── main.cpp                  (file chinh: setup + loop)
│   └── giai-doan-3/
│       ├── platformio.ini
│       └── src/
│           ├── config.h                  (giong GD2, them toc do)
│           ├── pca9685_driver.h
│           ├── pca9685_driver.cpp
│           ├── servo_ctrl.h              (them ham moveAll, wait)
│           ├── servo_ctrl.cpp            (them S, A, W logic)
│           ├── cmd_parser.h              (them lenh S, A, W)
│           ├── cmd_parser.cpp
│           └── main.cpp
├── bai-thuc-hanh-5/
│   ├── giai-doan-1/
│   │   └── robot_basic.py                (ket noi Serial co ban)
│   ├── giai-doan-2/
│   │   └── robot_sequence.py             (chuoi hanh dong pick-and-place)
│   └── giai-doan-3/
│       ├── requirements.txt
│       ├── config.py                     (tham so cau hinh GUI)
│       ├── serial_comm.py                (module giao tiep Serial)
│       └── main.py                       (GUI Tkinter hoan chinh)
└── bai-thuc-hanh-6/
    ├── giai-doan-1/
    │   ├── platformio.ini
    │   └── src/
    │       └── main.cpp                  (BLYNK + 1 servo V0)
    ├── giai-doan-2/
    │   ├── platformio.ini
    │   └── src/
    │       └── main.cpp                  (BLYNK + 6 servo V0-V5 + Home V6 + Status V7)
    └── giai-doan-3/
        ├── platformio.ini
        └── src/
            └── main.cpp                  (giong GD2 — ghi chu them ve Mobile + reconnect)
```

---

## BÀI THỰC HÀNH 4: ĐIỀU KHIỂN SERVO CƠ BẢN SỬ DỤNG BỘ PCA9685

### Giai đoạn 1: Kết nối phần cứng và kiểm tra I2C

**File: `src/main.cpp`**

Chức năng: Quét bus I2C, tìm và xác nhận PCA9685 tại địa chỉ 0x40.

Yêu cầu:
- Include Wire.h
- Wire.begin(21, 22) — SDA=21, SCL=22
- Vòng lặp quét địa chỉ 1–126, dùng Wire.beginTransmission() + Wire.endTransmission()
- In kết quả ra Serial 115200
- Kết quả mong đợi: "I2C device found at 0x40"
- Comment giải thích: I2C la gi, tai sao quet tu 1 den 126, endTransmission tra ve 0 nghia la gi

**File: `platformio.ini`**
```
[env:esp32dev]
platform = espressif32
board = esp32dev
framework = arduino
upload_speed = 115200
monitor_speed = 115200
lib_deps =
    adafruit/Adafruit PWM Servo Driver Library
```

### Giai đoạn 2: Điều khiển servo cơ bản qua Serial

**File: `src/config.h`**

Chức năng: Khai báo cấu hình 6 khớp robot dưới dạng struct và mảng.

Yêu cầu:
- Định nghĩa struct JointConfig: channel, minAngle, maxAngle, homeAngle, name (const char*)
- Mảng JointConfig joints[6] chứa thông số 6 khớp theo bảng ở trên
- Hằng số: NUM_JOINTS=6, SERVOMIN=102, SERVOMAX=512, PWM_FREQ=50
- Hằng số: SDA_PIN=21, SCL_PIN=22, DEFAULT_SPEED=5
- Comment giải thích: tai sao SERVOMIN=102, SERVOMAX=512 (tinh tu cong thuc xung PWM)

**File: `src/pca9685_driver.h` + `src/pca9685_driver.cpp`**

Chức năng: Module giao tiếp với PCA9685, chuyển đổi góc → xung PWM.

Yêu cầu:
- Hàm `void pca9685_init()` — khởi tạo Adafruit_PWMServoDriver, gọi begin(), setPWMFreq(50)
- Hàm `void pca9685_setAngle(uint8_t channel, int angle)` — chuyển góc sang pulse bằng map(), gọi setPWM()
- Hàm `int angleToPulse(int angle)` — trả về giá trị pulse 12 bit
- Biến toàn cục: Adafruit_PWMServoDriver pca (đối tượng driver)
- Comment giải thích: PWM 50Hz, chu ky 20ms, do phan giai 12 bit, cong thuc chuyen doi

**File: `src/servo_ctrl.h` + `src/servo_ctrl.cpp`**

Chức năng: Quản lý 6 servo: clamp góc, di chuyển mượt (smooth move), lưu trạng thái.

Yêu cầu:
- Mảng `int currentAngles[6]` — lưu góc hiện tại của 6 khớp
- Hàm `void servo_init()` — đưa 6 servo về Home, cập nhật currentAngles
- Hàm `int clampAngle(int joint, int angle)` — giới hạn góc trong [min, max] của khớp đó
- Hàm `void servo_move(int joint, int targetAngle)` — di chuyển mượt: lặp từng 1°, mỗi bước delay tỷ lệ nghịch với tốc độ (delay = 20 / speed ms), gọi pca9685_setAngle()
- Hàm `void servo_home(int joint)` — về Home 1 khớp. Nếu joint = -1 thì về Home tất cả
- Hàm `int servo_getAngle(int joint)` — trả về currentAngles[joint]
- Hàm `void servo_printStatus()` — in "STA:a0,a1,a2,a3,a4,a5" ra Serial
- Hàm `void servo_printInfo()` — in thông tin cấu hình từng khớp (min, max, home, speed)
- Comment giải thích: co che clamp bao ve co cau, smooth move giup giam tai co hoc

**File: `src/cmd_parser.h` + `src/cmd_parser.cpp`**

Chức năng: Phân tích lệnh từ Serial Monitor.

Yêu cầu:
- Hàm `void cmd_parse(String input)` — nhận chuỗi từ Serial, phân tích lệnh:
  - **M <id> <angle>**: di chuyển 1 khớp → gọi servo_move(). Phản hồi "OK" hoặc "ERR:..."
  - **G <id>**: đọc góc → gọi servo_getAngle(). Phản hồi "VAL:<id>:<angle>"
  - **T**: trạng thái → gọi servo_printStatus()
  - **H [id]**: về Home → gọi servo_home(). Không có id thì home tất cả. Phản hồi "OK"
  - **I**: thông tin → gọi servo_printInfo()
  - Kiểm tra: id hợp lệ (0–5), nếu sai trả "ERR:INVALID_JOINT"
  - Kiểm tra: đủ tham số, nếu thiếu trả "ERR:MISSING_PARAM"
- Dùng String.indexOf(' ') và String.substring() để tách tham số
- Comment giải thích: cau truc lenh, cach phan tich chuoi, xu ly loi

**File: `src/main.cpp`**

Chức năng: File chính — khởi tạo hệ thống và vòng lặp đọc lệnh Serial.

Yêu cầu:
- setup(): Serial.begin(115200), Wire.begin(21,22), pca9685_init(), servo_init(), in "ARM INIT" rồi "ARM READY"
- loop(): đọc Serial.readStringUntil('\n'), trim(), nếu không rỗng thì gọi cmd_parse()
- Comment giải thích: luong hoat dong cua firmware

### Giai đoạn 3: Điều khiển nâng cao — tốc độ, đồng thời và chuỗi hành động

**Kế thừa toàn bộ file từ GĐ2, bổ sung/sửa đổi:**

**Sửa `config.h`:**
- Thêm trường `speed` vào JointConfig (mặc định = 5, range 1–10)

**Sửa `servo_ctrl.h` + `servo_ctrl.cpp`:**
- Thêm mảng `int jointSpeed[6]` — tốc độ riêng từng khớp
- Thêm hàm `void servo_setSpeed(int joint, int speed)` — cập nhật tốc độ (clamp 1–10)
- Thêm hàm `void servo_moveAll(int angles[6])` — di chuyển đồng thời 6 khớp bằng cách: tính số bước lớn nhất, lặp từng bước, mỗi bước tính góc trung gian cho từng khớp (tỷ lệ), gọi pca9685_setAngle()
- Thêm biến `bool isMoving` và hàm `void servo_wait()` — chờ hoàn thành, phản hồi "DONE"
- Comment giải thích: thuat toan di chuyen dong thoi, noi suy tuyen tinh

**Sửa `cmd_parser.h` + `cmd_parser.cpp`:**
- Thêm lệnh **S <id> <speed>**: thay đổi tốc độ → gọi servo_setSpeed(). Phản hồi "OK"
- Thêm lệnh **A <a0> <a1> <a2> <a3> <a4> <a5>**: di chuyển đồng thời → gọi servo_moveAll(). Phản hồi "OK"
- Thêm lệnh **W**: chờ hoàn thành → gọi servo_wait(). Phản hồi "DONE"
- Kiểm tra: lệnh A phải đủ 6 tham số, nếu thiếu trả "ERR:NEED_6_ANGLES"
- Comment giải thích: lenh nang cao, tai sao can W de dong bo

---

## BÀI THỰC HÀNH 5: ĐIỀU KHIỂN SERVO THÔNG QUA HỆ THỐNG GUI

### Giai đoạn 1: Kết nối Serial cơ bản với Python

**File: `robot_basic.py`**

Chức năng: Script Python kết nối Serial, gửi lệnh cơ bản điều khiển robot.

Yêu cầu:
- Import serial, time
- Biến PORT = 'COM9' (ghi chu: thay COM port thuc te), BAUD = 115200
- Mở serial.Serial(PORT, BAUD, timeout=2), sleep(2) chờ ESP32 reset
- Hàm send(cmd): gửi (cmd + '\n').encode(), đọc readline().decode().strip(), in kết quả
- Chuỗi lệnh: H → W → M 0 45 → W → T → M 0 135 → W → H → W
- Đóng robot.close(), in "Done."
- Comment giải thích: tai sao sleep(2), tai sao can '\n', timeout la gi

### Giai đoạn 2: Chuỗi hành động tự động pick-and-place

**File: `robot_sequence.py`**

Chức năng: Script tự động gửi chuỗi lệnh gắp-di chuyển-thả vật thể.

Yêu cầu:
- Kế thừa hàm send() từ GĐ1
- Danh sách sequence là list các tuple (command, description):
  ```
  ('H',                        'Ve Home'),
  ('S 0 3',                    'Giam toc do J0'),
  ('M 5 60',                   'Mo kep'),
  ('A 90 120 130 90 90 60',    'Tiep can vi tri A'),
  ('A 90 140 140 90 90 60',    'Ha xuong'),
  ('M 5 110',                  'Gap vat'),
  ('A 90 100 90 90 90 110',    'Nang len'),
  ('M 0 45',                   'Xoay den vi tri B'),
  ('A 45 130 130 90 90 110',   'Ha xuong'),
  ('M 5 60',                   'Tha vat'),
  ('H',                        'Ve Home'),
  ```
- Vòng lặp: in mô tả, gửi lệnh, gửi W chờ hoàn thành
- Comment giải thích: cau truc du lieu tuple, tai sao can W giua cac lenh

### Giai đoạn 3: Giao diện GUI hoàn chỉnh

**File: `requirements.txt`**
```
pyserial>=3.5
```

**File: `config.py`**

Chức năng: Tham số cấu hình cho GUI.

Yêu cầu:
- BAUD_RATE = 115200
- TIMEOUT = 2
- INIT_DELAY = 2
- Danh sách JOINTS: mỗi khớp là dict {name, channel, min, max, home}
- Màu sắc GUI: BG_COLOR, SLIDER_COLOR, BTN_COLOR, LOG_COLOR
- Comment giải thích: tap trung cau hinh 1 noi de de thay doi

**File: `serial_comm.py`**

Chức năng: Module giao tiếp Serial — kết nối, gửi lệnh, nhận phản hồi, xử lý lỗi.

Yêu cầu:
- Class RobotSerial:
  - __init__(self): self.ser = None, self.connected = False
  - connect(self, port): mở Serial, sleep, đọc phản hồi khởi tạo, return True/False
  - disconnect(self): đóng Serial
  - send(self, cmd): gửi lệnh, đọc phản hồi, return response string
  - get_status(self): gửi 'T', parse kết quả "STA:a0,a1,..." thành list [a0,a1,...], return list int
  - is_connected(self): return self.connected
- Xử lý ngoại lệ: SerialException, timeout
- Comment giải thích: dong goi giao tiep Serial thanh class, xu ly loi

**File: `main.py`**

Chức năng: GUI Tkinter hoàn chỉnh điều khiển robot.

Yêu cầu:
- Import tkinter, tkinter.ttk, serial_comm, config
- Class RobotGUI:
  - Cửa sổ chính: title "Robot Arm 6DOF Controller", kích thước 700x500
  - **Panel kết nối** (trên cùng): Combobox chọn COM port (liệt kê port có sẵn bằng serial.tools.list_ports), nút Connect/Disconnect
  - **Panel điều khiển** (giữa): 6 thanh trượt Scale cho J0–J5, mỗi thanh có Label hiển thị tên khớp + góc hiện tại, phạm vi from_=min, to=max. Khi kéo thanh → gọi send("M <id> <angle>")
  - **Panel nút chức năng** (dưới thanh trượt): Nút HOME (gửi H, đồng bộ thanh trượt), Nút TRẠNG THÁI (gửi T, hiển thị log), Nút DỪNG KHẨN CẤP (gửi H ngay lập tức, màu đỏ)
  - **Panel Log** (dưới cùng): Text widget hiển thị lệnh gửi và phản hồi, auto-scroll
  - Khi Connect: gửi T để đồng bộ trạng thái, cập nhật giá trị thanh trượt
  - Khi kéo thanh trượt: dùng command callback của Scale, debounce bằng after() 100ms
- Liệt kê COM port: dùng `import serial.tools.list_ports; [p.device for p in serial.tools.list_ports.comports()]`
- Comment giải thích: cau truc GUI, event-driven, tai sao can debounce

---

## BÀI THỰC HÀNH 6: ĐIỀU KHIỂN SERVO THÔNG QUA HỆ THỐNG IOT

### Giai đoạn 1: Kết nối BLYNK cơ bản với 1 servo

**File: `platformio.ini`**
```
[env:esp32dev]
platform = espressif32
board = esp32dev
framework = arduino
upload_speed = 115200
monitor_speed = 115200
lib_deps =
    adafruit/Adafruit PWM Servo Driver Library
    blynkkk/Blynk
```

**File: `src/main.cpp`**

Chức năng: Kết nối ESP32 với BLYNK, điều khiển 1 servo (J0) từ xa qua Web Dashboard.

Yêu cầu:
- #define BLYNK_PRINT Serial, BLYNK_TEMPLATE_ID, BLYNK_TEMPLATE_NAME, BLYNK_AUTH_TOKEN (placeholder "YOUR_...")
- Include: WiFi.h, BlynkSimpleEsp32.h, Wire.h, Adafruit_PWMServoDriver.h
- Biến: ssid[], pass[] (placeholder), pca (driver), SERVOMIN=102, SERVOMAX=512
- Hàm angleToPulse(angle): map()
- Hàm moveServo(ch, angle): setPWM() + Serial print
- BLYNK_WRITE(V0): đọc param.asInt(), gọi moveServo(0, angle)
- setup(): Serial 115200, Wire.begin(21,22), pca.begin(), setPWMFreq(50), moveServo(0, 90), Blynk.begin()
- loop(): Blynk.run()
- Comment giải thích: BLYNK_WRITE la macro callback, luong du lieu Dashboard → Cloud → WiFi → ESP32 → PCA9685 → Servo, tai sao define phai truoc include

### Giai đoạn 2: Điều khiển toàn bộ 6 khớp qua IoT

**File: `src/main.cpp`**

Chức năng: Mở rộng điều khiển 6 khớp, thêm nút Home và hiển thị trạng thái.

Yêu cầu:
- Kế thừa GĐ1, bổ sung:
- Mảng homeAngles[6] = {90, 70, 90, 90, 90, 90}, currentAngles[6] tương tự
- BlynkTimer timer
- BLYNK_WRITE(V0) đến BLYNK_WRITE(V5): mỗi macro gọi moveServo(i, param.asInt())
- BLYNK_WRITE(V6) — nút Home: nếu param==1, lặp 6 khớp gọi moveServo(i, homeAngles[i]), rồi Blynk.virtualWrite(i, homeAngles[i]) để đồng bộ Slider, cuối cùng Blynk.virtualWrite(V7, "HOME")
- Hàm sendStatus(): tạo chuỗi "STA:a0,a1,...", gửi lên V7
- setup(): thêm timer.setInterval(3000L, sendStatus)
- loop(): thêm timer.run()
- Comment giải thích: BlynkTimer khac delay/millis, tai sao can dong bo Slider khi Home, co che push du lieu len cloud

### Giai đoạn 3: Hoàn thiện hệ thống IoT

**File: `src/main.cpp`**

Chức năng: Giống GĐ2, bổ sung ghi chú hướng dẫn kiểm thử Mobile và reconnect.

Yêu cầu:
- Code giống GĐ2 100% (BLYNK SDK tự hỗ trợ Mobile và reconnect)
- Bổ sung **block comment lớn** ở đầu file giải thích:
  ```
  // === GIAI DOAN 3: HOAN THIEN HE THONG IOT ===
  // 
  // Firmware nay GIONG HOAN TOAN Giai doan 2.
  // BLYNK SDK da tich hop san:
  //   1. Ho tro ca Web Dashboard va Mobile App (cung 1 firmware)
  //   2. Tu dong reconnect khi mat WiFi (Blynk.run() xu ly ben trong)
  //   3. Dong bo trang thai giua Web va Mobile theo thoi gian thuc
  //
  // HUONG DAN KIEM THU:
  //   - Cai BLYNK IoT app tren dien thoai (iOS/Android)
  //   - Dang nhap cung tai khoan BLYNK da tao
  //   - Mo Device "Robot Arm IoT" tren app
  //   - Keo Slider tren dien thoai -> robot phan hoi
  //   - Tat WiFi router 10 giay -> bat lai -> ESP32 tu reconnect
  //   - Mo ca Web + Mobile cung luc -> dong bo 2 chieu
  ```

---

## QUY TẮC BỔ SUNG

1. **Mỗi file đều bắt đầu bằng comment header:**
```cpp
// =============================================
// BAI THUC HANH 4 - GIAI DOAN 2
// File: config.h
// Mo ta: Khai bao cau hinh 6 khop Robot Arm
// =============================================
```

2. **Mỗi hàm đều có comment mô tả:**
```cpp
// ------------------------------------------
// Ham: servo_move
// Chuc nang: Di chuyen 1 khop den goc dich
//   - Clamp goc trong pham vi [min, max]
//   - Di chuyen muot: 1 do moi buoc
//   - Toc do dieu chinh bang delay
// Tham so:
//   joint: chi so khop (0-5)
//   targetAngle: goc dich (do)
// ------------------------------------------
void servo_move(int joint, int targetAngle) {
    ...
}
```

3. **Các giá trị placeholder cần thay thế được đánh dấu rõ:**
```cpp
char ssid[] = "TEN_WIFI_CUA_BAN";       // <-- THAY BANG TEN WIFI THUC TE
char pass[] = "MAT_KHAU_WIFI";           // <-- THAY BANG MAT KHAU WIFI
#define BLYNK_AUTH_TOKEN "YOUR_TOKEN"     // <-- THAY BANG AUTH TOKEN TU BLYNK
```

4. **Firmware phải xử lý lỗi đầu vào an toàn:** không crash khi nhận lệnh sai, luôn phản hồi "ERR:..." kèm mô tả lỗi.

5. **Python code tuân thủ PEP 8**, dùng f-string cho format, type hint cho hàm quan trọng.

6. **Tất cả file platformio.ini đều có upload_port = COM9 kèm ghi chú `; doi thanh COM port thuc te`.**

---

## KẾT QUẢ MONG ĐỢI

Sau khi hoàn thành, tổng cộng sẽ có:
- **Bài 4**: 3 thư mục giai đoạn, ~15 file mã nguồn C/C++
- **Bài 5**: 3 thư mục giai đoạn, ~6 file mã nguồn Python
- **Bài 6**: 3 thư mục giai đoạn, ~3 file mã nguồn C/C++ + 3 file platformio.ini

Tổng: ~27 file, mỗi file có comment đầy đủ, biên dịch/chạy được thực tế, phù hợp in trong giáo trình thực hành đại học.
