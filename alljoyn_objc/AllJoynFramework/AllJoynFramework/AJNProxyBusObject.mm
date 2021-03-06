////////////////////////////////////////////////////////////////////////////////
//    Copyright (c) Open Connectivity Foundation (OCF), AllJoyn Open Source
//    Project (AJOSP) Contributors and others.
//
//    SPDX-License-Identifier: Apache-2.0
//
//    All rights reserved. This program and the accompanying materials are
//    made available under the terms of the Apache License, Version 2.0
//    which accompanies this distribution, and is available at
//    http://www.apache.org/licenses/LICENSE-2.0
//
//    Copyright (c) Open Connectivity Foundation and Contributors to AllSeen
//    Alliance. All rights reserved.
//
//    Permission to use, copy, modify, and/or distribute this software for
//    any purpose with or without fee is hereby granted, provided that the
//    above copyright notice and this permission notice appear in all
//    copies.
//
//    THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL
//    WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED
//    WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE
//    AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL
//    DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
//    PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
//    TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
//    PERFORMANCE OF THIS SOFTWARE.
////////////////////////////////////////////////////////////////////////////////

#import <alljoyn/BusAttachment.h>
#import <alljoyn/ProxyBusObject.h>
#import <alljoyn/Observer.h>

#import "AJNProxyBusObject.h"
#import "AJNBusAttachment.h"
#import "AJNInterfaceDescription.h"
#import "AJNInterfaceMember.h"
#import "AJNInterfaceProperty.h"
#import "AJNMessage.h"
#import "AJNMessageArgument.h"

////////////////////////////////////////////////////////////////////////////////
//
//  Asynchronous proxy bus object callback implementation
//

using namespace ajn;

class AJNProxyBusObjectAsyncCallbackImpl : public ProxyBusObject::Listener, public MessageReceiver
{
private:
    __weak id<AJNProxyBusObjectDelegate> m_delegate;
public:
    /** Constructors */
    AJNProxyBusObjectAsyncCallbackImpl(id<AJNProxyBusObjectDelegate> delegate) : m_delegate(delegate) { }

    /** Destructor */
    virtual ~AJNProxyBusObjectAsyncCallbackImpl()
    {
        m_delegate = nil;
    }

    /**
     * Callback registered with IntrospectRemoteObjectAsync()
     *
     * @param status ER_OK if successful
     * @param obj       Remote bus object that was introspected
     * @param context   Context passed in IntrospectRemoteObjectAsync()
     */
    void IntrospectionCallback(QStatus status, ProxyBusObject* obj, void* context)
    {
        if ([m_delegate respondsToSelector:@selector(didCompleteIntrospectionOfObject:context:withStatus:)]) {
            __block id<AJNProxyBusObjectDelegate> theDelegate = m_delegate;
            dispatch_async(dispatch_get_main_queue(), ^{
                [theDelegate didCompleteIntrospectionOfObject:[[AJNProxyBusObject alloc] initWithHandle:(AJNHandle)obj] context:context withStatus:status];
            });
        }

        delete this;
    }

    /**
     * ReplyHandlers are MessageReceiver methods which are called by AllJoyn library
     * to forward AllJoyn method_reply and error responses to AllJoyn library users.
     *
     * @param message   The received message.
     * @param context   User-defined context passed to MethodCall and returned upon reply.
     */
    void ReplyHandler(Message& message, void* context)
    {
        if ([m_delegate respondsToSelector:@selector(didReceiveMethodReply:context:)]) {
            __block id<AJNProxyBusObjectDelegate> theDelegate = m_delegate;
            dispatch_async(dispatch_get_main_queue(), ^{
                [theDelegate didReceiveMethodReply:[[AJNMessage alloc] initWithHandle:(AJNHandle)&message] context:context];
            });
        }

        delete this;
    }

    /**
     * Callback registered with GetPropertyAsync()
     *
     * @param status    - ER_OK if the property get request was successfull or:
     *                  - ER_BUS_OBJECT_NO_SUCH_INTERFACE if the specified interfaces does not exist on the remote object.
     *                  - ER_BUS_NO_SUCH_PROPERTY if the property does not exist
     *                  - Other error status codes indicating the reason the get request failed.
     * @param obj       Remote bus object that was introspected
     * @param value     If status is ER_OK a MsgArg containing the returned property value
     * @param context   Caller provided context passed in to GetPropertyAsync()
     */
    void GetPropertyCallback(QStatus status, ProxyBusObject* obj, const MsgArg& value, void* context)
    {
        if ([m_delegate respondsToSelector:@selector(didReceiveValueForProperty:ofObject:completionStatus:context:)]) {
            __block id<AJNProxyBusObjectDelegate> theDelegate = m_delegate;
            dispatch_async(dispatch_get_main_queue(), ^{
                [theDelegate didReceiveValueForProperty:[[AJNMessageArgument alloc] initWithHandle:(AJNHandle)&value] ofObject:[[AJNProxyBusObject alloc] initWithHandle:(AJNHandle)obj] completionStatus:status context:context];
            });
        }

        delete this;
    }

