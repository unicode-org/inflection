/*
 * Copyright 2016-2024 Apple Inc. All rights reserved.
 */
#pragma once

#include <inflection/util/fwd.hpp>
#include <inflection/Object.hpp>
#include <string>

/**
 * @brief An object that represents a specific geographical, political,
 * or cultural region.
 * @details A <code>ULocale</code> object represents a specific geographical, political,
 * or cultural region. An operation that requires a <code>Locale</code> to perform
 * its task is called <em>locale-sensitive</em> and uses the <code>Locale</code>
 * to tailor information for the user. For example, displaying a number
 * is a locale-sensitive operation--the number should be formatted
 * according to the customs/conventions of the user's native country,
 * region, or culture.
 *
 * The Locale class is not suitable for subclassing. It is similar to std::locale,
 * ICU4J ULocale, ICU4C Locale and CFLocale. ICU and Core Foundation support BCP 47.
 * C++ std::locale does not support BCP 47.
 *
 * <P>
 * You can create a <code>Locale</code> object using the constructor in
 * this class:
 * \htmlonly<blockquote>\endhtmlonly
 * <pre>
 *       ULocale(const char* language,
 *               const char* country,
 *               const char* variant)
 * </pre>
 * \htmlonly</blockquote>\endhtmlonly
 * The first argument to the constructors is a valid <STRONG>ISO
 * Language Code.</STRONG> These codes are the lower-case two to three letter
 * codes as defined by ISO-639.
 * You can find a full list of these codes at:
 * <br/><a href ="http://www.loc.gov/standards/iso639-2/">
 * http://www.loc.gov/standards/iso639-2/</a>
 *
 * <P>
 * The second argument to the constructors is a valid <STRONG>ISO Country
 * Code.</STRONG> These codes are the upper-case two-letter codes
 * as defined by ISO-3166.
 * You can find a full list of these codes at a number of sites, such as:
 * <br/><a href="https://www.iso.org/obp/ui/#iso:pub:PUB500001:en">
 * https://www.iso.org/obp/ui/#iso:pub:PUB500001:en</a>
 *
 * <P>
 * The third constructor requires a third argument--the <STRONG>Variant.</STRONG>
 * The Variant codes are vendor and browser-specific.
 * For example, use REVISED for a language's revised script orthography, and POSIX for POSIX.
 * Where there are two variants, separate them with an underscore, and
 * put the most important one first. For
 * example, a Traditional Spanish collation might be referenced, with
 * "ES", "ES", "Traditional_POSIX".
 *
 * <P>
 * Because a <code>ULocale</code> object is just an identifier for a region,
 * no validity check is performed when you construct a <code>Locale</code>.
 *
 * <P>
 * Once you've created a <code>Locale</code> you can query it for information about
 * itself. Use <code>getCountry</code> to get the ISO Country Code and
 * <code>getLanguage</code> to get the ISO Language Code. You can
 * use <code>getDisplayCountry</code> to get the
 * name of the country suitable for displaying to the user. Similarly,
 * you can use <code>getDisplayLanguage</code> to get the name of
 * the language suitable for displaying to the user. Interestingly,
 * the <code>getDisplayXXX</code> methods are themselves locale-sensitive
 * and have two versions: one that uses the default locale and one
 * that takes a locale as an argument and displays the name or country in
 * a language appropriate to that locale.
 *
 * A <code>Locale</code> is the mechanism for identifying the kind of object
 * (<code>NumberFormat</code>) that you would like to get. The locale is
 * <STRONG>just</STRONG> a mechanism for identifying objects,
 * <STRONG>not</STRONG> a container for the objects themselves.
 */
