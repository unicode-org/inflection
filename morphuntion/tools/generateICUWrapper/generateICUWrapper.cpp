//
//  main.cpp
//  icu4cxx_generate
//
//  Created by Henry Mason on 10/8/16.
//  Copyright 2016-2024 Apple Inc. All rights reserved.
//
#include <iostream>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <utility>
#include <string>
#include <vector>
#include <unordered_set>
#include <regex>


using namespace std;

static const unordered_map<string, string> classForIcuPrefix {
    {"ubrk", "BreakIterator"},
    {"unorm2", "Normalizer2"},
    {"unum", "NumberFormat"},
    {"uregex", "RegularExpression"},
    {"utrans", "Transliterator"},
    {"uset", "UnicodeSet"},
};

static const unordered_map<string, string> icuTypeForClass {
    {"BreakIterator", "UBreakIterator"},
    {"Normalizer2", "UNormalizer2"},
    {"NumberFormat", "UNumberFormat"},
    {"RegularExpression", "URegularExpression"},
    {"Transliterator", "UTransliterator"},
    {"UnicodeSet", "USet"},
};

static const unordered_map<string, string> additionalHeaders {
    {"Normalizer2", "icu4cxx/UnicodeSet.hpp"},
    {"Transliterator", "icu4cxx/UnicodeSet.hpp"},
};

// These are the irregular renaming cases. They may provide bad conflicts.
static const unordered_map<string, string> methodRename {
    {"utrans_register", "registerInstance"},
    {"utrans_openInverse", "createInverse"},
};

static unordered_map<string, string> getClassForIcuType() {
    unordered_map<string, string> ret;
    for (const auto& it : icuTypeForClass) {
        ret[it.second] = it.first;
        // e.g. "USet *" -> "ICUSet"
        ret[it.second + " *"] = it.first;
        // e.g. "const USet *" -> "const ICUSet &"
        ret["const " + it.second + " *"] = "const " + it.first + " &";
    }
    return ret;
}
static const unordered_map<string, string> classForIcuType = getClassForIcuType();

struct RegexToMatch {
    regex from;
    string to;
    RegexToMatch(const string& from, const string& to)
        : from(from)
        , to(to)
    {
    }
};

static inline void ltrim(string &s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int ch) {
        return !std::isspace(ch);
    }));
}

// trim from end (in place)
static inline void rtrim(string &s) {
    s.erase(std::find_if(s.rbegin(), s.rend(), [](int ch) {
        return !std::isspace(ch);
    }).base(), s.end());
}

// trim from both ends (in place)
static inline void trim(string &s) {
    ltrim(s);
    rtrim(s);
}

static std::string ArgTypeForICUType(const std::string &icuType) {
    if (icuType == "UBool" ) {
        // Map UBool -> bool
        return "bool";
    }
    if (icuType == "UText *[]") {
        return "UText **";
    }
    if (icuType == "UChar *[]") {
        return "UChar **";
    }
    const auto &it = classForIcuType.find(std::string(icuType));
    if (it != classForIcuType.end()) {
        return it->second;
    }
    return icuType;
}

static std::string ReturnTypeForICUType(const std::string &icuType) {
    std::string ret = ArgTypeForICUType(icuType);
    if (ret.back() == '&') {
        // Return values instead of references.
        return ret.substr(0, ret.size()-1);
    }
    return ret;
}


static const unordered_set<string> icuTypeSkipList {
    "const USerializedSet *",
    "USerializedSet *",
    "const UEnumeration *",
    "UEnumeration *",
};

static const unordered_map<string, string> cxxStringTypeForCStringType {
    {"const UChar *", "std::u16string_view "},
    {"const char *", "std::string_view "},
};

struct Function {
    string returnType;
    string name;
    vector<pair<string, string>> args;

    Function(const Function& other)
    : returnType(other.returnType), name(other.name), args(other.args)
    {
    }

    Function(const string& returnType, const string& name, const vector<pair<string, string>>& args)
    : returnType(returnType), name(name), args(args)
    {
    }

    size_t argCount() const {
        size_t count = 0;
        for (const auto &arg : args) {
            if (arg.first == "UErrorCode *") {
                continue;
            }
            count++;
        }
        return count;
    }

    bool hasErrorCodeArg() const {
        for (const auto &arg : args) {
            if (arg.first == "UErrorCode *") {
                return true;
            }
        }
        return false;
    }

    bool firstArgIsConst() const {
        if (args.empty()) {
            return false;
        }
        return args[0].first.find("const") != string::npos;
    }

    bool isStatic() const  {
        return args.empty() || (!args.empty() && (classForIcuType.find(args[0].first) == classForIcuType.end()));
    }