    /**
    * Callback registered with GetPropertyAsync()
    *
    * @param status            - ER_OK if the property get request was successful or:
    *                          - ER_BUS_OBJECT_NO_SUCH_INTERFACE if the specified interface does not exist on the remote object
    *                          - ER_BUS_NO_SUCH_PROPERTY if the property does not exist
    *                          - Other error status codes indicating the reason the get request failed
    * @param obj               Remote bus object that was introspected
    * @param value             If status is ER_OK a MsgArg containing the returned property value
    * @param errorName         Error name
    * @param errorDescription  Error description
    * @param context           Caller provided context passed in to GetPropertyAsync()
    */
    void GetPropertyAsyncCallback(QStatus status, ProxyBusObject* obj, const MsgArg& value, const qcc::String& errorName, const qcc::String& errorDescription, void* context)
    {
        if ([m_delegate respondsToSelector:@selector(didReceiveValueAndErrorsForProperty:ofObject:completionStatus:context:withErrorName:withErrorDescription:)]) {
            __block id<AJNProxyBusObjectDelegate> theDelegate = m_delegate;
            dispatch_async(dispatch_get_main_queue(), ^{
                [theDelegate didReceiveValueAndErrorsForProperty:[[AJNMessageArgument alloc] initWithHandle:(AJNHandle)&value]
                                                        ofObject:[[AJNProxyBusObject alloc] initWithHandle:(AJNHandle)obj]
                                                completionStatus:status
                                                         context:context
                                                   withErrorName:[NSString stringWithCString:errorName.c_str() encoding:NSUTF8StringEncoding]
                                            withErrorDescription:[NSString stringWithCString:errorDescription.c_str() encoding:NSUTF8StringEncoding] ];
            });
        }
    }

    /**
     * Callback registered with GetAllPropertiesAsync()
     *
     * @param status    - ER_OK if the get all properties request was successfull or:
     *                  - ER_BUS_OBJECT_NO_SUCH_INTERFACE if the specified interfaces does not exist on the remote object.
     *                  - Other error status codes indicating the reason the get request failed.
     * @param obj         Remote bus object that was introspected
     * @param[out] values If status is ER_OK an array of dictionary entries, signature "a{sv}" listing the properties.
     * @param context     Caller provided context passed in to GetPropertyAsync()
     */
    void GetAllPropertiesCallback(QStatus status, ProxyBusObject* obj, const MsgArg& values, void* context)
    {
        if ([m_delegate respondsToSelector:@selector(didReceiveValuesForAllProperties:ofObject:completionStatus:context:)]) {
            __block id<AJNProxyBusObjectDelegate> theDelegate = m_delegate;
            dispatch_async(dispatch_get_main_queue(), ^{
                [theDelegate didReceiveValuesForAllProperties:[[AJNMessageArgument alloc] initWithHandle:(AJNHandle)&values] ofObject:[[AJNProxyBusObject alloc] initWithHandle:(AJNHandle)obj] completionStatus:status context:context];
            });
        }
        delete this;
    }

    /**
     * Callback registered with GetAllPropertiesAsync()
     *
     * @param status           - ER_OK if the get all properties request was successful or:
     *                         - ER_BUS_OBJECT_NO_SUCH_INTERFACE if the specified interface does not exist on the remote object
     *                         - Other error status codes indicating the reason the get request failed
     * @param obj              Remote bus object that was introspected
     * @param[out] values      If status is ER_OK an array of dictionary entries, signature "a{sv}" listing the properties
     * @param errorName        Error name
     * @param errorDescription Error description
     * @param context          Caller provided context passed in to GetPropertyAsync()
     */
    void GetAllPropertiesAsyncCallback(QStatus status, ProxyBusObject* obj, const MsgArg& values, const qcc::String& errorName, const qcc::String& errorDescription, void* context)
    {
        if ([m_delegate respondsToSelector:@selector(didReceiveValuesForAllProperties:ofObject:completionStatus:context:)]) {
            __block id<AJNProxyBusObjectDelegate> theDelegate = m_delegate;
            dispatch_async(dispatch_get_main_queue(), ^{
                [theDelegate didReceiveValuesAndErrorsForAllProperties:[[AJNMessageArgument alloc] initWithHandle:(AJNHandle)&values] ofObject:[[AJNProxyBusObject alloc] initWithHandle:(AJNHandle)obj] completionStatus:status context:context withErrorName:[NSString stringWithCString:errorName.c_str() encoding:NSUTF8StringEncoding] withErrorDescription:[NSString stringWithCString:errorDescription.c_str() encoding:NSUTF8StringEncoding] ];
            });
        }
        delete this;
    }

    /**
     * Callback registered with SetPropertyAsync()
     *
     * @param status    - ER_OK if the property was successfully set or:
     *                  - ER_BUS_OBJECT_NO_SUCH_INTERFACE if the specified interfaces does not exist on the remote object.
     *                  - ER_BUS_NO_SUCH_PROPERTY if the property does not exist
     *                  - Other error status codes indicating the reason the set request failed.
     * @param obj       Remote bus object that was introspected
     * @param context   Caller provided context passed in to SetPropertyAsync()
     */
    void SetPropertyCallback(QStatus status, ProxyBusObject* obj, void* context)
    {
        if ([m_delegate respondsToSelector:@selector(didComleteSetPropertyOnObject:completionStatus:context:)]) {
            __block id<AJNProxyBusObjectDelegate> theDelegate = m_delegate;
            dispatch_async(dispatch_get_main_queue(), ^{
                [theDelegate didCompleteSetPropertyOnObject:[[AJNProxyBusObject alloc] initWithHandle:(AJNHandle)obj] completionStatus:status context:context];
            });
        }
        delete this;
    }

