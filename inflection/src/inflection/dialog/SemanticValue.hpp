/*
 * Copyright 2018-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <inflection/dialog/fwd.hpp>
#include <inflection/Object.hpp>
#include <utility>
#include <string>

/**
 * @brief Represents an ontological label with a value.
 *
 */
class INFLECTION_CLASS_API inflection::dialog::SemanticValue
    : public virtual ::std::pair<::std::u16string, ::std::u16string>
    , public virtual ::inflection::Object
{
public:
    typedef ::std::pair<::std::u16string, ::std::u16string> super;
public:
    /**
     * The string value associated with a semantic label
     */
    const ::std::u16string& getValue() const;

    /**
     * The semantic label (e.g. ontology node name)
     */
    const ::std::u16string& getLabel() const;

    ::std::u16string toString() const override;

    SemanticValue(const ::std::u16string& label, const ::std::u16string& string);
    /**
     * Destructor
     */
    ~SemanticValue() override;
};

inline
const ::std::u16string& inflection::dialog::SemanticValue::getValue() const
{
    return ::std::get<1>(*this);
}

inline
const ::std::u16string& inflection::dialog::SemanticValue::getLabel() const
{
    return ::std::get<0>(*this);
}
