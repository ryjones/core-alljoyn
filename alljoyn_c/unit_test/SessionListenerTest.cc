/******************************************************************************
 *    Copyright (c) Open Connectivity Foundation (OCF), AllJoyn Open Source
 *    Project (AJOSP) Contributors and others.
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
 *    THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL
 *    WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED
 *    WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE
 *    AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL
 *    DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
 *    PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 *    TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 *    PERFORMANCE OF THIS SOFTWARE.
 ******************************************************************************/

#include <gtest/gtest.h>

#include "ajTestCommon.h"

#include <qcc/Thread.h>
#include <alljoyn_c/DBusStdDefines.h>
#include <alljoyn_c/SessionListener.h>
#include <alljoyn_c/BusAttachment.h>

/*constants*/
static const char* INTERFACE_NAME = "org.alljoyn.test.SessionListenerTest";
static const char* OBJECT_PATH = "/org/alljoyn/test/SessionListenerTest";
static const alljoyn_sessionport SESSION_PORT = 42;

/* session listener flags */
static QCC_BOOL session_lost_flag = QCC_FALSE;
static QCC_BOOL session_lost_reason_flag = QCC_FALSE;
static QCC_BOOL session_member_added_flag = QCC_FALSE;
static QCC_BOOL session_member_removed_flag = QCC_FALSE;
static alljoyn_sessionlostreason sessionlostreason = ALLJOYN_SESSIONLOST_INVALID;

/* session port listener flags */
static QCC_BOOL accept_session_joiner_flag = QCC_FALSE;
static QCC_BOOL session_joined_flag = QCC_FALSE;

static alljoyn_sessionid joinsessionid;

/* bus listener flags */
static QCC_BOOL listener_registered_flag = QCC_FALSE;
static QCC_BOOL listener_unregistered_flag = QCC_FALSE;
static QCC_BOOL found_advertised_name_flag = QCC_FALSE;
static QCC_BOOL lost_advertised_name_flag = QCC_FALSE;
static QCC_BOOL name_owner_changed_flag = QCC_FALSE;
static QCC_BOOL bus_stopping_flag = QCC_FALSE;
static QCC_BOOL bus_disconnected_flag = QCC_FALSE;
static QCC_BOOL prop_changed_flag = QCC_FALSE;
static alljoyn_transportmask transport_found = 0;

/* member names - access must be synchronized */
static qcc::Mutex member_added_lock;
static char* member_added_uniquename = NULL;
static qcc::Mutex member_removed_lock;
static char* member_removed_uniquename = NULL;

/*
 * Reallocate a string 'buffer' to fit the provided 'name', while holding 'bufferLock'.
 * The 'name' argument is optional - if NULL is provided, the buffer is simply freed.
 */
void ReallocateName(char** buffer, qcc::Mutex* bufferLock, const char* name) {
    char* newName;
    if (name != NULL) {
        /* Allocate the new buffer */
        int uniqueNameSize = strlen(name);
        newName = (char*) malloc(uniqueNameSize * sizeof(char) + sizeof(char));
        strncpy(newName, name, uniqueNameSize);
        newName[uniqueNameSize] = '\0';
    } else {
        newName = NULL;
    }

    ASSERT_EQ(ER_OK, bufferLock->Lock(MUTEX_CONTEXT));
    free(*buffer);
    *buffer = newName;
    ASSERT_EQ(ER_OK, bufferLock->Unlock(MUTEX_CONTEXT));
}

/* session listener functions */
static void AJ_CALL session_lost(const void* context, alljoyn_sessionid sessionId, alljoyn_sessionlostreason reason) {
    QCC_UNUSED(context);
    QCC_UNUSED(sessionId);

    sessionlostreason = reason;
    session_lost_flag = QCC_TRUE;
}

static void AJ_CALL session_member_added(const void* context, alljoyn_sessionid sessionid, const char* uniqueName) {
    QCC_UNUSED(context);
    QCC_UNUSED(sessionid);

    ReallocateName(&member_added_uniquename, &member_added_lock, uniqueName);
    session_member_added_flag = QCC_TRUE;
}

