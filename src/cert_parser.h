#pragma once
#include <string>

namespace certtool {

// pemPath: chung chi can kiem tra.
// issuerPemPath: chung chi cua issuer (co the trung voi pemPath neu la self-signed),
//                de trong ("") neu khong co.
// Tra ve true neu doc/parse/verify thanh cong theo dung fail-closed semantics.
bool PrintCertInfo(const std::string& pemPath, const std::string& issuerPemPath = "");

} // namespace certtool
