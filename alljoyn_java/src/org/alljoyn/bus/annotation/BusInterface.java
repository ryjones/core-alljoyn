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
 * Indicates that a given Java interface is also an AllJoyn interface.
 * AllJoyn interfaces contain AllJoyn methods and AllJoyn signals which are
 * exposed to external applications.
 */
@Documented
@Retention(RetentionPolicy.RUNTIME)
@Target(ElementType.TYPE)
public @interface BusInterface {

    /**
     * Override of the default interface name.
     * The default AllJoyn interface name is the fully qualified name of
     * the Java interface.  The override may be used to specify the
     * explicit AllJoyn interface name.
     *
     * @return name specified by the BusInterface annotation
     */
    String name() default "";

    /**
     * specify if the interface is announced
     * possible values are "true" or "false"
     * defaults to "false"
     *
     * @return announced value specified in the BusInterface annotation
     */
    String announced() default "false";

    /**
     * This interface's description language
     *
     * @return descriptionLangauge specified in the BusInterface annotation
     */
    String descriptionLanguage() default "";

    /**
     * This interface's description
     *
     * @return description specified in the BusInterface annotation
     */
    String description() default "";

    /**
     * Class name of an org.alljoyn.bus.Translator instance used
     * to translate this interface's descriptions
     *
     * @return descriptionTranslator specified in the BusInterface annotation
     */
    @Deprecated String descriptionTranslator() default "";
}