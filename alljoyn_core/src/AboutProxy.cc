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


#include <alljoyn/AboutProxy.h>
#include <alljoyn/AboutObjectDescription.h>
#include <alljoyn/AboutData.h>
#include <alljoyn/AllJoynStd.h>
#include <qcc/Debug.h>

#include <stdio.h>

#define QCC_MODULE "ALLJOYN_ABOUT"

namespace ajn {

AboutProxy::AboutProxy(BusAttachment& bus, const char* busName, SessionId sessionId) :
    ProxyBusObject(bus, busName, org::alljoyn::About::ObjectPath, sessionId)
{
    QCC_DbgTrace(("AboutProxy::%s", __FUNCTION__));
    const InterfaceDescription* p_InterfaceDescription = bus.GetInterface(org::alljoyn::About::InterfaceName);
    QCC_ASSERT(p_InterfaceDescription);
    AddInterface(*p_InterfaceDescription);
}

AboutProxy::~AboutProxy()
{
}

QStatus AboutProxy::GetObjectDescription(MsgArg& objectDesc)
{
    QCC_DbgTrace(("AboutProxy::%s", __FUNCTION__));
    QStatus status = ER_OK;

    Message replyMsg(GetBusAttachment());
    status = MethodCall(org::alljoyn::About::InterfaceName, "GetObjectDescription", NULL, 0, replyMsg);
    if (ER_OK != status) {
        if (replyMsg->GetErrorName() != NULL) {
            if (strcmp(replyMsg->GetErrorName(), org::alljoyn::Bus::ErrorName) == 0 && replyMsg->GetArg(1)) {
                status = static_cast<QStatus>(replyMsg->GetArg(1)->v_uint16);
            } else {
                QCC_LogError(status, ("AboutProxy::GetObjectDescription error %s", replyMsg->GetErrorDescription().c_str()));
            }
        }
        return status;
    }
    const ajn::MsgArg* returnArgs = NULL;
    size_t numArgs = 0;
    replyMsg->GetArgs(numArgs, returnArgs);
    if (numArgs == 1) {
        objectDesc = returnArgs[0];
        // Since the returnArgs are const we cannot change its ownership flags
        // as soon as this function ends the returnArgs will go out of scope and
        // any information it points to will be freed unless we call Stabilize to
        // copy that information.
        objectDesc.Stabilize();
    } else {
        //TODO change to more meaningful error status
        status = ER_FAIL;
    }
    return status;
}

QStatus AboutProxy::GetAboutData(const char* languageTag, MsgArg& data)
{
    QCC_DbgTrace(("AboutClient::%s", __FUNCTION__));
    QStatus status = ER_OK;
    Message replyMsg(GetBusAttachment());
    MsgArg args[1];
    status = args[0].Set("s", languageTag);
    if (ER_OK != status) {
        return status;
    }

    status = MethodCall(org::alljoyn::About::InterfaceName, "GetAboutData", args, 1, replyMsg);
    if (ER_OK != status) {
        if (replyMsg->GetErrorName() != NULL) {
            if (strcmp(replyMsg->GetErrorName(), "org.alljoyn.Error.LanguageNotSupported") == 0) {
                status = ER_LANGUAGE_NOT_SUPPORTED;
            } else if (strcmp(replyMsg->GetErrorName(), org::alljoyn::Bus::ErrorName) == 0 && replyMsg->GetArg(1)) {
                status = static_cast<QStatus>(replyMsg->GetArg(1)->v_uint16);
            } else {
                QCC_LogError(status, ("AboutProxy::GetAboutData error %s", replyMsg->GetErrorDescription().c_str()));
            }
        }
        return status;
    }
    const ajn::MsgArg* returnArgs = NULL;
    size_t numArgs = 0;
    replyMsg->GetArgs(numArgs, returnArgs);
    if (numArgs == 1) {
        data = returnArgs[0];
        // Since the returnArgs are const we cannot change its ownership flags
        // as soon as this function ends the returnArgs will go out of scope and
        // any information it points to will be freed unless we call Stabilize to
        // copy that information.
        data.Stabilize();
    } else {
        //TODO change to more meaningful error status
        status = ER_FAIL;
    }
    return status;
}

QStatus AboutProxy::GetVersion(uint16_t& version)
{
    QCC_DbgTrace(("AboutProxy::%s", __FUNCTION__));
    QStatus status = ER_OK;

    MsgArg arg;
    status = GetProperty(org::alljoyn::About::InterfaceName, "Version", arg);
    if (ER_OK == status) {
        version = arg.v_variant.val->v_int16;
    }

    return status;
}
}