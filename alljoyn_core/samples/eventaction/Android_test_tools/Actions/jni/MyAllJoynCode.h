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

#include <alljoyn/Status.h>
#include <alljoyn/BusAttachment.h>
#include <alljoyn/ProxyBusObject.h>
#include <alljoyn/DBusStd.h>
#include <alljoyn/AllJoynStd.h>
#include <alljoyn/SessionPortListener.h>
#include <alljoyn/SessionListener.h>
#include <alljoyn/Session.h>
#include <qcc/String.h>
#include <stdio.h>
#include <qcc/platform.h>
#include "Constants.h"
#include "ActionInfo.h"

#include <alljoyn/about/AboutClient.h>
#include <alljoyn/about/AnnouncementRegistrar.h>

#ifndef _MY_ALLJOYN_CODE_
#define _MY_ALLJOYN_CODE_

class MyAllJoynCode;

class MyAllJoynCode :
    public ajn::services::AnnounceHandler,
    public ajn::BusAttachment::JoinSessionAsyncCB,
    public ajn::SessionListener {
  public:
    /**
     * Construct a MyAllJoynCode object
     *
     */
    MyAllJoynCode(JavaVM* vm, jobject jobj)
        : vm(vm), jobj(jobj), mBusAttachment(NULL), AnnounceHandler()
    { };

    /**
     * Destructor
     */
    ~MyAllJoynCode() {
        shutdown();
    };

    /**
     * Setup AllJoyn, creating the objects needed and registering listeners.
     *
     * @param packageName	This value is provided to the BusAttachment constructor to name the application
     *
     */
    void initialize(const char* packageName);

    /**
     * Join an AllJoyn session.
     *
     * @param sessionName	The busName/Wellknown name to join
     * @param port			The port value that the remote side has bound a session
     *
     */
    void joinSession(const char* sessionName, short port);

    /**
     * Leave an AllJoyn session.
     *
     * @param sessionId	The ID of the session to leave
     *
     */
    void leaveSession(int sessionId);

    /**
     * Perform an IntrospectionWithDescription request over AllJoyn
     *
     * @param sessionName	The busName/Wellknown name to introspect
     * @param path			introspect this specific path
     * @param sessionId		The ID of the session that the is established with sessionName
     *
     *
     */
    char* introspectWithDescriptions(const char* sessionName, const char* path, int sessionId);

    void callAction(ActionInfo* action);

    /**
     * Free up and release the objects used
     */
    void shutdown();

    /* From About */
    void Announce(unsigned short version, unsigned short port, const char* busName,
                  const ajn::services::AboutClient::ObjectDescriptions& objectDescs,
                  const ajn::services::AboutClient::AboutData& aboutData);

    /* For MethodCallAsync */
    void AsyncCallReplyHandler(ajn::Message& msg, void* context);

    /* From SessionListener */
    virtual void SessionLost(ajn::SessionId sessionId, ajn::SessionListener::SessionLostReason reason);

    /** JoinSessionAsync callback */
    virtual void JoinSessionCB(QStatus status, ajn::SessionId sessionId, const ajn::SessionOpts& opts, void* context);

  private:
    JavaVM* vm;
    jobject jobj;

    std::map<qcc::String, qcc::String> mBusFriendlyMap;
    std::map<qcc::String, int> mBusSessionMap;
    std::map<qcc::String, short> mBusPortMap;

    ajn::BusAttachment* mBusAttachment;
};

#endif //_MY_ALLJOYN_CODE_