static void AJ_CALL session_member_removed(const void* context, alljoyn_sessionid sessionId, const char* uniqueName) {
    QCC_UNUSED(context);
    QCC_UNUSED(sessionId);

    ReallocateName(&member_removed_uniquename, &member_removed_lock, uniqueName);
    session_member_removed_flag = QCC_TRUE;
}

/* session port listener */
static QCC_BOOL AJ_CALL accept_session_joiner(const void* context, alljoyn_sessionport sessionPort,
                                              const char* joiner,  const alljoyn_sessionopts opts) {
    QCC_UNUSED(context);
    QCC_UNUSED(joiner);
    QCC_UNUSED(opts);

    QCC_BOOL ret = QCC_FALSE;
    if (sessionPort == SESSION_PORT) {
        ret = QCC_TRUE;
    }
    accept_session_joiner_flag = QCC_TRUE;
    return ret;
}

static void AJ_CALL session_joined(const void* context, alljoyn_sessionport sessionPort,
                                   alljoyn_sessionid id, const char* joiner) {
    QCC_UNUSED(context);
    QCC_UNUSED(joiner);

    EXPECT_EQ(SESSION_PORT, sessionPort);
    joinsessionid = id;
    session_joined_flag = QCC_TRUE;
}

/* bus listener functions */
static void AJ_CALL found_advertised_name(const void* context, const char* name, alljoyn_transportmask transport, const char* namePrefix) {
    QCC_UNUSED(context);
    QCC_UNUSED(name);
    QCC_UNUSED(namePrefix);

    transport_found |= transport;
    found_advertised_name_flag = QCC_TRUE;
}
static void AJ_CALL lost_advertised_name(const void* context, const char* name, alljoyn_transportmask transport, const char* namePrefix) {
    QCC_UNUSED(context);
    QCC_UNUSED(name);
    QCC_UNUSED(transport);
    QCC_UNUSED(namePrefix);
    lost_advertised_name_flag = QCC_TRUE;
}

/* Exposed methods */
static void AJ_CALL ping_method(alljoyn_busobject bus, const alljoyn_interfacedescription_member* member, alljoyn_message msg)
{
    QCC_UNUSED(member);

    alljoyn_msgarg outArg = alljoyn_msgarg_create();
    outArg = alljoyn_message_getarg(msg, 0);
    const char* str;
    alljoyn_msgarg_get(outArg, "s", &str);
    QStatus status = alljoyn_busobject_methodreply_args(bus, msg, outArg, 1);
    EXPECT_EQ(ER_OK, status) << "  Actual Status: " << QCC_StatusText(status);
}