    /**
     * Callback registered with SetPropertyAsync()
     *
     * @param status    - ER_OK if the property was successfully set or:
     *                  - ER_BUS_OBJECT_NO_SUCH_INTERFACE if the specified interfaces does not exist on the remote object.
     *                  - ER_BUS_NO_SUCH_PROPERTY if the property does not exist
     *                  - Other error status codes indicating the reason the set request failed.
     * @param obj       Remote bus object that was introspected
     * @param errorName        Error name
     * @param errorDescription Error description
     * @param context   Caller provided context passed in to SetPropertyAsync()
     */
    void SetPropertyAsyncCallback(QStatus status, ProxyBusObject* obj, const qcc::String& errorName, const qcc::String& errorDescription, void* context)
    {
        if ([m_delegate respondsToSelector:@selector(didComleteSetPropertyOnObject:completionStatus:context:)]) {
            __block id<AJNProxyBusObjectDelegate> theDelegate = m_delegate;
            dispatch_async(dispatch_get_main_queue(), ^{
                [theDelegate didCompleteSetPropertyOnObjectAndReturnedErrors:[[AJNProxyBusObject alloc] initWithHandle:(AJNHandle)obj] completionStatus:status context:context withErrorName:[NSString stringWithCString:errorName.c_str() encoding:NSUTF8StringEncoding] withErrorDescription:[NSString stringWithCString:errorDescription.c_str() encoding:NSUTF8StringEncoding]];
            });
        }
        delete this;
    }
};

class AJNPropertiesChangedListenerImpl : public ajn::ProxyBusObject::PropertiesChangedListener {
    public:
        AJNPropertiesChangedListenerImpl(id<AJNPropertiesChangedDelegate> delegate): m_delegate(delegate) {}

        /**
         * Callback to receive property changed events.
         *
         * @param obj           Remote bus object that owns the property that changed.
         * @param ifaceName     Name of the interface that defines the property.
         * @param changed       Property values that changed as an array of dictionary entries, signature "a{sv}".
         * @param invalidated   Properties whose values have been invalidated, signature "as".
         * @param context       Caller provided context passed in to RegisterPropertiesChangedListener
         */
        void PropertiesChanged(ajn::ProxyBusObject& obj, const char* ifaceName, const MsgArg& changed, const MsgArg& invalidated, void* context)
        {
            if ([m_delegate respondsToSelector:@selector(didPropertiesChanged:inteface:changedMsgArg:invalidatedMsgArg:context:)]) {
                [m_delegate didPropertiesChanged:[[AJNProxyBusObject alloc] initWithHandle:(AJNHandle)&obj]
                                        inteface:[NSString stringWithCString:ifaceName encoding:NSUTF8StringEncoding]
                                   changedMsgArg:[[AJNMessageArgument alloc] initWithHandle:(AJNHandle)(&changed)]
                               invalidatedMsgArg:[[AJNMessageArgument alloc] initWithHandle:(AJNHandle)(&invalidated)]
                                         context:context];
            }
        }
    private:
        __weak id<AJNPropertiesChangedDelegate> m_delegate;
};

////////////////////////////////////////////////////////////////////////////////

using namespace ajn;

@interface AJNBusAttachment(Private)

@property (nonatomic, readonly) BusAttachment *busAttachment;

@end

@interface AJNMessageArgument(Private)

@property (nonatomic, readonly) MsgArg *msgArg;

@end

@interface AJNInterfaceMember(Private)

@property (nonatomic, readonly) ajn::InterfaceDescription::Member *member;

@end

@interface AJNMessage(Private)

@property (nonatomic, readonly) _Message *message;

@end


@interface AJNProxyBusObject()

@property (nonatomic, strong) AJNBusAttachment *bus;
@property (nonatomic, readonly) ProxyBusObject *proxyBusObject;
@property (nonatomic, readwrite) BOOL shouldDeleteHandleOnDealloc;
@end

@implementation AJNProxyBusObject

@synthesize bus = _bus;

- (NSString *)path
{
    return [NSString stringWithCString:self.proxyBusObject->GetPath().c_str() encoding:NSUTF8StringEncoding];
}

- (NSString *)serviceName
{
    return [NSString stringWithCString:self.proxyBusObject->GetServiceName().c_str() encoding:NSUTF8StringEncoding];
}

- (NSString *)uniqueName
{
    return [NSString stringWithCString:self.proxyBusObject->GetUniqueName().c_str() encoding:NSUTF8StringEncoding];
}

- (AJNSessionId)sessionId
{
    return self.proxyBusObject->GetSessionId();
}

- (NSArray *)interfaces
{
    size_t interfaceCount = self.proxyBusObject->GetInterfaces();
    NSMutableArray *interfaces = [[NSMutableArray alloc] initWithCapacity:interfaceCount];
    const ajn::InterfaceDescription** pInterfaces = new const ajn::InterfaceDescription *[interfaceCount];
    self.proxyBusObject->GetInterfaces(pInterfaces, interfaceCount);
    for (int i = 0; i < interfaceCount; i++) {
        const ajn::InterfaceDescription *interface = pInterfaces[i];
        [interfaces addObject:[[AJNInterfaceDescription alloc] initWithHandle:(AJNHandle)interface]];
    }
    delete [] pInterfaces;
    return interfaces;
}

