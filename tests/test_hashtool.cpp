#include "catch_amalgamated.hpp"
#include "hash_engine.h"
#include <fstream>
#include <cstdio>

using hashtool::HashTextHex;
using hashtool::HashFileHex;

TEST_CASE("SHA-256 cua chuoi rong dung chuan NIST", "[sha256]") {
    REQUIRE(HashTextHex("sha256", "") ==
        "e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855");
}

TEST_CASE("SHA-256 cua \"abc\" dung chuan NIST", "[sha256]") {
    REQUIRE(HashTextHex("sha256", "abc") ==
        "ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad");
}

TEST_CASE("Do dai hex dau ra dung cho moi thuat toan SHA co kich thuoc co dinh", "[length]") {
    CHECK(HashTextHex("sha224", "abc").size() == 56);
    CHECK(HashTextHex("sha256", "abc").size() == 64);
    CHECK(HashTextHex("sha384", "abc").size() == 96);
    CHECK(HashTextHex("sha512", "abc").size() == 128);
    CHECK(HashTextHex("sha3-224", "abc").size() == 56);
    CHECK(HashTextHex("sha3-256", "abc").size() == 64);
    CHECK(HashTextHex("sha3-384", "abc").size() == 96);
    CHECK(HashTextHex("sha3-512", "abc").size() == 128);
}

TEST_CASE("SHAKE co do dai output dung theo --outlen", "[shake]") {
    CHECK(HashTextHex("shake128", "abc", 16).size() == 32);
    CHECK(HashTextHex("shake128", "abc", 32).size() == 64);
    CHECK(HashTextHex("shake256", "abc", 64).size() == 128);
}

TEST_CASE("Hash co tinh tat dinh: cung input ra cung output", "[deterministic]") {
    std::string h1 = HashTextHex("sha256", "quynhan");
    std::string h2 = HashTextHex("sha256", "quynhan");
    REQUIRE(h1 == h2);
}

TEST_CASE("Hash tu file va hash tu text phai khop voi noi dung giong nhau", "[file-vs-text]") {
    const std::string content = "noi dung test giong nhau";
    const std::string tmpPath = "tmp_test_file_vs_text.bin";

    {
        std::ofstream out(tmpPath, std::ios::binary);
        out << content;
    }

    std::string fromFile = HashFileHex("sha256", tmpPath);
    std::string fromText = HashTextHex("sha256", content);

    std::remove(tmpPath.c_str());

    REQUIRE(fromFile == fromText);
}

TEST_CASE("Thuat toan khong ho tro phai nem loi (fail closed)", "[negative]") {
    REQUIRE_THROWS_AS(HashTextHex("md5", "abc"), std::runtime_error);
}

TEST_CASE("SHAKE thieu --outlen phai nem loi (fail closed)", "[negative]") {
    REQUIRE_THROWS_AS(HashTextHex("shake128", "abc"), std::runtime_error);
}

TEST_CASE("Hash file khong ton tai phai nem loi (fail closed)", "[negative]") {
    REQUIRE_THROWS_AS(HashFileHex("sha256", "file_khong_ton_tai_12345.bin"), std::runtime_error);
}
