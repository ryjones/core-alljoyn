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
/* DO NOT EDIT THIS FILE - it is machine generated */
#include <jni.h>
/* Header for class org_allseen_sample_event_tester_BusHandler */

#ifndef _Included_org_allseen_sample_event_tester_BusHandler
#define _Included_org_allseen_sample_event_tester_BusHandler
#ifdef __cplusplus
extern "C" {
#endif
/*
 * Class:     org_allseen_sample_event_tester_BusHandler
 * Method:    initialize
 * Signature: (Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_org_allseen_sample_event_tester_BusHandler_initialize
    (JNIEnv *, jobject, jstring);

JNIEXPORT void JNICALL Java_org_allseen_sample_event_tester_BusHandler_startRuleEngine
    (JNIEnv *, jobject);

/*
 * Class:     org_allseen_sample_event_tester_BusHandler
 * Method:    dointrospection
 * Signature: (Ljava/lang/String;Ljava/lang/String;I)Ljava/langString;
 */
JNIEXPORT jstring JNICALL Java_org_allseen_sample_event_tester_BusHandler_doIntrospection
    (JNIEnv *, jobject, jstring, jstring, jint);

JNIEXPORT void JNICALL Java_org_allseen_sample_event_tester_BusHandler_introspectionDone
    (JNIEnv *, jobject, jint);

JNIEXPORT jboolean JNICALL Java_org_allseen_sample_event_tester_BusHandler_enableEvent
    (JNIEnv *, jobject, jstring, jstring, jstring, jstring, jstring);

JNIEXPORT void JNICALL Java_org_allseen_sample_event_tester_BusHandler_callAction
    (JNIEnv *, jobject, jstring, jstring, jstring, jstring, jstring);

/*
 * Class:     org_allseen_sample_event_tester_BusHandler
 * Method:    shutdown
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_org_allseen_sample_event_tester_BusHandler_shutdown
    (JNIEnv *, jobject);

#ifdef __cplusplus
}
#endif
#endif