#include "cert_parser.h"
#include <iostream>
#include <string>

int main(int argc, char* argv[]) {
    std::string inputPath, issuerPath;
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--in" && i + 1 < argc) inputPath = argv[++i];
        else if (arg == "--issuer" && i + 1 < argc) issuerPath = argv[++i];
        else {
            std::cerr << "Tham so khong hop le: " << arg << "\n";
            std::cerr << "Su dung: certtool --in <cert.pem> [--issuer <issuer_cert.pem>]\n";
            return 1;
        }
    }
    if (inputPath.empty()) {
        std::cerr << "Thieu tham so --in.\n";
        return 1;
    }
    bool ok = certtool::PrintCertInfo(inputPath, issuerPath);
    return ok ? 0 : 1;
}
