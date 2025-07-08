/*
 * Copyright 2018-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <inflection/dialog/fwd.hpp>
#include <inflection/Object.hpp>
#include <string>

/**
 * @brief Represents an ontological label with a value.
 *
 */
class INFLECTION_CLASS_API inflection::dialog::SemanticValue
    : public virtual ::inflection::Object
{
public:
    /// @cond
    typedef ::inflection::Object super;
    /// @endcond

private:
    ::std::u16string label {  };
    ::std::u16string value {  };
public:
    /**
     * The string value associated with a semantic label
     */
    const ::std::u16string& getValue() const;

    /**
     * The semantic label (e.g. ontology node name)
     */
    const ::std::u16string& getLabel() const;

    /**
     * Returns a string representation of this label-value pair.
     */
    ::std::u16string toString() const override;

    /**
     * String compares the values of this pair.
     */
    bool operator==(const SemanticValue& other) const;
    /**
     * String compares the order of the label and value of this pair.
     */
    std::strong_ordering operator<=>(const SemanticValue&) const;

    /**
     * Construct a SemanticValue by a label and a string value.
     */
    SemanticValue(const ::std::u16string& label, const ::std::u16string& string);
    /**
     * Destructor
     */
    ~SemanticValue() override;
};

inline
const ::std::u16string& inflection::dialog::SemanticValue::getValue() const
{
    return value;
}

inline
const ::std::u16string& inflection::dialog::SemanticValue::getLabel() const
{
    return label;
}
