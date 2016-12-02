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
#ifndef _SESSIONLOSTREASONINTERFACE_H
#define _SESSIONLOSTREASONINTERFACE_H

#include "ScriptableObject.h"
#include <qcc/ManagedObj.h>

class _SessionLostReasonInterface : public ScriptableObject {
  public:
    static std::map<qcc::String, int32_t>& Constants();
    _SessionLostReasonInterface(Plugin& plugin);
    virtual ~_SessionLostReasonInterface();

  private:
    static std::map<qcc::String, int32_t> constants;
};

typedef qcc::ManagedObj<_SessionLostReasonInterface> SessionLostReasonInterface;

#endif // _SESSIONLOSTREASONINTERFACE_H