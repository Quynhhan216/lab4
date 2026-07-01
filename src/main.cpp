#include "hash_engine.h"
#include "kat_runner.h"
#include <cryptopp/base64.h>
#include <cryptopp/hex.h>
#include <cryptopp/filters.h>
#include <iostream>
#include <fstream>
#include <string>

static void PrintUsage() {
    std::cerr << "Su dung: hashtool --algo <thuat_toan> [--in <file> | --text <chuoi>]\n";
    std::cerr << "                  [--out <file>] [--outlen <so_byte>] [--encode hex|base64|raw]\n";
    std::cerr << "                  [--stream] | --kat <vectors.json>\n";
    std::cerr << "Vi du:   hashtool --algo sha256 --in test.txt\n";
    std::cerr << "         hashtool --algo sha256 --text \"abc\"\n";
    std::cerr << "         hashtool --algo sha256 --in test.txt --out digest.bin --encode raw\n";
    std::cerr << "         hashtool --algo shake128 --outlen 32 --in test.txt --encode base64\n";
}

static std::string EncodeBytes(const std::string& raw, const std::string& encoding) {
    if (encoding == "raw") return raw;
    if (encoding == "hex") {
        std::string hex;
        CryptoPP::StringSource(raw, true, new CryptoPP::HexEncoder(new CryptoPP::StringSink(hex)));
        for (auto& c : hex) c = (char)std::tolower((unsigned char)c);
        return hex;
    }
    if (encoding == "base64") {
        std::string b64;
        CryptoPP::StringSource(raw, true, new CryptoPP::Base64Encoder(new CryptoPP::StringSink(b64), false));
        return b64;
    }
    throw std::runtime_error("Gia tri --encode khong hop le: " + encoding + " (chi nhan hex|base64|raw)");
}

int main(int argc, char* argv[]) {
    std::string algo, inputPath, inputText, outPath, katPath, encoding;
    int outlen = -1;
    bool hasIn = false, hasText = false, hasEncode = false;
    bool streamFlag = false;

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--algo" && i + 1 < argc) algo = argv[++i];
        else if (arg == "--in" && i + 1 < argc) { inputPath = argv[++i]; hasIn = true; }
        else if (arg == "--text" && i + 1 < argc) { inputText = argv[++i]; hasText = true; }
        else if (arg == "--out" && i + 1 < argc) outPath = argv[++i];
        else if (arg == "--outlen" && i + 1 < argc) {
            try { outlen = std::stoi(argv[++i]); }
            catch (...) { std::cerr << "Gia tri --outlen khong hop le.\n"; return 1; }
        }
        else if (arg == "--encode" && i + 1 < argc) { encoding = argv[++i]; hasEncode = true; }
        else if (arg == "--stream") streamFlag = true;
        else if (arg == "--kat" && i + 1 < argc) katPath = argv[++i];
        else { std::cerr << "Tham so khong hop le: " << arg << "\n"; PrintUsage(); return 1; }
    }
    (void)streamFlag;

    if (!katPath.empty()) {
        bool ok = hashtool::RunKatFile(katPath);
        return ok ? 0 : 1;
    }

    if (hasIn == hasText) {
        std::cerr << "Phai chon dung mot trong hai: --in HOAC --text (khong dung ca hai, khong bo trong).\n";
        PrintUsage();
        return 1;
    }
    if (algo.empty()) {
        std::cerr << "Thieu tham so bat buoc --algo.\n";
        PrintUsage();
        return 1;
    }

    if (!hasEncode) encoding = outPath.empty() ? "hex" : "raw";

    try {
        std::string raw = hasIn
            ? hashtool::HashFileRaw(algo, inputPath, outlen)
            : hashtool::HashTextRaw(algo, inputText, outlen);

        std::string encoded = EncodeBytes(raw, encoding);

        if (!outPath.empty()) {
            std::ofstream out(outPath, std::ios::binary);
            if (!out.is_open()) {
                std::cerr << "Khong ghi duoc file --out: " << outPath << "\n";
                return 1;
            }
            out.write(encoded.data(), (std::streamsize)encoded.size());
            std::cerr << "Da ghi ket qua vao: " << outPath << " (encode=" << encoding << ")\n";
        } else {
            if (encoding == "raw") {
                std::cerr << "Canh bao: --encode raw khong danh de in ra man hinh, hay dung --out.\n";
                return 1;
            }
            std::cout << encoded << "\n";
        }
    } catch (const std::exception& e) {
        std::cerr << "Loi: " << e.what() << "\n";
        return 1;
    }

    return 0;
}
