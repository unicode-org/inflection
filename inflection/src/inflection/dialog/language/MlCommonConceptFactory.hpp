/*
 * Copyright 2025 Unicode Incorporated and others. All rights reserved.
 */
#pragma once

#include <inflection/dialog/language/fwd.hpp>
#include <inflection/dialog/CommonConceptFactoryImpl.hpp>
#include <inflection/grammar/synthesis/fwd.hpp>
#include <inflection/dialog/Plurality.hpp>

namespace inflection::dialog::language {

class MlCommonConceptFactory : public CommonConceptFactoryImpl {
    using super = CommonConceptFactoryImpl;

public:
    explicit MlCommonConceptFactory(const ::inflection::util::ULocale& language);
    ~MlCommonConceptFactory() override;

protected:
    ::inflection::dialog::SpeakableString quantifyType(
        const ::inflection::dialog::SpeakableString& formattedNumber,
        const ::inflection::dialog::SemanticFeatureConceptBase& semanticConcept,
        bool useDefault,
        ::inflection::dialog::Plurality::Rule countType) const override;
};

} // namespace inflection::dialog::language

