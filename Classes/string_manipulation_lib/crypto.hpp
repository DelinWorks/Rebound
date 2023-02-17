#ifndef __CRYPTO_H__
#define __CRYPTO_H__

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include <openssl/aes.h>
#include <openssl/evp.h>

namespace crypto
{
    class AESCrypt
    {
    public:
        unsigned char* key_data;
        int key_data_len;

        AESCrypt(std::string key)
        {

        }

        ~AESCrypt()
        {
        }

        void handleErrors(void)
        {
            //ERR_print_errors_fp(stderr);
            abort();
        }

        int aes_encrypt(unsigned char* plaintext, int plaintext_len, unsigned char* key, unsigned char* iv, unsigned char* ciphertext)
        {
            EVP_CIPHER_CTX* ctx;

            int len;
            int ciphertext_len;

            if (!(ctx = EVP_CIPHER_CTX_new()))
                handleErrors();

            if (1 != EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv))
                handleErrors();

            if (1 != EVP_EncryptUpdate(ctx, ciphertext, &len, plaintext, plaintext_len))
                handleErrors();

            ciphertext_len = len;

            if (1 != EVP_EncryptFinal_ex(ctx, ciphertext + len, &len))
                handleErrors();

            ciphertext_len += len;

            EVP_CIPHER_CTX_free(ctx);

            return ciphertext_len;
        }

        int aes_decrypt(unsigned char* ciphertext, int ciphertext_len, unsigned char* key, unsigned char* iv, unsigned char* plaintext)
        {
            EVP_CIPHER_CTX* ctx;

            int len;
            int plaintext_len;

            if (!(ctx = EVP_CIPHER_CTX_new()))
                handleErrors();

            if (1 != EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv))
                handleErrors();

            if (1 != EVP_DecryptUpdate(ctx, plaintext, &len, ciphertext, ciphertext_len))
                handleErrors();
            plaintext_len = len;

            if (1 != EVP_DecryptFinal_ex(ctx, plaintext + len, &len))
                handleErrors();
            plaintext_len += len;

            EVP_CIPHER_CTX_free(ctx);

