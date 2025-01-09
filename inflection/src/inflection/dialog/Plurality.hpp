/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <unicode/upluralrules.h>
#include <inflection/dialog/fwd.hpp>
#include <inflection/util/fwd.hpp>
#include <inflection/Object.hpp>
#include <set>
#include <string_view>

/**
 * @brief Provides an interface to PluralRules.
 */
class inflection::dialog::Plurality final
    : public virtual ::inflection::Object
{

public:
    typedef ::inflection::Object super;

public:
    enum class Rule {
        ZERO,
        ONE,
        TWO,
        FEW,
        MANY,
        OTHER
    };

private:
    UPluralRules* pluralRules {  };
    ::std::set<Rule> defaultSupportedCount {  };

private:
    Rule convertKeywordToCount(std::u16string_view keyword) const;

public:
    /**
     * Returns the set of Count that are valid for the given locale.
     */
    const ::std::set<Rule>& getDefaultSupportedCount() const;
    Rule select(double number) const;

    // Generated
    /**
     * Construct a factory for the specified locale.
     */
    explicit Plurality(const ::inflection::util::ULocale& locale);
    /**
     * Destructor
     */
    ~Plurality() override;

private:
    Plurality(const Plurality& source) = delete;
    Plurality& operator=(const Plurality&) = delete;
};
