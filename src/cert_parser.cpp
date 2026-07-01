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

static X509* LoadCertFromFile(const std::string& path) {
    BIO* bio = BIO_new_file(path.c_str(), "r");
    if (!bio) return nullptr;
    X509* cert = PEM_read_bio_X509(bio, nullptr, nullptr, nullptr);
    BIO_free(bio);
    return cert;
}

// Kiem tra "TBS structure integrity" va "algorithm consistency" khi KHONG co
// issuer public key de verify chu ky day du.
static bool CheckStructureAndAlgorithmConsistency(X509* cert) {
    unsigned char* tbsDer = nullptr;
    int tbsLen = i2d_re_X509_tbs(cert, &tbsDer);
    bool structureOk = (tbsLen > 0 && tbsDer != nullptr);
    if (tbsDer) OPENSSL_free(tbsDer);

    const X509_ALGOR* outerAlg = nullptr;
    X509_get0_signature(nullptr, &outerAlg, cert);
    const X509_ALGOR* tbsAlg = X509_get0_tbs_sigalg(cert);

    bool algorithmConsistent = (outerAlg != nullptr && tbsAlg != nullptr &&
                                 X509_ALGOR_cmp(outerAlg, tbsAlg) == 0);

    std::cout << "  - TBS structure integrity: " << (structureOk ? "OK" : "THAT BAI") << "\n";
    std::cout << "  - Algorithm consistency (TBS vs outer): "
               << (algorithmConsistent ? "OK" : "THAT BAI (khong khop)") << "\n";

    return structureOk && algorithmConsistent;
}

bool PrintCertInfo(const std::string& pemPath, const std::string& issuerPemPath) {
    X509* cert = LoadCertFromFile(pemPath);
    if (!cert) {
        std::cerr << "Khong doc/parse duoc X.509 certificate: " << pemPath << "\n";
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

    bool result = true;

    if (!issuerPemPath.empty()) {
        X509* issuerCert = LoadCertFromFile(issuerPemPath);
        if (!issuerCert) {
            std::cerr << "Signature Verification: THAT BAI (khong doc duoc issuer cert: "
                      << issuerPemPath << ")\n";
            X509_free(cert);
            return false;
        }
        EVP_PKEY* issuerPubKey = X509_get_pubkey(issuerCert);
        if (!issuerPubKey) {
            std::cerr << "Signature Verification: THAT BAI (khong doc duoc public key cua issuer)\n";
            X509_free(issuerCert);
            X509_free(cert);
            return false;
        }

        int verifyRc = X509_verify(cert, issuerPubKey);
        if (verifyRc == 1) {
            std::cout << "Signature Verification: HOP LE (da xac minh bang public key cua issuer)\n";
            result = true;
        } else {
            std::cout << "Signature Verification: KHONG HOP LE (chu ky khong khop voi issuer duoc cung cap)\n";
            result = false;
        }

        EVP_PKEY_free(issuerPubKey);
        X509_free(issuerCert);
    } else {
        std::cout << "Signature Verification: KHONG THE XAC MINH DAY DU (khong co issuer public key)\n";
        std::cout << "  Chi kiem tra duoc cac dieu kien sau (khong thay the cho xac minh chu ky):\n";
        bool structOk = CheckStructureAndAlgorithmConsistency(cert);
        result = structOk;
    }

    X509_free(cert);
    return result;
}

} // namespace certtool
