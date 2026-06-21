#include "hash_engine.h"
#include "kat_runner.h"
#include <iostream>
#include <string>

static void PrintUsage() {
    std::cerr << "Su dung: hashtool --algo <thuat_toan> --in <file> [--outlen <so_byte>] [--stream]\n";
    std::cerr << "         hashtool --kat <duong_dan_vectors.json>\n";
    std::cerr << "Vi du:   hashtool --algo sha256 --in test.txt\n";
    std::cerr << "         hashtool --kat vectors/sha256_kat.json\n";
}

int main(int argc, char* argv[]) {
    std::string algo, inputPath, katPath;
    int outlen = -1;
    bool streamFlag = false;

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--algo" && i + 1 < argc) {
            algo = argv[++i];
        } else if (arg == "--in" && i + 1 < argc) {
            inputPath = argv[++i];
        } else if (arg == "--outlen" && i + 1 < argc) {
            try {
                outlen = std::stoi(argv[++i]);
            } catch (...) {
                std::cerr << "Gia tri --outlen khong hop le.\n";
                return 1;
            }
        } else if (arg == "--stream") {
            streamFlag = true;
        } else if (arg == "--kat" && i + 1 < argc) {
            katPath = argv[++i];
        } else {
            std::cerr << "Tham so khong hop le: " << arg << "\n";
            PrintUsage();
            return 1;
        }
    }

    (void)streamFlag;

    if (!katPath.empty()) {
        bool allPassed = hashtool::RunKatFile(katPath);
        return allPassed ? 0 : 1;
    }

    if (algo.empty() || inputPath.empty()) {
        std::cerr << "Thieu tham so bat buoc (--algo hoac --in).\n";
        PrintUsage();
        return 1;
    }

    try {
        std::string digest = hashtool::HashFileHex(algo, inputPath, outlen);
        std::cout << digest << "  " << inputPath << "\n";
    } catch (const std::exception& e) {
        std::cerr << "Loi: " << e.what() << "\n";
        return 1;
    }

    return 0;
}
