/******************************************************************************
 * Copyright (c) Open Connectivity Foundation (OCF) and AllJoyn Open
 *    Source Project (AJOSP) Contributors and others.
 *
 *    SPDX-License-Identifier: Apache-2.0
 *
 *    All rights reserved. This program and the accompanying materials are
 *    made available under the terms of the Apache License, Version 2.0
 *    which accompanies this distribution, and is available at
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 *    Copyright (c) Open Connectivity Foundation and Contributors to AllSeen
 *    Alliance. All rights reserved.
 *
 *    Permission to use, copy, modify, and/or distribute this software for
 *    any purpose with or without fee is hereby granted, provided that the
 *    above copyright notice and this permission notice appear in all
 *    copies.
 *
 *     THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL
 *     WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED
 *     WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE
 *     AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL
 *     DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
 *     PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 *     TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 *     PERFORMANCE OF THIS SOFTWARE.
 ******************************************************************************/

#ifndef _ALLJOYN_TESTSECURITYMANAGER_H
#define _ALLJOYN_TESTSECURITYMANAGER_H

#include <string>

#include <alljoyn/BusAttachment.h>
#include "InMemoryKeyStore.h"

using namespace std;
using namespace ajn;
using namespace qcc;

class TestSecurityManager :
    public SessionListener {

  public:
    TestSecurityManager(string appName = "TestSecurityManager");

    ~TestSecurityManager();

    QStatus Init();

    QStatus Claim(BusAttachment& peerBus, const PermissionPolicy::Acl& manifest);

    QStatus UpdateIdentity(BusAttachment& peerBus, const PermissionPolicy::Acl& manifest);

    QStatus InstallMembership(BusAttachment& peerBus, const GUID128& group);

    QStatus UpdatePolicy(const BusAttachment& peerBus, const PermissionPolicy& policy);

    QStatus Reset(const BusAttachment& peerBus);

    const KeyInfoNISTP256& GetCaPublicKeyInfo()
    {
        return caPublicKeyInfo;
    }

    qcc::String GetUniqueName()
    {
        return bus.GetUniqueName();
    }

  private:
    BusAttachment bus;
    SessionOpts opts;
    DefaultECDHEAuthListener authListener;
    Crypto_ECC caKeyPair;
    KeyInfoNISTP256 caPublicKeyInfo;
    CertificateX509 caCertificate;
    GUID128 adminGroup;
    GUID128 identityGuid;
    string identityName;
    int certSerialNumber;
    int policyVersion;
    InMemoryKeyStoreListener keyStoreListener;

    /* SessionListener */
    virtual void SessionLost(SessionId sessionId,
                             SessionLostReason reason);

    QStatus EndManagement(BusAttachment& peerBus);

    QStatus InstallAdminMembership();

    void IssueCertificate(const ECCPublicKey& appPubKey,
                          CertificateX509& cert,
                          bool isCA = false);

    void GenerateIdentityCertificate(const ECCPublicKey& appPubKey,
                                     IdentityCertificate& cert);

    void GenerateMembershipCertificate(const ECCPublicKey& appPubKey,
                                       const GUID128& group,
                                       MembershipCertificate& cert);

    QStatus ClaimSelf();

    void AddAdminAcl(const PermissionPolicy& in,
                     PermissionPolicy& out);
};

#endif /* _ALLJOYN_TESTSECURITYMANAGER_H */