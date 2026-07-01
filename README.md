# Lab 4 — Hashing, PKI, and Practical Attacks

Bài tập môn Mật mã và Ứng dụng (Thầy Tú). Gồm hai công cụ dòng lệnh
(`hashtool`, `certtool`), một bộ đo hiệu năng, và hai bài minh hoạ tấn công
thực tế trên hàm băm cũ (MD5 collision, length-extension attack), kèm một lần
triển khai HTTPS thật bằng chứng chỉ công khai.

## 1. Tổng quan các thành phần

| Thành phần | Mô tả | Thư viện |
|---|---|---|
| `hashtool` | Băm file/text bằng SHA-224/256/384/512, SHA3-224/256/384/512, SHAKE128/256. Hỗ trợ streaming I/O và bộ chạy KAT (Known Answer Test). | Crypto++ |
| `certtool` | Trích xuất thông tin chứng chỉ X.509 (Subject, Issuer, thời hạn, public key, thuật toán ký, Key Usage, Subject Alternative Name). | OpenSSL |
| `bench_hash` | Đo throughput bốn thuật toán SHA-256/SHA-512/SHA3-256/SHA3-512, xuất CSV có mean/median/stdev/95% CI. | Crypto++ |
| `hashtool_tests` | Unit test cho `hash_engine` (9 test case, 18 assertion). | Catch2 (bản amalgamated) |

## 2. Dependencies

| Thư viện | Version đã test | Ghi chú |
|---|---|---|
| CMake | ≥ 3.16 | Build system, out-of-source |
| g++ | 13.3.0 (Ubuntu), 15.2.0 (MinGW64) | Cần hỗ trợ C++17 |
| Crypto++ | 8.9.0 | Hashing (`hashtool`, `bench_hash`) |
| OpenSSL | 3.0.13 (Ubuntu), 3.6.1 (MSYS2) | X.509 parsing (`certtool`) |
| nlohmann/json | qua package `nlohmann-json3-dev` | Parse file KAT |
| Catch2 | v3.9.0 (amalgamated, tải qua wget) | Unit test |
| Python 3 + matplotlib + pandas | — | Vẽ biểu đồ benchmark (tuỳ chọn, không cần để build) |

## 3. Cài đặt

### Ubuntu 24.04 LTS

```bash
sudo apt update
sudo apt install -y build-essential cmake git \
    libcrypto++-dev libcrypto++-utils \
    libssl-dev nlohmann-json3-dev
```

### Windows 11 (MSYS2 MinGW64)

Mở terminal **MSYS2 MinGW64** (không phải MSYS2 thường hay PowerShell/CMD):

```bash
pacman -S --needed \
    mingw-w64-x86_64-toolchain \
    mingw-w64-x86_64-cmake \
    mingw-w64-x86_64-crypto++ \
    mingw-w64-x86_64-openssl \
    mingw-w64-x86_64-nlohmann-json
```

### Catch2 (cả hai OS, vì không có package ổn định cho bản amalgamated)

```bash
mkdir -p tests
cd tests
wget https://raw.githubusercontent.com/catchorg/Catch2/v3.9.0/extras/catch_amalgamated.hpp
wget https://raw.githubusercontent.com/catchorg/Catch2/v3.9.0/extras/catch_amalgamated.cpp
cd ..
```

## 4. Build (CMake, out-of-source)

```bash
mkdir -p build && cd build
cmake ..              # Windows MinGW64: cmake .. -G "MinGW Makefiles"
cmake --build .
```

Build ra bốn executable: `hashtool`, `certtool`, `bench_hash`, `hashtool_tests`.

## 5. Cấu trúc project

