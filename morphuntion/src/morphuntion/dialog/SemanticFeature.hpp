/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <morphuntion/dialog/fwd.hpp>
#include <morphuntion/Object.hpp>
#include <string>
#include <set>

/**
 * @brief Represents a single semantic feature
 */
class MORPHUNTION_CLASS_API morphuntion::dialog::SemanticFeature
    : public virtual ::morphuntion::Object
{
public:
    typedef ::morphuntion::Object super;

public:
    enum Type {
        NoValue,
        BoundedValue,
        UnboundedValue
    };

private:
    Type type {  };
    ::std::u16string name {  };
    ::std::set<::std::u16string> boundedValues {  };
    bool aliased {  };

public:
    /**
     * Constructs a SemanticFeature that has no values.
     * @param name The name of the SemanticFeature.
     */
    static SemanticFeature* noValue(const ::std::u16string& name);
    /**
     * Constructs a bounded SemanticFeature that has the provided values.
     * @param name The name of the SemanticFeature.
     * @param values The possible values of the new SemanticFeature.
     */
    static SemanticFeature* boundedValue(const ::std::u16string& name, const ::std::set<::std::u16string>& values);
    /**
     * Constructs an unbounded SemanticFeature that has no restrictions on the possible values.
     * @param name The name of the SemanticFeature.
     */
    static SemanticFeature* unboundedValue(const ::std::u16string& name);

public:
    /**
     * Returns the type of this SemanticFeature
     */
    virtual Type getType() const;
    /**
     * Returns the name of this SemanticFeature
     */
    virtual const ::std::u16string& getName() const;
    /**
     * Returns the possible values of this SemanticFeature. Only bounded SemanticFeatures can have a non-empty set of bounded values.
     */
    virtual const ::std::set<::std::u16string>& getBoundedValues() const;
    virtual bool isAliased() const;
    bool operator<(const SemanticFeature& other) const;
    bool operator==(const SemanticFeature& other) const;

    SemanticFeature(const ::std::u16string& name, Type type, const ::std::set<::std::u16string>& boundedValues, bool aliased);
    /**
     * The copy constructor
     */
    SemanticFeature(const SemanticFeature& other);
    /**
     * The destructor
     */
    ~SemanticFeature() override;

private:
    SemanticFeature& operator=(const SemanticFeature& other) = delete;
};
