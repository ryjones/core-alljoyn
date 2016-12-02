/**
 * @file StringStream.cc
 *
 * Sink/Source wrapper for std::string
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
#include <qcc/platform.h>

#include <cstring>

#include <qcc/Event.h>
#include <qcc/Pipe.h>
#include <qcc/Stream.h>

#include <Status.h>

using namespace std;
using namespace qcc;

#define QCC_MODULE "STREAM"

QStatus Pipe::PullBytes(void* buf, size_t reqBytes, size_t& actualBytes, uint32_t timeout)
{
    static const size_t cleanupSize = 4096;

    QStatus status = ER_OK;
    char* _buf = (char*)buf;

    /* Pipe has no network delay so doesn't need long timeouts */
    if (timeout != Event::WAIT_FOREVER) {
        timeout = min(timeout, (uint32_t)5);
    }

    lock.Lock();
    while ((ER_OK == status) && (0 < reqBytes)) {
        size_t b = min(str.size() - outIdx, reqBytes);
        if (0 < b) {
            memcpy(_buf, str.data() + outIdx, b);
            _buf += b;
            outIdx += b;
            reqBytes -= b;
        }
        /* If we haven't read anything yet block until data is available */
        if ((0 < reqBytes) && (_buf == (char*)buf)) {
            isWaiting = true;
            lock.Unlock();
            status = Event::Wait(event, timeout);
            lock.Lock();
            event.ResetEvent();
        } else {
            break;
        }
    }

    /* Perform clean-up if source is completely used up or if outIdx gets too large */
    if (outIdx >= str.size()) {
        str.clear();
        outIdx = 0;
    } else if (outIdx >= cleanupSize) {
        str = str.substr(outIdx);
        outIdx = 0;
    }
    lock.Unlock();

    actualBytes = _buf - (char*)buf;
    return status;
}

QStatus Pipe::PushBytes(const void* buf, size_t numBytes, size_t& numSent)
{
    QStatus status = ER_OK;

    str.append((const char*)buf, numBytes);
    numSent = numBytes;

    lock.Lock();
    if (isWaiting) {
        isWaiting = false;
        status = event.SetEvent();
    }
    lock.Unlock();

    return status;
}