    enum ArgumentSyle {
        Member,
        Ctor
    };

    enum ArgumentPosition {
        Decl,
        Invocation,
    };

    void writeArgs(stringstream &ss, const ArgumentSyle style, const ArgumentPosition pos) const {

        bool skipFirstArgument = (style == Member) && (pos == Decl) && (!isStatic());
        bool includeErrorCode = (pos == Invocation);
        bool includeTypes = (pos == Decl);
        bool includeWrappee = (style == Member) && (pos == Invocation) && (!isStatic());

        bool hasWrittenAnArg = false;
        bool hasSkippedFirstArgument = false;
        for (auto it = args.begin(); it != args.end(); ++it) {

            if (skipFirstArgument && !hasSkippedFirstArgument) {
                hasSkippedFirstArgument = true;
                continue;
            }
            if (!includeErrorCode && (it->first == "UErrorCode *")) {
                continue;
            }
            if (hasWrittenAnArg) {
                ss << ", ";
            } else {
                hasWrittenAnArg = true;
            }


            const auto &cxxStringTypeIt = cxxStringTypeForCStringType.find(it->first);
            if (cxxStringTypeIt != cxxStringTypeForCStringType.end()) {
                if ((it+1)->first == "int32_t") {
                    if (pos == Decl) {
                        ss << cxxStringTypeIt->second << it->second;
                    } else {
                        ss << "(" << it->first << ")" << it->second << ".data(), ";
                        ss << "(int32_t)" << it->second << ".size()";
                    }
                    // Skip the next (length) arugment
                    ++it;
                    continue;
                }
            }

            if (includeTypes) {
                ss << ArgTypeForICUType(it->first) << " ";
            }
            if (it->first == "UErrorCode *") {
                ss << "&ec"; // ignore name given in headers
            } else if (includeWrappee && (it == args.begin())) {
                ss << "wrappee_";
            } else {
                ss << it->second;
                // If this is an ICU object...
                if ((pos == Invocation) && (classForIcuType.find(it->first) != classForIcuType.end())) {
                    // ...unwrap it
                    ss << ".wrappee_";
                }
            }
        }
    }

    void writeErrorHandlingPreabmle(stringstream &ss) const  {
        if (hasErrorCodeArg()) {
            ss << "    UErrorCode ec = U_ZERO_ERROR;\n";
        }
    }

    void writeInvocation(stringstream &ss, bool forCtor) const {
        ss << name << "(";
        writeArgs(ss, forCtor ? Ctor : Member, Invocation);
        ss << ")";
    }

    ::std::string getErrorHandlingTrailer() const {
        if (hasErrorCodeArg()) {
            return "    ::morphuntion::exception::ICUException::throwOnFailure(ec);\n";
        }
        return "";
    }
};

