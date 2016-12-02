/**
 * @file CryptoHash.cc
 *
 * Windows platform-specific implementation for hash function classes from Crypto.h
 */

/******************************************************************************
 *  * 
 *    Copyright (c) 2016 Open Connectivity Foundation and AllJoyn Open
 *    Source Project Contributors and others.
 *    
 *    All rights reserved. This program and the accompanying materials are
 *    made available under the terms of the Apache License, Version 2.0
 *    which accompanies this distribution, and is available at
 *    http://www.apache.org/licenses/LICENSE-2.0

 ******************************************************************************/

#include <windows.h>
#include <bcrypt.h>

#include <qcc/platform.h>
#include <qcc/Debug.h>
#include <qcc/Crypto.h>
#include <qcc/Util.h>

#include <Status.h>

#include <qcc/CngCache.h>

using namespace std;
using namespace qcc;

#define QCC_MODULE "CRYPTO"

namespace qcc {

class Crypto_Hash::Context {
  public:

    Context(size_t digestSize) : digestSize(digestSize), handle(0), hashObj(NULL) { }

    ~Context() {
        if (handle) {
            BCryptDestroyHash(handle);
        }
        delete [] hashObj;
    }

    size_t digestSize;
    BCRYPT_HASH_HANDLE handle;
    uint8_t* hashObj;
    DWORD hashObjLen;
  private:
    /**
     * Private copy constructor to prevent copying
     *
     * @param src Context to be copied.
     */
    Context(const Context&);
    /**
     * Assignment operator
     *
     * @param src source Context
     *
     * @return copy of Context
     */
    Context& operator=(const Context&);
};

QStatus Crypto_Hash::Init(Algorithm alg, const uint8_t* hmacKey, size_t keyLen)
{
    QStatus status = ER_OK;

    if (ctx) {
        delete ctx;
        ctx = NULL;
        initialized = false;
    }

    MAC = hmacKey != NULL;

    if (MAC && (keyLen == 0)) {
        status = ER_CRYPTO_ERROR;
        QCC_LogError(status, ("HMAC key length cannot be zero"));
        return status;
    }

    switch (alg) {
    case qcc::Crypto_Hash::SHA1:
        ctx = new Context(SHA1_SIZE);
        break;

    case qcc::Crypto_Hash::SHA256:
        ctx = new Context(SHA256_SIZE);
        break;

    default:
        return ER_BAD_ARG_1;
    }

    // Open algorithm provider if required
    if (!cngCache.algHandles[alg][MAC]) {
        status = cngCache.OpenHashHandle(alg, MAC);
        if (ER_OK != status) {
            delete ctx;
            ctx = NULL;
            return status;
        }
    }

    // Get length of hash object and allocate the object
    DWORD got;
    if (!BCRYPT_SUCCESS(BCryptGetProperty(cngCache.algHandles[alg][MAC], BCRYPT_OBJECT_LENGTH, (PBYTE)&ctx->hashObjLen, sizeof(DWORD), &got, 0))) {
        status = ER_CRYPTO_ERROR;
        QCC_LogError(status, ("Failed to get object length property"));
        delete ctx;
        ctx = NULL;
        return status;
    }

    ctx->hashObj = new uint8_t[ctx->hashObjLen];

    if (!BCRYPT_SUCCESS(BCryptCreateHash(cngCache.algHandles[alg][MAC], &ctx->handle, ctx->hashObj, ctx->hashObjLen, (PUCHAR)hmacKey, (ULONG)keyLen, 0))) {
        status = ER_CRYPTO_ERROR;
        QCC_LogError(status, ("Failed to create hash"));
        delete ctx;
        ctx = NULL;
    }

    if (status == ER_OK) {
        initialized = true;
    }
    return status;
}

Crypto_Hash::~Crypto_Hash(void)
{
    if (ctx) {
        delete ctx;
    }
}

QStatus Crypto_Hash::Update(const uint8_t* buf, size_t bufSize)
{
    QStatus status = ER_OK;

    if (!buf) {
        return ER_BAD_ARG_1;
    }
    if (initialized) {
        if (!BCRYPT_SUCCESS(BCryptHashData(ctx->handle, (PUCHAR)buf, bufSize, 0))) {
            status = ER_CRYPTO_ERROR;
            QCC_LogError(status, ("Updating hash digest"));
        }
    } else {
        status = ER_CRYPTO_HASH_UNINITIALIZED;
        QCC_LogError(status, ("Hash function not initialized"));
    }
    return status;
}

QStatus Crypto_Hash::Update(const qcc::String& str)
{
    return Update((const uint8_t*)str.data(), str.size());
}

QStatus Crypto_Hash::Update(const vector<uint8_t, SecureAllocator<uint8_t> >& d)
{
    return Update(d.data(), d.size());
}

QStatus Crypto_Hash::GetDigest(uint8_t* digest, bool keepAlive)
{
    QStatus status = ER_OK;

    if (!digest) {
        return ER_BAD_ARG_1;
    }
    if (initialized) {
        /* keep alive is not allowed for HMAC */
        if (MAC && keepAlive) {
            status = ER_CRYPTO_ERROR;
            QCC_LogError(status, ("Keep alive is not allowed for HMAC"));
            keepAlive = false;
        }
        Context* keep = NULL;
        /* To keep the hash alive we need to copy the context before calling BCryptFinishHash */
        if (keepAlive) {
            keep = new Context(ctx->digestSize);
            keep->hashObjLen = ctx->hashObjLen;
            keep->hashObj = new uint8_t[ctx->hashObjLen];
            if (!BCRYPT_SUCCESS(BCryptDuplicateHash(ctx->handle, &keep->handle, keep->hashObj, keep->hashObjLen, 0))) {
                status = ER_CRYPTO_ERROR;
                QCC_LogError(status, ("Failed to create hash"));
                delete keep;
                keep = NULL;
            }
        }
        if (!BCRYPT_SUCCESS(BCryptFinishHash(ctx->handle, digest, ctx->digestSize, 0))) {
            status = ER_CRYPTO_ERROR;
            QCC_LogError(status, ("Finalizing hash digest"));
        }
        if (keep) {
            delete ctx;
            ctx = keep;
        } else {
            initialized = false;
        }
    } else {
        status = ER_CRYPTO_HASH_UNINITIALIZED;
        QCC_LogError(status, ("Hash function not initialized"));
    }
    return status;
}

}