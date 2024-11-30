/*
 * Copyright 2018-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <morphuntion/dialog/fwd.hpp>
#include <morphuntion/Object.hpp>
#include <utility>
#include <string>

/**
 * @brief Represents an ontological label with a value.
 *
 */
class MORPHUNTION_CLASS_API morphuntion::dialog::SemanticValue
    : public virtual ::std::pair<::std::u16string, ::std::u16string>
    , public virtual ::morphuntion::Object
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
const ::std::u16string& morphuntion::dialog::SemanticValue::getValue() const
{
    return ::std::get<1>(*this);
}

inline
const ::std::u16string& morphuntion::dialog::SemanticValue::getLabel() const
{
    return ::std::get<0>(*this);
}
