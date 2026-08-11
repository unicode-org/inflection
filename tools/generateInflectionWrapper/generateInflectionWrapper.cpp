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
    regex classRegex("[ ]*class[ ]+INFLECTION_CLASS_API[ ]+([^ \n]+)");
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
        RegexToMatch("(?:::)?inflection::dialog::proxy::", "MDP"),
        RegexToMatch("(?:::)?inflection::dialog::", "MD"),
        RegexToMatch("(?:::)?inflection::dictionary::", "MDICT"),
        RegexToMatch("(?:::)?inflection::resources::", "MR"),
        RegexToMatch("(?:::)?inflection::analysis::", "MANA"),
        RegexToMatch("(?:::)?inflection::lang::features::", "MFEAT"),
        RegexToMatch("(?:::)?inflection::lang::", "MLANG"),
        RegexToMatch("^(?:::)?inflection::([A-Z][A-Z_a-z]+)$", "M$1"),
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
        RegexToMatch("class INFLECTION_CLASS_API ([^{]|\n)*\\{", ""),
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
        RegexToMatch("^", "INFLECTION_CTYPE(" + typeName + ")\n"),
        RegexToMatch("([^\\(])\\);", "$1, UErrorCode* status);"),
        RegexToMatch("\\(\\);", "(UErrorCode* status);"),
        RegexToMatch("\n(.*\\()", "\nINFLECTION_CAPI $1"),
        RegexToMatch("\n[^\n]*operator!=[^\n]*\n", "\n"),
    };

    for (const auto& regexToMatch : normalizationMap) {
        functions = regex_replace(functions, regexToMatch.from, regexToMatch.to, regex_constants::format_default);
    }

    map<string, string> functionPrefixMap({
        {"inflection::lang::features::LanguageGrammarFeatures", "mfeat_"},
        {"inflection::lang::features::LanguageGrammarFeatures_Feature", "mfeat_"},
        {"inflection::lang::features::LanguageGrammarFeatures_GrammarCategory", "mfeat_"},
        {"inflection::lang::features::LanguageGrammarFeatures_GrammarFeatures", "mfeat_"},
        {"inflection::dialog::CommonConceptFactory", "mccf_"},
        {"inflection::dialog::InflectableStringConcept", "minf_"},
        {"inflection::dialog::LocalizedCommonConceptFactoryProvider", "mlccfp_"},
        {"inflection::dialog::NumberConcept", "mnum_"},
        {"inflection::dialog::PronounConcept", "mpron_"},
        {"inflection::dialog::SemanticFeature", "msf_"},
        {"inflection::dialog::SemanticFeatureConceptBase", "msfcb_"},
        {"inflection::dialog::SemanticFeatureModel", "msfmod_"},
        {"inflection::dialog::SemanticFeatureModel_DisplayValue", "mdv_"},
        {"inflection::dialog::SemanticValue", "msv_"},
        {"inflection::dialog::SpeakableString", "mss_"},
        {"inflection::dictionary::DictionaryMetaData", "mdict_"},
        {"inflection::resources::DataRegistrationService", "mdr_"},
        {"inflection::Token", "mtok_"},
        {"inflection::TokenChain", "mtokc_"},
        {"inflection::Tokenizer", "mtok_"},
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
        RegexToMatch("const\\s+:*std::u16string(\\s*[*&])?", "const char16_t* "),
        RegexToMatch(":*std::u16string_view(\\s*[*&])?", "const char16_t* "),
        RegexToMatch(":*std::u16string(\\s*[*&])?", "char16_t* "),
        RegexToMatch("(const\\s+)?:*inflection::util::ULocale(\\s*[*&])?", "const char* "),
        RegexToMatch(":*icu4cxx::UnicodeSet(\\s*[*&])?", "USet*"),
        RegexToMatch("(:*inflection::dialog::)?SpeakableString(\\s*[*&])?", "IDSpeakableString*"),
        RegexToMatch("(:*inflection::dialog::)?SemanticFeatureModel_DisplayValue(\\s*[*&])?", "IDSemanticFeatureModel_DisplayValue*"),
        RegexToMatch("(:*inflection::dialog::)?SemanticFeatureModel(\\s*[*&])?", "IDSemanticFeatureModel*"),
        RegexToMatch("(:*inflection::dialog::)?NumberConcept(\\s*[*&])?", "IDNumberConcept*"),
        RegexToMatch("(:*inflection::dialog::)?InflectableStringConcept(\\s*[*&])?", "IDInflectableStringConcept*"),
        RegexToMatch("(:*inflection::dialog::)?GrammaticalCountConcept(\\s*[*&])?", "IDGrammaticalCountConcept*"),
        RegexToMatch("(:*inflection::dialog::)?SemanticFeatureConceptBase(\\s*[*&])?", "IDSemanticFeatureConcept*"),
        RegexToMatch(" {2,}", " "),
    };

    for (const auto& regexToMatch : typeMigration) {
        functions = regex_replace(functions, regexToMatch.from, regexToMatch.to, regex_constants::format_default);
    }

    vector<RegexToMatch> headerInclusion = {
        RegexToMatch("USet", "#include <unicode/uset.h>\n"),
        RegexToMatch("INFLECTION_CAPI", "#include <inflection/api.h>\n"),
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
        std::cerr << "usage: generateInflectionWrapper Whatever.hpp [ ... ]\n";
        return 1;
    }

    std::string hSuffix(".hpp");
    std::string headerCpp(argv[1]);
    if (headerCpp.size() <= hSuffix.size() ||
        headerCpp.compare(headerCpp.size() - hSuffix.size(), hSuffix.size(), hSuffix) != 0)
    {
        std::cerr << "usage: generateInflectionWrapper Whatever.hpp [ ... ]\n";
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
