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
static std::string HashFileWithRaw(const std::string& filepath, int outputLenBytes) {
    HashAlgo hash;
    std::string digest;
    try {
        CryptoPP::FileSource(filepath.c_str(), true,
            new CryptoPP::HashFilter(hash, new CryptoPP::StringSink(digest), false, outputLenBytes));
    } catch (const CryptoPP::Exception& e) {
        throw std::runtime_error("Khong doc duoc file '" + filepath + "': " + e.what());
    }
    return digest;
}

template <typename HashAlgo>
static std::string HashTextWithRaw(const std::string& text, int outputLenBytes) {
    HashAlgo hash;
    std::string digest;
    CryptoPP::StringSource(text, true,
        new CryptoPP::HashFilter(hash, new CryptoPP::StringSink(digest), false, outputLenBytes));
    return digest;
}

static std::string RawToHex(const std::string& raw) {
    std::string hex;
    CryptoPP::StringSource(raw, true,
        new CryptoPP::HexEncoder(new CryptoPP::StringSink(hex)));
    std::transform(hex.begin(), hex.end(), hex.begin(),
        [](unsigned char c) { return std::tolower(c); });
    return hex;
}

std::string HashFileRaw(const std::string& algoRaw, const std::string& filepath, int outputLenBytes) {
    std::string algo = ToLower(algoRaw);
    if (algo == "sha224")   return HashFileWithRaw<CryptoPP::SHA224>(filepath, -1);
    if (algo == "sha256")   return HashFileWithRaw<CryptoPP::SHA256>(filepath, -1);
    if (algo == "sha384")   return HashFileWithRaw<CryptoPP::SHA384>(filepath, -1);
    if (algo == "sha512")   return HashFileWithRaw<CryptoPP::SHA512>(filepath, -1);
    if (algo == "sha3-224") return HashFileWithRaw<CryptoPP::SHA3_224>(filepath, -1);
    if (algo == "sha3-256") return HashFileWithRaw<CryptoPP::SHA3_256>(filepath, -1);
    if (algo == "sha3-384") return HashFileWithRaw<CryptoPP::SHA3_384>(filepath, -1);
    if (algo == "sha3-512") return HashFileWithRaw<CryptoPP::SHA3_512>(filepath, -1);
    if (algo == "shake128") {
        if (outputLenBytes <= 0) throw std::runtime_error("SHAKE128 can --outlen (so byte)");
        return HashFileWithRaw<CryptoPP::SHAKE128>(filepath, outputLenBytes);
    }
    if (algo == "shake256") {
        if (outputLenBytes <= 0) throw std::runtime_error("SHAKE256 can --outlen (so byte)");
        return HashFileWithRaw<CryptoPP::SHAKE256>(filepath, outputLenBytes);
    }
    throw std::runtime_error("Thuat toan khong duoc ho tro: " + algoRaw);
}

std::string HashTextRaw(const std::string& algoRaw, const std::string& text, int outputLenBytes) {
    std::string algo = ToLower(algoRaw);
    if (algo == "sha224")   return HashTextWithRaw<CryptoPP::SHA224>(text, -1);
    if (algo == "sha256")   return HashTextWithRaw<CryptoPP::SHA256>(text, -1);
    if (algo == "sha384")   return HashTextWithRaw<CryptoPP::SHA384>(text, -1);
    if (algo == "sha512")   return HashTextWithRaw<CryptoPP::SHA512>(text, -1);
    if (algo == "sha3-224") return HashTextWithRaw<CryptoPP::SHA3_224>(text, -1);
    if (algo == "sha3-256") return HashTextWithRaw<CryptoPP::SHA3_256>(text, -1);
    if (algo == "sha3-384") return HashTextWithRaw<CryptoPP::SHA3_384>(text, -1);
    if (algo == "sha3-512") return HashTextWithRaw<CryptoPP::SHA3_512>(text, -1);
    if (algo == "shake128") {
        if (outputLenBytes <= 0) throw std::runtime_error("SHAKE128 can outlen");
        return HashTextWithRaw<CryptoPP::SHAKE128>(text, outputLenBytes);
    }
    if (algo == "shake256") {
        if (outputLenBytes <= 0) throw std::runtime_error("SHAKE256 can outlen");
        return HashTextWithRaw<CryptoPP::SHAKE256>(text, outputLenBytes);
    }
    throw std::runtime_error("Thuat toan khong duoc ho tro: " + algoRaw);
}

std::string HashFileHex(const std::string& algo, const std::string& filepath, int outputLenBytes) {
    return RawToHex(HashFileRaw(algo, filepath, outputLenBytes));
}

std::string HashTextHex(const std::string& algo, const std::string& text, int outputLenBytes) {
    return RawToHex(HashTextRaw(algo, text, outputLenBytes));
}

} // namespace hashtool
