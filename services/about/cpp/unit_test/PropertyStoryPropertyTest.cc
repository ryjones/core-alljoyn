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
#include <gtest/gtest.h>
#include <alljoyn/MsgArg.h>
#include <alljoyn/about/PropertyStoreProperty.h>

using namespace ajn;
using namespace ajn::services;

TEST(PropertyStorePropertyTest, constructor_one_param) {
    PropertyStoreProperty prop("Foo");
    EXPECT_STREQ("Foo", prop.getPropertyName().c_str());
    EXPECT_TRUE(prop.getIsPublic());
    EXPECT_FALSE(prop.getIsWritable());
    EXPECT_TRUE(prop.getIsAnnouncable());
    EXPECT_STREQ("", prop.getLanguage().c_str());
}

TEST(PropertyStorePropertyTest, constructor_two_param) {
    MsgArg arg("s", "TestMsg");
    PropertyStoreProperty prop("Foo", arg);
    EXPECT_STREQ("Foo", prop.getPropertyName().c_str());
    EXPECT_TRUE(prop.getIsPublic());
    EXPECT_FALSE(prop.getIsWritable());
    EXPECT_TRUE(prop.getIsAnnouncable());
    EXPECT_STREQ("", prop.getLanguage().c_str());
    MsgArg out = prop.getPropertyValue();
    const char* outStr;
    out.Get("s", &outStr);
    EXPECT_STREQ("TestMsg", outStr);
}

TEST(PropertyStorePropertyTest, constructor_five_param) {
    MsgArg arg("s", "TestMsg");
    PropertyStoreProperty prop("Foo", arg, false, true, false);
    EXPECT_STREQ("Foo", prop.getPropertyName().c_str());
    EXPECT_FALSE(prop.getIsPublic());
    EXPECT_TRUE(prop.getIsWritable());
    EXPECT_FALSE(prop.getIsAnnouncable());
    EXPECT_STREQ("", prop.getLanguage().c_str());
    MsgArg out = prop.getPropertyValue();
    const char* outStr;
    out.Get("s", &outStr);
    EXPECT_STREQ("TestMsg", outStr);
}

TEST(PropertyStorePropertyTest, constructor_six_param) {
    MsgArg arg("s", "TestMsg");
    PropertyStoreProperty prop("Foo", arg, "es", true, true, true);
    EXPECT_STREQ("Foo", prop.getPropertyName().c_str());
    EXPECT_TRUE(prop.getIsPublic());
    EXPECT_TRUE(prop.getIsWritable());
    EXPECT_TRUE(prop.getIsAnnouncable());
    EXPECT_STREQ("es", prop.getLanguage().c_str());
    MsgArg out = prop.getPropertyValue();
    const char* outStr;
    out.Get("s", &outStr);
    EXPECT_STREQ("TestMsg", outStr);
}


TEST(PropertyStorePropertyTest, setFlags) {
    PropertyStoreProperty prop("Foo");

    prop.setFlags(false, true, false);
    EXPECT_FALSE(prop.getIsPublic());
    EXPECT_TRUE(prop.getIsWritable());
    EXPECT_FALSE(prop.getIsAnnouncable());
}

TEST(PropertyStorePropertyTest, setLanguage) {
    PropertyStoreProperty prop("Foo");

    prop.setLanguage("fr");
    EXPECT_STREQ("fr", prop.getLanguage().c_str());
}

TEST(PropertyStorePropertyTest, setIsPublic) {
    PropertyStoreProperty prop("Foo");

    prop.setIsPublic(false);
    EXPECT_FALSE(prop.getIsPublic());
}


TEST(PropertyStorePropertyTest, setIsWritable) {
    PropertyStoreProperty prop("Foo");

    prop.setIsWritable(true);
    EXPECT_TRUE(prop.getIsWritable());
}

TEST(PropertyStorePropertyTest, setIsAnnouncable) {
    PropertyStoreProperty prop("Foo");

    prop.setIsAnnouncable(false);
    EXPECT_FALSE(prop.getIsAnnouncable());
}

// Why is it the only way to set the value is through a constructor?