-(NSArray *)children
{
    size_t childCount = self.proxyBusObject->GetChildren();
    NSMutableArray *children = [[NSMutableArray alloc] initWithCapacity:childCount];
    ajn::ProxyBusObject** pChildProxies = new ajn::ProxyBusObject *[childCount];
    self.proxyBusObject->GetChildren(pChildProxies, childCount);
    for (int i = 0; i < childCount; i++) {
        const ajn::ProxyBusObject *child = pChildProxies[i];
        [children addObject:[[AJNProxyBusObject alloc] initWithHandle:(AJNHandle)child]];
    }
    delete [] pChildProxies;
    return children;
}

- (BOOL)isValid
{
    return self.proxyBusObject->IsValid() ? YES : NO;
}

- (BOOL)isSecure
{
    return self.proxyBusObject->IsSecure() ? YES : NO;
}

/**
 * Helper to return the C++ API object that is encapsulated by this objective-c class
 */
- (ProxyBusObject*)proxyBusObject
{
    return static_cast<ProxyBusObject*>(self.handle);
}

- (id)initWithBusAttachment:(AJNBusAttachment*)busAttachment serviceName:(NSString*)serviceName objectPath:(NSString*)path sessionId:(AJNSessionId)sessionId
{
    return [self initWithBusAttachment:busAttachment serviceName:serviceName objectPath:path sessionId:sessionId enableSecurity:NO];
}

- (id)initWithBusAttachment:(AJNBusAttachment *)busAttachment serviceName:(NSString *)serviceName objectPath:(NSString *)path sessionId:(AJNSessionId)sessionId enableSecurity:(BOOL)shouldEnableSecurity
{
    self = [super init];
    if (self) {
        self.bus = busAttachment;
        self.handle = new ProxyBusObject(*((BusAttachment*)busAttachment.handle), [serviceName UTF8String], [path UTF8String], sessionId, shouldEnableSecurity);
        self.shouldDeleteHandleOnDealloc = YES;
    }
    return self;
}

- (id)initWithBusAttachment:(AJNBusAttachment *)busAttachment usingProxyBusObject:(AJNHandle)proxyBusObject
{
    self = [super init];
    if (self) {
        self.bus = busAttachment;
        self.handle = new ProxyBusObject(*(ProxyBusObject *)proxyBusObject);
        self.shouldDeleteHandleOnDealloc = YES;
    }
    return self;
}

- (void)dealloc
{
    if (YES == self.shouldDeleteHandleOnDealloc) {
        ProxyBusObject *ptr = (ProxyBusObject *)self.handle;
        delete ptr;
    }
    self.handle = nil;
}

- (QStatus)addInterfaceNamed:(NSString*)interfaceName
{
    AJNInterfaceDescription *interfaceDescription = [self.bus interfaceWithName:interfaceName];
    return [self addInterfaceFromDescription:interfaceDescription];
}

- (QStatus)addInterfaceFromDescription:(AJNInterfaceDescription*)interfaceDescription
{
    QStatus status = ER_FAIL;
    if (interfaceDescription) {
        status = self.proxyBusObject->AddInterface(*((InterfaceDescription*)interfaceDescription.handle));
    }
    return status;

}

- (AJNInterfaceDescription*)interfaceWithName:(NSString*)name
{
    return [[AJNInterfaceDescription alloc] initWithHandle:(AJNHandle)self.proxyBusObject->GetInterface([name UTF8String])];
}

- (BOOL)implementsInterfaceWithName:(NSString*)name
{
    return self.proxyBusObject->ImplementsInterface([name UTF8String]) ? YES : NO;
}


- (AJNProxyBusObject*)childAtPath:(NSString*)path
{
    return [[AJNProxyBusObject alloc] initWithHandle:self.proxyBusObject->GetChild([path UTF8String])];
}

- (QStatus)addChild:(AJNProxyBusObject*)child
{
    return self.proxyBusObject->AddChild(*child.proxyBusObject);
}

- (QStatus)removeChildAtPath:(NSString*)path
{
    return self.proxyBusObject->RemoveChild([path UTF8String]);
}

- (QStatus)callMethod:(AJNInterfaceMember*)method withArguments:(NSArray*)arguments methodReply:(AJNMessage**)reply
{
    AJNMessage *msg;
    return [self callMethod:method withArguments:arguments methodReply:reply timeout:ajn::ProxyBusObject::DefaultCallTimeout flags:0 msg:&msg];
}

- (QStatus)callMethod:(AJNInterfaceMember*)method withArguments:(NSArray*)arguments methodReply:(AJNMessage**)reply timeout:(uint32_t)timeout flags:(uint8_t)flags msg:(AJNMessage**)callMsg
{
    QStatus status;
    MsgArg * pArgs = new MsgArg[arguments.count];
    for (int i = 0; i < arguments.count; i++) {
        pArgs[i] = *[[arguments objectAtIndex:i] msgArg];
    }
    Message *replyMsg = new Message(*self.bus.busAttachment);
    Message *methodCallMsg = new Message(*self.bus.busAttachment);
    status = self.proxyBusObject->MethodCall(*(method.member), pArgs, arguments.count, *replyMsg, timeout, flags, methodCallMsg);
    delete [] pArgs;
    *reply = [[AJNMessage alloc] initWithHandle:replyMsg];
    *callMsg = [[AJNMessage alloc] initWithHandle:methodCallMsg];
    return status;
}