class INFLECTION_CLASS_API inflection::util::ULocale
    : public virtual ::inflection::Object
{
public:
    /// @cond
    typedef ::inflection::Object super;
    /// @endcond

public:
    /**
     * Construct a locale from language, country, variant.
     * If an error occurs, then the constructed object will be "bogus"
     * (isBogus() will return TRUE).
     *
     * @param language Lowercase two-letter or three-letter ISO-639 code.
     *  This parameter can instead be an ICU style C locale (e.g. "en_US"),
     *  but the other parameters must not be used.
     *  This parameter can be NULL; if so,
     *  the locale is initialized to match the current default locale.
     *  (This is the same as using the default constructor.)
     *  Please note: The Java Locale class does NOT accept the form
     *  'new Locale("en_US")' but only 'new Locale("en","US")'
     *
     * @param country  Uppercase two-letter ISO-3166 code. (optional)
     * @param variant  Uppercase vendor and browser specific code. See class
     *                 description. (optional)
     * @param keywordsAndValues A string consisting of keyword/values pairs, such as
     *                 "collation=phonebook;currency=euro"
     */
    ULocale(std::string_view language,
            std::string_view country = std::string_view(),
            std::string_view variant = std::string_view(),
            std::string_view keywordsAndValues = std::string_view());

    /**
     * Initializes a Locale object from another Locale object.
     *
     * @param other The Locale object being copied in.
     */
    ULocale(const ::inflection::util::ULocale& other);


    /**
     * Destructor
     */
    ~ULocale() override;

    /**
     * Replaces the entire contents of *this with the specified value.
     *
     * @param other The Locale object being copied in.
     * @return      *this
     */
    ::inflection::util::ULocale& operator=(const ::inflection::util::ULocale& other);

    /**
     * Checks if two locale keys are the same.
     *
     * @param other The locale key object to be compared with this.
     * @return      True if the two locale keys are the same, false otherwise.
     */
    bool   operator==(const    ::inflection::util::ULocale&     other) const;

    /**
     * String compares the order of the locale key.
     *
     * @param other The locale key object to be compared with this.
     */
    std::strong_ordering   operator<=>(const    ::inflection::util::ULocale&     other) const;

    /**
     * Returns the locale's ISO-639 language code.
     * @return      An alias to the code
     */
    std::string_view getLanguage( ) const;

    /**
     * Returns the locale's ISO-15924 abbreviation script code.
     * @return      An alias to the code
     * @see uscript_getShortName
     * @see uscript_getCode
     */
    std::string_view getScript( ) const;

    /**
     * Returns the locale's ISO-3166 country code.
     * @return      An alias to the code
     */
    std::string_view getCountry( ) const;

    /**
     * Returns the locale's variant code.
     * @return      An alias to the code
     */
    std::string_view getVariant( ) const;

    /**
     * Returns the fallback locale for this locale.
     */
    ULocale getFallback() const;

    /**
     * Returns the programmatic name of the entire locale, with the language,
     * country and variant separated by underbars. If a field is missing, up
     * to two leading underbars will occur. Example: "en", "de_DE", "en_US_WIN",
     * "de__POSIX", "fr__MAC", "__MAC", "_MT", "_FR_EURO"
     * @return      A pointer to "name".
     */
    ::std::u16string toString() const override;

    /**
     * Returns the programmatic name of the entire locale, with the language,
     * country and variant separated by underbars. If a field is missing, up
     * to two leading underbars will occur. Example: "en", "de_DE", "en_US_WIN",
     * "de__POSIX", "fr__MAC", "__MAC", "_MT", "_FR_EURO"
     * @return      A pointer to "name".
     */
    const ::std::string &getName() const;

    /**
     * Generates a hash code compatible with std::hash for the locale.
     */
    std::size_t operator()(const inflection::util::ULocale& locale) const noexcept;

private:
    ::std::string language {  };
    ::std::string script {  };
    ::std::string country {  };
    ::std::string variant {  };
    ::std::string fullName {  };
};

inline bool
inflection::util::ULocale::operator==(const ::inflection::util::ULocale& other) const
{
    return other <=> *this == 0;
}

inline std::strong_ordering
inflection::util::ULocale::operator<=>(const ::inflection::util::ULocale& other) const
{
    return fullName <=> other.fullName;
}

inline ::std::string_view
inflection::util::ULocale::getCountry() const
{
    return country;
}

inline ::std::string_view
inflection::util::ULocale::getLanguage() const
{
    return language;
}

inline ::std::string_view
inflection::util::ULocale::getScript() const
{
    return script;
}

inline ::std::string_view
inflection::util::ULocale::getVariant() const
{
    return variant;
}

inline const ::std::string &
inflection::util::ULocale::getName() const
{
    return fullName;
}