class SessionListenerTest : public testing::Test {
  public:
    virtual void SetUp() {
        resetFlags();
        servicebus = alljoyn_busattachment_create("SessionListenerTestService", QCC_FALSE);
        objectName = ajn::genUniqueName(servicebus);
        EXPECT_EQ(ER_OK, alljoyn_busattachment_start(servicebus));
        EXPECT_EQ(ER_OK, alljoyn_busattachment_connect(servicebus, ajn::getConnectArg().c_str()));

        alljoyn_interfacedescription testIntf = NULL;
        status = alljoyn_busattachment_createinterface(servicebus, INTERFACE_NAME, &testIntf);
        EXPECT_EQ(ER_OK, status) << "  Actual Status: " << QCC_StatusText(status);
        EXPECT_TRUE(testIntf != NULL);
        if (testIntf != NULL) {
            status = alljoyn_interfacedescription_addmethod(testIntf, "ping", "s", "s", "in,out", 0, 0);
            EXPECT_EQ(ER_OK, status) << "  Actual Status: " << QCC_StatusText(status);
            alljoyn_interfacedescription_activate(testIntf);

            testObj = alljoyn_busobject_create(OBJECT_PATH, QCC_FALSE, NULL, NULL);

            status = alljoyn_busobject_addinterface(testObj, testIntf);
            EXPECT_EQ(ER_OK, status) << "  Actual Status: " << QCC_StatusText(status);

            /* register method handlers */
            alljoyn_interfacedescription_member ping_member;
            EXPECT_TRUE(alljoyn_interfacedescription_getmember(testIntf, "ping", &ping_member));

            status = alljoyn_busobject_addmethodhandler(testObj, ping_member, &ping_method, NULL);
            EXPECT_EQ(ER_OK, status) << "  Actual Status: " << QCC_StatusText(status);

            /* register the bus object */
            status = alljoyn_busattachment_registerbusobject(servicebus, testObj);
            EXPECT_EQ(ER_OK, status) << "  Actual Status: " << QCC_StatusText(status);

            /* Create session port listener */
            alljoyn_sessionportlistener_callbacks spl_cbs = {
                &accept_session_joiner, /* accept session joiner CB */
                &session_joined        /* session joined CB */
            };
            sessionPortListener = alljoyn_sessionportlistener_create(&spl_cbs, NULL);

            /* Bind SessionPort session opts specify this is a multipoint session */
            opts = alljoyn_sessionopts_create(ALLJOYN_TRAFFIC_TYPE_MESSAGES, QCC_TRUE, ALLJOYN_PROXIMITY_ANY, ALLJOYN_TRANSPORT_ANY);
            alljoyn_sessionport sp = SESSION_PORT;
            status = alljoyn_busattachment_bindsessionport(servicebus, &sp, opts, sessionPortListener);
            EXPECT_EQ(ER_OK, status) << "  Actual Status: " << QCC_StatusText(status);

            /* request name from the bus */
            int flag = DBUS_NAME_FLAG_REPLACE_EXISTING | DBUS_NAME_FLAG_DO_NOT_QUEUE;
            EXPECT_EQ(ER_OK, alljoyn_busattachment_requestname(servicebus, objectName.c_str(), flag));
            EXPECT_EQ(ER_OK, alljoyn_busattachment_advertisename(servicebus, objectName.c_str(), alljoyn_sessionopts_get_transports(opts)));

            clientbus = alljoyn_busattachment_create("SessionListenerTestClient", QCC_FALSE);
            EXPECT_EQ(ER_OK, alljoyn_busattachment_start(clientbus));
            EXPECT_EQ(ER_OK, alljoyn_busattachment_connect(clientbus, ajn::getConnectArg().c_str()));

            /* register bus listener */
            alljoyn_buslistener_callbacks buslistenerCbs = {
                NULL, //&listener_registered,
                NULL, //&listener_unregistered,
                &found_advertised_name,
                &lost_advertised_name,
                NULL, //&name_owner_changed,
                NULL, //&bus_stopping,
                NULL, //&bus_disconnected,
                NULL, //&bus_prop_changed
            };
            buslistener = alljoyn_buslistener_create(&buslistenerCbs, NULL);
            alljoyn_busattachment_registerbuslistener(clientbus, buslistener);

            alljoyn_sessionlistener_callbacks sessionlisternerCbs = {
                &session_lost,
                &session_member_added,
                &session_member_removed
            };
            sessionListener = alljoyn_sessionlistener_create(&sessionlisternerCbs, NULL);
        }
    }

    virtual void TearDown() {
        resetFlags();
        alljoyn_sessionopts_destroy(opts);
        alljoyn_sessionportlistener_destroy(sessionPortListener);
        alljoyn_busattachment_unregisterbuslistener(clientbus, buslistener);
        EXPECT_NO_FATAL_FAILURE(alljoyn_sessionlistener_destroy(sessionListener));
        EXPECT_NO_FATAL_FAILURE(alljoyn_buslistener_destroy(buslistener));
        alljoyn_busattachment_stop(clientbus);
        alljoyn_busattachment_join(clientbus);
        alljoyn_busattachment_destroy(clientbus);
        alljoyn_busattachment_stop(servicebus);
        alljoyn_busattachment_join(servicebus);
        EXPECT_NO_FATAL_FAILURE(alljoyn_busattachment_destroy(servicebus));
        alljoyn_busobject_destroy(testObj);
    }

