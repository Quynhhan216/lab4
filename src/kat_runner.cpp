#include "kat_runner.h"
#include "hash_engine.h"
#include <cryptopp/hex.h>
#include <cryptopp/filters.h>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <nlohmann/json.hpp>

namespace hashtool {

static std::string HexToBytes(const std::string& hex) {
    std::string raw;
    CryptoPP::StringSource(hex, true,
        new CryptoPP::HexDecoder(new CryptoPP::StringSink(raw)));
    return raw;
}

static std::string ToLowerStr(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(),
        [](unsigned char c) { return std::tolower(c); });
    return s;
}

bool RunKatFile(const std::string& jsonPath) {
    std::ifstream f(jsonPath);
    if (!f.is_open()) {
        std::cerr << "Khong mo duoc file KAT: " << jsonPath << "\n";
        return false;
    }

    nlohmann::json cases;
    try {
        f >> cases;
    } catch (const std::exception& e) {
        std::cerr << "File JSON loi cu phap: " << e.what() << "\n";
        return false;
    }

    int total = 0, passed = 0;

    for (const auto& tc : cases) {
        ++total;
        std::string algo = tc.value("algo", "");
        int outlen = tc.value("outlen", -1);
        std::string expected = ToLowerStr(tc.value("expected_hex", ""));

        std::string inputBytes;
        if (tc.contains("input_text")) {
            inputBytes = tc.value("input_text", "");
        } else if (tc.contains("input_hex")) {
            inputBytes = HexToBytes(tc.value("input_hex", ""));
        } else {
            std::cout << "[FAIL] case " << total << ": thieu input_text/input_hex\n";
            continue;
        }

        std::string actual;
        try {
            actual = ToLowerStr(HashTextHex(algo, inputBytes, outlen));
        } catch (const std::exception& e) {
            std::cout << "[FAIL] case " << total << " (" << algo << "): " << e.what() << "\n";
            continue;
        }

        if (actual == expected) {
            std::cout << "[PASS] case " << total << " (" << algo << ")\n";
            ++passed;
        } else {
            std::cout << "[FAIL] case " << total << " (" << algo << ")\n";
            std::cout << "       expected: " << expected << "\n";
            std::cout << "       actual:   " << actual << "\n";
        }
    }

    std::cout << "\nTong ket: " << passed << "/" << total << " PASS\n";
    return passed == total;
}

} // namespace hashtool
