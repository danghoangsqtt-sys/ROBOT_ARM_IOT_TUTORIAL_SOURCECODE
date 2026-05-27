# 🤖 Giáo Trình Thực Hành: Cánh Tay Robot 6DOF với ESP32 & IoT

![PlatformIO](https://img.shields.io/badge/PlatformIO-orange?style=for-the-badge&logo=PlatformIO)
![C++](https://img.shields.io/badge/C++-00599C?style=for-the-badge&logo=c%2B%2B&logoColor=white)
![Python](https://img.shields.io/badge/Python-3776AB?style=for-the-badge&logo=python&logoColor=white)
![Blynk](https://img.shields.io/badge/Blynk_IoT-17b6a4?style=for-the-badge&logo=blynk&logoColor=white)

Chào mừng đến với kho lưu trữ mã nguồn mẫu cho **Giáo trình Thực hành Điều khiển Cánh tay Robot 6DOF**. Dự án này cung cấp toàn bộ mã nguồn C/C++ (cho vi điều khiển ESP32) và Python (cho giao diện máy tính) từ cơ bản đến nâng cao, kết hợp cùng hệ sinh thái Internet of Things (Blynk).

---

## 📸 1. Tổng quan Phần cứng

Dưới đây là các linh kiện phần cứng được sử dụng trong giáo trình:

<div align="center">
  <img src="docs/images/cac-thanh-phan-linh-kien.png" alt="Sơ đồ bố trí linh kiện" width="600"/>
  <p><i>Sơ đồ bố trí tổng thể các linh kiện của Cánh tay Robot</i></p>
</div>

### Danh sách chi tiết:

| Linh kiện | Chức năng chính |
| :--- | :--- |
| **ESP32 DevKit V1** | Vi điều khiển trung tâm, xử lý logic, đọc Serial và kết nối WiFi/Blynk. |
| **Module PCA9685** | Giao tiếp I2C, xuất xung PWM 16 kênh điều khiển cùng lúc nhiều Servo. |
| **Cánh tay Robot 6DOF** | Cơ cấu chấp hành hợp kim nhôm, gồm 6 động cơ Servo MG996R/MG995. |
| **Nguồn Adapter 6V - 15A** | Cung cấp dòng điện đủ lớn cho 6 Servo MG996R/MG995 hoạt động đồng thời ở tải đầy đủ mà không sập nguồn. (**Không dùng nguồn 3A** — dòng điện khóa cứng (stall current) mỗi servo có thể lên đến 2.5A, tổng cộng ~15A cho 6 servo.) |

---

## ⚙️ 2. Sơ đồ kết nối (Wiring)

<div align="center">
  <img src="docs/images/so-do-dau-noi-phan-cung.png" alt="Sơ đồ kết nối phần cứng" width="700"/>
  <p><i>Sơ đồ chi tiết kết nối các chân tín hiệu và nguồn điện</i></p>
</div>

Hệ thống sử dụng giao thức **I2C** để kết nối giữa ESP32 và PCA9685:

*   **ESP32 `GPIO 21`** $\rightarrow$ PCA9685 `SDA`
*   **ESP32 `GPIO 22`** $\rightarrow$ PCA9685 `SCL`
*   **ESP32 `GND`** $\rightarrow$ PCA9685 `GND`
*   **ESP32 `3V3`** $\rightarrow$ PCA9685 `VCC` (Cấp nguồn logic)
*   **Nguồn ngoài 6V/15A** $\rightarrow$ Cổng Terminal màu xanh của PCA9685 (Nguồn riêng cho Servo, chú ý **V+ / GND**). ⚠️ *Dùng ít nhất 15A — mỗi servo có thể kéo đến 2.5A khi bị khóa cứng.*

---

## 📂 3. Cấu trúc Dự án

Dự án được thiết kế theo dạng **Multi-root Workspace** của VS Code, chia thành 3 Bài thực hành lớn, mỗi bài có 3 giai đoạn tiến triển:

```text
ROBOT_ARM_IOT_TUTORIAL_SOURCECODE/
├── RobotArmTutorial.code-workspace   <-- [MỞ FILE NÀY TRONG VS CODE]
├── robot-arm-source-code/
│   ├── bai-thuc-hanh-4/              <-- Lập trình C++ (PlatformIO)
│   │   ├── giai-doan-1/              (Quét I2C)
│   │   ├── giai-doan-2/              (Điều khiển Servo cơ bản, Serial CLI)
│   │   └── giai-doan-3/              (Smooth move, di chuyển đồng thời)
│   ├── bai-thuc-hanh-5/              <-- Lập trình Python GUI
│   │   ├── giai-doan-1/              (Giao tiếp Serial cơ bản)
│   │   ├── giai-doan-2/              (Tự động Pick-and-Place)
│   │   └── giai-doan-3/              (Giao diện Tkinter hoàn chỉnh)
│   └── bai-thuc-hanh-6/              <-- Lập trình IoT (Blynk)
│       ├── giai-doan-1/              (Kết nối WiFi, điều khiển 1 khớp)
│       ├── giai-doan-2/              (Điều khiển 6 khớp, đồng bộ trạng thái)
│       └── giai-doan-3/              (Tối ưu hóa, Mobile App & Auto Reconnect)
```

---

## 🚀 4. Hướng dẫn Sử dụng

### Dành cho Lập trình viên C/C++ (PlatformIO)
1. Cài đặt **Visual Studio Code** và Extension **PlatformIO IDE**.
2. Mở VS Code $\rightarrow$ `File` $\rightarrow$ `Open Workspace from File...` $\rightarrow$ Chọn file **`RobotArmTutorial.code-workspace`**.
3. Tại thanh trạng thái (Status Bar) bên dưới, chọn môi trường của Giai đoạn bạn muốn nạp code.
4. Mở file `platformio.ini` tương ứng và sửa `upload_port = COM9` thành cổng COM thực tế của bạn.
5. Bấm nút **Upload (mũi tên phải)** để nạp code xuống ESP32.

### Dành cho Giao diện Python
1. Cài đặt Python 3.8+.
2. Mở Terminal tại thư mục `bai-thuc-hanh-5/giai-doan-3`.
3. Cài đặt thư viện: `pip install -r requirements.txt` (chủ yếu là `pyserial`).
4. Chạy GUI: `python main.py`

---

## 📜 5. Giao thức Điều khiển (Serial Protocol)

Firmware hỗ trợ các tập lệnh văn bản thuần túy (Text-based) qua cổng Serial ở tốc độ **115200 baud**, kết thúc bằng ký tự ngắt dòng `\n`.

| Lệnh | Ý nghĩa | Cú pháp gửi | Phản hồi |
| :--- | :--- | :--- | :--- |
| **M** | Di chuyển 1 khớp | `M <joint_id> <angle>` (vd: `M 0 90`) | `OK` |
| **G** | Đọc góc khớp | `G <joint_id>` | `VAL:<id>:<angle>` |
| **T** | Đọc trạng thái tổng | `T` | `STA:a0,a1,a2,a3,a4,a5` |
| **H** | Về Home | `H` (Tất cả) hoặc `H <joint_id>` | `OK` |
| **S** | Đổi tốc độ | `S <joint_id> <speed>` (Speed 1-10) | `OK` |
| **A** | Di chuyển đồng thời | `A <a0> <a1> <a2> <a3> <a4> <a5>` | `OK` |
| **W** | Chờ di chuyển xong | `W` | `DONE` |

---

## ⚡ 6. PWM & Điều khiển Servo — Tài liệu Kỹ thuật

Phần này giải thích cách tín hiệu PWM được dùng để điều khiển servo thông qua module **PCA9685**.

### 6.1 Nguyên lý tín hiệu PWM

Servo RC tiêu chuẩn được điều khiển bằng tín hiệu **Điều chế Độ rộng Xung (PWM)**:

| Tham số | Giá trị | Ghi chú |
| :--- | :--- | :--- |
| **Tần số PWM** | **50 Hz** | Chu kỳ = 20 ms (tiêu chuẩn cho tất cả servo RC) |
| **Độ rộng xung tối thiểu** | **0.5 ms** | Tương ứng với góc **0°** |
| **Độ rộng xung tối đa** | **2.5 ms** | Tương ứng với góc **180°** |
| **Xung trung tính** | **1.5 ms** | Tương ứng với góc **90°** (vị trí Home) |

```
   Chu kỳ 20 ms (50 Hz)
  ┌──────────────────────────────────────────┐
  │                                          │
──┘◄0.5ms►┐                         Cao = 0°  (SERVOMIN)
  │       │
──┘◄─────1.5ms─────►┐               Cao = 90° (trung tính)
  │                  │
──┘◄──────────2.5ms──────────►┐     Cao = 180°(SERVOMAX)
```

### 6.2 Độ phân giải Ticks của PCA9685

PCA9685 hoạt động với **độ phân giải 12-bit** (4096 bước mỗi chu kỳ).
Với tần số 50 Hz (chu kỳ 20 ms), mỗi tick = 20 ms ÷ 4096 ≈ **4.88 µs**.

| Góc | Độ rộng xung | Ticks PCA9685 |
| :---: | :---: | :---: |
| **0°** | 0.5 ms | **102** ticks |
| **90°** | 1.5 ms | **307** ticks |
| **180°** | 2.5 ms | **512** ticks |

> **Cách tính 102 và 512:**
> - `SERVOMIN = 0.5 ms / (20 ms / 4096) = 0.5 / 0.00488 ≈ 102`
> - `SERVOMAX = 2.5 ms / (20 ms / 4096) = 2.5 / 0.00488 ≈ 512`

### 6.3 Công thức Góc → Độ rộng Xung

Để di chuyển servo đến bất kỳ góc nào từ 0° đến 180°, firmware ánh xạ góc sang số ticks của PCA9685:

```cpp
// Ánh xạ tuyến tính: góc [0°, 180°] → số ticks [SERVOMIN, SERVOMAX]
int pulse = map(angle, 0, 180, SERVOMIN, SERVOMAX);
//  trong đó: SERVOMIN = 102   (xung 0.5 ms → 0°)
//            SERVOMAX = 512   (xung 2.5 ms → 180°)

pca9685.setPWM(kenh, 0, pulse);  // Gửi lệnh tới PCA9685
```

**Ví dụ tính toán:**

| Góc mục tiêu | Công thức | Ticks | Độ rộng xung |
| :---: | :--- | :---: | :---: |
| 0° | `map(0,   0, 180, 102, 512)` | 102 | 0.50 ms |
| 45° | `map(45,  0, 180, 102, 512)` | 204 | 0.99 ms |
| 90° | `map(90,  0, 180, 102, 512)` | 307 | 1.50 ms |
| 135° | `map(135, 0, 180, 102, 512)` | 409 | 2.00 ms |
| 180° | `map(180, 0, 180, 102, 512)` | 512 | 2.50 ms |

### 6.4 Yêu cầu Nguồn điện

> [!WARNING]
> **Tuyệt đối không cấp nguồn 6 servo từ nguồn 3A.**
> Mỗi servo MG996R/MG995 có thể kéo đến **~2.5A khi bị khóa cứng (stall)**. Với 6 servo, dòng đỉnh có thể lên đến **~15A**. Nguồn yếu sẽ gây chuyển động giật cục, sụt áp và ESP32 bị reset.

| Tình huống | Dòng điện tiêu thụ | Khuyến nghị |
| :--- | :--- | :--- |
| 1 servo (không tải) | ~150 mA | — |
| 1 servo (đang chạy) | ~500–900 mA | — |
| 1 servo (khóa cứng) | ~2.5 A | — |
| **6 servo (tải đầy)** | **lên đến ~15 A** | **Dùng nguồn 6V / 15A** |

---

*Tài liệu được thiết kế chuyên biệt cho công tác giảng dạy hệ thống Nhúng & IoT. Chúc các bạn sinh viên học tập tốt!*