    void resetFlags() {
        /* session listener flags */
        session_lost_flag = false;
        session_lost_reason_flag = false;
        session_member_added_flag = false;
        session_member_removed_flag = false;
        sessionlostreason = ALLJOYN_SESSIONLOST_INVALID;

        /* session port listener flags */
        accept_session_joiner_flag = QCC_FALSE;
        session_joined_flag = QCC_FALSE;

        /* bus listener flags */
        listener_registered_flag = QCC_FALSE;
        listener_unregistered_flag = QCC_FALSE;
        found_advertised_name_flag = QCC_FALSE;
        lost_advertised_name_flag = QCC_FALSE;
        name_owner_changed_flag = QCC_FALSE;
        bus_stopping_flag = QCC_FALSE;
        bus_disconnected_flag = QCC_FALSE;
        prop_changed_flag = QCC_FALSE;
        transport_found = 0;

        /* free the global unique names */
        ReallocateName(&member_added_uniquename, &member_added_lock, NULL);
        ReallocateName(&member_removed_uniquename, &member_removed_lock, NULL);
    }
    QStatus status;
    alljoyn_busattachment servicebus;
    alljoyn_busattachment clientbus;
    alljoyn_busobject testObj;
    alljoyn_buslistener buslistener;
    alljoyn_sessionportlistener sessionPortListener;
    alljoyn_sessionlistener sessionListener;
    qcc::String objectName;

    alljoyn_sessionopts opts;
};

TEST_F(SessionListenerTest, sessionlosttest_remote_end_left_session) {
    resetFlags();
    /* Begin discover of the well-known name */
    status = alljoyn_busattachment_findadvertisedname(clientbus, objectName.c_str());
    EXPECT_EQ(ER_OK, status) << "  Actual Status: " << QCC_StatusText(status);
    for (size_t i = 0; i < 1000; ++i) {
        if (found_advertised_name_flag) {
            break;
        }
        qcc::Sleep(5);
    }
    EXPECT_TRUE(found_advertised_name_flag);

    alljoyn_sessionid sid;
    joinsessionid = 0;
    status = alljoyn_busattachment_joinsession(clientbus, objectName.c_str(), SESSION_PORT, sessionListener, &sid, opts);
    EXPECT_EQ(ER_OK, status) << "  Actual Status: " << QCC_StatusText(status);
    for (size_t i = 0; i < 1000; ++i) {
        if (session_joined_flag && session_member_added_flag) {
            break;
        }
        qcc::Sleep(5);
    }
    EXPECT_TRUE(session_joined_flag);
    EXPECT_EQ(sid, joinsessionid);
    EXPECT_TRUE(session_member_added_flag);

    ASSERT_EQ(ER_OK, member_added_lock.Lock(MUTEX_CONTEXT));
    std::string member_added_uniquename_local = member_added_uniquename;
    ASSERT_EQ(ER_OK, member_added_lock.Unlock(MUTEX_CONTEXT));
    EXPECT_STREQ(alljoyn_busattachment_getuniquename(servicebus), member_added_uniquename_local.c_str());

    status = alljoyn_busattachment_leavesession(servicebus, sid);
    EXPECT_EQ(ER_OK, status) << "  Actual Status: " << QCC_StatusText(status);
    for (size_t i = 0; i < 1000; ++i) {
        if (session_lost_flag && session_member_removed_flag) {
            break;
        }
        qcc::Sleep(5);
    }
    EXPECT_TRUE(session_lost_flag);
    EXPECT_EQ(ALLJOYN_SESSIONLOST_REMOTE_END_LEFT_SESSION, sessionlostreason);
    EXPECT_TRUE(session_member_removed_flag);

    ASSERT_EQ(ER_OK, member_removed_lock.Lock(MUTEX_CONTEXT));
    std::string member_removed_uniquename_local = member_removed_uniquename;
    ASSERT_EQ(ER_OK, member_removed_lock.Unlock(MUTEX_CONTEXT));
    EXPECT_STREQ(alljoyn_busattachment_getuniquename(servicebus), member_removed_uniquename_local.c_str());
}