- (QStatus)callMethod:(NSString*)ifaceName method:(NSString*)methodName withArguments:(NSArray*)args flags:(uint8_t)flags
{
    QStatus status = ER_OK;
    MsgArg * pArgs = new MsgArg[args.count];
    for (int i = 0; i < args.count; i++) {
        pArgs[i] = *[[args objectAtIndex:i] msgArg];
    }
    status = self.proxyBusObject->MethodCall([ifaceName UTF8String], [methodName UTF8String], pArgs, args.count, flags);
    delete [] pArgs;
    return status;
}

- (QStatus)callMethod:(AJNInterfaceMember*)method withArguments:(NSArray*)args flags:(uint8_t)flags
{
    QStatus status = ER_OK;
    MsgArg * pArgs = new MsgArg[args.count];
    for (int i = 0; i < args.count; i++) {
        pArgs[i] = *[[args objectAtIndex:i] msgArg];
    }
    status = self.proxyBusObject->MethodCall(*(method.member), pArgs, args.count, flags);
    delete [] pArgs;
    return status;
}


- (QStatus)callMethod:(AJNInterfaceMember*)method withArguments:(NSArray*)arguments methodReplyDelegate:(id<AJNProxyBusObjectDelegate>)replyDelegate context:(AJNHandle)context timeout:(uint32_t)timeout flags:(uint8_t)flags
{
    QStatus status;
    AJNProxyBusObjectAsyncCallbackImpl *callbackImpl = new AJNProxyBusObjectAsyncCallbackImpl(replyDelegate);
    MsgArg * pArgs = new MsgArg[arguments.count];
    for (int i = 0; i < arguments.count; i++) {
        pArgs[i] = *[[arguments objectAtIndex:i] msgArg];
    }
    status = self.proxyBusObject->MethodCallAsync(*(method.member), callbackImpl, (MessageReceiver::ReplyHandler)(&AJNProxyBusObjectAsyncCallbackImpl::ReplyHandler), pArgs, arguments.count, context, timeout, flags);
    delete [] pArgs;
    return status;
}

- (QStatus)callMethodWithName:(NSString*)methodName onInterfaceWithName:(NSString*)interfaceName withArguments:(NSArray*)arguments methodReply:(AJNMessage**)reply
{
    return [self callMethodWithName:methodName onInterfaceWithName:interfaceName withArguments:arguments methodReply:reply timeout:ajn::ProxyBusObject::DefaultCallTimeout flags:0];
}

- (QStatus)callMethodWithName:(NSString*)methodName onInterfaceWithName:(NSString*)interfaceName withArguments:(NSArray*)arguments methodReply:(AJNMessage**)reply timeout:(uint32_t)timeout flags:(uint8_t)flags
{
    QStatus status;
    MsgArg * pArgs = new MsgArg[arguments.count];
    for (int i = 0; i < arguments.count; i++) {
        pArgs[i] = *[[arguments objectAtIndex:i] msgArg];
    }
    Message *replyMsg = new Message(*self.bus.busAttachment);
    status = self.proxyBusObject->MethodCall([interfaceName UTF8String], [methodName UTF8String], pArgs, arguments.count, *replyMsg, timeout, flags);
    delete [] pArgs;
    *reply = [[AJNMessage alloc] initWithHandle:replyMsg shouldDeleteHandleOnDealloc:YES];
    return status;
}

- (QStatus)callMethodWithName:(NSString*)methodName onInterfaceWithName:(NSString*)interfaceName withArguments:(NSArray*)arguments methodReplyDelegate:(id<AJNProxyBusObjectDelegate>)replyDelegate context:(AJNHandle)context timeout:(uint32_t)timeout flags:(uint8_t)flags
{
    QStatus status;
    AJNProxyBusObjectAsyncCallbackImpl *callbackImpl = new AJNProxyBusObjectAsyncCallbackImpl(replyDelegate);
    MsgArg * pArgs = new MsgArg[arguments.count];
    for (int i = 0; i < arguments.count; i++) {
        pArgs[i] = *[[arguments objectAtIndex:i] msgArg];
    }
    status = self.proxyBusObject->MethodCallAsync([interfaceName UTF8String], [methodName UTF8String], callbackImpl, (MessageReceiver::ReplyHandler)(&AJNProxyBusObjectAsyncCallbackImpl::ReplyHandler), pArgs, arguments.count, context, timeout, flags);
    delete [] pArgs;
    return status;
}

- (QStatus)introspectRemoteObject
{
    return self.proxyBusObject->IntrospectRemoteObject();
}

- (QStatus)introspectRemoteObject:(uint32_t)timeout
{
    return self.proxyBusObject->IntrospectRemoteObject(timeout);
}

- (QStatus)introspectRemoteObject:(id<AJNProxyBusObjectDelegate>)completionHandler context:(AJNHandle)context
{
    QStatus status;
    AJNProxyBusObjectAsyncCallbackImpl *callbackImpl = new AJNProxyBusObjectAsyncCallbackImpl(completionHandler);
    status = self.proxyBusObject->IntrospectRemoteObjectAsync(callbackImpl, (ProxyBusObject::Listener::IntrospectCB)(&AJNProxyBusObjectAsyncCallbackImpl::IntrospectionCallback), context);
    return status;
}

