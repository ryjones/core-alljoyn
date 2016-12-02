////////////////////////////////////////////////////////////////////////////////
// // 
//    Copyright (c) 2016 Open Connectivity Foundation and AllJoyn Open
//    Source Project Contributors and others.
//    
//    All rights reserved. This program and the accompanying materials are
//    made available under the terms of the Apache License, Version 2.0
//    which accompanies this distribution, and is available at
//    http://www.apache.org/licenses/LICENSE-2.0

////////////////////////////////////////////////////////////////////////////////

#import <alljoyn/InterfaceDescription.h>
#import <alljoyn/MessageReceiver.h>
#import "AJNCConstants.h"
#import "AJNCChatObjectSignalHandlerImpl.h"
#import "AJNCChatReceiver.h"

using namespace ajn;

/**
 * Constructor for the AJN signal handler implementation.
 *
 * @param aDelegate         Objective C delegate called when one of the below virtual functions is called.     
 */    
AJNCChatObjectSignalHandlerImpl::AJNCChatObjectSignalHandlerImpl(id<AJNSignalHandler> aDelegate) : AJNSignalHandlerImpl(aDelegate), chatSignalMember(NULL)
{
}

AJNCChatObjectSignalHandlerImpl::~AJNCChatObjectSignalHandlerImpl()
{
    m_delegate = NULL;
}

void AJNCChatObjectSignalHandlerImpl::RegisterSignalHandler(ajn::BusAttachment &bus)
{
    if (chatSignalMember == NULL) {
        const ajn::InterfaceDescription* chatIntf = bus.GetInterface([kInterfaceName UTF8String]);
        
        /* Store the Chat signal member away so it can be quickly looked up */
        if (chatIntf) {
            chatSignalMember = chatIntf->GetMember("Chat");
            assert(chatSignalMember);
        }
    }
    /* Register signal handler */
    QStatus status =  bus.RegisterSignalHandler(this,
                                                (MessageReceiver::SignalHandler)(&AJNCChatObjectSignalHandlerImpl::ChatSignalHandler),
                                                chatSignalMember,
                                                NULL);
    if (status != ER_OK) {
        NSLog(@"ERROR:AJNCChatObjectSignalHandlerImpl::RegisterSignalHandler failed. %@", [AJNStatus descriptionForStatusCode:status] );
    }
}

void AJNCChatObjectSignalHandlerImpl::UnregisterSignalHandler(ajn::BusAttachment &bus)
{
    if (chatSignalMember == NULL) {
        const ajn::InterfaceDescription* chatIntf = bus.GetInterface([kInterfaceName UTF8String]);
        
        /* Store the Chat signal member away so it can be quickly looked up */
        chatSignalMember = chatIntf->GetMember("Chat");
        assert(chatSignalMember);
    }
    /* Register signal handler */
    QStatus status =  bus.UnregisterSignalHandler(this, static_cast<MessageReceiver::SignalHandler>(&AJNCChatObjectSignalHandlerImpl::ChatSignalHandler), chatSignalMember, NULL);

    if (status != ER_OK) {
        NSLog(@"ERROR:AJNCChatObjectSignalHandlerImpl::UnregisterSignalHandler failed. %@", [AJNStatus descriptionForStatusCode:status] );
    }
}

/** Receive a signal from another Chat client */
void AJNCChatObjectSignalHandlerImpl::ChatSignalHandler(const ajn::InterfaceDescription::Member* member, const char* srcPath, ajn::Message& msg)
{
    @autoreleasepool {
        NSString *message = [NSString stringWithCString:msg->GetArg(0)->v_string.str encoding:NSUTF8StringEncoding];
        NSString *from = [NSString stringWithCString:msg->GetSender() encoding:NSUTF8StringEncoding];
        NSString *objectPath = [NSString stringWithCString:msg->GetObjectPath() encoding:NSUTF8StringEncoding];
        ajn:SessionId sessionId = msg->GetSessionId();
        
        NSLog(@"Received signal [%@] from %@ on path %@ for session id %u [%s > %s]", message, from, objectPath, msg->GetSessionId(), msg->GetRcvEndpointName(), msg->GetDestination() ? msg->GetDestination() : "broadcast");
        
        dispatch_async(dispatch_get_main_queue(), ^{
            [(id<AJNChatReceiver>)m_delegate chatMessageReceived:message from:from onObjectPath:objectPath forSession:sessionId];
        });
        
    }
}    