```
lab4/
├── CMakeLists.txt
├── README.md
├── bench_results.csv               # Ket qua benchmark tren Ubuntu
├── bench_results_windows.csv       # Ket qua benchmark tren Windows
│
├── src/
│   ├── main.cpp                    # CLI cua hashtool
│   ├── hash_engine.h
│   ├── hash_engine.cpp             # Loi tinh hash (10 thuat toan)
│   ├── kat_runner.h
│   ├── kat_runner.cpp              # Chay vector test tu file JSON
│   ├── certtool_main.cpp           # CLI cua certtool
│   ├── cert_parser.h
│   └── cert_parser.cpp             # Parse X.509 bang OpenSSL
│
├── tests/
│   ├── test_hashtool.cpp           # 9 TEST_CASE, 18 assertion
│   ├── catch_amalgamated.hpp       # Catch2 v3.9.0 (tai qua wget)
│   └── catch_amalgamated.cpp
│
├── bench/
│   ├── bench_hash.cpp              # Chuong trinh do throughput
│   ├── plot_results.py             # Ve bieu do 1 OS (Ubuntu)
│   ├── plot_compare_os.py          # Ve bieu do so sanh Ubuntu vs Windows
│   ├── throughput_chart.png
│   └── throughput_compare_os.png
│
├── vectors/
│   └── sha256_kat.json             # Vector KAT: 2 vector FIPS 180-2 + 1 tu kiem
│
├── attacks/
│   ├── md5_collision/
│   │   ├── collision_program1.cpp  # Hai chuong trinh C++ co MD5 trung nhau
│   │   └── collision_program2.cpp
│   └── length_extension/
│       ├── original_input.bin      # key || message goc
│       ├── attack_output.txt       # MAC gia mao + message gia mao (hashpump)
│       ├── forged_message.bin
│       └── verify_input.bin
│
├── tls/
│   └── nginx_config_snippet.conf   # Cau hinh Nginx dung cho Phase TLS
│
└── docs/
    └── Lab4_BaoCao.docx            # Bao cao day du (muc tieu, thiet ke,
                                     # kiem thu, hieu nang, bao mat, phu luc)
```

## 6. Ví dụ sử dụng CLI

### hashtool

```bash
./hashtool --algo sha256 --in file.bin
./hashtool --algo shake128 --outlen 32 --in file.bin   # outlen tinh BANG BYTE
./hashtool --algo sha512 --in large.iso --stream
./hashtool --kat ../vectors/sha256_kat.json
```

### certtool

```bash
./certtool --in cert.pem
```

### Benchmark

```bash
./bench_hash > ../bench_results.csv
python3 ../bench/plot_results.py          # ve bieu do 1 OS
python3 ../bench/plot_compare_os.py       # ve bieu do so sanh 2 OS (can ca 2 file CSV)
```

### Unit test

```bash
ctest --output-on-failure
```

## 7. Known Limitations (hạn chế biết trước)

- **`certtool` chưa verify chữ ký chứng chỉ bằng public key của issuer** —
  hiện chỉ trích xuất và hiển thị thông tin (Subject, Issuer, Validity,
  Public Key, Signature Algorithm, Key Usage, SAN). Hàm `X509_verify` chưa
  được gọi.
- `--outlen` của SHAKE128/256 quy ước tính **bằng byte**, không phải bit —
  quyết định thiết kế riêng vì đề không quy định đơn vị cụ thể.
- `hashclash` và `hashpump` (công cụ tấn công dùng ở phần MD5 collision và
  length-extension) chỉ build và chạy trên Ubuntu, không phải lỗi
  cross-platform của `hashtool`/`certtool` vì đây là công cụ mượn ngoài,
  không phải deliverable chính.
- Repo gốc `bwall/HashPump` đã bị tác giả xoá; dùng bản mirror
  `timb-machine-mirrors/bwall-HashPump` thay thế.
- Triển khai TLS dùng DNS-01 challenge (không phải HTTP-01) vì mạng thực
  hiện lab không port-forward được. Chứng chỉ là Let's Encrypt ECDSA thật,
  nhưng server được kiểm thử qua ánh xạ domain về `127.0.0.1` trong file
  hosts, không public 24/7.

## 8. Tham khảo / Credits

- Crypto++ — <https://www.cryptopp.com>
- OpenSSL — <https://www.openssl.org>
- Catch2 — <https://github.com/catchorg/Catch2>
- hashclash (Marc Stevens) — <https://github.com/cr-marcstevens/hashclash>
- HashPump (bwall) — mirror: <https://github.com/timb-machine-mirrors/bwall-HashPump>
- Let's Encrypt / DuckDNS — chứng chỉ TLS công khai cho phần triển khai HTTPS