TEST_F(SessionListenerTest, sessionlosttest_closed_abruptly) {
    resetFlags();
    /* Begin discover of the well-known name */
    status = alljoyn_busattachment_findadvertisedname(clientbus, objectName.c_str());
    EXPECT_EQ(ER_OK, status) << "  Actual Status: " << QCC_StatusText(status);
    for (size_t i = 0; i < 1000; ++i) {
        if (found_advertised_name_flag) {
            break;
        }
        qcc::Sleep(5);
    }
    EXPECT_TRUE(found_advertised_name_flag);

    alljoyn_sessionid sid;
    joinsessionid = 0;
    status = alljoyn_busattachment_joinsession(clientbus, objectName.c_str(), SESSION_PORT, sessionListener, &sid, opts);
    EXPECT_EQ(ER_OK, status) << "  Actual Status: " << QCC_StatusText(status);
    for (size_t i = 0; i < 1000; ++i) {
        if (session_joined_flag && session_member_added_flag) {
            break;
        }
        qcc::Sleep(5);
    }
    EXPECT_TRUE(session_joined_flag);
    EXPECT_EQ(sid, joinsessionid);
    EXPECT_TRUE(session_member_added_flag);

    ASSERT_EQ(ER_OK, member_added_lock.Lock(MUTEX_CONTEXT));
    std::string member_added_uniquename_local = member_added_uniquename;
    ASSERT_EQ(ER_OK, member_added_lock.Unlock(MUTEX_CONTEXT));
    EXPECT_STREQ(alljoyn_busattachment_getuniquename(servicebus), member_added_uniquename_local.c_str());

    status = alljoyn_busattachment_disconnect(servicebus, ajn::getConnectArg().c_str());
    EXPECT_EQ(ER_OK, status) << "  Actual Status: " << QCC_StatusText(status);
    for (size_t i = 0; i < 1000; ++i) {
        if (session_lost_flag && session_member_removed_flag) {
            break;
        }
        qcc::Sleep(5);
    }
    EXPECT_TRUE(session_lost_flag);
    EXPECT_TRUE(session_member_removed_flag);
    EXPECT_EQ(ALLJOYN_SESSIONLOST_REMOTE_END_CLOSED_ABRUPTLY, sessionlostreason);

    /*
     * the servicebus has been disconnected so we can not use the getuniquename
     * function on the servicebus.  If the check done above passes then this
     * should also pass.
     */

    ASSERT_EQ(ER_OK, member_added_lock.Lock(MUTEX_CONTEXT));
    member_added_uniquename_local = member_added_uniquename;
    ASSERT_EQ(ER_OK, member_added_lock.Unlock(MUTEX_CONTEXT));

    ASSERT_EQ(ER_OK, member_removed_lock.Lock(MUTEX_CONTEXT));
    std::string member_removed_uniquename_local = member_removed_uniquename;
    ASSERT_EQ(ER_OK, member_removed_lock.Unlock(MUTEX_CONTEXT));

    EXPECT_STREQ(member_added_uniquename_local.c_str(), member_removed_uniquename_local.c_str());
}

