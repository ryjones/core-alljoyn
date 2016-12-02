/**
 * @file
 *
 * Source implementation used to retrieve bytes from qcc::String
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

#ifndef _QCC_STRINGSOURCE_H
#define _QCC_STRINGSOURCE_H

#include <qcc/platform.h>
#include <qcc/String.h>
#include <qcc/Event.h>
#include <qcc/Stream.h>

#include <algorithm>

namespace qcc {

/**
 * StringSource provides Source based retrieval from std:string storage.
 */
class StringSource : public Source {
  public:

    /**
     * Construct a StringSource.
     * @param str   Source contents
     */
    StringSource(const qcc::String str) : str(str), outIdx(0) { }

    /**
     * Construct a StringSource from data.
     *
     * @param data  Source contents
     * @param len   The length of the data.
     */
    StringSource(const void* data, size_t len) : outIdx(0) { str.insert(0, (const char*)data, len); }

    /** Destructor */
    virtual ~StringSource() { }

    /**
     * Pull bytes from the source.
     * The source is exhausted when ER_EOF is returned.
     *
     * @param buf          Buffer to store pulled bytes
     * @param reqBytes     Number of bytes requested to be pulled from source.
     * @param actualBytes  Actual number of bytes retrieved from source.
     * @param timeout      Timeout in milliseconds.
     * @return   ER_OK if successful. ER_EOF if source is exhausted. Otherwise an error.
     */
    QStatus PullBytes(void* buf, size_t reqBytes, size_t& actualBytes, uint32_t timeout = Event::WAIT_FOREVER);

    /**
     * Return the number of octets not yet read.
     *
     * @return  Number of octets not read.
     */
    size_t Remaining() const { return str.size() - outIdx; }

  private:
    qcc::String str;    /**< storage for byte stream */
    size_t outIdx;      /**< index to next byte in str to be returned */
};

}

#endif