static void RunOnFile(const char *className, const char *filePath, std::stringstream &dest) {
    ifstream fileStream(filePath);
    string header;

    fileStream.seekg(0, std::ios::end);
    header.reserve(fileStream.tellg());
    fileStream.seekg(0, std::ios::beg);

    header.assign((std::istreambuf_iterator<char>(fileStream)), std::istreambuf_iterator<char>());
    regex stableHeadersRegEx("(\\bU_CAPI\\b[^;]+;)");
    vector<RegexToMatch> normalizationMap = {
        RegexToMatch("\\bU_CAPI\\b", ""),
        RegexToMatch("\\bU_EXPORT2\\b", ""),
        RegexToMatch("\\s*/\\*[^\\*]*\\*/", ""),
        RegexToMatch("\\*", " *"),
        RegexToMatch("\\s+", " "),
        RegexToMatch("^\\s+", ""),
        RegexToMatch("\\(\\s+", "("),
        RegexToMatch("\\s+\\)", ")"),
        RegexToMatch("([a-zA-Z0-9_]+)(\\[\\s*\\])", "$2$1"),
    };
    regex functionParts("(.+) ([a-zA-Z0-9_]+)\\(([^)]*)\\);");
    regex argumentParts("\\s*(.*)\\s*\\b([a-zA-Z0-9_]+)");
    smatch functionsMatch;
    auto searchStart(header.cbegin());
    vector<Function> funcs;

    while (regex_search(searchStart, header.cend(), functionsMatch, stableHeadersRegEx)) {
        for (size_t i = 1; i < functionsMatch.size(); ++i) {
            string function(functionsMatch[i]);
            for (const auto& regexToMatch : normalizationMap) {
                string updatedFunction = regex_replace(function, regexToMatch.from, regexToMatch.to, regex_constants::format_default);
                function = updatedFunction;
            }

            string returnType;
            string functionName;
            string functionArgs;

            smatch functionMatch;
            if (regex_match(function, functionMatch, functionParts)) {
                // The first sub_match is the whole string; the next
                // sub_match is the first parenthesized expression.
                if (functionMatch.size() == 4) {
                    returnType = functionMatch[1].str();
                    functionName = functionMatch[2].str();
                    functionArgs = functionMatch[3].str();
                    if (functionArgs == "void") {
                        functionArgs = "";
                    }
                }
                else {
                    cerr << functionMatch.size() << " matches were returned for function parts" << endl;
                }
            }
            else {
                cerr << "Bad match for function parts" << endl;
            }

            vector<pair<string, string>> arguments;
            stringstream functionArgsStream(functionArgs);
            string argument;
            while (getline(functionArgsStream, argument, ',')) {
                smatch argumentMatch;
                if (regex_match(argument, argumentMatch, argumentParts)) {
                    // The first sub_match is the whole string; the next
                    // sub_match is the first parenthesized expression.
                    if (argumentMatch.size() == 3) {
                        string argType = argumentMatch[1].str();
                        string argName = argumentMatch[2].str();
                        trim(argType);
                        trim(argName);
                        arguments.emplace_back(argType, argName);
                    }
                    else {
                        cerr << argumentMatch.size() << " matches were returned for arguments" << endl;
                    }
                }
                else {
                    cerr << "Bad match for argument parts in " << function << endl;
                }
            }

            funcs.emplace_back(returnType, functionName, arguments);
        }
        searchStart += functionsMatch.position() + functionsMatch.length();
    }

    unordered_map<string, stringstream> declartionsForClass;

    unordered_set<string> hasWrittenCopyCtorForClass;
    for (const auto &func : funcs) {

        auto prefixPos = func.name.find('_');
        if (prefixPos == string::npos) {
            continue;
        }
        string prefix = func.name.substr(0, prefixPos);

        auto typeForPrefix = classForIcuPrefix.find(prefix);
        if (typeForPrefix == classForIcuPrefix.end()) {
//            std::cerr << "Skipping unknown prefix: " << prefix << "\n";
            continue;
        }

        if (!func.args.empty() && (icuTypeSkipList.find(func.args[0].first) != icuTypeSkipList.end() || icuTypeSkipList.find(func.returnType) != icuTypeSkipList.end())) {
            continue;
        }

        auto methodRenameResult = methodRename.find(func.name);
        string unprefixedFuncName;
        if (methodRenameResult == methodRename.end()) {
            unprefixedFuncName = func.name.substr(prefixPos+1, func.name.size());
        }
        else {
            unprefixedFuncName = methodRenameResult->second;
        }
        auto &ss = declartionsForClass[typeForPrefix->second];

        const auto &clsName = typeForPrefix->second;

        if (unprefixedFuncName.find("open") == 0) {
            // Special case for ctors
            ss << clsName << "(";
            func.writeArgs(ss, Function::Ctor, Function::Decl);
            ss << ") {\n";
            func.writeErrorHandlingPreabmle(ss);
            ss << "    wrappee_ = ";
            func.writeInvocation(ss, true);
            ss << ";\n";
            ss << func.getErrorHandlingTrailer();
            ss << "}\n";

            continue;
        }
        if (unprefixedFuncName == "close") {
            // Special case for dtor
            ss << "~" << clsName << "() noexcept {\n";
            ss << "    if (owned_) " << func.name << "(wrappee_);\n";
            ss << "}\n";

            // Also a good time to implement move ctor and assignment to fill out rule-of-five
            ss << clsName << "(" << clsName << " &&other) noexcept : wrappee_(other.wrappee_), owned_(other.owned_) {\n";
            ss << "    other.wrappee_ = nullptr;\n";
            ss << "}\n";
            ss << clsName << "& operator=(" << clsName << " &&other) noexcept {\n";
            ss << "    if (owned_) " << func.name << "(wrappee_);\n";
            ss << "    wrappee_ = other.wrappee_;\n";
            ss << "    owned_ = other.owned_;\n";
            ss << "    other.wrappee_ = nullptr;\n";
            ss << "    other.owned_ = false;\n";
            ss << "    return *this;\n";
            ss << "}\n";
            continue;
        }
        if (unprefixedFuncName == "clone") {
            // Special case for copy ctor
            ss << clsName << "(const " << clsName << " &" << func.args[0].second << ") {\n";
            func.writeErrorHandlingPreabmle(ss);
            ss << "    wrappee_ = ";
            func.writeInvocation(ss, true);
            ss << ";\n";
            ss << "    owned_ = true;\n"; // You just cloned it. You own the copy now.
            ss << func.getErrorHandlingTrailer();
            ss << "}\n";
            ss << clsName << "& operator=(const " << clsName << " &other) noexcept {\n";
            ss << "    " << clsName << " tmp(other);\n";
            ss << "    *this = std::move(tmp);\n";
            ss << "    return *this;\n";
            ss << "}\n";
            hasWrittenCopyCtorForClass.insert(typeForPrefix->second);
            continue;
        }


        if (func.isStatic()) {
            ss << "static ";
        }
        ss << ReturnTypeForICUType(func.returnType) << " " << unprefixedFuncName << "(";
        func.writeArgs(ss, Function::Member, Function::Decl);
        ss << ")";
        if (func.firstArgIsConst() && !func.isStatic()) {
            ss << " const";
        }
        ss <<" {\n";
        func.writeErrorHandlingPreabmle(ss);
        ss << "    ";
        auto errorHandlingTrailer(func.getErrorHandlingTrailer());
        if (func.returnType != "void") {
            if (errorHandlingTrailer.empty()) {
                ss << "return ";
            }
            else {
                ss << "auto retVal = ";
            }
        }
        func.writeInvocation(ss, false);
        ss << ";\n";
        ss << errorHandlingTrailer;
        if (unprefixedFuncName.find("Instance") != string::npos && unprefixedFuncName.compare(0, 3, "get") == 0) {
            ss << "    return " << clsName << "(retVal, false);\n";
        } else {
            if (func.returnType != "void" && !errorHandlingTrailer.empty()) {
                ss << "    return retVal;\n";
            }
        }
        ss << "}\n";
    }

    dest << "/*\n";
    dest << " * Copyright 2016-2024 Apple Inc. All rights reserved.\n";
    dest << " */\n";
    dest << "// This is a generated file. Do not edit this file.\n";
    dest << "#pragma once\n";
    dest << "#include <string_view>\n";
    dest << "#include <morphuntion/exception/ICUException.hpp>\n";
    dest << "#include <morphuntion/exception/NullPointerException.hpp>\n";
    string filePathStr(filePath);
    auto pathIndex = filePathStr.rfind('/');
    if (pathIndex != string::npos) {
        pathIndex++;
        filePathStr = filePathStr.substr(pathIndex, filePathStr.length() - pathIndex);
    }
    dest << "#include <unicode/" << filePathStr << ">\n";
    auto additionalHeadersResult = additionalHeaders.find(className);
    if (additionalHeadersResult != additionalHeaders.end()) {
        dest << "#include <" << additionalHeadersResult->second << ">\n";
    }
    dest << "\n";
    dest << "namespace icu4cxx {\n";

    for (const auto &cls : declartionsForClass) {
        const auto &clsName = cls.first;

        if (clsName != className) {
            continue;
        }

        dest << "class " << clsName << " {\n";
        dest << "public:\n";

        dest << cls.second.str() << "\n";

        if (hasWrittenCopyCtorForClass.find(clsName) == hasWrittenCopyCtorForClass.end()) {
            dest << clsName << "(const " << clsName << " &other) {\n";
            dest << "    if (other.owned_) {\n";
            dest << "        throw ::morphuntion::exception::NullPointerException();\n";
            dest << "    }\n";
            dest << "    wrappee_ = other.wrappee_;\n";
            dest << "    owned_ = false;\n";
            dest << "}\n";
            dest << clsName << "& operator= (const " << clsName << " &other) noexcept {\n";
            dest << "    " << clsName << " tmp(other);\n";
            dest << "    *this = std::move(tmp);\n";
            dest << "    return *this;\n";
            dest << "}\n";
        }

        const auto& icuType = icuTypeForClass.at(cls.first);
        dest << icuType << " *wrappee_ {  };\n";
        dest << cls.first << "(const " << icuType << " *other, bool owned = true) : wrappee_((" << icuType << " *)other), owned_(owned) {}\n";
        dest << "private:\n";
        dest << "bool owned_ = true;\n";

        dest << "};\n";
    }

    dest << "} //namespace icu4xx \n";
}

int main(int argc, const char * argv[]) {

    if (argc != 4) {
        std::cerr << "usage: icu4cxx_generate Whatever uwhatever.h Whatever.hpp\n";
        return 1;
    }
    
    std::stringstream ss;
    RunOnFile(argv[1], argv[2], ss);

    std::ofstream ofs;
    ofs.open(argv[3], std::ofstream::out);
    ofs << ss.str();
    ofs.close();

    std::cerr << "Emitted "<< argv[3] << "\n";

    return 0;
}
