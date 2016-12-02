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
////////////////////////////////////////////////////////////////////////////////
//
//  ALLJOYN MODELING TOOL - GENERATED CODE
//
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
//
//  DoorObject.h
//
////////////////////////////////////////////////////////////////////////////////

#import "AJNDoorObject.h"


////////////////////////////////////////////////////////////////////////////////
//
//  DoorObject
//
////////////////////////////////////////////////////////////////////////////////

@interface DoorObject : AJNDoorObject

- (instancetype)initWithLocation:(NSString *)location
                         keyCode:(NSNumber *)keyCode
                          isOpen:(BOOL)isOpen
                   busAttachment:(AJNBusAttachment *)busAttachment
                            path:(NSString *)path;

@end

////////////////////////////////////////////////////////////////////////////////
