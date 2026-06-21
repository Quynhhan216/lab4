#pragma once
#include <string>

namespace hashtool {

// Doc file JSON chua danh sach test case, chay tung case, in PASS/FAIL ra man hinh.
// Tra ve true neu TAT CA case PASS, false neu co it nhat 1 case FAIL.
bool RunKatFile(const std::string& jsonPath);

} // namespace hashtool