- (QStatus)introspectRemoteObject:(id<AJNProxyBusObjectDelegate>)completionHandler context:(AJNHandle)context timeout:(uint32_t)timeout
{
    QStatus status;
    AJNProxyBusObjectAsyncCallbackImpl *callbackImpl = new AJNProxyBusObjectAsyncCallbackImpl(completionHandler);
    status = self.proxyBusObject->IntrospectRemoteObjectAsync(callbackImpl, (ProxyBusObject::Listener::IntrospectCB)(&AJNProxyBusObjectAsyncCallbackImpl::IntrospectionCallback), context, timeout);
    return status;
}

- (QStatus)buildFromXml:(NSString*)xmlProxyObjectDescription errorLogId:(NSString*)identifier
{
    return self.proxyBusObject->ParseXml([xmlProxyObjectDescription UTF8String], [identifier UTF8String]);
}

- (AJNMessageArgument*)propertyWithName:(NSString*)propertyName forInterfaceWithName:(NSString*)interfaceName
{
    MsgArg *pArg = new MsgArg;
    QStatus status = self.proxyBusObject->GetProperty([interfaceName UTF8String], [propertyName UTF8String], *pArg);
    if (status != ER_OK) {
        NSLog(@"ERROR: AJNProxyBusObject::propertyWithName:forInterfaceName: failed. %@.", [AJNStatus descriptionForStatusCode:status]);
    }
    return [[AJNMessageArgument alloc] initWithHandle:pArg];
}

- (AJNMessageArgument*)propertyWithName:(NSString*)propertyName forInterfaceWithName:(NSString*)interfaceName fetchTimeout:(uint32_t)timeout
{
    MsgArg *pArg = new MsgArg;
    QStatus status = self.proxyBusObject->GetProperty([interfaceName UTF8String], [propertyName UTF8String], *pArg, timeout);
    if (status != ER_OK) {
        NSLog(@"ERROR: AJNProxyBusObject::propertyWithName:forInterfaceName: failed. %@.", [AJNStatus descriptionForStatusCode:status]);
    }
    return [[AJNMessageArgument alloc] initWithHandle:pArg];
}

- (QStatus)propertyWithName:(NSString*)propertyName forInterfaceWithName:(NSString*)interfaceName withValue:(AJNMessageArgument**)value withErrorName:(NSString**)errorName withErrorDescription:(NSString**)errorDescription fetchTimeout:(uint32_t)timeout
{
    qcc::String ajErrorName;
    qcc::String ajErrorDescription;
    MsgArg *pArg = new MsgArg;
    QStatus status = self.proxyBusObject->GetProperty([interfaceName UTF8String], [propertyName UTF8String], *pArg, ajErrorName, ajErrorDescription, timeout);

    *value = [[AJNMessageArgument alloc] initWithHandle:pArg];

    *errorName = [NSString stringWithCString:ajErrorName.c_str() encoding:NSUTF8StringEncoding];
    *errorDescription = [NSString stringWithCString:ajErrorName.c_str() encoding:NSUTF8StringEncoding];

    return status;
}


- (QStatus)propertyWithName:(NSString *)propertyName forInterfaceWithName:(NSString *)interfaceName completionDelegate:(id<AJNProxyBusObjectDelegate>)delegate context:(AJNHandle)context timeout:(uint32_t)timeout
{
    QStatus status;
    AJNProxyBusObjectAsyncCallbackImpl *callbackImpl = new AJNProxyBusObjectAsyncCallbackImpl(delegate);
    status = self.proxyBusObject->GetPropertyAsync([interfaceName UTF8String], [propertyName UTF8String], callbackImpl, (ProxyBusObject::Listener::GetPropertyCB)(&AJNProxyBusObjectAsyncCallbackImpl::GetPropertyCallback), context, timeout);
    if (status != ER_OK) {
        NSLog(@"ERROR: AJNProxyBusObject::propertyWithName:forInterfaceName:completionDelegate:context:timeout: failed. %@.", [AJNStatus descriptionForStatusCode:status]);
    }
    return status;
}

- (QStatus)propertyWithNameAndErrors:(NSString *)propertyName forInterfaceWithName:(NSString *)interfaceName completionDelegate:(id<AJNProxyBusObjectDelegate>)delegate context:(void*)context timeout:(uint32_t)timeout
{
    QStatus status;
    AJNProxyBusObjectAsyncCallbackImpl *callbackImpl = new AJNProxyBusObjectAsyncCallbackImpl(delegate);
    status = self.proxyBusObject->GetPropertyAsync([interfaceName UTF8String], [propertyName UTF8String], callbackImpl, (ProxyBusObject::Listener::GetPropertyAsyncCB)(&AJNProxyBusObjectAsyncCallbackImpl::GetPropertyAsyncCallback), context, timeout);
    if (status != ER_OK) {
        NSLog(@"ERROR: AJNProxyBusObject::propertyWithName:forInterfaceName:completionDelegate:context:timeout: failed. %@.", [AJNStatus descriptionForStatusCode:status]);
    }
    return status;
}


- (QStatus)propertyValues:(AJNMessageArgument**)values ofInterfaceWithName:(NSString*)interfaceName
{
    MsgArg *pArg = new MsgArg;
    QStatus status = self.proxyBusObject->GetAllProperties([interfaceName UTF8String], *pArg);
    *values = [[AJNMessageArgument alloc] initWithHandle:pArg];
    return status;
}

