/*
 * Copyright 2021-2024 Apple Inc. All rights reserved.
 */
#include "inflection/tokenizer/iterator/ICUTokenExtractorIterator.hpp"

#include <inflection/exception/ICUException.hpp>
#include <inflection/exception/InvalidConfigurationException.hpp>
#include <inflection/util/ULocale.hpp>
#include <inflection/npc.hpp>
#include <memory>
#include <mutex>
#include <stack>
#include <vector>

/*
 The maximum number of cached tokenizers per language. Try not to go beyond a typical number of simultaneous threads.
 */
static constexpr int8_t MAX_TOKENIZER_POOL_SIZE = 64;

static std::mutex& CLASS_MUTEX() {
    static auto classMutex = new std::mutex();
    return *npc(classMutex);
}

static UBreakIterator* createBreakIterator() {
    UErrorCode status = U_ZERO_ERROR;
    auto result = ubrk_open(UBRK_WORD, "fi", (const UChar *)u"", 0, &status);
    ::inflection::exception::ICUException::throwOnFailure(status);
    return result;
}

static ::std::stack<UBreakIterator*, std::vector<UBreakIterator*>>* gTokenizerCache = nullptr;

/*
 We cache tokenizers for performance reasons. Caching can save an order of magnitude of time in some cases.
 */
static UBreakIterator* checkoutCachedTokenizer()
{
    std::lock_guard<std::mutex> guard(CLASS_MUTEX());
    if (gTokenizerCache == nullptr) {
        gTokenizerCache = new ::std::stack<UBreakIterator*, std::vector<UBreakIterator*>>();
    }

    UBreakIterator* tokenizer;
    if (npc(gTokenizerCache)->empty()) {
        UErrorCode status = U_ZERO_ERROR;
        static auto source = createBreakIterator();
        tokenizer = ubrk_clone(source, &status);
        ::inflection::exception::ICUException::throwOnFailure(status);
    }
    else {
        tokenizer = npc(gTokenizerCache)->top();
        npc(gTokenizerCache)->pop();
    }

    if (tokenizer == nullptr) {
        throw ::inflection::exception::InvalidConfigurationException(u"BreakIterator returned null. Perhaps the data files could not be found.");
    }

    return tokenizer;
}

static void checkinCachedTokenizer(UBreakIterator* tokenizer)
{
    std::lock_guard<std::mutex> guard(CLASS_MUTEX());
    if (tokenizer != nullptr) {
        if (gTokenizerCache != nullptr && gTokenizerCache->size() <= MAX_TOKENIZER_POOL_SIZE) {
            try {
                gTokenizerCache->push(tokenizer);
                tokenizer = nullptr;
            }
            catch (const std::bad_alloc& e) {
                // Oh well, delete it below
                e.what();
            }
        }
        if (tokenizer != nullptr) {
            // Max cached tokenizers reached
            ubrk_close(tokenizer);
        }
    }
}

icu4cxx::BreakIterator getCachedWordBreakIterator(const char16_t *str, int32_t strLen)
{
    UBreakIterator *breakIterator = checkoutCachedTokenizer();
    auto status = U_ZERO_ERROR;
    ubrk_setText(breakIterator, (const UChar *)str, strLen, &status);
    ::inflection::exception::ICUException::throwOnFailure(status);
    return {breakIterator, false};
}

namespace inflection::tokenizer::iterator {

ICUTokenExtractorIterator::ICUTokenExtractorIterator(const char16_t *str, int32_t strLen)
    : breakIterator(getCachedWordBreakIterator(str, strLen))
{
}

ICUTokenExtractorIterator::~ICUTokenExtractorIterator()
{
    checkinCachedTokenizer(breakIterator.wrappee_);
    breakIterator.wrappee_ = nullptr;
}

int32_t ICUTokenExtractorIterator::getNextBoundary()
{
    return breakIterator.next();
}

void ICUTokenExtractorIterator::reset(std::u16string_view str)
{
    auto status = U_ZERO_ERROR;
    ubrk_setText(breakIterator.wrappee_, (const UChar *)str.data(), int32_t(str.length()), &status);
    ::inflection::exception::ICUException::throwOnFailure(status);
}

} // namespace inflection::tokenizer
