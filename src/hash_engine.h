#pragma once
#include <string>

namespace hashtool {

// Hash mot file. outputLenBytes chi dung cho SHAKE128/256 (don vi: byte).
std::string HashFileHex(const std::string& algo,
                         const std::string& filepath,
                         int outputLenBytes = -1);

// Hash truc tiep mot chuoi byte trong RAM (dung cho --text va cho KAT runner).
std::string HashTextHex(const std::string& algo,
                         const std::string& text,
                         int outputLenBytes = -1);

} // namespace hashtool
