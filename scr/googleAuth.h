#ifndef GOOGLEAUTH_H
#define GOOGLEAUTH_H


#include <iostream>
#include <stdio.h>
#include <string.h>
#include <openssl/hmac.h>
//#include <openssl/evp.h>
#include <time.h>
#include <math.h>
#include <cstring>
#include <string>
#include <stdint.h>
#include <stdlib.h>


using namespace std;

class GoogleAuth
{
private:
    char key[20];
    size_t len = 0;
    uint8_t* key32 = nullptr;
    size_t key32len = 0;
    int8_t base32_vals[256] =
    {
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        14, 11, 26, 27, 28, 29, 30, 31,  1, -1, -1, -1, -1,  0, -1, -1,
        -1,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14,
        15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, -1, -1, -1, -1, -1,
        -1,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14,
        15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, -1, -1, -1, -1,
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    };

public:
    GoogleAuth(std::string key_)
    {
        strncpy_s(this->key, key_.c_str(), sizeof(key_) - 1);
        this->len = strlen(this->key);

        this->key32 = (uint8_t*)this->key;
        this->key32len = this->decode_b32key(&this->key32, this->len);
    }

    std::string get()
    {
        time_t utime = this->getTime();

        uint8_t* hash{};
        uint32_t result;
        uint64_t offset;
        uint32_t truncH;

        hash = (uint8_t*)HMAC(EVP_sha1(), key32, key32len, (const unsigned char*)&utime, sizeof(utime), NULL, 0);
       // hmac_sha1(key32, key32len,(const unsigned char*)&utime, sizeof(utime), hash,NULL );
        offset = hash[19] & 0x0f;

        truncH =
            (hash[offset] & 0x7f) << 24 |
            (hash[offset + 1] & 0xff) << 16 |
            (hash[offset + 2] & 0xff) << 8 |
            (hash[offset + 3] & 0xff);

        result = truncH % 1000000;

        char strNum[64];
        sprintf_s(strNum, 64, "%06d", result);        // 转换为字符串，不足6位，前面补0

        return strNum;
    }

private:
    time_t getTime()
    {
        time_t utime = (floor((unsigned long)time(NULL) / 30));
        uint32_t endian;

        endian = 0xdeadbeef;
        if ((*(const uint8_t*)&endian) == 0xef) {
            utime = ((utime & 0x00000000ffffffff) << 32) | ((utime & 0xffffffff00000000) >> 32);
            utime = ((utime & 0x0000ffff0000ffff) << 16) | ((utime & 0xffff0000ffff0000) >> 16);
            utime = ((utime & 0x00ff00ff00ff00ff) << 8) | ((utime & 0xff00ff00ff00ff00) >> 8);
        };

        return utime;
    }

    size_t decode_b32key(uint8_t** k, size_t len)
    {
        size_t keylen;
        size_t pos;
        keylen = 0;
        for (pos = 0; pos <= (len - 8); pos += 8)
        {
            (*k)[keylen + 0] = (this->base32_vals[(*k)[pos + 0]] << 3) & 0xF8; // 5 MSB
            (*k)[keylen + 0] |= (this->base32_vals[(*k)[pos + 1]] >> 2) & 0x07; // 3 LSB
            if ((*k)[pos + 2] == '=') {
                keylen += 1;
                break;
            }

            //byte 1
            (*k)[keylen + 1] = (this->base32_vals[(*k)[pos + 1]] << 6) & 0xC0; // 2 MSB
            (*k)[keylen + 1] |= (this->base32_vals[(*k)[pos + 2]] << 1) & 0x3E; // 5  MB
            (*k)[keylen + 1] |= (this->base32_vals[(*k)[pos + 3]] >> 4) & 0x01; // 1 LSB
            if ((*k)[pos + 4] == '=') {
                keylen += 2;
                break;
            }

            //byte 2
            (*k)[keylen + 2] = (this->base32_vals[(*k)[pos + 3]] << 4) & 0xF0; // 4 MSB
            (*k)[keylen + 2] |= (this->base32_vals[(*k)[pos + 4]] >> 1) & 0x0F; // 4 LSB
            if ((*k)[pos + 5] == '=') {
                keylen += 3;
                break;
            }

            //byte 3
            (*k)[keylen + 3] = (this->base32_vals[(*k)[pos + 4]] << 7) & 0x80; // 1 MSB
            (*k)[keylen + 3] |= (this->base32_vals[(*k)[pos + 5]] << 2) & 0x7C; // 5  MB
            (*k)[keylen + 3] |= (this->base32_vals[(*k)[pos + 6]] >> 3) & 0x03; // 2 LSB
            if ((*k)[pos + 7] == '=') {
                keylen += 4;
                break;
            }

            //byte 4
            (*k)[keylen + 4] = (this->base32_vals[(*k)[pos + 6]] << 5) & 0xE0; // 3 MSB
            (*k)[keylen + 4] |= (this->base32_vals[(*k)[pos + 7]] >> 0) & 0x1F; // 5 LSB
            keylen += 5;
        }
        (*k)[keylen] = 0;

        return keylen;
    }

};

std::string GAget(std::string key)
{
    GoogleAuth* GoogleAUTH = new GoogleAuth(key);
    std::string password = GoogleAUTH->get();
    return password;
}
#endif
