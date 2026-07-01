#pragma once
#include <string>

namespace hashtool {

std::string HashFileHex(const std::string& algo, const std::string& filepath, int outputLenBytes = -1);
std::string HashTextHex(const std::string& algo, const std::string& text, int outputLenBytes = -1);

// Tra ve du lieu hash duoi dang RAW BYTES (khong ma hoa), dung de --encode tu chon
// hex/base64/raw o lop CLI.
std::string HashFileRaw(const std::string& algo, const std::string& filepath, int outputLenBytes = -1);
std::string HashTextRaw(const std::string& algo, const std::string& text, int outputLenBytes = -1);

} // namespace hashtool
