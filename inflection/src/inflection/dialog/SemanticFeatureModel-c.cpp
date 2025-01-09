/*
 * Copyright 2021-2024 Apple Inc. All rights reserved.
 */
#include <inflection/dialog/SemanticFeatureModel.h>
#include <inflection/dialog/SemanticFeatureModel.hpp>
#include <inflection/util/ULocale.hpp>
#include <inflection/util/TypeConversionUtils.hpp>
#include <inflection/npc.hpp>

INFLECTION_CAPI IDSemanticFeatureModel*
isfmod_create(const char* locale, UErrorCode* status)
{
    try {
        return (IDSemanticFeatureModel*)new ::inflection::dialog::SemanticFeatureModel(inflection::util::ULocale(npc(locale)));
    }
    catch (const ::std::exception& e) {
        inflection::util::TypeConversionUtils::convert(e, status);
    }
    return nullptr;
}

INFLECTION_CAPI void
isfmod_destroy(IDSemanticFeatureModel* thisObject)
{
    delete (::inflection::dialog::SemanticFeatureModel*)thisObject;
}
