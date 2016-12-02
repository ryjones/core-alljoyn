#ifndef _ALLJOYN_AUTHMECHANONYMOUS_H
#define _ALLJOYN_AUTHMECHANONYMOUS_H
/**
 * @file
 * This file defines the class for the DBUS ANONYMOUS authentication method
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
#error Only include AuthMechAnonymous.h in C++ code.
#endif

#include <qcc/platform.h>
#include <qcc/String.h>
#include "AuthMechanism.h"


namespace ajn {

/**
 * Derived class for the DBUS Anonymous authentication method
 *
 * %AuthMechAnonymous inherits from AuthMechanism and implements/reimplements its interface.
 */
class AuthMechAnonymous : public AuthMechanism {
  public:

    /**
     * Returns the static name for this authentication method
     *
     * @return string "ANONYMOUS"
     *
     * @see AuthMechAnonymous::GetName
     */
    static const char* AuthName() { return "ANONYMOUS"; }

    /**
     * Returns the name for this authentication method
     *
     * returns the same result as \b AuthMechAnonymous::AuthName;
     *
     * @return the static name for the anonymous authentication mechanism.
     *
     */
    const char* GetName() { return AuthName(); }

    /**
     * Function of type AuthMechanismManager::AuthMechFactory
     *
     * @param keyStore   The key store available to this authentication mechanism.
     * @param listener   The listener to register (listener is not used by AuthMechAnonymous)
     *
     * @return  An object of class AuthMechAnonymous
     */
    static AuthMechanism* Factory(KeyStore& keyStore, ProtectedAuthListener& listener) { return new AuthMechAnonymous(keyStore, listener); }

    /**
     * Responses flow from clients to servers.
     *        ANONYMOUS always responds with OK.
     * @param challenge Challenge provided by the server; the anonymous authentication
     *                  mechanism will ignore this parameter.
     * @param result    Return ALLJOYN_AUTH_OK
     *
     * @return an empty string.
     */
    qcc::String Response(const qcc::String& challenge, AuthMechanism::AuthResult& result) {
        QCC_UNUSED(challenge);
        result = ALLJOYN_AUTH_OK;
        return "";
    }

    /**
     * Server's challenge to be sent to the client.
     *
     * the anonymous authentication mechanism always responds with an empty
     * string and AuthResult of ALLJOYN_AUTH_OK when InitialChallenge is called.
     *
     * @param result Returns ALLJOYN_AUTH_OK
     *
     * @return an empty string
     *
     * @see AuthMechanism::InitialChallenge
     */
    qcc::String InitialChallenge(AuthMechanism::AuthResult& result) { result = ALLJOYN_AUTH_OK; return ""; }

    /**
     * Challenges flow from servers to clients -- ANONYMOUS doesn't send anything after the initial challenge.
     *
     * Process a response from a client and returns a challenge.
     *
     * @param response Response from client the anonymous authentication mechanism challenge will ignore this parameter.
     * @param result   Returns ALLJOYN_AUTH_OK
     *
     * @return an empty string
     *
     * @see AuthMechanism::Challenge
     */
    qcc::String Challenge(const qcc::String& response, AuthMechanism::AuthResult& result) {
        QCC_UNUSED(response);
        result = ALLJOYN_AUTH_OK;
        return "";
    }

    ~AuthMechAnonymous() { }

  private:

    AuthMechAnonymous(KeyStore& keyStore, ProtectedAuthListener& listener) : AuthMechanism(keyStore, listener) { }

};

}

#endif