TEST_F(SessionListenerTest, sessionmember_added_removed) {
    resetFlags();
    /* Begin discover of the well-known name */
    status = alljoyn_busattachment_findadvertisedname(clientbus, objectName.c_str());
    EXPECT_EQ(ER_OK, status) << "  Actual Status: " << QCC_StatusText(status);
    for (size_t i = 0; i < 1000; ++i) {
        if (found_advertised_name_flag) {
            break;
        }
        qcc::Sleep(5);
    }
    EXPECT_TRUE(found_advertised_name_flag);

    alljoyn_sessionid sid;
    joinsessionid = 0;
    status = alljoyn_busattachment_joinsession(clientbus, objectName.c_str(), SESSION_PORT, NULL, &sid, opts);
    EXPECT_EQ(ER_OK, status) << "  Actual Status: " << QCC_StatusText(status);
    for (size_t i = 0; i < 1000; ++i) {
        if (session_joined_flag) {
            break;
        }
        qcc::Sleep(5);
    }
    EXPECT_TRUE(session_joined_flag);
    EXPECT_EQ(sid, joinsessionid);

    /* we now have a session id we can use to set the session listener for the service */
    status = alljoyn_busattachment_setsessionlistener(servicebus, sid, sessionListener);
    EXPECT_EQ(ER_OK, status) << "  Actual Status: " << QCC_StatusText(status);

    alljoyn_busattachment clientbus2 = alljoyn_busattachment_create("SessionListenerTestClient", QCC_FALSE);
    EXPECT_EQ(ER_OK, alljoyn_busattachment_start(clientbus2));
    EXPECT_EQ(ER_OK, alljoyn_busattachment_connect(clientbus2, ajn::getConnectArg().c_str()));

    /* we are only interested in the sessionListener used by the service not any of the clients */
    resetFlags();
    status = alljoyn_busattachment_joinsession(clientbus2, objectName.c_str(), SESSION_PORT, NULL, &sid, opts);
    EXPECT_EQ(ER_OK, status) << "  Actual Status: " << QCC_StatusText(status);
    for (size_t i = 0; i < 1000; ++i) {
        if (session_joined_flag && session_member_added_flag) {
            break;
        }
        qcc::Sleep(5);
    }
    EXPECT_TRUE(session_joined_flag);
    EXPECT_EQ(sid, joinsessionid);
    EXPECT_TRUE(session_member_added_flag);

    ASSERT_EQ(ER_OK, member_added_lock.Lock(MUTEX_CONTEXT));
    std::string member_added_uniquename_local = member_added_uniquename;
    ASSERT_EQ(ER_OK, member_added_lock.Unlock(MUTEX_CONTEXT));
    EXPECT_STREQ(alljoyn_busattachment_getuniquename(clientbus2), member_added_uniquename_local.c_str())
        << "clientbus UID = " << alljoyn_busattachment_getuniquename(clientbus) << "\nservicebus UID = "
        << alljoyn_busattachment_getuniquename(servicebus);

    status = alljoyn_busattachment_leavesession(clientbus, sid);
    EXPECT_EQ(ER_OK, status) << "  Actual Status: " << QCC_StatusText(status);
    for (size_t i = 0; i < 1000; ++i) {
        if (session_member_removed_flag) {
            break;
        }
        qcc::Sleep(5);
    }
    EXPECT_TRUE(session_member_removed_flag);

    ASSERT_EQ(ER_OK, member_removed_lock.Lock(MUTEX_CONTEXT));
    std::string member_removed_uniquename_local = member_removed_uniquename;
    ASSERT_EQ(ER_OK, member_removed_lock.Unlock(MUTEX_CONTEXT));
    EXPECT_STREQ(alljoyn_busattachment_getuniquename(clientbus), member_removed_uniquename_local.c_str());


    resetFlags();
    status = alljoyn_busattachment_leavesession(clientbus2, sid);
    EXPECT_EQ(ER_OK, status) << "  Actual Status: " << QCC_StatusText(status);
    for (size_t i = 0; i < 1000; ++i) {
        if (session_lost_flag && session_member_removed_flag) {
            break;
        }
        qcc::Sleep(5);
    }
    EXPECT_TRUE(session_lost_flag);
    EXPECT_EQ(ALLJOYN_SESSIONLOST_REMOTE_END_LEFT_SESSION, sessionlostreason);
    EXPECT_TRUE(session_member_removed_flag);

    ASSERT_EQ(ER_OK, member_removed_lock.Lock(MUTEX_CONTEXT));
    member_removed_uniquename_local = member_removed_uniquename;
    ASSERT_EQ(ER_OK, member_removed_lock.Unlock(MUTEX_CONTEXT));
    EXPECT_STREQ(alljoyn_busattachment_getuniquename(clientbus2), member_removed_uniquename_local.c_str());

    alljoyn_busattachment_destroy(clientbus2);
}

/* session listener flags */
static QCC_BOOL service_session_lost_flag = QCC_FALSE;
static QCC_BOOL service_session_lost_reason_flag = QCC_FALSE;
static QCC_BOOL service_session_member_added_flag = QCC_FALSE;
static QCC_BOOL service_session_member_removed_flag = QCC_FALSE;
static alljoyn_sessionlostreason service_sessionlostreason = ALLJOYN_SESSIONLOST_INVALID;

