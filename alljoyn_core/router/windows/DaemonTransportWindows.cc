/**
 * @file
 */

/******************************************************************************
 * Copyright AllSeen Alliance. All rights reserved.
 *
 *    Permission to use, copy, modify, and/or distribute this software for any
 *    purpose with or without fee is hereby granted, provided that the above
 *    copyright notice and this permission notice appear in all copies.
 *
 *    THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 *    WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 *    MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 *    ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 *    WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 *    ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 *    OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 ******************************************************************************/

#include <qcc/platform.h>
#include <qcc/String.h>

#include "../DaemonTransport.h"

#define QCC_MODULE "ALLJOYN"

using namespace std;
using namespace qcc;

namespace ajn {

const char* DaemonTransport::TransportName = "";

void* DaemonTransport::Run(void* arg)
{
    UNREFERENCED_PARAMETER(arg);
    return NULL;
}

QStatus DaemonTransport::NormalizeTransportSpec(const char* inSpec, qcc::String& outSpec, map<qcc::String, qcc::String>& argMap) const
{
    UNREFERENCED_PARAMETER(inSpec);
    UNREFERENCED_PARAMETER(outSpec);
    UNREFERENCED_PARAMETER(argMap);
    return ER_OK;
}

QStatus DaemonTransport::StartListen(const char* listenSpec)
{
    UNREFERENCED_PARAMETER(listenSpec);
    return ER_OK;
}

QStatus DaemonTransport::StopListen(const char* listenSpec)
{
    UNREFERENCED_PARAMETER(listenSpec);
    return ER_OK;
}

} // namespace ajn
