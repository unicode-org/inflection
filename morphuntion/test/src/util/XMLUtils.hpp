/**
 * Copyright 2022-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <libxml/tree.h>
#include <map>
#include <string>

class XMLUtils final {

public:
    static std::map<std::u16string, std::u16string> getAttributes(xmlNodePtr node);
    static void getContent(::std::u16string* result, xmlNodePtr nodePtr);
    static std::u16string getRequiredAttribute(xmlNodePtr node, const char *attributeName);
private:
    XMLUtils() = delete;
};
