#ifndef _ALLJOYN_AUTHMECHSRP_H
#define _ALLJOYN_AUTHMECHSRP_H
/**
 * @file
 *
 * This file defines the class for the AllJoyn PeerGroup Pin authentication mechanism
 *
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

#ifndef __cplusplus
#error Only include AuthMechSRP.h in C++ code.
#endif

#include <qcc/platform.h>
#include <qcc/String.h>
#include <qcc/Crypto.h>

#include "AuthMechanism.h"

namespace ajn {

/**
 * Derived class for the ALLJOYN_SRP_KEYX authentication mechanism
 */
class AuthMechSRP : public AuthMechanism {
  public:

    /**
     * Returns the static name for this authentication method
     * @return string "ALLJOYN_SRP_KEYX"
     * @see AuthMechSRP GetName
     */
    static const char* AuthName() { return "ALLJOYN_SRP_KEYX"; }

    /**
     * Returns the name for this authentication method
     * returns the same result as @b AuthMechSRP::AuthName;
     * @return the static name for the Pin authentication mechanism.
     */
    const char* GetName() { return AuthName(); }

    /**
     * Initialize this authentication mechanism.
     *
     * @param authenticationRole  Indicates if the authentication method is initializing as a challenger or a responder.
     * @param authenticationPeer  The bus name of the remote peer that is being authenticated.
     *
     * @return ER_OK if the authentication mechanism was succesfully initialized
     *         otherwise an error status.
     */
    QStatus Init(AuthRole authenticationRole, const qcc::String& authenticationPeer);

    /**
     * Function of type AuthMechanismManager::AuthMechFactory. The listener cannot be NULL for
     * this authentication mechanism.
     *
     * @param keyStore   The key store available to this authentication mechanism.
     * @param listener   The listener to register
     *
     * @return  An object of class AuthMechSRP
     */
    static AuthMechanism* Factory(KeyStore& keyStore, ProtectedAuthListener& listener) { return new AuthMechSRP(keyStore, listener); }

    /**
     * Client initiates the conversation by sending a random nonce
     */
    qcc::String InitialResponse(AuthResult& result);

    /**
     * Client's response to a challenge from the server
     */
    qcc::String Response(const qcc::String& challenge, AuthResult& result);

    /**
     * Server's challenge to be sent to the client
     * @param response Response used by server
     * @param result Server Challenge sent to the client
     */
    qcc::String Challenge(const qcc::String& response, AuthResult& result);

    /**
     * Indicates that this authentication mechanism is interactive and requires application or user
     * input.
     *
     * @return  Always returns true for this authentication mechanism.
     */
    bool IsInteractive() { return true; }

    ~AuthMechSRP() { }

  private:

    /**
     * Objects must be created using the factory function
     */
    AuthMechSRP(KeyStore& keyStore, ProtectedAuthListener& listener);

    /**
     * Computes the master secret.
     */
    void ComputeMS();

    /**
     * Compute the verifier string.
     */
    qcc::String ComputeVerifier(const char* label);

    uint8_t step;
    qcc::Crypto_SRP srp;
    qcc::String clientRandom;
    qcc::String serverRandom;

    /**
     * Hash of all the challenges and responses used for final verification.
     */
    qcc::Crypto_SHA1 msgHash;
};

}

#endif