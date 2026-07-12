/*
 * Copyright 2026 Unicode Incorporated and others. All rights reserved.
 */
#include <inflection/tokenizer/ControlCleaver.hpp>

#include <inflection/tokenizer/Token.hpp>
#include <inflection/tokenizer/Token_Delim.hpp>
#include <inflection/tokenizer/Token_Head.hpp>
#include <inflection/tokenizer/Token_Word.hpp>
#include <inflection/tokenizer/Tokenizer.hpp>
#include <inflection/tokenizer/TokenUtil.hpp>
#include <inflection/npc.hpp>
#include <unicode/uchar.h>

namespace inflection::tokenizer {

void ControlCleaver::modifyTokens(const ::inflection::tokenizer::Tokenizer* /*tokenizer*/, std::u16string_view /*originalInput*/, inflection::tokenizer::Token_Head* head)
{
    auto token = npc(head)->getNext();
    while (token != nullptr) {
        if (npc(token)->getLength() > 1) {
            std::u16string_view value = npc(token)->getValue();
            int32_t valLen = static_cast<int32_t>(value.length());

            int32_t trailStart = valLen;
            while (trailStart > 0) {
                UChar32 cp;
                int32_t prev = trailStart;
                U16_PREV(value, 0, prev, cp);
                if (!static_cast<bool>(u_hasBinaryProperty(cp, UCHAR_BIDI_CONTROL))) {
                    break;
                }
                trailStart = prev;
            }

            if (trailStart == valLen) {
                token = npc(token)->getNext();
                continue;
            }

            inflection::tokenizer::Token* chainTail = npc(token)->getPrevious();
            const auto nextToken = npc(token)->getNext();
            int32_t tokenStart = npc(token)->getStartChar();
            bool significant = npc(token)->isSignificant();

            if (0 < trailStart) {
                std::u16string_view middle = value.substr(0, trailStart);
                if (significant) {
                    std::u16string_view cleanValue = npc(token)->getCleanValue();
                    std::u16string_view middleLower = cleanValue.substr(0, cleanValue.length() - (valLen - trailStart));
                    chainTail = TokenUtil::appendToken(chainTail, new ::inflection::tokenizer::Token_Word(tokenStart, tokenStart + trailStart, middle, middleLower, TokenUtil::isSignificant(middle)));
                } else {
                    chainTail = TokenUtil::appendToken(chainTail, new ::inflection::tokenizer::Token_Delim(tokenStart, tokenStart + trailStart, middle, middle));
                }
            }

            for (int32_t i = trailStart; i < valLen; ) {
                UChar32 cp;
                U16_GET(value, 0, i, valLen, cp);
                int32_t charLen = U16_LENGTH(cp);
                std::u16string_view controlChar = value.substr(i, charLen);
                chainTail = TokenUtil::appendToken(chainTail, new ::inflection::tokenizer::Token_Delim(tokenStart + i, tokenStart + i + charLen, controlChar, controlChar));
                i += charLen;
            }

            TokenUtil::appendToken(chainTail, nextToken);
            delete token;
            token = nextToken;
            continue;
        }
        token = npc(token)->getNext();
    }
}

} // namespace inflection::tokenizer
