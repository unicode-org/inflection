/*
 * Copyright 2024-2024 Apple Inc. All rights reserved.
 */
//
// Created by Ronak Agarwal on 2/11/24.
//

#include "util/XMLUtils.hpp"
#include <morphuntion/util/StringUtils.hpp>
#include <morphuntion/exception/XMLParseException.hpp>
#include <morphuntion/npc.hpp>

std::map<std::u16string, std::u16string> XMLUtils::getAttributes(xmlNodePtr node) {
    xmlAttr* attribute = node->properties;
    std::map<std::u16string, std::u16string> result;
    while (attribute) {
        xmlChar* value = xmlNodeListGetString(node->doc, attribute->children, 1);
        result.emplace(morphuntion::util::StringUtils::to_u16string(reinterpret_cast<const char*>(attribute->name)), morphuntion::util::StringUtils::to_u16string(reinterpret_cast<const char*>(value)));
        xmlFree(value);
        attribute = attribute->next;
    }
    return result;
}

void XMLUtils::getContent(::std::u16string* result, xmlNodePtr nodePtr) {
    xmlChar* content = xmlNodeGetContent(nodePtr);
    if (content != nullptr) {
        ::morphuntion::util::StringUtils::convert(result, reinterpret_cast<const char*>(content));
        xmlFree(content);
    }
    else {
        npc(result)->clear();
    }
}

std::u16string XMLUtils::getRequiredAttribute(xmlNodePtr node, const char *attributeName) {
    xmlChar * attributeValue = xmlGetProp(node, (const xmlChar *)attributeName);
    if (attributeValue == nullptr) {
        throw ::morphuntion::exception::XMLParseException(std::u16string(u"The attribute ") + ::morphuntion::util::StringUtils::to_u16string(attributeName) + u" is required on " + ::morphuntion::util::StringUtils::to_u16string((const char *)npc(node)->name));
    }
    ::std::u16string attributeStr;
    ::morphuntion::util::StringUtils::convert(&attributeStr, reinterpret_cast<const char*>(attributeValue));
    xmlFree(attributeValue);
    return attributeStr;
}