            return plaintext_len;
        }

        unsigned char* encrypt(char* plain)
        {
            unsigned char* key = (unsigned char*)"01234567890123456789012345678901";
            unsigned char* iv = (unsigned char*)"0123456789012345";

            unsigned char* decryptedtext;

            int decryptedtext_len = strlen((char*)decryptedtext), ciphertext_len;

            strncpy((char*)decryptedtext, plain, decryptedtext_len);

            unsigned char* ciphertext = (unsigned char*)malloc(decryptedtext_len);

            ciphertext_len = aes_encrypt(decryptedtext, decryptedtext_len, key, iv, ciphertext);

            return ciphertext;
        }

        char* decrypt(unsigned char* cipher)
        {
            unsigned char* key = (unsigned char*)"01234567890123456789012345678901";
            unsigned char* iv = (unsigned char*)"0123456789012345";

            unsigned char* ciphertext = cipher;

            int decryptedtext_len, ciphertext_len = strlen((char*)ciphertext);

            unsigned char* decryptedtext = (unsigned char*)malloc(ciphertext_len);

            decryptedtext_len = aes_decrypt(ciphertext, ciphertext_len, key, iv, decryptedtext);

            return (char*)decryptedtext;
        }

        ///**
        //* Create a 256 bit key and IV using the supplied key_data. salt can be added for taste.
        //* Fills in the encryption and decryption ctx objects and returns 0 on success
        //**/
        //int aes_init(unsigned char* key_data, int key_data_len, unsigned char* salt, EVP_CIPHER_CTX* e_ctx, EVP_CIPHER_CTX* d_ctx)
        //{
        //    int i, nrounds = 5;
        //    unsigned char key[32], iv[32];

        //    /*
        //     * Gen key & IV for AES 256 CBC mode. A SHA1 digest is used to hash the supplied key material.
        //     * nrounds is the number of times the we hash the material. More rounds are more secure but
        //     * slower.
        //     */
        //    i = EVP_BytesToKey(EVP_aes_256_cbc(), EVP_sha1(), salt, key_data, key_data_len, nrounds, key, iv);
        //    if (i != 32) {
        //        printf("Key size is %d bits - should be 256 bits\n", i);
        //        return -1;
        //    }

        //    EVP_CIPHER_CTX_init(e_ctx);
        //    EVP_EncryptInit_ex(e_ctx, EVP_aes_256_cbc(), NULL, key, iv);
        //    EVP_CIPHER_CTX_init(d_ctx);
        //    EVP_DecryptInit_ex(d_ctx, EVP_aes_256_cbc(), NULL, key, iv);

        //    en = e_ctx;
        //    de = d_ctx;

        //    return 0;
        //}

        ///*
        // * Encrypt *len bytes of data
        // * All data going in & out is considered binary (unsigned char[])
        // */
        //unsigned char* aes_encrypt(EVP_CIPHER_CTX* e, unsigned char* plaintext, int* len)
        //{
        //    /* max ciphertext len for a n bytes of plaintext is n + AES_BLOCK_SIZE -1 bytes */
        //    int c_len = *len + AES_BLOCK_SIZE, f_len = 0;
        //    unsigned char* ciphertext = (unsigned char*)malloc(c_len);

        //    /* allows reusing of 'e' for multiple encryption cycles */
        //    EVP_EncryptInit_ex(e, NULL, NULL, NULL, NULL);

        //    /* update ciphertext, c_len is filled with the length of ciphertext generated,
        //      *len is the size of plaintext in bytes */
        //    EVP_EncryptUpdate(e, ciphertext, &c_len, plaintext, *len);

        //    /* update ciphertext with the final remaining bytes */
        //    EVP_EncryptFinal_ex(e, ciphertext + c_len, &f_len);

        //    *len = c_len + f_len;
        //    return ciphertext;
        //}

        ///*
        // * Decrypt *len bytes of ciphertext
        // */
        //unsigned char* aes_decrypt(EVP_CIPHER_CTX* e, unsigned char* ciphertext, int* len)
        //{
        //    /* plaintext will always be equal to or lesser than length of ciphertext*/
        //    int p_len = *len, f_len = 0;
        //    unsigned char* plaintext = (unsigned char*)malloc(p_len);

        //    EVP_DecryptInit_ex(e, NULL, NULL, NULL, NULL);
        //    EVP_DecryptUpdate(e, plaintext, &p_len, ciphertext, *len);
        //    EVP_DecryptFinal_ex(e, plaintext + p_len, &f_len);

        //    *len = p_len + f_len;
        //    return plaintext;
        //}

        //char* encrypt(char* data)
        //{
        //    unsigned char* plaintext = (unsigned char*)(data);
        //    int size = strlen(data) + 1;
        //    unsigned char* ciphertext = aes_encrypt(en, plaintext, &size);
        //    return (char*)(ciphertext);
        //}

        //char* decrypt(char* data)
        //{
        //    unsigned char* ciphertext = (unsigned char*)(data);
        //    int size = strlen((char*)ciphertext) + 1;
        //    unsigned char* plaintext = aes_decrypt(de, ciphertext, &size);
        //    return (char*)(plaintext);
        //}

        //// Easy to use but slow, consider using POD-like types for easier computation times (c_encrypt)
        //// The diffrence wont be noticable because its in microsecond level (roughly ~6 microseconds diffrence)
        //// But it's a good practice to optimize your slow ass code :>
        //void encrypt(std::string& data)
        //{
        //    unsigned char* plaintext = (unsigned char*)data.c_str();
        //    int size = strlen(data.c_str()) + 1;
        //    unsigned char* ciphertext = aes_encrypt(en, plaintext, &size);
        //    data = std::string((char*)ciphertext);
        //}

        //// Easy to use but slow, consider using POD-like types for easier computation times (c_decrypt)
        //// The diffrence wont be noticable because its in microsecond level (roughly ~6 microseconds diffrence)
        //// But it's a good practice to optimize your slow ass code :>
        //void decrypt(std::string& data)
        //{
        //    unsigned char* ciphertext = (unsigned char*)data.c_str();
        //    int size = strlen(data.c_str()) + 1;
        //    unsigned char* plaintext = aes_decrypt(de, ciphertext, &size);
        //    data = std::string((char*)plaintext);
        //}

        //void encrypt(std::vector<uint8_t> &data)
        //{
        //    //unsigned char* plaintext = (unsigned char*)malloc(0);
        //    //std::copy(data.begin(), data.end(), plaintext);
        //    //int size = data.size() + 1;
        //    //unsigned char* ciphertext = aes_encrypt(en, plaintext, &size);
        //    //data.clear();
        //    //data = std::vector<uint8_t>(ciphertext, ciphertext + strlen((char*)ciphertext) + 1);

        //    std::string sdata(data.begin(), data.end());
        //    encrypt(sdata);
        //    data.clear();
        //    data = std::vector<uint8_t>(sdata.begin(), sdata.end());
        //}
        //
        //void decrypt(std::vector<uint8_t>& data)
        //{
        //    //unsigned char* ciphertext = (unsigned char*)malloc(0);
        //    //std::copy(data.begin(), data.end(), ciphertext);
        //    //int size = data.size() + 1;
        //    //unsigned char* plaintext = aes_decrypt(de, ciphertext, &size);
        //    //data.clear();
        //    //data = std::vector<uint8_t>(plaintext, plaintext + strlen((char*)plaintext) + 1);

        //    std::string sdata(data.begin(), data.end());
        //    decrypt(sdata);
        //    data.clear();
        //    data = std::vector<uint8_t>(sdata.begin(), sdata.end());
        //}
    };
}

#endif
