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

package org.alljoyn.bus;

import org.alljoyn.bus.BusException;
import org.alljoyn.bus.annotation.BusInterface;
import org.alljoyn.bus.annotation.BusMethod;

@BusInterface(name = "org.alljoyn.bus.SimpleDescriptionInterface",
                descriptionLanguage="en",
                description="This is a simple interface",
                descriptionTranslator="org.alljoyn.bus.SimpleDescriptionInterfaceTranslator")

public interface SimpleDescriptionInterface {
    @BusMethod(name="Ping", description="The ping method sends a small piece of data")
    String ping(String inStr) throws BusException;
}