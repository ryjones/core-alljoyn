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
#include "NativeObject.h"

#include "TypeMapping.h"
#include <qcc/Debug.h>

#define QCC_MODULE "ALLJOYN_JS"

NativeObject::NativeObject(Plugin& plugin, NPObject* objectValue) :
    plugin(plugin),
    objectValue(NPN_RetainObject(objectValue))
{
    QCC_DbgTrace(("%s(objectValue=%p)", __FUNCTION__, objectValue));
    plugin->nativeObjects[this] = objectValue;
}

NativeObject::NativeObject(Plugin& plugin) :
    plugin(plugin),
    objectValue(NULL)
{
    QCC_DbgTrace(("%s", __FUNCTION__));
    NPVariant variant = NPVARIANT_VOID;
    if (NewObject(plugin, variant)) {
        objectValue = NPVARIANT_TO_OBJECT(variant);
        plugin->nativeObjects[this] = objectValue;
    } else {
        NPN_ReleaseVariantValue(&variant);
    }
}

NativeObject::~NativeObject()
{
    QCC_DbgTrace(("%s", __FUNCTION__));
    plugin->nativeObjects.erase(this);
    Invalidate();
}

void NativeObject::Invalidate()
{
    QCC_DbgTrace(("%s", __FUNCTION__));
    if (objectValue) {
        NPN_ReleaseObject(objectValue);
        objectValue = NULL;
    }
}

bool NativeObject::operator==(const NativeObject& that) const
{
    NPVariant a, b;
    OBJECT_TO_NPVARIANT(objectValue, a);
    OBJECT_TO_NPVARIANT(that.objectValue, b);
    return plugin->StrictEquals(a, b);
}