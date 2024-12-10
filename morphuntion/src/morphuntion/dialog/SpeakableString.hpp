/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <morphuntion/util/fwd.hpp>
#include <morphuntion/dialog/fwd.hpp>
#include <morphuntion/Object.hpp>
#include <morphuntion/dialog/SpeakableConcept.hpp>
#include <string>

/**
 * @brief Represents a string with both a spoken and written representation
 */
class MORPHUNTION_CLASS_API morphuntion::dialog::SpeakableString
    : public virtual SpeakableConcept
{
public:
    typedef ::morphuntion::dialog::SpeakableConcept super;

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
    bool operator==(const ::morphuntion::dialog::SpeakableString& o) const;
    bool operator!=(const ::morphuntion::dialog::SpeakableString& o) const;
    SpeakableString operator+(const ::morphuntion::dialog::SpeakableString& o) const;
    SpeakableString& operator=(const ::morphuntion::dialog::SpeakableString& o);
    SpeakableString& operator+=(const ::morphuntion::dialog::SpeakableString& o);

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
    virtual bool contains(const ::std::u16string& s) const;

    /**
     * Constructs a speakable string given the print part.
     *
     * @param print - the expected print part for the speakable string
     */
    explicit SpeakableString(const ::std::u16string& print);
    /**
     * Constructs a speakable string given both the print and speak part.
     *
     * @param print - the expected print part for the speakable string
     * @param speak - the expected speak part for the speakable string
     */
    SpeakableString(const ::std::u16string& print, const ::std::u16string& speak);
    /**
     * Copy constructor.
     */
    SpeakableString(const SpeakableString& print);
    /**
     * Destructor
     */
    ~SpeakableString() override;
};