- (QStatus)propertyValues:(AJNMessageArgument **)values ofInterfaceWithName:(NSString*)interfaceName withErrorName:(NSString**)errorName withErrorDescription:(NSString**)errorDescription timeout:(uint32_t)timeout
{
    qcc::String ajErrorName;
    qcc::String ajErrorDescription;
    MsgArg *pArg = new MsgArg;

    QStatus status = self.proxyBusObject->GetAllProperties([interfaceName UTF8String], *pArg, ajErrorName, ajErrorDescription);

    *values = [[AJNMessageArgument alloc] initWithHandle:pArg];
    *errorName = [NSString stringWithCString:ajErrorName.c_str() encoding:NSUTF8StringEncoding];
    *errorDescription = [NSString stringWithCString:ajErrorDescription.c_str() encoding:NSUTF8StringEncoding];

    return status;
}


- (QStatus)propertyValuesForInterfaceWithName:(NSString *)interfaceName completionDelegate:(id<AJNProxyBusObjectDelegate>)delegate context:(AJNHandle)context timeout:(uint32_t)timeout
{
    QStatus status;
    AJNProxyBusObjectAsyncCallbackImpl *callbackImpl = new AJNProxyBusObjectAsyncCallbackImpl(delegate);
    status = self.proxyBusObject->GetAllPropertiesAsync([interfaceName UTF8String], callbackImpl, (ProxyBusObject::Listener::GetAllPropertiesCB)(&AJNProxyBusObjectAsyncCallbackImpl::GetAllPropertiesCallback), context, timeout);
    if (status != ER_OK) {
        NSLog(@"ERROR: AJNProxyBusObject::propertyValuesForInterfaceWithName:completionDelegate:context:timeout: failed. %@.", [AJNStatus descriptionForStatusCode:status]);
    }
    return status;
}

- (QStatus)propertyValuesAndErrorsForInterfaceWithName:(NSString *)interfaceName completionDelegate:(id<AJNProxyBusObjectDelegate>)delegate context:(AJNHandle)context timeout:(uint32_t)timeout
{
    QStatus status;
    AJNProxyBusObjectAsyncCallbackImpl *callbackImpl = new AJNProxyBusObjectAsyncCallbackImpl(delegate);
    status = self.proxyBusObject->GetAllPropertiesAsync([interfaceName UTF8String], callbackImpl, (ProxyBusObject::Listener::GetAllPropertiesAsyncCB)(&AJNProxyBusObjectAsyncCallbackImpl::GetAllPropertiesAsyncCallback), context, timeout);
    if (status != ER_OK) {
        NSLog(@"ERROR: AJNProxyBusObject::propertyValuesForInterfaceWithName:completionDelegate:context:timeout: failed. %@.", [AJNStatus descriptionForStatusCode:status]);
    }
    return status;
}


- (QStatus)setPropertyWithName:(NSString*)propertyName forInterfaceWithName:(NSString*)interfaceName toValue:(AJNMessageArgument*)value
{
    return self.proxyBusObject->SetProperty([interfaceName UTF8String], [propertyName UTF8String], *value.msgArg);
}

- (QStatus)setPropertyWithName:(NSString*)propertyName forInterfaceWithName:(NSString*)interfaceName toValue:(AJNMessageArgument*)value setTimeout:(uint32_t)timeout
{
    return self.proxyBusObject->SetProperty([interfaceName UTF8String], [propertyName UTF8String], *value.msgArg, timeout);
}

- (QStatus)setPropertyWithName:(NSString*)propertyName forInterfaceWithName:(NSString*)interfaceName toValue:(AJNMessageArgument*)value withErrorName:(NSString**)errorName withErrorDescription:(NSString**)errorDescription setTimeout:(uint32_t)timeout
{
    qcc::String ajErrorName;
    qcc::String ajErrorDescription;
    QStatus status = self.proxyBusObject->SetProperty([interfaceName UTF8String], [propertyName UTF8String], *value.msgArg, ajErrorName, ajErrorDescription, timeout);

    *errorName = [NSString stringWithCString:ajErrorName.c_str() encoding:NSUTF8StringEncoding];
    *errorDescription = [NSString stringWithCString:ajErrorDescription.c_str() encoding:NSUTF8StringEncoding];

    return status;

}

- (QStatus)setPropertyWithName:(NSString *)propertyName forInterfaceWithName:(NSString *)interfaceName toValue:(AJNMessageArgument *)value completionDelegate:(id<AJNProxyBusObjectDelegate>)delegate context:(AJNHandle)context timeout:(uint32_t)timeout
{
    QStatus status;
    AJNProxyBusObjectAsyncCallbackImpl *callbackImpl = new AJNProxyBusObjectAsyncCallbackImpl(delegate);
    status = self.proxyBusObject->SetPropertyAsync([interfaceName UTF8String], [propertyName UTF8String], *value.msgArg, callbackImpl, (ProxyBusObject::Listener::SetPropertyCB)(&AJNProxyBusObjectAsyncCallbackImpl::SetPropertyCallback), context, timeout);
    if (status != ER_OK) {
        NSLog(@"ERROR: AJNProxyBusObject::propertyValuesForInterfaceWithName:completionDelegate:context:timeout: failed. %@.", [AJNStatus descriptionForStatusCode:status]);
    }
    return status;
}

