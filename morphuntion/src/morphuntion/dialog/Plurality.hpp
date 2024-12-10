/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <unicode/upluralrules.h>
#include <morphuntion/dialog/fwd.hpp>
#include <morphuntion/util/fwd.hpp>
#include <morphuntion/Object.hpp>
#include <set>
#include <string_view>

/**
 * @brief Provides an interface to PluralRules.
 */
class morphuntion::dialog::Plurality final
    : public virtual ::morphuntion::Object
{

public:
    typedef ::morphuntion::Object super;

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
    explicit Plurality(const ::morphuntion::util::ULocale& locale);
    /**
     * Destructor
     */
    ~Plurality() override;

private:
    Plurality(const Plurality& source) = delete;
    Plurality& operator=(const Plurality&) = delete;
};
