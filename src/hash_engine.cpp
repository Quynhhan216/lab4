#include "hash_engine.h"
#include <cryptopp/sha.h>
#include <cryptopp/sha3.h>
#include <cryptopp/shake.h>
#include <cryptopp/hex.h>
#include <cryptopp/files.h>
#include <cryptopp/filters.h>
#include <algorithm>
#include <stdexcept>
#include <cctype>

namespace hashtool {

static std::string ToLower(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(),
        [](unsigned char c) { return std::tolower(c); });
    return s;
}

template <typename HashAlgo>
static std::string HashFileWith(const std::string& filepath, int outputLenBytes) {
    HashAlgo hash;
    std::string digest;
    try {
        CryptoPP::FileSource(filepath.c_str(), true,
            new CryptoPP::HashFilter(hash,
                new CryptoPP::HexEncoder(new CryptoPP::StringSink(digest)),
                false, outputLenBytes));
    } catch (const CryptoPP::Exception& e) {
        throw std::runtime_error("Khong doc duoc file '" + filepath + "': " + e.what());
    }
    std::transform(digest.begin(), digest.end(), digest.begin(),
        [](unsigned char c) { return std::tolower(c); });
    return digest;
}

template <typename HashAlgo>
static std::string HashTextWith(const std::string& text, int outputLenBytes) {
    HashAlgo hash;
    std::string digest;
    CryptoPP::StringSource(text, true,
        new CryptoPP::HashFilter(hash,
            new CryptoPP::HexEncoder(new CryptoPP::StringSink(digest)),
            false, outputLenBytes));
    std::transform(digest.begin(), digest.end(), digest.begin(),
        [](unsigned char c) { return std::tolower(c); });
    return digest;
}

// LUU Y: them thuat toan moi thi phai sua CA HAI ham duoi day (File va Text),
// khong se bi lech tinh nang giua 2 duong nhap lieu.

std::string HashFileHex(const std::string& algoRaw,
                         const std::string& filepath,
                         int outputLenBytes) {
    std::string algo = ToLower(algoRaw);

    if (algo == "sha224")   return HashFileWith<CryptoPP::SHA224>(filepath, -1);
    if (algo == "sha256")   return HashFileWith<CryptoPP::SHA256>(filepath, -1);
    if (algo == "sha384")   return HashFileWith<CryptoPP::SHA384>(filepath, -1);
    if (algo == "sha512")   return HashFileWith<CryptoPP::SHA512>(filepath, -1);
    if (algo == "sha3-224") return HashFileWith<CryptoPP::SHA3_224>(filepath, -1);
    if (algo == "sha3-256") return HashFileWith<CryptoPP::SHA3_256>(filepath, -1);
    if (algo == "sha3-384") return HashFileWith<CryptoPP::SHA3_384>(filepath, -1);
    if (algo == "sha3-512") return HashFileWith<CryptoPP::SHA3_512>(filepath, -1);

    if (algo == "shake128") {
        if (outputLenBytes <= 0)
            throw std::runtime_error("SHAKE128 can --outlen (so byte), vi du --outlen 32");
        return HashFileWith<CryptoPP::SHAKE128>(filepath, outputLenBytes);
    }
    if (algo == "shake256") {
        if (outputLenBytes <= 0)
            throw std::runtime_error("SHAKE256 can --outlen (so byte), vi du --outlen 64");
        return HashFileWith<CryptoPP::SHAKE256>(filepath, outputLenBytes);
    }

    throw std::runtime_error("Thuat toan khong duoc ho tro: " + algoRaw);
}

std::string HashTextHex(const std::string& algoRaw,
                         const std::string& text,
                         int outputLenBytes) {
    std::string algo = ToLower(algoRaw);

    if (algo == "sha224")   return HashTextWith<CryptoPP::SHA224>(text, -1);
    if (algo == "sha256")   return HashTextWith<CryptoPP::SHA256>(text, -1);
    if (algo == "sha384")   return HashTextWith<CryptoPP::SHA384>(text, -1);
    if (algo == "sha512")   return HashTextWith<CryptoPP::SHA512>(text, -1);
    if (algo == "sha3-224") return HashTextWith<CryptoPP::SHA3_224>(text, -1);
    if (algo == "sha3-256") return HashTextWith<CryptoPP::SHA3_256>(text, -1);
    if (algo == "sha3-384") return HashTextWith<CryptoPP::SHA3_384>(text, -1);
    if (algo == "sha3-512") return HashTextWith<CryptoPP::SHA3_512>(text, -1);

    if (algo == "shake128") {
        if (outputLenBytes <= 0)
            throw std::runtime_error("SHAKE128 can outlen, vi du 32");
        return HashTextWith<CryptoPP::SHAKE128>(text, outputLenBytes);
    }
    if (algo == "shake256") {
        if (outputLenBytes <= 0)
            throw std::runtime_error("SHAKE256 can outlen, vi du 64");
        return HashTextWith<CryptoPP::SHAKE256>(text, outputLenBytes);
    }

    throw std::runtime_error("Thuat toan khong duoc ho tro: " + algoRaw);
}

} // namespace hashtool
