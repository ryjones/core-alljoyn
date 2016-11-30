/**
 * @file
 *
 * This file just wraps platform specific header files that define the thread
 * abstraction interface.
 */

/******************************************************************************
 *
 *
 * Copyright (c) Open Connectivity Foundation (OCF) and AllJoyn Open
 *    Source Project (AJOSP) Contributors and others.
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
 *     THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL
 *     WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED
 *     WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE
 *     AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL
 *     DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
 *     PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 *     TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 *     PERFORMANCE OF THIS SOFTWARE.
 ******************************************************************************/
#ifndef _OS_QCC_THREAD_H
#define _OS_QCC_THREAD_H

#include <qcc/platform.h>
#include <winsock2.h>
#include <windows.h>
#include <process.h>

/**
 * Windows uses __stdcall for certain API calls
 */
#define STDCALL __stdcall

/**
 * Redefine Windows' Sleep function to match Linux.
 *
 * @param x     Number of seconds to sleep.
 */
#define sleep(x) Sleep(x)

namespace qcc {

typedef HANDLE ThreadHandle;            ///< Window process handle typedef.
typedef unsigned int ThreadId;          ///< Windows uses uint process IDs.

/**
 * Windows' thread function return type
 */
typedef unsigned int ThreadInternalReturn;

}

#endif