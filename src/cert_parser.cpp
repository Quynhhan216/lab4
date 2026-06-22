#include "cert_parser.h"
#include <openssl/x509.h>
#include <openssl/x509v3.h>
#include <openssl/pem.h>
#include <openssl/bio.h>
#include <openssl/err.h>
#include <iostream>

namespace certtool {

static std::string Asn1TimeToString(const ASN1_TIME* t) {
    BIO* bio = BIO_new(BIO_s_mem());
    ASN1_TIME_print(bio, t);
    char* data = nullptr;
    long len = BIO_get_mem_data(bio, &data);
    std::string result(data, len);
    BIO_free(bio);
    return result;
}

static std::string X509NameToString(X509_NAME* name) {
    BIO* bio = BIO_new(BIO_s_mem());
    X509_NAME_print_ex(bio, name, 0, XN_FLAG_RFC2253);
    char* data = nullptr;
    long len = BIO_get_mem_data(bio, &data);
    std::string result(data, len);
    BIO_free(bio);
    return result;
}

bool PrintCertInfo(const std::string& pemPath) {
    BIO* bio = BIO_new_file(pemPath.c_str(), "r");
    if (!bio) {
        std::cerr << "Khong mo duoc file: " << pemPath << "\n";
        return false;
    }

    X509* cert = PEM_read_bio_X509(bio, nullptr, nullptr, nullptr);
    BIO_free(bio);

    if (!cert) {
        std::cerr << "Khong parse duoc X.509 certificate.\n";
        return false;
    }

    std::cout << "Subject: " << X509NameToString(X509_get_subject_name(cert)) << "\n";
    std::cout << "Issuer:  " << X509NameToString(X509_get_issuer_name(cert)) << "\n";

    const ASN1_TIME* notBefore = X509_get0_notBefore(cert);
    const ASN1_TIME* notAfter  = X509_get0_notAfter(cert);
    std::cout << "Valid from: " << Asn1TimeToString(notBefore) << "\n";
    std::cout << "Valid to:   " << Asn1TimeToString(notAfter) << "\n";

    EVP_PKEY* pubkey = X509_get_pubkey(cert);
    if (pubkey) {
        int bits = EVP_PKEY_bits(pubkey);
        const char* typeName = EVP_PKEY_get0_type_name(pubkey);
        std::cout << "Public Key: " << (typeName ? typeName : "(khong xac dinh)")
                   << ", " << bits << " bit\n";
        EVP_PKEY_free(pubkey);
    } else {
        std::cout << "Public Key: (khong doc duoc)\n";
    }

    int sigNid = X509_get_signature_nid(cert);
    std::cout << "Signature Algorithm: " << OBJ_nid2ln(sigNid) << "\n";

    int idx = X509_get_ext_by_NID(cert, NID_key_usage, -1);
    if (idx >= 0) {
        X509_EXTENSION* ext = X509_get_ext(cert, idx);
        BIO* extBio = BIO_new(BIO_s_mem());
        if (X509V3_EXT_print(extBio, ext, 0, 0)) {
            char* data = nullptr;
            long len = BIO_get_mem_data(extBio, &data);
            std::cout << "Key Usage: " << std::string(data, len) << "\n";
        }
        BIO_free(extBio);
    } else {
        std::cout << "Key Usage: (khong co extension nay)\n";
    }

    int sanIdx = X509_get_ext_by_NID(cert, NID_subject_alt_name, -1);
    if (sanIdx >= 0) {
        X509_EXTENSION* ext = X509_get_ext(cert, sanIdx);
        BIO* sanBio = BIO_new(BIO_s_mem());
        if (X509V3_EXT_print(sanBio, ext, 0, 0)) {
            char* data = nullptr;
            long len = BIO_get_mem_data(sanBio, &data);
            std::cout << "Subject Alternative Names: " << std::string(data, len) << "\n";
        }
        BIO_free(sanBio);
    } else {
        std::cout << "Subject Alternative Names: (khong co extension nay)\n";
    }

    X509_free(cert);
    return true;
}

} // namespace certtool
