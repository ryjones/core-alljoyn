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

package org.alljoyn.services.common;

import java.util.Map;

import org.alljoyn.bus.AboutKeys;
import org.alljoyn.bus.Variant;


/**
 * NOTE: The Common Service classes were incorporated from the now deprecated AJCore About Services project,
 *       so that Config Services no longer depends on alljoyn_about.jar.
 *       This specific class continues to be unnecessary. Remove in release 17.04.
 *
 * An interface to be implemented in order to receive Announcements.
 * @deprecated see org.alljoyn.bus.AboutListener
 */
@Deprecated
public interface AnnouncementHandler
{
    /**
     * Handle a received About Announcement
     * @deprecated org.alljoyn.bus.AboutListener
     * @param serviceName the peer's AllJoyn bus name
     * @param port the peer's bound port for accepting About session connections
     * @param objectDescriptions the peer's BusInterfaces and BusObjects
     * @param aboutData a map of peer's properties.
     * @see AboutKeys
     * @see //AboutTransport#Announce(short, short, BusObjectDescription[], java.util.Map)
     */
    @Deprecated
    public void onAnnouncement(String serviceName, short port, BusObjectDescription[] objectDescriptions, Map<String, Variant> aboutData);

    /**
     * Handle a loss of connectivity with this bus name
     * @deprecated
     * @param serviceName the AllJoyn bus name of the lost peer
     */
    @Deprecated
    public void onDeviceLost(String serviceName);
}
