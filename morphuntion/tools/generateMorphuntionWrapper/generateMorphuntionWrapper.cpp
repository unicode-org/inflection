/*
 * Copyright 2019-2024 Apple Inc. All rights reserved.
 */
#include <iostream>
#include <fstream>
#include <sstream>
#include <utility>
#include <tuple>
#include <string>
#include <vector>
#include <map>
#include <regex>

using namespace std;

struct RegexToMatch {
    regex from;
    string to;
    RegexToMatch(string from, string to)
        : from(from)
        , to(to)
    {
    }
};

static void RunOnFile(const std::string& filePath, std::stringstream &dest) {
    ifstream fileStream(filePath);
    string header;

    fileStream.seekg(0, std::ios::end);
    header.reserve(fileStream.tellg());
    fileStream.seekg(0, std::ios::beg);

    header.assign((std::istreambuf_iterator<char>(fileStream)), std::istreambuf_iterator<char>());
    string className;
    regex classRegex("[ ]*class[ ]+MORPHUNTION_CLASS_API[ ]+([^ \n]+)");
    std::smatch matches;
    if (std::regex_search(header, matches, classRegex)) {
        className = matches[1];
    } else {
        std::cout << "Header match not found\n";
        exit(1);
    }

    auto functions = header;
    size_t startOfComment = 0;
    size_t endOfComment = 0;

    while ((startOfComment = functions.find("/*", startOfComment)) != string::npos
        && (endOfComment = functions.find("*/", startOfComment)) != string::npos)
    {
        functions.erase(startOfComment, endOfComment - startOfComment + 2); // +2 for end of comment
    }

    auto typeName(className);
    auto bareTypeName(className);
    auto lastColon = bareTypeName.rfind(':');
    if (lastColon != string::npos) {
        bareTypeName.erase(0, lastColon + 1);
    }

    vector<RegexToMatch> remapNameMap = {
        RegexToMatch("(?:::)?morphuntion::dialog::proxy::", "MDP"),
        RegexToMatch("(?:::)?morphuntion::dialog::", "MD"),
        RegexToMatch("(?:::)?morphuntion::dictionary::", "MDICT"),
        RegexToMatch("(?:::)?morphuntion::resources::", "MR"),
        RegexToMatch("(?:::)?morphuntion::analysis::", "MANA"),
        RegexToMatch("(?:::)?morphuntion::lang::features::", "MFEAT"),
        RegexToMatch("(?:::)?morphuntion::lang::", "MLANG"),
        RegexToMatch("^(?:::)?morphuntion::([A-Z][A-Z_a-z]+)$", "M$1"),
    };

    for (const auto& regexToMatch : remapNameMap) {
        typeName = regex_replace(typeName, regexToMatch.from, regexToMatch.to, regex_constants::format_default);
    }

    vector<RegexToMatch> normalizationMap = {
        RegexToMatch("//.*", ""),
        RegexToMatch(",\\s*\n", ", "),
        RegexToMatch("#.*", ""),
        RegexToMatch(".*friend class .*", ""),
        RegexToMatch(".*(private|protected):(.|\n)*?public:", ""),
        RegexToMatch(".*(private|protected):(.|\n)*\\};", "};"),
        RegexToMatch(".*public:.*", ""),
        RegexToMatch(".*typedef.*super;\n", ""),
        RegexToMatch("(virtual|explicit|inline) ", ""),
        RegexToMatch(" override;", ";"),
        RegexToMatch("class MORPHUN_CLASS_API ([^{]|\n)*\\{", ""),
        RegexToMatch("  +", " "),
        RegexToMatch("\\s*\n", "\n"),
        RegexToMatch("\n\\s*", "\n"),
        RegexToMatch("\n+", "\n"),
        RegexToMatch("\n$", ""),
        RegexToMatch("^\n", ""),
        RegexToMatch("\\};\n([^(){}]*\\([^(){}]*\\)[^(){}]\n*[^(){}]*\\{[^{}]*\\})+", "};"),
        RegexToMatch("\\};$", ""),
        RegexToMatch("\\s*(const\\s*)?(=\\s*0)?;", ";"),
        RegexToMatch("([^(]+ [^(]+\\()(.*)\\)\\s*(const)?;", "$1$3 " + typeName + "* thisObject, $2);"),
        RegexToMatch(", \\)", ")"),
        RegexToMatch("\\( +", "("),
        RegexToMatch("static (.*)\\((const )?" + typeName + "\\* thisObject(, )?", "$1("),
        RegexToMatch("\\b" + bareTypeName + "\\b", typeName),
        RegexToMatch("^", "MORPHUN_CTYPE(" + typeName + ")\n"),
        RegexToMatch("([^\\(])\\);", "$1, CFErrorRef* error);"),
        RegexToMatch("\\(\\);", "(CFErrorRef* error);"),
        RegexToMatch("\n(.*\\()", "\nMORPHUN_CAPI $1"),
        RegexToMatch("\n[^\n]*operator!=[^\n]*\n", "\n"),
    };

    for (const auto& regexToMatch : normalizationMap) {
        functions = regex_replace(functions, regexToMatch.from, regexToMatch.to, regex_constants::format_default);
    }

    map<string, string> functionPrefixMap({
        {"morphuntion::lang::features::LanguageGrammarFeatures", "mfeat_"},
        {"morphuntion::lang::features::LanguageGrammarFeatures_Feature", "mfeat_"},
        {"morphuntion::lang::features::LanguageGrammarFeatures_GrammarCategory", "mfeat_"},
        {"morphuntion::lang::features::LanguageGrammarFeatures_GrammarFeatures", "mfeat_"},
        {"morphuntion::dialog::CommonConceptFactory", "mccf_"},
        {"morphuntion::dialog::InflectableStringConcept", "minf_"},
        {"morphuntion::dialog::LocalizedCommonConceptFactoryProvider", "mlccfp_"},
        {"morphuntion::dialog::NumberConcept", "mnum_"},
        {"morphuntion::dialog::PronounConcept", "mpron_"},
        {"morphuntion::dialog::SemanticFeature", "msf_"},
        {"morphuntion::dialog::SemanticFeatureConceptBase", "msfcb_"},
        {"morphuntion::dialog::SemanticFeatureModel", "msfmod_"},
        {"morphuntion::dialog::SemanticFeatureModel_DisplayValue", "mdv_"},
        {"morphuntion::dialog::SemanticValue", "msv_"},
        {"morphuntion::dialog::SpeakableString", "mss_"},
        {"morphuntion::dictionary::DictionaryMetaData", "mdict_"},
        {"morphuntion::resources::DataRegistrationService", "mdr_"},
        {"morphuntion::Token", "mtok_"},
        {"morphuntion::TokenChain", "mtokc_"},
        {"morphuntion::Tokenizer", "mtok_"},
    });
    auto functionPrefix = functionPrefixMap.find(className);

    if (functionPrefix != functionPrefixMap.end()) {
        regex destructor("~" + typeName + R"(\(CFErrorRef\* error\);)");
        functions = regex_replace(functions, destructor, "void " + functionPrefix->second + "destroy($1);", regex_constants::format_default);

        regex constructor("\\b" + typeName + "\\(");
        functions = regex_replace(functions, constructor, functionPrefix->second + "create(", regex_constants::format_default);

        regex functionPrefixRegex(" ([^ (\n_]*\\()");
        functions = regex_replace(functions, functionPrefixRegex, " " + functionPrefix->second + "$1", regex_constants::format_default);

        regex bareStructRegex("\\b" + functionPrefix->first + "\\b");
        functions = regex_replace(functions, bareStructRegex, bareTypeName, regex_constants::format_default);
    }

    vector<RegexToMatch> typeMigration = {
        RegexToMatch("const\\s+:*std::u16string(\\s*[*&])?", "CFStringRef "),
        RegexToMatch(":*std::u16string_view(\\s*[*&])?", "CFStringRef "),
        RegexToMatch(":*std::u16string(\\s*[*&])?", "CFMutableStringRef "),
        RegexToMatch("const\\s+:*std::vector<[^>]*>(\\s*[*&])?", "CFArrayRef "),
        RegexToMatch(":*std::vector<[^>]*>(\\s*[*&])?", "CFMutableArrayRef "),
        RegexToMatch("const\\s+:*std::map<[^>]*>(\\s*[*&])?", "CFDictionaryRef "),
        RegexToMatch(":*std::map<[^>]*>(\\s*[*&])?", "CFMutableDictionaryRef "),
        RegexToMatch("const\\s+:*std::set<[^>]*>(\\s*[*&])?", "CFSetRef "),
        RegexToMatch(":*std::set<[^>]*>(\\s*[*&])?", "CFMutableSetRef "),
        RegexToMatch("(const\\s+)?:*morphuntion::util::ULocale(\\s*[*&])?", "CFLocaleRef "),
        RegexToMatch(":*icu4cxx::UnicodeSet(\\s*[*&])?", "USet*"),
        RegexToMatch(":*morphuntion::Chunk", "MTokenChain"),
        RegexToMatch(":*morphuntion::TokenChain", "MTokenChain"),
        RegexToMatch("(:*morphuntion::dialog::)?SpeakableString(\\s*[*&])?", "MDSpeakableString*"),
        RegexToMatch("(:*morphuntion::dialog::)?SemanticFeatureModel_DisplayValue(\\s*[*&])?", "MDSemanticFeatureModel_DisplayValue*"),
        RegexToMatch("(:*morphuntion::dialog::)?SemanticFeatureModel(\\s*[*&])?", "MDSemanticFeatureModel*"),
        RegexToMatch("(:*morphuntion::dialog::)?NumberConcept(\\s*[*&])?", "MDNumberConcept*"),
        RegexToMatch("(:*morphuntion::dialog::)?InflectableStringConcept(\\s*[*&])?", "MDInflectableStringConcept*"),
        RegexToMatch("(:*morphuntion::dialog::)?GrammaticalCountConcept(\\s*[*&])?", "MDGrammaticalCountConcept*"),
        RegexToMatch("(:*morphuntion::dialog::)?SemanticFeatureConceptBase(\\s*[*&])?", "MDSemanticFeatureConcept*"),
        RegexToMatch(" {2,}", " "),
    };

    for (const auto& regexToMatch : typeMigration) {
        functions = regex_replace(functions, regexToMatch.from, regexToMatch.to, regex_constants::format_default);
    }

    vector<RegexToMatch> headerInclusion = {
        RegexToMatch("CF(Mutable)?String", "#include <CoreFoundation/CFString.h>\n"),
        RegexToMatch("CF(Mutable)?Array", "#include <CoreFoundation/CFArray.h>\n"),
        RegexToMatch("CF(Mutable)?Dictionary", "#include <CoreFoundation/CFDictionary.h>\n"),
        RegexToMatch("CF(Mutable)?Set", "#include <CoreFoundation/CFSet.h>\n"),
        RegexToMatch("CFLocale", "#include <CoreFoundation/CFLocale.h>\n"),
        RegexToMatch("CFError", "#include <CoreFoundation/CFError.h>\n"),
        RegexToMatch("USet", "#include <unicode/uset.h>\n"),
        RegexToMatch("MORPHUNTION_CAPI", "#include <morphuntion/api.h>\n"),
        RegexToMatch("^", "#pragma once\n"),
    };

    for (const auto& regexToMatch : headerInclusion) {
        if (regex_search(functions, regexToMatch.from)) {
            functions.insert(0, regexToMatch.to);
        }
    }

    dest << functions;
}

int main(int argc, const char * argv[]) {
    if (argc <= 1) {
        std::cerr << "usage: generateMorphuntionWrapper Whatever.hpp [ ... ]\n";
        return 1;
    }

    std::string hSuffix(".hpp");
    std::string headerCpp(argv[1]);
    if (headerCpp.size() <= hSuffix.size() ||
        headerCpp.compare(headerCpp.size() - hSuffix.size(), hSuffix.size(), hSuffix) != 0)
    {
        std::cerr << "usage: generateMorphuntionWrapper Whatever.hpp [ ... ]\n";
        return 1;
    }
    std::string headerC(headerCpp, 0, headerCpp.size() - hSuffix.size());
    headerC += ".h";

    std::stringstream ss;
    RunOnFile(headerCpp, ss);

    ::std::ofstream ofs;
    ofs.open(headerC, std::ofstream::out);
    ofs << ss.str();
    ofs.close();

    std::cerr << "Emitted " << headerC << "\n";

    return 0;
}
