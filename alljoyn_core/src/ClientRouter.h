/**
 * @file
 * ClientRouter is responsible for routing Bus messages between a single remote
 * endpoint and a single local endpoint
 */

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
#ifndef _ALLJOYN_CLIENTROUTER_H
#define _ALLJOYN_CLIENTROUTER_H

#include <qcc/platform.h>

#include <qcc/Thread.h>
#include <qcc/String.h>

#include "Router.h"
#include "LocalTransport.h"

#include <alljoyn/Status.h>

namespace ajn {

/**
 * %ClientRouter is responsible for routing Bus messages between a single remote
 * endpoint and a single local endpoint
 */
class ClientRouter : public Router {
    friend class _LocalEndpoint;

  public:

    /**
     * Route an incoming Message Bus Message from an endpoint.
     *
     * @param sender  Endpoint that is sending the message
     * @param msg     Message to be processed.
     * @return
     *      - ER_OK if successful
     *      - ER_BUS_NO_ENDPOINT if unable to find endpoint
     *      - An error status otherwise
     */
    QStatus PushMessage(Message& msg, BusEndpoint& sender);

    /**
     * Register an endpoint.
     *
     * This method must be called by an endpoint before attempting to use the router.
     *
     * @param endpoint   Endpoint being registered.
     * @return ER_OK if successful.
     */
    QStatus RegisterEndpoint(BusEndpoint& endpoint);

    /**
     * Un-register an endpoint.
     *
     * This method must be called by an endpoint before the endpoint is deallocted.
     *
     * @param epName   Name of Endpoint being un-registered.
     */
    void UnregisterEndpoint(const qcc::String& epName, EndpointType epType);

    /**
     * Find the endpoint that owns the given unique or well-known name.
     *
     * @param busname    Unique or well-known bus name
     *
     * @return  Returns the requested endpoint or an invalid endpoint if the
     *          endpoint was not found.
     */
    BusEndpoint FindEndpoint(const qcc::String& busname);

    /**
     * Generate a unique endpoint name.
     *
     * This method is not used for client-side Bus instances. An empty string is returned.
     *
     * @return An empty string.
     */
    qcc::String GenerateUniqueName(void) { return ""; }

    /**
     * Indicate that this is not a daemon bus instance.
     *
     * @return false since the client router is never part of a daemon.
     */
    bool IsDaemon() const { return false; }

    /**
     * Return true if this router is in contact with a bus (either locally or remotely)
     *
     * This method can be used to determine whether messages sent to "the bus" will be routed.
     *
     * @return true iff the messages can be routed currently.
     */
    bool IsBusRunning(void) const { return localEndpoint->IsValid() && nonLocalEndpoint->IsValid(); }

    /**
     * Set the global GUID of the bus.
     * The global GUID is not used/needed for client-side routing.
     *
     * @param guid   GUID of bus associated with this router.
     */
    void SetGlobalGUID(const qcc::GUID128& guid) {
        QCC_UNUSED(guid);
    }

    /**
     * Destructor
     */
    ~ClientRouter();

  private:
    LocalEndpoint localEndpoint;   /**< Local endpoint */
    BusEndpoint nonLocalEndpoint;  /**< Last non-local enpoint to register */

};

}

#endif