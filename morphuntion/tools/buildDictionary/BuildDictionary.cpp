/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#include "Dictionary.hpp"
#include "DictionaryLogger.hpp"
#include "LexicalDictionaryBuilder.hpp"
#include <morphuntion/lang/features/LanguageGrammarFeatures.hpp>
#include <morphuntion/util/ULocale.hpp>
#include <morphuntion/npc.hpp>
#include <iostream>
#include <fstream>

static const char USAGE_STRING[] =
        "Usage: buildDictionary --locale LOCALE --outfile OUTFILE --infile INFILE [--supplementalfile INFILE] [--inflectionfile INFILE]";

static void checkArgument(bool failureCondition, std::string_view message) {
    if (failureCondition) {
        std::cerr << message << std::endl;
        std::cerr << USAGE_STRING << std::endl;
        exit(-1);
    }
}

int main(int argc, const char* const argv[])
{
    ::morphuntion::util::ULocale locale("");
    ::std::string sourceFilename;
    ::std::string targetFilename;
    ::std::string additionalSourceFileName;
    ::std::string sourceInflectionFilename;
    bool verbose = false;
    bool addAffixPatternMappings = false;

    for (int32_t i = 1; i < argc; i++) {
        if (std::string("--locale") == argv[i]) {
            checkArgument(!locale.getLanguage().empty(), "Multiple --locale parameters defined");
            checkArgument(i >= argc, "Need a locale after --locale");
            locale = morphuntion::util::ULocale(argv[++i]);
        } else if (std::string("--outfile") == argv[i]) {
            checkArgument(!targetFilename.empty(), "Multiple --outfile parameters defined");
            checkArgument(i >= argc, "Need a file path after --outfile");
            targetFilename = argv[++i];
        } else if (std::string("--infile") == argv[i]) {
            checkArgument(!sourceFilename.empty(), "Multiple --infile parameters defined");
            checkArgument(i >= argc, "Need a file path after --infile");
            sourceFilename = argv[++i];
        } else if (std::string("--supplementalfile") == argv[i]) {
            checkArgument(!additionalSourceFileName.empty(), "Multiple --supplementalfile parameters defined");
            checkArgument(i >= argc, "Need a file path after --supplementalfile");
            additionalSourceFileName = argv[++i];
        } else if (std::string("--inflectionfile") == argv[i]) {
            checkArgument(!sourceInflectionFilename.empty(), "Multiple --inflectionfile parameters defined");
            checkArgument(i >= argc, "Need a file path after --inflectionfile");
            sourceInflectionFilename = argv[++i];
        } else if (std::string("--affix-patterns-mappings") == argv[i]) {
            addAffixPatternMappings = true;
        } else if (std::string("--verbose") == argv[i]) {
            verbose = true;
        } else {
            checkArgument(true, std::string("Unknown argument: ") + argv[i]);
        }
    }
    checkArgument(locale.getLanguage().empty(), "Required parameter --locale is missing");
    checkArgument(sourceFilename.empty(), "Required parameter --infile is missing");
    checkArgument(targetFilename.empty(), "Required parameter --outfile is missing");
    checkArgument(addAffixPatternMappings && sourceInflectionFilename.empty(), "The --affix-patterns-mappings requires --inflectionfile");

    auto dictionary = Dictionary::setupDictionary(locale, sourceFilename, additionalSourceFileName);

    for (const auto& grammarCategory : morphuntion::lang::features::LanguageGrammarFeatures::getLanguageGrammarFeatures(locale).getCategories()) {
        if (grammarCategory.second.isUniqueValues()) {
            // Add only values that are unambiguous and the category name is implied.
            for (const auto& grammarCategoryValue : grammarCategory.second.getValues()) {
                npc(dictionary)->createGrammemeTypeIfNonExistant(grammarCategoryValue);
            }
        }
    }

    ::std::ofstream writer(targetFilename.c_str(), std::ios::out|std::ios::binary);
    if (!writer) {
        std::cerr << "Could not write output to " + targetFilename << std::endl;
        exit(-1);
    }
    DictionaryLogger logger(writer, verbose);
    LexicalDictionaryBuilder::writeDictionary(writer, logger, *npc(dictionary), sourceInflectionFilename, addAffixPatternMappings);
    logger.logWithOffset(locale.getName() + " final offset");

    delete dictionary;

    ::std::cout << "Dictionary written for " << locale.getName() << ::std::endl;
    return 0;
}