/* member names - access must be synchronized */
static qcc::Mutex service_member_added_lock;
static char* service_member_added_uniquename = NULL;
static qcc::Mutex service_member_removed_lock;
static char* service_member_removed_uniquename = NULL;

void resetServicSessionListenerFlags() {
    service_session_lost_flag = QCC_FALSE;
    service_session_lost_reason_flag = QCC_FALSE;
    service_session_member_added_flag = QCC_FALSE;
    service_session_member_removed_flag = QCC_FALSE;
    sessionlostreason = ALLJOYN_SESSIONLOST_INVALID;

    /* free the global unique names */
    ReallocateName(&service_member_added_uniquename, &service_member_added_lock, NULL);
    ReallocateName(&service_member_removed_uniquename, &service_member_removed_lock, NULL);
}

/* service session listener functions */
static void AJ_CALL service_session_lost(const void* context, alljoyn_sessionid sessionId, alljoyn_sessionlostreason reason) {
    QCC_UNUSED(context);
    QCC_UNUSED(sessionId);

    service_sessionlostreason = reason;
    service_session_lost_flag = QCC_TRUE;
}

static void AJ_CALL service_session_member_added(const void* context, alljoyn_sessionid sessionid, const char* uniqueName) {
    QCC_UNUSED(context);
    QCC_UNUSED(sessionid);

    ReallocateName(&service_member_added_uniquename, &service_member_added_lock, uniqueName);
    service_session_member_added_flag = QCC_TRUE;
}

static void AJ_CALL service_session_member_removed(const void* context, alljoyn_sessionid sessionId, const char* uniqueName) {
    QCC_UNUSED(context);
    QCC_UNUSED(sessionId);

    ReallocateName(&service_member_removed_uniquename, &service_member_removed_lock, uniqueName);
    service_session_member_removed_flag = QCC_TRUE;
}

