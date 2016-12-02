#ifndef _CRYPTOECC_OLDENCODING_H
#define _CRYPTOECC_OLDENCODING_H
/**
 * @file
 *
 * This file provide wrappers around ECC cryptographic algorithms.
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

#include <qcc/CryptoECC.h>

namespace qcc {

/**
 * The size of the ECC big value
 */
static const size_t ECC_BIGVAL_SZ = 9;

/**
 * The size of the ECC public key
 */
static const size_t ECC_PUBLIC_KEY_SZ = (2 * ECC_BIGVAL_SZ * sizeof(uint32_t)) + sizeof(uint32_t);

/**
 * The Old encoding ECC public key big endian byte array
 */
struct ECCPublicKeyOldEncoding {
    uint8_t data[ECC_PUBLIC_KEY_SZ];
};

typedef ECCPublicKeyOldEncoding ECCSecretOldEncoding;

/**
 * Elliptic Curve Cryptography Old Encoding
 */
class Crypto_ECC_OldEncoding {

  public:

    /**
     * Re encode public key to old encoding
     * @param newenc public key in current encoding
     * @param oldenc[out] public key in old encoding
     * @return
     *      ER_OK if the process succeeds
     *      ER_FAIL otherwise
     */
    static QStatus AJ_CALL ReEncode(const ECCPublicKey* newenc, ECCPublicKeyOldEncoding* oldenc);
    /**
     * Re encode old encoding public key to current encoding
     * @param oldenc public key in old encoding
     * @param newenc[out] public key in current encoding
     * @return
     *      ER_OK if the process succeeds
     *      ER_FAIL otherwise
     */
    static QStatus AJ_CALL ReEncode(const ECCPublicKeyOldEncoding* oldenc, ECCPublicKey* newenc);


    /**
     * Generates the Diffie-Hellman shared secret in the old encoding.
     * @param   ecc the Crypto_ECC object
     * @param   peerPublicKey the peer's public key
     * @param   secret the output shared secret
     * @return
     *      ER_OK if the shared secret is successfully generated.
     *      ER_FAIL otherwise
     *      Other error status.
     */
    static QStatus AJ_CALL GenerateSharedSecret(Crypto_ECC& ecc, const ECCPublicKey* peerPublicKey, ECCSecretOldEncoding* secret);

};

} /* namespace qcc */


#endif