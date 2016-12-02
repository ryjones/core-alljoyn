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

package org.alljoyn.bus.annotation;

import java.lang.annotation.Documented;
import java.lang.annotation.ElementType;
import java.lang.annotation.Retention;
import java.lang.annotation.RetentionPolicy;
import java.lang.annotation.Target;

/**
 * Indicates that a particular member of an AllJoyn exportable interface
 * is defined to be a AllJoyn property.  In addition to the annotation,
 * property methods must be of the form "{@code getProperty}"
 * and/or "{@code setProperty}".  In this case, "Property" is the
 * AllJoyn name of the property and it is both a read and write property.
 */
@Documented
@Retention(RetentionPolicy.RUNTIME)
@Target(ElementType.METHOD)
public @interface BusProperty {

    /**
     * Override of property name.
     * The default property name is the method name without the "get"
     * or "set" prefix.  For example, if the method is getState() then
     * the default property name is "State".
     *
     * @return name specified in the BusProperty annotation
     */
    String name() default "";

    /**
     * Signature of property.
     *
     * @see Signature
     *
     * @return signature specified in the BusProperty annotation
     */
    String signature() default "";

    /**
     * Description of this property
     *
     * @return descroption specified in the BusProperty annotation
     */
    String description() default "";

    /** EmitChangedSignal annotate flag. */
    int ANNOTATE_EMIT_CHANGED_SIGNAL = 1;

    /** EmitChangedSignal annotate flag for invalidation notifications. */
    int ANNOTATE_EMIT_CHANGED_SIGNAL_INVALIDATES = 2;

    /**
     * Annotate introspection data for method.
     * The annotation may be the flag ANNOTATE_EMIT_CHANGED_SIGNAL or
     * ANNOTATE_EMIT_CHANGED_SIGNAL_INVALIDATES.  See
     * org.freedesktop.DBus.Property.EmitsChangedSignal in the D-Bus
     * Specification.
     *
     * @return annotation EmitChagedSignal annotate flag specified in the BusProperty annotation
     */
    int annotation() default 0;

   /**
    * Timeout specified in milliseconds to wait for a reply.
    * The default value is -1.
    * The value -1 means use the implementation dependent default timeout.
    *
    * @return timeout specified in the BusMethod annotation
    */
    int timeout() default -1;
}