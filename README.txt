- Define hàm nhanh:
Extension: C/C++ Advanced Lint
Có thêm tính năng auto move function implementation
Link: https://marketplace.visualstudio.com/items?itemName=jbenden.c-cpp-flylint
Bấm tổ hợp phím:
▶️ Ctrl + . (chấm) → hiện menu hành động
hoặc chuột phải → chọn "Refactor..."

- Chuyển file .h -> cpp: Alt + o

- build projet: pio run -v
- build và nap: pio run -t upload
- config: pio run -t menuconfig
- clear build: pio run -t clean
- xóa flash: pio run -t erase

* partitions
1. Phân vùng tối giản chỉ một app (“factory” only, không OTA, không SPIFFS)
Nếu bạn không dùng OTA cập nhật và không cần SPIFFS, chỉ giữ NVS + app:
csv
# Name,   Type, SubType, Offset,   Size,    Flags
nvs,      data, nvs,     0x9000,   0x6000,
otadata,  data, ota,     0xF000,   0x2000,
factory,  app,  factory, 0x11000,  0xEF000,
NVS (24 KiB) đủ để lưu config.
OTADATA (8 KiB) chuẩn.
Factory app chiếm hết phần còn lại: 0xEF000 ≈ 960 KiB.
2. Phân vùng với 2 OTA và SPIFFS dung lượng nhỏ
Giữ 2 slot OTA, nhưng chỉ reserve 64 KiB cho SPIFFS:
csv
# Name,    Type, SubType, Offset,   Size,     Flags
nvs,       data, nvs,     0x9000,   0x6000,
otadata,   data, ota,     0xF000,   0x2000,
app0,      app,  ota_0,   0x11000,  0xB0000,  
app1,      app,  ota_1,   0xC1000,  0xB0000,
spiffs,    data, spiffs,  0x162000, 0x10000,
app0/app1 mỗi slot ~704 KiB (0xB0000)
SPIFFS 64 KiB (0x10000)
Tổng flash 1 048 576 B chia đều cho OTA+SPIFFS.

Hướng dẫn áp dụng
Tạo file partitions.csv ở gốc project với nội dung theo 1 trong 2 mẫu trên.
Mở platformio.ini, thêm hoặc sửa:
ini
[env:esp32dev]
framework = espidf
board_build.partitions = partitions.csv
Clean & build lại:
bash
pio run --target clean
pio run


Phím tắt nhanh:
1. Format code: shirt + alt +Factory
2. impliment hàm: Ctrl + .
3. Chuyển file .h và .cpp: Alt + o
5. Tìm kiếm: Ctrl + shift + F