TEST_F(SessionListenerTest, removesessionmember) {
    alljoyn_sessionlistener_callbacks service_sessionlisternerCbs = {
        &service_session_lost,
        &service_session_member_added,
        &service_session_member_removed
    };
    alljoyn_sessionlistener service_sessionListener = alljoyn_sessionlistener_create(&service_sessionlisternerCbs, NULL);

    resetFlags();
    resetServicSessionListenerFlags();
    /* Begin discover of the well-known name */
    status = alljoyn_busattachment_findadvertisedname(clientbus, objectName.c_str());
    EXPECT_EQ(ER_OK, status) << "  Actual Status: " << QCC_StatusText(status);
    for (size_t i = 0; i < 1000; ++i) {
        if (found_advertised_name_flag) {
            break;
        }
        qcc::Sleep(5);
    }
    EXPECT_TRUE(found_advertised_name_flag);

    alljoyn_sessionid sid;
    joinsessionid = 0;
    status = alljoyn_busattachment_joinsession(clientbus, objectName.c_str(), SESSION_PORT, sessionListener, &sid, opts);
    EXPECT_EQ(ER_OK, status) << "  Actual Status: " << QCC_StatusText(status);
    for (size_t i = 0; i < 1000; ++i) {
        if (session_joined_flag && session_member_added_flag) {
            break;
        }
        qcc::Sleep(5);
    }
    EXPECT_TRUE(session_joined_flag);
    EXPECT_EQ(sid, joinsessionid);
    EXPECT_TRUE(session_member_added_flag);
    EXPECT_STREQ(alljoyn_busattachment_getuniquename(servicebus), member_added_uniquename);

    /* we now have a session id we can use to set the session listener for the service */
    status = alljoyn_busattachment_setsessionlistener(servicebus, sid, service_sessionListener);
    EXPECT_EQ(ER_OK, status) << "  Actual Status: " << QCC_StatusText(status);

    alljoyn_busattachment clientbus2 = alljoyn_busattachment_create("SessionListenerTestClient", QCC_FALSE);
    EXPECT_EQ(ER_OK, alljoyn_busattachment_start(clientbus2));
    EXPECT_EQ(ER_OK, alljoyn_busattachment_connect(clientbus2, ajn::getConnectArg().c_str()));

    /* we are only interested in the sessionListener used by the service not any of the clients */
    resetFlags();
    resetServicSessionListenerFlags();
    status = alljoyn_busattachment_joinsession(clientbus2, objectName.c_str(), SESSION_PORT, sessionListener, &sid, opts);
    EXPECT_EQ(ER_OK, status) << "  Actual Status: " << QCC_StatusText(status);
    for (size_t i = 0; i < 1000; ++i) {
        if (session_joined_flag && service_session_member_added_flag && session_member_added_flag) {
            break;
        }
        qcc::Sleep(5);
    }
    EXPECT_TRUE(session_joined_flag);
    EXPECT_EQ(sid, joinsessionid);
    EXPECT_TRUE(session_member_added_flag);
    EXPECT_TRUE(service_session_member_added_flag);

    ASSERT_EQ(ER_OK, service_member_added_lock.Lock(MUTEX_CONTEXT));
    std::string service_member_added_uniquename_local = service_member_added_uniquename;
    ASSERT_EQ(ER_OK, service_member_added_lock.Unlock(MUTEX_CONTEXT));
    EXPECT_STREQ(alljoyn_busattachment_getuniquename(clientbus2), service_member_added_uniquename_local.c_str())
        << "clientbus UID = " << alljoyn_busattachment_getuniquename(clientbus) << "\nservicebus UID = "
        << alljoyn_busattachment_getuniquename(servicebus);


    resetFlags();
    resetServicSessionListenerFlags();
    status = alljoyn_busattachment_removesessionmember(servicebus, sid, alljoyn_busattachment_getuniquename(clientbus));
    EXPECT_EQ(ER_OK, status) << "  Actual Status: " << QCC_StatusText(status);
    for (size_t i = 0; i < 1000; ++i) {
        if (service_session_member_removed_flag && session_lost_flag) {
            break;
        }
        qcc::Sleep(5);
    }
    EXPECT_TRUE(service_session_member_removed_flag);

    ASSERT_EQ(ER_OK, service_member_removed_lock.Lock(MUTEX_CONTEXT));
    std::string service_member_removed_uniquename_local = service_member_removed_uniquename;
    ASSERT_EQ(ER_OK, service_member_removed_lock.Unlock(MUTEX_CONTEXT));
    EXPECT_STREQ(alljoyn_busattachment_getuniquename(clientbus), service_member_removed_uniquename_local.c_str());

    EXPECT_TRUE(session_lost_flag);
    EXPECT_EQ(ALLJOYN_SESSIONLOST_REMOVED_BY_BINDER, sessionlostreason);



    resetFlags();
    resetServicSessionListenerFlags();
    status = alljoyn_busattachment_removesessionmember(servicebus, sid, alljoyn_busattachment_getuniquename(clientbus2));
    EXPECT_EQ(ER_OK, status) << "  Actual Status: " << QCC_StatusText(status);
    for (size_t i = 0; i < 1000; ++i) {
        if (service_session_lost_flag && service_session_member_removed_flag && session_lost_flag) {
            break;
        }
        qcc::Sleep(5);
    }
    EXPECT_TRUE(service_session_lost_flag);
    EXPECT_EQ(ALLJOYN_SESSIONLOST_REMOTE_END_LEFT_SESSION, service_sessionlostreason);
    EXPECT_TRUE(service_session_member_removed_flag);

    ASSERT_EQ(ER_OK, service_member_removed_lock.Lock(MUTEX_CONTEXT));
    service_member_removed_uniquename_local = service_member_removed_uniquename;
    ASSERT_EQ(ER_OK, service_member_removed_lock.Unlock(MUTEX_CONTEXT));
    EXPECT_STREQ(alljoyn_busattachment_getuniquename(clientbus2), service_member_removed_uniquename_local.c_str());

    EXPECT_TRUE(session_lost_flag);
    EXPECT_EQ(ALLJOYN_SESSIONLOST_REMOVED_BY_BINDER, sessionlostreason);

    alljoyn_sessionlistener_destroy(service_sessionListener);
    resetServicSessionListenerFlags();
    alljoyn_busattachment_destroy(clientbus2);
}
