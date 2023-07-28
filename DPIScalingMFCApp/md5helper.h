#pragma once

//Code from: https://stackoverflow.com/a/13259720/981766

#include <Wincrypt.h>
#include <sstream>


std::string GetHashText(const void* data, const size_t data_size)
{
    HCRYPTPROV hProv = NULL;

    if (!CryptAcquireContext(&hProv, NULL, NULL, PROV_RSA_AES, CRYPT_VERIFYCONTEXT)) {
        return "";
    }

    BOOL hash_ok = FALSE;
    HCRYPTPROV hHash = NULL;
    
    hash_ok = CryptCreateHash(hProv, CALG_MD5, 0, 0, &hHash);

    if (!hash_ok) {
        CryptReleaseContext(hProv, 0);
        return "";
    }

    if (!CryptHashData(hHash, static_cast<const BYTE*>(data), data_size, 0)) {
        CryptDestroyHash(hHash);
        CryptReleaseContext(hProv, 0);
        return "";
    }

    DWORD cbHashSize = 0, dwCount = sizeof(DWORD);
    if (!CryptGetHashParam(hHash, HP_HASHSIZE, (BYTE*)&cbHashSize, &dwCount, 0)) {
        CryptDestroyHash(hHash);
        CryptReleaseContext(hProv, 0);
        return "";
    }

    std::vector<BYTE> buffer(cbHashSize);
    if (!CryptGetHashParam(hHash, HP_HASHVAL, reinterpret_cast<BYTE*>(&buffer[0]), &cbHashSize, 0)) {
        CryptDestroyHash(hHash);
        CryptReleaseContext(hProv, 0);
        return "";
    }

    std::ostringstream oss;

    for (std::vector<BYTE>::const_iterator iter = buffer.begin(); iter != buffer.end(); ++iter) {
        oss.fill('0');
        oss.width(2);
        oss << std::hex << static_cast<const int>(*iter);
    }

    CryptDestroyHash(hHash);
    CryptReleaseContext(hProv, 0);
    return oss.str();
}
