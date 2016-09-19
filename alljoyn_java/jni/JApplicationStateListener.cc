/******************************************************************************
 * Copyright AllSeen Alliance. All rights reserved.
 *
 *    Permission to use, copy, modify, and/or distribute this software for any
 *    purpose with or without fee is hereby granted, provided that the above
 *    copyright notice and this permission notice appear in all copies.
 *
 *    THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 *    WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 *    MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 *    ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 *    WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 *    ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 *    OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 ******************************************************************************/

#include <jni.h>
#include <alljoyn/PermissionConfigurationListener.h>
#include <alljoyn/PermissionConfigurator.h>
#include <qcc/Debug.h>
#include <qcc/CryptoECC.h>

#include "JApplicationStateListener.h"
#include "alljoyn_jni_helper.h"

#define QCC_MODULE "JNI_APPSTLSTR"

using namespace ajn;
using namespace qcc;

JApplicationStateListener::JApplicationStateListener(jobject jlistener) : jasListener(NULL)
{
    QCC_DbgTrace(("%s", __FUNCTION__));

    JNIEnv* env = GetEnv();

    if (!jlistener) {
        QCC_LogError(ER_FAIL, ("%s: jlistener null", __FUNCTION__));
        return;
    }

    QCC_DbgPrintf(("%s: Taking weak global reference to listener %p", __FUNCTION__, jlistener));
    jasListener = env->NewWeakGlobalRef(jlistener);
    if (!jasListener) {
        QCC_LogError(ER_FAIL, ("%s: Can't create new weak global reference", __FUNCTION__));
        return;
    }

    JLocalRef<jclass> clazz = env->GetObjectClass(jlistener);
    if (!clazz) {
        QCC_LogError(ER_FAIL, ("%s: Can't GetObjectClass()", __FUNCTION__));
        return;
    }

    MID_state = env->GetMethodID(clazz, "state", "(Ljava/lang/String;Lorg/alljoyn/bus/common/KeyInfoNISTP256;Lorg/alljoyn/bus/PermissionConfigurator$ApplicationState;)V");
    if (!MID_state) {
        QCC_DbgPrintf(("%s: Can't find state", __FUNCTION__));
    }
}

JApplicationStateListener::~JApplicationStateListener()
{
    QCC_DbgTrace(("%s", __FUNCTION__));
    if (jasListener) {
        QCC_DbgPrintf(("%s: Releasing weak global reference to listener %p", __FUNCTION__, jasListener));
        GetEnv()->DeleteWeakGlobalRef(jasListener);
        jasListener = NULL;
    }
}

void JApplicationStateListener::State(const char* busName, const qcc::KeyInfoNISTP256& publicKeyInfo, PermissionConfigurator::ApplicationState state)
{
    QCC_DbgTrace(("%s", __FUNCTION__));

    /*
     * JScopedEnv will automagically attach the JVM to the current native
     * thread.
     */
    JScopedEnv env;

    /*
     * The weak global reference jpinglistener cannot be directly used.  We have to get
     * a "hard" reference to it and then use that.  If you try to use a weak reference
     * directly you will crash and burn.
     */
    jobject jo = env->NewLocalRef(jasListener);
    if (!jo) {
        QCC_LogError(ER_FAIL, ("%s: Can't get new local reference to listener", __FUNCTION__));
        return;
    }

    /*
     * This call out to the listener means that the state  method
     * must be MT-Safe.  This is implied by the definition of the listener.
     */
    QCC_DbgPrintf(("%s: Call out to listener object and method", __FUNCTION__));

    jstring jbusName = env->NewStringUTF(busName);
    if (env->ExceptionCheck()) {
        QCC_LogError(ER_FAIL, ("%s: Exception", __FUNCTION__));
        return;
    }

    jobject jstate = NULL;
    if (state == PermissionConfigurator::ApplicationState::NOT_CLAIMABLE) {
        jstate = PermissionConfiguratorApplicationState_NOT_CLAIMABLE;
    } else if (state == PermissionConfigurator::ApplicationState::CLAIMABLE) {
        jstate = PermissionConfiguratorApplicationState_CLAIMABLE;
    } else if (state == PermissionConfigurator::ApplicationState::CLAIMED) {
        jstate = PermissionConfiguratorApplicationState_CLAIMED;
    } else if (state == PermissionConfigurator::ApplicationState::NEED_UPDATE) {
        jstate = PermissionConfiguratorApplicationState_NEED_UPDATE;
    }

    const ECCPublicKey* pubKey = publicKeyInfo.GetPublicKey();
    if (pubKey == NULL) {
        QCC_LogError(ER_FAIL, ("%s: pubKey is null", __FUNCTION__));
        return;
    }

    jobject jpublicKeyInfo = env->NewObject(CLS_KeyInfoNISTP256, MID_KeyInfoNISTP256_cnstrctr);

    JLocalRef<jbyteArray> arrayX = ToJByteArray(pubKey->GetX(), pubKey->GetCoordinateSize());
    JLocalRef<jbyteArray> arrayY = ToJByteArray(pubKey->GetY(), pubKey->GetCoordinateSize());

    jobject jpublicKey = env->NewObject(CLS_ECCPublicKey, MID_ECCPublicKey_cnstrctr, arrayX.move(), arrayY.move());

    env->CallObjectMethod(jpublicKeyInfo, MID_KeyInfoNISTP256_setPublicKey, jpublicKey);

    env->CallObjectMethod(jo, MID_state, jbusName, jpublicKeyInfo, jstate);
}
