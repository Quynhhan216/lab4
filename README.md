# Lab 4 — Hashing, PKI, and Practical Attacks

Bài tập môn Mật mã và Ứng dụng. Bao gồm 2 CLI tool (`hashtool`, `certtool`),
1 benchmark suite, và 2 demo tấn công thực tế (MD5 collision, length-extension).

## 1. Tổng quan các thành phần

| Thành phần | Mô tả | Thư viện |
|---|---|---|
| `hashtool` | Hash file/text bằng SHA-224/256/384/512, SHA3-224/256/384/512, SHAKE128/256. Hỗ trợ streaming I/O, KAT runner. | Crypto++ |
| `certtool` | Trích xuất thông tin X.509 certificate (Subject, Issuer, Validity, Public Key, Signature Algorithm, Key Usage, SAN). | OpenSSL |
| `bench_hash` | Benchmark throughput 4 thuật toán hash, xuất CSV (mean/median/stdev/95% CI). | Crypto++ |
| `hashtool_tests` | Unit test (Catch2) cho `hash_engine`. | Catch2 (amalgamated) |

## 2. Dependencies

| Thư viện | Version đã test | Ghi chú |
|---|---|---|
| CMake | ≥ 3.16 | Build system |
| g++ | 13.3.0 (Ubuntu), 15.2.0 (MinGW64) | Cần hỗ trợ C++17 |
| Crypto++ | 8.9.0 | Hashing |
| OpenSSL | 3.0.13 (Ubuntu), 3.6.1 (MSYS2) | X.509 parsing |
| nlohmann/json | qua package `nlohmann-json3-dev` | Parse file KAT |
| Catch2 | v3.9.0 (amalgamated, tải qua wget) | Unit test |

## 3. Cài đặt

### Ubuntu 24.04 LTS
\`\`\`bash
sudo apt update
sudo apt install -y build-essential cmake git \\
    libcrypto++-dev libcrypto++-utils \\
    libssl-dev nlohmann-json3-dev
\`\`\`

### Windows 11 (MSYS2 MinGW64)
Mở terminal **MSYS2 MinGW64** (không phải MSYS2 thường):
\`\`\`bash
pacman -S --needed \\
    mingw-w64-x86_64-toolchain \\
    mingw-w64-x86_64-cmake \\
    mingw-w64-x86_64-crypto++ \\
    mingw-w64-x86_64-openssl \\
    mingw-w64-x86_64-nlohmann-json
\`\`\`

### Catch2 (cả 2 OS, vì không có package ổn định cho amalgamated header)
\`\`\`bash
mkdir -p tests
cd tests
wget https://raw.githubusercontent.com/catchorg/Catch2/v3.9.0/extras/catch_amalgamated.hpp
wget https://raw.githubusercontent.com/catchorg/Catch2/v3.9.0/extras/catch_amalgamated.cpp
cd ..
\`\`\`

## 4. Build (CMake, out-of-source)

\`\`\`bash
mkdir -p build && cd build
cmake ..              # Windows MinGW64: cmake .. -G "MinGW Makefiles"
cmake --build .
\`\`\`

Build ra 4 executable: \`hashtool\`, \`certtool\`, \`bench_hash\`, \`hashtool_tests\`.

## 5. Cấu trúc project

\`\`\`
lab4/
├── CMakeLists.txt
├── README.md
├── bench_results.csv          # ket qua benchmark (Phase 11)
├── src/
│   ├── main.cpp                  # CLI cua hashtool
│   ├── hash_engine.h / .cpp       # loi tinh hash
│   ├── kat_runner.h / .cpp        # chay vector test JSON
│   ├── certtool_main.cpp          # CLI cua certtool
│   └── cert_parser.h / .cpp       # parse X.509 (OpenSSL)
├── tests/
│   ├── test_hashtool.cpp
│   └── catch_amalgamated.hpp / .cpp
├── bench/
│   ├── bench_hash.cpp
│   ├── plot_results.py
│   └── throughput_chart.png
├── vectors/
│   └── sha256_kat.json
├── attacks/
│   ├── md5_collision/            # Phase 9
│   └── length_extension/         # Phase 10
└── tls/
    └── nginx_config_snippet.conf # Phase 8
\`\`\`

## 6. Ví dụ sử dụng CLI

### hashtool
\`\`\`bash
./hashtool --algo sha256 --in file.bin
./hashtool --algo shake128 --outlen 32 --in file.bin   # outlen tinh BANG BYTE
./hashtool --algo sha512 --in large.iso --stream
./hashtool --kat ../vectors/sha256_kat.json
\`\`\`

### certtool
\`\`\`bash
./certtool --in cert.pem
\`\`\`

### Benchmark
\`\`\`bash
./bench_hash > ../bench_results.csv
\`\`\`

### Unit test
\`\`\`bash
ctest --output-on-failure
\`\`\`

## 7. Known Limitations (hạn chế biết trước)

- **`certtool` chưa verify chữ ký certificate bằng public key của issuer** —
  chỉ trích xuất và hiển thị thông tin (subject, issuer, validity, public key,
  signature algorithm, key usage, SAN). Phần xác minh mã hoá (`X509_verify`)
  chưa được triển khai.
- `--outlen` của SHAKE128/256 quy ước tính **bằng byte** (không phải bit) —
  quyết định thiết kế riêng vì đề không ghi rõ đơn vị.
- `hashclash` và `hashpump` (công cụ tấn công ở Phase 9, 10) chỉ build và test
  trên Ubuntu — không phải lỗi cross-platform của `hashtool`/`certtool`, vì
  2 tool này là công cụ tấn công mượn ngoài, không phải deliverable chính.
- Repo gốc `bwall/HashPump` đã bị tác giả xoá; dùng bản mirror
  `timb-machine-mirrors/bwall-HashPump` thay thế.
- TLS deployment (Phase 8) dùng DNS-01 challenge (không HTTP-01) vì mạng nhà
  không port-forward được; certificate là Let's Encrypt ECDSA thật, nhưng
  server được test qua ánh xạ `/etc/hosts` về localhost, không public 24/7.

## 8. Tham khảo / Credits

- Crypto++ — https://www.cryptopp.com
- OpenSSL — https://www.openssl.org
- Catch2 — https://github.com/catchorg/Catch2
- hashclash (Marc Stevens) — https://github.com/cr-marcstevens/hashclash
- HashPump (bwall) — mirror: https://github.com/timb-machine-mirrors/bwall-HashPump
- Let's Encrypt / DuckDNS — chứng chỉ TLS công khai cho Phase 8
