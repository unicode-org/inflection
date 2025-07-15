/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#include <inflection/dialog/SpeakableString.hpp>

#include <inflection/util/ULocale.hpp>
#include <inflection/npc.hpp>

namespace inflection::dialog {

SpeakableString::SpeakableString(std::u16string_view print)
    : super()
    , print(print)
    , speak(nullptr)
{
}

SpeakableString::SpeakableString(std::u16string_view print, std::u16string_view speak)
    : super()
    , print(print)
{
    if (print != speak) {
        this->speak = new ::std::u16string(speak);
    }
}

SpeakableString::SpeakableString(const SpeakableString& sstring)
    : super()
    , print(sstring.print)
{
    if (!sstring.speakEqualsPrint()) {
        this->speak = new ::std::u16string(*npc(sstring.speak));
    }
}

SpeakableString::~SpeakableString()
{
    if (speak != nullptr && speak != &print) {
        delete speak;
    }
}

const ::std::u16string& SpeakableString::getPrint() const
{
    return print;
}

const ::std::u16string& SpeakableString::getSpeak() const
{
    return speak == nullptr ? print : *npc(speak);
}

bool SpeakableString::speakEqualsPrint() const
{
    return speak == nullptr;
}

::std::u16string SpeakableString::toString() const
{
    if (speakEqualsPrint()) {
        return u"SS[" + print + u']';
    }
    return u"SS[" + print + u"/tts:" + *npc(speak) + u']';
}

bool SpeakableString::operator==(const SpeakableString& o) const
{
    if (this == &o) {
        return true;
    }
    return this->print == o.print && ((this->speak == nullptr && o.speak == nullptr) || (this->speak != nullptr && o.speak != nullptr && *this->speak == *o.speak));
}

SpeakableString SpeakableString::operator+(const SpeakableString& o) const
{
    ::std::u16string print(this->print);
    print.append(o.print);
    if (this->speak != nullptr || o.speak != nullptr) {
        ::std::u16string speak;
        if (this->speak != nullptr) {
            speak.append(*npc(this->speak));
        }
        else {
            speak.append(this->print);
        }
        if (o.speak != nullptr) {
            speak.append(*npc(o.speak));
        }
        else {
            speak.append(o.print);
        }
        return SpeakableString(print, speak);
    }

    return SpeakableString(print);
}

SpeakableString& SpeakableString::operator=(const SpeakableString& o)
{
    this->print = o.print;
    if (this->speak != nullptr) {
        delete this->speak;
        this->speak = nullptr;
    }
    if (!o.speakEqualsPrint()) {
        this->speak = new ::std::u16string(*npc(o.speak));
    }
    return *this;
}

SpeakableString& SpeakableString::operator+=(const SpeakableString& o)
{
    return *this = *this + o;
}

bool SpeakableString::isEmpty() const
{
    return print.empty() && (speak == nullptr || npc(speak)->empty());
}

SpeakableString* SpeakableString::toSpeakableString() const
{
    return new SpeakableString(*npc(this));
}

bool SpeakableString::contains(::std::u16string_view s) const
{
    return getPrint().find(s) != ::std::u16string::npos;
}


} // namespace inflection::dialog
