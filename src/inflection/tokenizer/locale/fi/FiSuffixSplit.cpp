/*
 * Copyright 2016-2026 Apple Inc. All rights reserved.
 */
#include <inflection/tokenizer/locale/fi/FiSuffixSplit.hpp>

#include <inflection/tokenizer/locale/fi/FiSegment.hpp>
#include <inflection/tokenizer/locale/fi/FiTokenizerConfig.hpp>
#include <inflection/util/ArrayUtils.hpp>
#include <inflection/npc.hpp>
#include <unicode/uchar.h>
#include <bit>
#include <map>
#include <memory>
#include <set>

using inflection::util::ArrayUtils;

namespace inflection::tokenizer::locale::fi {

FiSuffixSplit::FiSuffixSplit(FiSegment* segment)
    : segment(segment)
    , logProb(FiTokenizerConfig::MINLOGPROB)
    , found_(npc(segment)->found())
{
}

static constexpr struct {
    const char16_t *suffix;
    const char16_t *notNominativeSuffix;
    int8_t split;
} NOT_NOMINATIVE_SUFFIX[] = {
    {u"an", u"aan", 1},
    {u"en", u"een", 1},
};

bool FiSuffixSplit::containsMiddleSuffix(std::u16string_view suffix)
{
    static constexpr const char16_t* const MIDDLESUFFIXSET[] = {
        // This must be kept in sorted order.
        u"a",
        u"an",
        u"en",
        u"han",
        u"in",
        u"jen",
        u"sta",
        u"ten",
    };
    return ArrayUtils::contains<MIDDLESUFFIXSET>(suffix);
}

bool FiSuffixSplit::containsClitic(std::u16string_view suffix)
{
    static constexpr const char16_t* const CLITICSET[] = {
        // This must be kept in sorted order.
        u"han",
        u"hän",
        u"ka",
        u"kaan",
        u"kin",
        u"ko",
        u"kä",
        u"kään",
        u"kö",
        u"pa",
        u"pä",
        u"s",
    };
    return ArrayUtils::contains<CLITICSET>(suffix);
}

static constexpr const char16_t* const SUFFIX_SPLIT_PRIMITIVE[] = {
    // This must be kept in sorted order.
    // the list of suffixes will be split by tokenization module
    u"an",
    u"en",
    u"han",
    u"hen",
    u"hin",
    u"hon",
    u"hun",
    u"hyn",
    u"hän",
    u"hön",
    u"in",
    u"ksi",
    u"lla",
    u"lle",
    u"llä",
    u"lta",
    u"ltä",
    u"on",
    u"seen",
    u"siin",
    u"ssa",
    u"ssä",
    u"sta",
    u"stä",
    u"un",
    u"yn",
    u"än",
    u"ön",
};

bool FiSuffixSplit::containsSuffixSplit(::std::u16string_view suffix)
{
    return ArrayUtils::contains<SUFFIX_SPLIT_PRIMITIVE>(suffix);
}

static constexpr const char16_t* const ALL_SUFFIX_SEQUENCE_PRIMITIVE[] = {
    // suffix sequences
    u"a",
    u"aan",
    u"akaan",
    u"akin",
    u"akseen",
    u"aksemme",
    u"akseni",
    u"aksesi",
    u"amme",
    u"an",
    u"ani",
    u"ansa",
    u"asi",
    u"da",
    u"daan",
    u"dakseen",
    u"daksemme",
    u"daksesi",
    u"den",
    u"det",
    u"duissa",
    u"dun",
    u"dut",
    u"dyn",
    u"dyt",
    u"dä",
    u"däkseen",
    u"dään",
    u"e",
    u"een",
    u"ein",
    u"ekö",
    u"empi",
    u"en",
    u"ensa",
    u"ensä",
    u"esi",
    u"essa",
    u"essaan",
    u"essani",
    u"essasi",
    u"essä",
    u"essään",
    u"ettava",
    u"ettiin",
    u"ettu",
    u"etty",
    u"ettävä",
    u"etulla",
    u"etun",
    u"etut",
    u"han",
    u"hen",
    u"hensä",
    u"hin",
    u"hon",
    u"hun",
    u"hyn",
    u"hän",
    u"hön",
    u"hönsä",
    u"i",
    u"ia",
    u"iaan",
    u"iamme",
    u"iani",
    u"idemme",
    u"iden",
    u"ideni",
    u"idenkin",
    u"idensa",
    u"idensä",
    u"idän",
    u"idät",
    u"iemme",
    u"ien",
    u"ieni",
    u"iensa",
    u"iensä",
    u"ihin",
    u"ihinsa",
    u"ihinsä",
    u"iin",
    u"iinsa",
    u"iinsä",
    u"ikseen",
    u"iksi",
    u"il",
    u"illa",
    u"illaan",
    u"illasi",
    u"ille",
    u"illeen",
    u"illemme",
    u"illeni",
    u"illesi",
    u"illä",
    u"ilta",
    u"iltaan",
    u"iltä",
    u"iltään",
    u"immaksi",
    u"immalla",
    u"immalle",
    u"imman",
    u"immassa",
    u"immasta",
    u"immat",
    u"imme",
    u"immilla",
    u"immillaan",
    u"immille",
    u"immillä",
    u"immin",
    u"immissa",
    u"immista",
    u"immistä",
    u"immäksi",
    u"immän",
    u"immässä",
    u"immästä",
    u"immät",
    u"impaa",
    u"impaan",
    u"impana",
    u"impia",
    u"impien",
    u"impiin",
    u"impinä",
    u"impiä",
    u"impänä",
    u"impään",
    u"in",
    u"ina",
    u"inaan",
    u"ine",
    u"ineen",
    u"inemme",
    u"inenne",
    u"ini",
    u"insa",
    u"insä",
    u"inta",
    u"intä",
    u"inä",
    u"inään",
    u"is",
    u"isi",
    u"isiin",
    u"isimme",
    u"isin",
    u"isit",
    u"isitte",
    u"isivat",
    u"isivät",
    u"issa",
    u"issaan",
    u"issä",
    u"issään",
    u"ista",
    u"istaan",
    u"istani",
    u"istasi",
    u"istä",
    u"istään",
    u"it",
    u"ita",
    u"itaan",
    u"itamme",
    u"itta",
    u"itte",
    u"itten",
    u"ittä",
    u"itä",
    u"itään",
    u"ivat",
    u"ivät",
    u"iä",
    u"iäni",
    u"iään",
    u"ja",
    u"jaan",
    u"jani",
    u"janne",
    u"jasi",
    u"jen",
    u"jensa",
    u"jesi",
    u"jä",
    u"jämme",
    u"jään",
    u"kaa",
    u"kaan",
    u"ki",
    u"kin",
    u"ko",
    u"koon",
    u"kse",
    u"kseen",
    u"kseksi",
    u"ksella",
    u"kselle",
    u"kselta",
    u"ksemme",
    u"ksen",
    u"kseni",
    u"ksenne",
    u"ksesi",
    u"ksessa",
    u"ksesta",
    u"ksetta",
    u"ksi",
    u"kä",
    u"kää",
    u"kään",
    u"kö",
    u"köön",
    u"la",
    u"laan",
    u"lakseen",
    u"leella",
    u"leelle",
    u"leen",
    u"leena",
    u"leensa",
    u"leet",
    u"leiden",
    u"leita",
    u"len",
    u"lessa",
    u"lessaan",
    u"lessä",
    u"lessään",
    u"li",
    u"lla",
    u"llaan",
    u"llain",
    u"llamme",
    u"llani",
    u"llasi",
    u"lle",
    u"lleen",
    u"llemme",
    u"lleni",
    u"llenne",
    u"llesi",
    u"llä",
    u"lläni",
    u"lläsi",
    u"llään",
    u"lta",
    u"ltaan",
    u"ltani",
    u"ltasi",
    u"ltä",
    u"ltään",
    u"lun",
    u"lut",
    u"lutta",
    u"lyistä",
    u"lyn",
    u"lystä",
    u"lyt",
    u"lä",
    u"lään",
    u"ma",
    u"maa",
    u"maan",
    u"maansa",
    u"maksi",
    u"malla",
    u"malle",
    u"malta",
    u"mamme",
    u"man",
    u"mana",
    u"mani",
    u"mansa",
    u"masi",
    u"massa",
    u"massaan",
    u"masta",
    u"mastaan",
    u"mat",
    u"maton",
    u"matonta",
    u"matta",
    u"mattomaksi",
    u"mattoman",
    u"mattomana",
    u"mattomat",
    u"mattomia",
    u"mattomiin",
    u"mattomiksi",
    u"me",
    u"mia",
    u"miaan",
    u"mien",
    u"miin",
    u"milla",
    u"mille",
    u"miseen",
    u"misellä",
    u"misen",
    u"misesta",
    u"missa",
    u"missä",
    u"mista",
    u"mistä",
    u"mistään",
    u"miä",
    u"mmaksi",
    u"mmalla",
    u"mmalle",
    u"mmalta",
    u"mman",
    u"mmassa",
    u"mmasta",
    u"mmat",
    u"mme",
    u"mmekä",
    u"mmiksi",
    u"mmille",
    u"mmilta",
    u"mmin",
    u"mmissä",
    u"mmista",
    u"mmäksi",
    u"mmälle",
    u"mmällä",
    u"mmän",
    u"mmässä",
    u"mmät",
    u"mpaa",
    u"mpaan",
    u"mpana",
    u"mpi",
    u"mpia",
    u"mpien",
    u"mpiin",
    u"mpina",
    u"mpiä",
    u"mpänä",
    u"mpää",
    u"mpään",
    u"mä",
    u"mäksi",
    u"mällä",
    u"män",
    u"mänsä",
    u"mänä",
    u"mäsi",
    u"mässä",
    u"mässään",
    u"mästä",
    u"mästäni",
    u"mästään",
    u"mät",
    u"mättä",
    u"mättömiä",
    u"mättömäksi",
    u"mättömän",
    u"mätön",
    u"mätöntä",
    u"mää",
    u"mään",
    u"määnsä",
    u"n",
    u"na",
    u"naan",
    u"nain",
    u"nani",
    u"nasi",
    u"ne",
    u"nee",
    u"neeksi",
    u"neella",
    u"neelle",
    u"neempi",
    u"neen",
    u"neena",
    u"neeni",
    u"neensa",
    u"neensä",
    u"neenä",
    u"neeseen",
    u"neessä",
    u"neesta",
    u"neestä",
    u"neet",
    u"neiden",
    u"neille",
    u"neillä",
    u"neiltä",
    u"neina",
    u"neisiin",
    u"neissa",
    u"neista",
    u"neistä",
    u"neita",
    u"neitä",
    u"ni",
    u"nka",
    u"nkin",
    u"nkun",
    u"nkä",
    u"nne",
    u"nneista",
    u"nnessä",
    u"nnyt",
    u"nsa",
    u"nsä",
    u"nu",
    u"nut",
    u"nutta",
    u"ny",
    u"nyt",
    u"nyttä",
    u"nä",
    u"nään",
    u"o",
    u"on",
    u"oni",
    u"onsa",
    u"osi",
    u"s",
    u"sa",
    u"seen",
    u"seensa",
    u"seet",
    u"si",
    u"siin",
    u"simme",
    u"sin",
    u"sit",
    u"sitte",
    u"sivat",
    u"sivät",
    u"ssa",
    u"ssaan",
    u"ssamme",
    u"ssani",
    u"ssanne",
    u"ssasi",
    u"ssä",
    u"ssäni",
    u"ssäsi",
    u"ssään",
    u"sta",
    u"staan",
    u"stain",
    u"stamme",
    u"stani",
    u"stasi",
    u"sti",
    u"stä",
    u"stämme",
    u"stäni",
    u"stänne",
    u"stäsi",
    u"stään",
    u"sut",
    u"syt",
    u"t",
    u"ta",
    u"taan",
    u"taessa",
    u"tain",
    u"taisiin",
    u"takseen",
    u"takuta",
    u"tamme",
    u"tani",
    u"tanut",
    u"tasi",
    u"tava",
    u"tavaa",
    u"tavaksi",
    u"tavan",
    u"tavana",
    u"tavina",
    u"tavissa",
    u"te",
    u"temme",
    u"ten",
    u"teni",
    u"tenne",
    u"tensa",
    u"tensä",
    u"tesi",
    u"tessa",
    u"tessaan",
    u"tessään",
    u"ti",
    u"tiin",
    u"tka",
    u"tko",
    u"tkut",
    u"tkä",
    u"tse",
    u"tta",
    u"ttaessa",
    u"ttaisi",
    u"ttaisiin",
    u"ttava",
    u"ttavaa",
    u"ttavaksi",
    u"ttavan",
    u"ttavana",
    u"ttavassa",
    u"ttavat",
    u"ttavia",
    u"ttavien",
    u"ttavinta",
    u"ttavissa",
    u"tte",
    u"ttiin",
    u"ttu",
    u"ttua",
    u"ttuaan",
    u"ttuihin",
    u"ttuja",
    u"ttujen",
    u"ttuna",
    u"ttuun",
    u"tty",
    u"ttyihin",
    u"ttyinä",
    u"ttyjen",
    u"ttyjä",
    u"ttynä",
    u"ttyyn",
    u"ttyä",
    u"ttyään",
    u"ttä",
    u"ttäessä",
    u"ttäisiin",
    u"ttävien",
    u"ttävissä",
    u"ttävä",
    u"ttäväksi",
    u"ttävän",
    u"ttävänä",
    u"ttävässä",
    u"ttävät",
    u"ttävää",
    u"ttävään",
    u"tuaan",
    u"tuilla",
    u"tuille",
    u"tuimmat",
    u"tuimpia",
    u"tuimpien",
    u"tuin",
    u"tuissa",
    u"tuista",
    u"tuja",
    u"tujen",
    u"tuksi",
    u"tulla",
    u"tulta",
    u"tun",
    u"tuna",
    u"tuneen",
    u"tuneet",
    u"tuneita",
    u"tussa",
    u"tusta",
    u"tut",
    u"tuun",
    u"ty",
    u"tyihin",
    u"tyille",
    u"tyillä",
    u"tyissä",
    u"tyistä",
    u"tyjen",
    u"tyjä",
    u"tyksi",
    u"tyllä",
    u"tyn",
    u"tynä",
    u"tyssä",
    u"tystä",
    u"tyt",
    u"tyyn",
    u"tyä",
    u"tyään",
    u"tä",
    u"täkseen",
    u"tämme",
    u"täni",
    u"tänne",
    u"täsi",
    u"tävä",
    u"täväkseen",
    u"täväksi",
    u"tävää",
    u"tään",
    u"u",
    u"uista",
    u"un",
    u"uni",
    u"unsa",
    u"va",
    u"vaa",
    u"vaan",
    u"vaksi",
    u"valla",
    u"valle",
    u"valta",
    u"vamme",
    u"vampi",
    u"van",
    u"vana",
    u"vani",
    u"vansa",
    u"vasi",
    u"vassa",
    u"vasta",
    u"vat",
    u"via",
    u"vien",
    u"viin",
    u"viksi",
    u"villa",
    u"ville",
    u"villä",
    u"vilta",
    u"vimmista",
    u"vina",
    u"vine",
    u"vissa",
    u"vissä",
    u"vista",
    u"vistä",
    u"viä",
    u"vä",
    u"väksi",
    u"välle",
    u"vällä",
    u"vältä",
    u"vän",
    u"vänsä",
    u"vänä",
    u"vässä",
    u"västä",
    u"vät",
    u"vätkä",
    u"vää",
    u"vään",
    u"y",
    u"yn",
    u"ysi",
    u"ä",
    u"äkseen",
    u"äkseni",
    u"ämme",
    u"än",
    u"äni",
    u"änne",
    u"änsä",
    u"äsi",
    u"ään",
    u"ön",
    u"önsä",
};

static constexpr bool validateContainment() {
    for (const auto suffix : SUFFIX_SPLIT_PRIMITIVE) {
        if (!ArrayUtils::contains<ALL_SUFFIX_SEQUENCE_PRIMITIVE>(suffix)) {
            return false;
        }
    }
    return true;
}
static_assert(validateContainment(), "Not all suffix split primitives are in all known suffix sequences.");

static constexpr auto generateBreakPoints(std::u16string_view suffixSeq) {
    uint16_t breakPoints = 0;
    const auto notNominativeResult = ArrayUtils::searchSorted<NOT_NOMINATIVE_SUFFIX>(suffixSeq, [](const auto& item) { return item.notNominativeSuffix; });
    if (notNominativeResult != nullptr) {
        breakPoints = static_cast<uint16_t>(1 << notNominativeResult->split);
    }
    else if (ArrayUtils::contains<SUFFIX_SPLIT_PRIMITIVE>(suffixSeq)) {
        breakPoints = 1;
    }
    else if (suffixSeq.length() >= FiTokenizerConfig::MINSUFFIXSPLIT) {
        const uint8_t suffixSeqLen = static_cast<uint8_t>(suffixSeq.length());
        for (uint8_t i = 1; i < suffixSeqLen - 1; i++) {
            auto first(suffixSeq.substr(0, i));
            auto second(suffixSeq.substr(i, suffixSeqLen - i));
            if (ArrayUtils::contains<ALL_SUFFIX_SEQUENCE_PRIMITIVE>(first)
                && ArrayUtils::contains<ALL_SUFFIX_SEQUENCE_PRIMITIVE>(second))
            {
                if (ArrayUtils::contains<SUFFIX_SPLIT_PRIMITIVE>(first))
                {
                    breakPoints |= 1;
                    breakPoints |= 1 << i;
                }
                else if (ArrayUtils::contains<SUFFIX_SPLIT_PRIMITIVE>(second))
                {
                    breakPoints |= 1 << i;
                }
                else if (second.starts_with(u"i") && second.length() > 1
                    && ArrayUtils::contains<SUFFIX_SPLIT_PRIMITIVE>(second.substr(1)))
                {
                    breakPoints |= 1 << (i + 1);
                }
                else {
                    // None of the conditions have been met. Keep going.
                    continue;
                }
                break;
            }
        }
    }
    return breakPoints;
}

// This computes the break points for all suffix sequences.
// The breakpoints are compressed to a bit mask representing the breakpoints in the suffix.
// Given the current suffixes in 2026, the maximum length is 10, but the highest split is 7.
static uint16_t* loadSuffixInfo()
{
    auto suffixInfo = new uint16_t[std::ssize(ALL_SUFFIX_SEQUENCE_PRIMITIVE)];
    int32_t index = 0;
    for (const auto suffix : ALL_SUFFIX_SEQUENCE_PRIMITIVE) {
        suffixInfo[index++] = generateBreakPoints(suffix);
    }

    return suffixInfo;
}

bool FiSuffixSplit::isSuffix(std::u16string_view str)
{
    return ArrayUtils::contains<ALL_SUFFIX_SEQUENCE_PRIMITIVE>(str);
}

void FiSuffixSplit::addSuffixInfo(std::u16string_view suffix, ::std::vector<int32_t>* indexes, int32_t offset)
{
    auto result = ArrayUtils::searchSorted<ALL_SUFFIX_SEQUENCE_PRIMITIVE>(suffix, [](auto item) { return item; });
    if (result != nullptr) {
        static auto suffixInfo = loadSuffixInfo();
        auto splits = suffixInfo[result - ALL_SUFFIX_SEQUENCE_PRIMITIVE];
        while (splits != 0) {
            // e.g. 10011000 - 1 = 10010111
            uint32_t minusOne = splits - 1;
            // e.g. 10011000 ^ 10010111 = 00001111
            // e.g. 00001111 >> 1 = 00000111
            uint32_t bitCountAsPosition = (splits ^ minusOne) >> 1;
            // e.g. popcount(00000111) = 3
            npc(indexes)->push_back(offset + std::popcount(bitCountAsPosition));
            // Remove the least significant bit.
            // e.g. 10011000 & 10010111 = 10010000
            splits &= minusOne;
        }
    }
}

bool FiSuffixSplit::isInNotNominative(const FiSegment* segment, std::u16string_view suffix)
{
    for (const auto& entry : NOT_NOMINATIVE_SUFFIX) {
        if (suffix == entry.suffix) {
            return npc(segment)->getLabel().ends_with(entry.notNominativeSuffix);
        }
    }
    return true;
}

void FiSuffixSplit::stemming(::std::vector<::std::u16string_view>* result, std::u16string_view word, FiSuffixSplit::FiSuffixSplit::SuffixType suffixType)
{
    auto const maxAffixIdx = ::std::min(int32_t(suffixType == SuffixType::MIDDLE_SUFFIX ? FiTokenizerConfig::MAXMIDDLESUFFIXLENGTH : FiTokenizerConfig::MAXCLITICLENGTH + FiTokenizerConfig::MAXSUFFIXLENGTH), int32_t(word.length() - FiTokenizerConfig::MINSTEMLENGTH));
    for (int32_t i = 1; i <= maxAffixIdx; i++) {
        auto stem(word.substr(0, word.length() - i));
        auto affix(word.substr(word.length() - i, word.length() - (word.length() - i)));
        auto valid_ending = false;
        if (suffixType == SuffixType::MIDDLE_SUFFIX && containsMiddleSuffix(affix)) {
            valid_ending = true;
        }
        else if (suffixType == SuffixType::SUFFIX) {
            if (i > 0 && (containsClitic(affix) || isSuffix(affix)) && word.length() - i >= FiTokenizerConfig::MINSTEMLENGTH) {
                valid_ending = true;
            }
            else {
                int32_t affixLen = (int32_t)affix.length();
                for (int32_t j = 1; j < affixLen - 1 && !valid_ending; j++) {
                    valid_ending = isSuffix(affix.substr(0, affix.length() - j))
                        && containsClitic(affix.substr(affix.length() - j, affix.length() - (affix.length() - j)));
                }
            }
        }
        if (valid_ending) {
            npc(result)->emplace_back(stem);
        }
    }
}

bool FiSuffixSplit::found() const
{
    return found_;
}

float FiSuffixSplit::getLogProb() const
{
    return logProb;
}

FiSegment* FiSuffixSplit::getSegment() const
{
    return segment;
}

int32_t FiSuffixSplit::getStart() const
{
    return npc(segment)->getStart();
}

int32_t FiSuffixSplit::getEnd() const
{
    return npc(segment)->getEnd();
}

void FiSuffixSplit::addBoundaries(::std::vector<int32_t>* boundaries, FiSuffixSplit::SuffixType suffixType)
{
    if (found_) {
        logProb = npc(segment)->getLogProb();
        return npc(segment)->addBoundaries(boundaries);
    }
    auto const start = getStart();
    ::std::vector<int32_t> indexes;
    ::std::unique_ptr<FiSegment> max_stem;
    auto minLength_stemStrLength = int32_t(npc(segment)->getLabel().length());
    ::std::vector<::std::u16string_view> stems;
    stemming(&stems, npc(segment)->getLabel(), suffixType);
    for (const auto& stem : stems) {
        ::std::unique_ptr<FiSegment> stem_segment(npc(new FiSegment(segment, start, int32_t(start + stem.length()), false)));
        indexes.clear();
        stem_segment->addBoundaries(&indexes);
        if (stem_segment->found()) {
            found_ = true;
            if (stem_segment->getLogProb() > logProb) {
                logProb = stem_segment->getLogProb();
                max_stem.reset(stem_segment.release());
            }
        }
        auto stemLen = int32_t(stem.length());
        if (minLength_stemStrLength > stemLen && stemLen >= FiTokenizerConfig::MINUNKNOWNSTEMLENGTH) {
            minLength_stemStrLength = stemLen;
        }
    }
    int32_t maxStemLen;
    if (found_) {
        if (max_stem.get() == nullptr) {
            return;
        }
        max_stem->addBoundaries(boundaries);
        maxStemLen = max_stem->getLength();
    } else {
        maxStemLen = minLength_stemStrLength;
        npc(boundaries)->insert(npc(boundaries)->end(), indexes.begin(), indexes.end());
    }
    auto suffix = npc(segment)->getLabel().substr(maxStemLen);
    if (suffixType == FiSuffixSplit::SuffixType::SUFFIX && isInNotNominative(segment, suffix)) {
        FiSuffixSplit::addSuffixInfo(suffix, boundaries, start + maxStemLen);
    }
}

} // namespace inflection::tokenizer::locale::fi