- (QStatus)setPropertyWithNameAndReturnErrors:(NSString *)propertyName forInterfaceWithName:(NSString *)interfaceName toValue:(AJNMessageArgument *)value completionDelegate:(id<AJNProxyBusObjectDelegate>)delegate context:(AJNHandle)context timeout:(uint32_t)timeout
{
    QStatus status;
    AJNProxyBusObjectAsyncCallbackImpl *callbackImpl = new AJNProxyBusObjectAsyncCallbackImpl(delegate);
    status = self.proxyBusObject->SetPropertyAsync([interfaceName UTF8String], [propertyName UTF8String], *value.msgArg, callbackImpl, (ProxyBusObject::Listener::SetPropertyAsyncCB)(&AJNProxyBusObjectAsyncCallbackImpl::SetPropertyAsyncCallback), context, timeout);
    if (status != ER_OK) {
        NSLog(@"ERROR: AJNProxyBusObject::propertyValuesForInterfaceWithName:completionDelegate:context:timeout: failed. %@.", [AJNStatus descriptionForStatusCode:status]);
    }
    return status;
}

- (QStatus)setPropertyWithName:(NSString*)propertyName forInterfaceWithName:(NSString*)interfaceName toUIntValue:(uint32_t)value
{
    return self.proxyBusObject->SetProperty([interfaceName UTF8String], [propertyName UTF8String], value);
}

- (QStatus)setPropertyWithName:(NSString*)propertyName forInterfaceWithName:(NSString *)interfaceName toUIntValue:(uint32_t)value witErrorName:(NSString**)errorName withErrorDescription:(NSString**)errorDescription
{
    qcc::String ajErrorName;
    qcc::String ajErrorDescription;
    QStatus status = self.proxyBusObject->SetProperty([interfaceName UTF8String], [propertyName UTF8String], value, ajErrorName, ajErrorDescription);

    *errorName = [NSString stringWithCString:ajErrorName.c_str() encoding:NSUTF8StringEncoding];
    *errorDescription = [NSString stringWithCString:ajErrorName.c_str() encoding:NSUTF8StringEncoding];

    return status;
}

- (QStatus)setPropertyWithName:(NSString*)propertyName forInterfaceWithName:(NSString*)interfaceName toIntValue:(int32_t)value
{
    return self.proxyBusObject->SetProperty([interfaceName UTF8String], [propertyName UTF8String], value);
}

- (QStatus)setPropertyWithName:(NSString*)propertyName forInterfaceWithName:(NSString *)interfaceName toIntValue:(int32_t)value witErrorName:(NSString**)errorName withErrorDescription:(NSString**)errorDescription
{
    qcc::String ajErrorName;
    qcc::String ajErrorDescription;
    QStatus status = self.proxyBusObject->SetProperty([interfaceName UTF8String], [propertyName UTF8String], value, ajErrorName, ajErrorDescription);

    *errorName = [NSString stringWithCString:ajErrorName.c_str() encoding:NSUTF8StringEncoding];
    *errorDescription = [NSString stringWithCString:ajErrorName.c_str() encoding:NSUTF8StringEncoding];

    return status;
}


- (QStatus)setPropertyWithName:(NSString*)propertyName forInterfaceWithName:(NSString*)interfaceName toStringValue:(NSString*)value
{
    return self.proxyBusObject->SetProperty([interfaceName UTF8String], [propertyName UTF8String], [value UTF8String]);
}

- (QStatus)setPropertyWithName:(NSString *)propertyName forInterfaceWithName:(NSString *)interfaceName toStringValue:(NSString *)value withErrorName:(NSString**)errorName withErrorDescription:(NSString**)errorDescription
{
    qcc::String ajErrorName;
    qcc::String ajErrorDescription;
    QStatus status = self.proxyBusObject->SetProperty([interfaceName UTF8String], [propertyName UTF8String], [value UTF8String], ajErrorName, ajErrorDescription);

    *errorName = [NSString stringWithCString:ajErrorName.c_str() encoding:NSUTF8StringEncoding];
    *errorDescription = [NSString stringWithCString:ajErrorName.c_str() encoding:NSUTF8StringEncoding];

    return status;
}

- (QStatus)registerPropertiesChangedListener:(NSString*)iface properties:(NSArray*)properties delegate:(id<AJNPropertiesChangedDelegate>)listener context:(AJNHandle)context
{
    AJNPropertiesChangedListenerImpl *impl = new AJNPropertiesChangedListenerImpl(listener);
    const char **propArray = new const char*[properties.count];
    for (int i = 0; i < properties.count; i++) {
        NSString *tmp = [properties objectAtIndex:i];
        propArray[i] = [tmp UTF8String];
    }
    return self.proxyBusObject->RegisterPropertiesChangedListener([iface UTF8String], propArray, properties.count, *impl, context);
}

- (QStatus)unregisterPropertiesChangedListener:(NSString*)iface delegate:(id<AJNPropertiesChangedDelegate>)listener;
{
    AJNPropertiesChangedListenerImpl *impl = new AJNPropertiesChangedListenerImpl(listener);
    return self.proxyBusObject->UnregisterPropertiesChangedListener([iface UTF8String], *impl);
}

- (QStatus)secureConnection:(BOOL)forceAuthentication
{
    return self.proxyBusObject->SecureConnection(forceAuthentication);
}

- (QStatus)secureConnectionAsync:(BOOL)forceAuthentication
{
    return self.proxyBusObject->SecureConnectionAsync(forceAuthentication);
}

- (void)enablePropertyCaching
{
    self.proxyBusObject->EnablePropertyCaching();
}

@end
