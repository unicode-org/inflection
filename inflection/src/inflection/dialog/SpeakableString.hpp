/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <inflection/util/fwd.hpp>
#include <inflection/dialog/fwd.hpp>
#include <inflection/Object.hpp>
#include <inflection/dialog/SpeakableConcept.hpp>
#include <string>

/**
 * @brief Represents a string with both a spoken and written representation
 */
class INFLECTION_CLASS_API inflection::dialog::SpeakableString
    : public virtual SpeakableConcept
{
public:
    /**
     * super class.
     */
    typedef ::inflection::dialog::SpeakableConcept super;

private:
    ::std::u16string print {  };
    ::std::u16string* speak {  };

public:
    /**
     * Get the print string of the speakable string.
     *
     * @return string
     */
    virtual const ::std::u16string& getPrint() const;
    /**
     * Get the speak string of the speakable string.
     *
     * @return string
     */
    virtual const ::std::u16string& getSpeak() const;
    /**
     * Checks that the speak string equals the print string.
     *
     * @return boolean
     */
    virtual bool speakEqualsPrint() const;

    ::std::u16string toString() const override;
    bool operator==(const ::inflection::dialog::SpeakableString& o) const;
    bool operator!=(const ::inflection::dialog::SpeakableString& o) const;
    SpeakableString operator+(const ::inflection::dialog::SpeakableString& o) const;
    SpeakableString& operator=(const ::inflection::dialog::SpeakableString& o);
    SpeakableString& operator+=(const ::inflection::dialog::SpeakableString& o);

    /**
     * A speakable string is empty if it's missing both the print and the speak parts.
     *
     * @return boolean
     */
    virtual bool isEmpty() const;
    /**
     * Makes a copy of the speakable string.
     *
     * @return speakable string.
     */
    SpeakableString* toSpeakableString() const override;
    /**
     * Checks that the print part contains a given substring.
     *
     * @param s - the substring to check for containment.
     * @return  boolean
     */
    virtual bool contains(std::u16string_view s) const;

    /**
     * Constructs a speakable string given the print part.
     *
     * @param print - the expected print part for the speakable string
     */
    explicit SpeakableString(std::u16string_view print);
    /**
     * Constructs a speakable string given both the print and speak part.
     *
     * @param print - the expected print part for the speakable string
     * @param speak - the expected speak part for the speakable string
     */
    SpeakableString(std::u16string_view print, std::u16string_view speak);
    /**
     * Copy constructor.
     */
    SpeakableString(const SpeakableString& print);
    /**
     * Destructor
     */
    ~SpeakableString() override;
};
