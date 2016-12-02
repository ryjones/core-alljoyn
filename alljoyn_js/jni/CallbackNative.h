/*
 *  * 
 *    Copyright (c) 2016 Open Connectivity Foundation and AllJoyn Open
 *    Source Project Contributors and others.
 *    
 *    All rights reserved. This program and the accompanying materials are
 *    made available under the terms of the Apache License, Version 2.0
 *    which accompanies this distribution, and is available at
 *    http://www.apache.org/licenses/LICENSE-2.0

 */
#ifndef _CALLBACKNATIVE_H
#define _CALLBACKNATIVE_H

#include "AboutObjHost.h"
#include "BusErrorHost.h"
#include "InterfaceDescriptionNative.h"
#include "MessageHost.h"
#include "NativeObject.h"
#include "PluginData.h"
#include "ProxyBusObjectHost.h"
#include "SessionOptsHost.h"
#include "SocketFdHost.h"
#include <alljoyn/Session.h>
#include <vector>

class CallbackNative : public NativeObject {
  public:
    CallbackNative(Plugin& plugin, NPObject* objectValue);
    virtual ~CallbackNative();

    void onCallback(QStatus status);
    void onCallback(QStatus status, bool b);
    void onCallback(QStatus status, qcc::String& s);
    void onCallback(QStatus status, uint32_t u);
    void onCallback(QStatus status, ajn::SessionId id, SessionOptsHost& opts);
    void onCallback(QStatus status, ajn::SessionPort port);
    void onCallback(QStatus status, MessageHost& message, const ajn::MsgArg* args, size_t numArgs);
    void onCallback(QStatus status, AboutObjHost& aboutObj);
    void onCallback(QStatus status, ProxyBusObjectHost& proxyBusObject);
    void onCallback(QStatus status, SocketFdHost& socketFd);
    void onCallback(QStatus status, InterfaceDescriptionNative* interfaceDescription);
    void onCallback(QStatus status, InterfaceDescriptionNative** interfaceDescription, size_t numInterfaces);
    void onCallback(QStatus status, std::vector<ProxyBusObjectHost>& children);
    void onCallback(BusErrorHost& busError);

    static void DispatchCallback(Plugin& plugin, CallbackNative* callbackNative, QStatus status);
    static void DispatchCallback(Plugin& plugin, CallbackNative* callbackNative, QStatus status, bool b);
    static void DispatchCallback(Plugin& plugin, CallbackNative* callbackNative, QStatus status, qcc::String& s);
    static void DispatchCallback(Plugin& plugin, CallbackNative* callbackNative, QStatus status, uint32_t u);
    static void DispatchCallback(Plugin& plugin, CallbackNative* callbackNative, QStatus status, ajn::SessionPort port);
    static void DispatchCallback(Plugin& plugin, CallbackNative* callbackNative, QStatus status, AboutObjHost& aboutObj);
    static void DispatchCallback(Plugin& plugin, CallbackNative* callbackNative, QStatus status, ProxyBusObjectHost& proxyBusObject);
    static void DispatchCallback(Plugin& plugin, CallbackNative* callbackNative, QStatus status, SocketFdHost& socketFd);
    static void DispatchCallback(Plugin& plugin, CallbackNative* callbackNative, QStatus status, InterfaceDescriptionNative* interfaceDescription);
    static void DispatchCallback(Plugin& plugin, CallbackNative* callbackNative, QStatus status, InterfaceDescriptionNative** interfaceDescription, size_t numInterfaces);
    static void DispatchCallback(Plugin& plugin, CallbackNative* callbackNative, QStatus status, std::vector<ProxyBusObjectHost>& children);

  private:
    static void _StatusCallbackCB(PluginData::CallbackContext* ctx);
    static void _BoolCallbackCB(PluginData::CallbackContext* ctx);
    static void _StringCallbackCB(PluginData::CallbackContext* ctx);
    static void _UnsignedLongCallbackCB(PluginData::CallbackContext* ctx);
    static void _BindSessionPortCallbackCB(PluginData::CallbackContext* ctx);
    static void _GetAboutObjectCallbackCB(PluginData::CallbackContext* ctx);
    static void _GetProxyBusObjectCallbackCB(PluginData::CallbackContext* ctx);
    static void _GetSessionFdCallbackCB(PluginData::CallbackContext* ctx);
    static void _GetInterfaceCallbackCB(PluginData::CallbackContext* ctx);
    static void _GetInterfacesCallbackCB(PluginData::CallbackContext* ctx);
    static void _GetChildrenCallbackCB(PluginData::CallbackContext* ctx);
};

#endif // _CALLBACKNATIVE_H