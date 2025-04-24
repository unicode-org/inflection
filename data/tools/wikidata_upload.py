"""
Sample script for uploading lexicon data to Wikidata using tfsl library.

Follow these instructions to get tfsl dependency:
https://phabricator.wikimedia.org/source/tool-twofivesixlex/browse/main/

git clone https://phabricator.wikimedia.org/source/tool-twofivesixlex tfsl

Code expects tfsl checkout location next to inflection repository.

Sample input_file is provided as new_lexeme_sample.json
"""

import argparse
import json
import os
import sys
import time

sys.path.append(os.path.join(os.path.dirname(__file__), "..", "..", "..", "tfsl"))
import tfsl

# Easy conversion from humanly readable form to/from Wikidata codes.
WIKI_TYPES = {
    "singular": "Q110786",
    "plural": "Q146786",
    "nominative": "Q131105",
    "genitive": "Q146233",
    "dative": "Q145599",
    "accusative": "Q146078",
    "vocative": "Q185077",
    "locative": "Q202142",
    "instrumental": "Q192997",
    "animate": "Q51927507",
    # Grammatical gender
    "feminine": "Q1775415",
    "masculine": "Q499327",
    "neuter": "Q1775461",
    # To be used in "sex or gender" (P21) to indicate that the human subject is a male/female
    # or "semantic gender" (P10339) to indicate that a word refers to a male/female person.
    "female": "Q6581072",
    "male": "Q6581097",
    "gender": "P5185",
    "semanticgender": "P10339",
    "noun": "Q1084",
    "proper noun": "Q147276",
    "pronouns": "Q36224",
    "possesive pronouns": "Q1502460",
    "adjective": "Q34698",
    "personal pronoun": "Q468801",
    # Word or form that substitutes for another word, broader scope than pronoun.
    "pro-form": "Q2006180",
}
# Make it a bidirectional dictionary.
WIKI_TYPES.update(dict([reversed(i) for i in WIKI_TYPES.items()]))


def make_search_key(lemma, cat, lang):
    """Makes a key for searching through wikidata for duplicates.

    Args:
        lemma (str): Lemma
        cat (str): gramatical category
        lang (str): language
    """
    return "-".join([lemma, cat, lang])


def load_lexemes(lang, input_file):
    """
    Loads lexemes from the input_file.

    Args:
        lang (str): Requested language.
        input_file (str): Path to JSON file with new lexemes.
    """
    print("Processing lexemes...")
    try:
        with open(input_file, "r", encoding="utf-8") as file:
            data = json.load(file)
            for lexeme in data:
                # Validate before further processing.
                if (
                    lexeme["language"] != lang
                    or lexeme["grammaticalCategory"] not in WIKI_TYPES
                ):
                    print(
                        f'Invalid entry {lang}, {lexeme["grammaticalCategory"]} in\n{lexeme}'
                    )
                    exit(-1)
                for form in lexeme["forms"]:
                    for feature in form["grammaticalFeatures"]:
                        if feature not in WIKI_TYPES:
                            print(f"Invalid feature: {feature} in\n{lexeme}, ")
                            exit(-1)
    except FileNotFoundError:
        print(f"Error: File '{input_file}' not found.")
    except json.JSONDecodeError:
        print(f"Error: Invalid JSON in file '{input}'.")
    print(f"Collected {len(data)} items.")
    return data


def load_wikidata(lang, wiki_file):
    """
    Loads lemma, language and grammatical category for easy lookup.

    Args:
        lang (str): Language to filter by.
        wiki_file (str): Path to wikidata JSON file.
    """
    print("Processing wikidata, it may take a while...")
    # Lemma-language-grammatical category set to allow for duplication of
    # different types.
    result = set()
    wiki_lang = tfsl.languages.get_first_lang(lang).item
    count = 0
    try:
        with open(wiki_file, "r", encoding="utf-8") as file:
            for line in file:
                # File is large, ~5GB, so we load line by line.
                # Wikidata is an array of objects, this removes commas, newlines...
                line = line.strip("[],\n")
                try:
                    data = json.loads(line)
                    if data["language"] != wiki_lang or lang not in data["lemmas"]:
                        continue
                    count += 1
                    lemma = data["lemmas"][lang]["value"]
                    cat = data["lexicalCategory"]
                    result.add(make_search_key(lemma, cat, lang))
                except json.JSONDecodeError:
                    # We can't do much except to skip.
                    continue
    except FileNotFoundError:
        print(f"Error: File '{wiki_file}' not found.")
    except PermissionError:
        print(f"Error: Permission denied to read file '{wiki_file}'.")
    print(f"Collected {count} items for {lang}.")
    return result


def filter_duplicates(lexemes, wikidata):
    """Filters out lexemes already in wikidata.

    Args:
        lexemes (list): List of loaded lexemes.
        wikidata (list): Wikidata dump.
    """
    count = 0
    new_lexemes = []
    for lexeme in lexemes:
        key = make_search_key(
            lexeme["lemma"],
            WIKI_TYPES[lexeme["grammaticalCategory"]],
            lexeme["language"],
        )
        if key not in wikidata:
            count += 1
            new_lexemes.append(lexeme)
    print(f"New lexemes: {count}, duplicates: {len(lexemes)-count}.")
    return new_lexemes


def build_tfsl_lexemes(new_lexemes):
    """For each new_lexeme builds a corresponding tfsl one.

    Args:
        new_lexemes (list): New lexemes from the input_file.
    """
    tfsl_lexemes = []
    for new_lexeme in new_lexemes:
        wiki_lang = tfsl.languages.get_first_lang(new_lexeme["language"])
        lexeme = tfsl.Lexeme(
            new_lexeme["lemma"] @ wiki_lang,
            wiki_lang,
            WIKI_TYPES[new_lexeme["grammaticalCategory"]],
        )

        # Adjectives don't have this, they carry gender in the form.
        if "grammaticalGender" in new_lexeme:
            lexeme += tfsl.Statement(
                WIKI_TYPES["gender"],
                tfsl.ItemValue(WIKI_TYPES[new_lexeme["grammaticalGender"]]),
            )

        for form in new_lexeme["forms"]:
            features = []
            for feature in form["grammaticalFeatures"]:
                features.append(WIKI_TYPES[feature])
            lexeme += tfsl.LexemeForm(form["value"] @ wiki_lang, features)

        if "descriptions" in new_lexeme:
            glosses = []
            for gloss in new_lexeme["descriptions"]:
                glosses.append(
                    gloss["value"] @ tfsl.languages.get_first_lang(gloss["language"])
                )
            lexeme += tfsl.LexemeSense(glosses)
        else:
            print(f'WARNING: Description missing for {new_lexeme["lemma"]}.')

        tfsl_lexemes.append(lexeme)

    return tfsl_lexemes


def upload_to_wikidata(lexemes, username, password, delay):
    """Uploads new lexemes to WIkidata.

    Args:
        lexemes (list): List of lexemes to upload.
        username (str): Wikidata user name.
        password (str): Wikidata password.
        delay (int): Delay between lexeme pushes (for rate limiting).
    """
    session = tfsl.WikibaseSession(username, password)

    print("Pushing new lexemes to Wikidata:")
    for lexeme in lexemes:
        print(f"{lexeme}")
        session.push(lexeme)
        time.sleep(delay / 1000)


def upload_data(username, password, lang, input_file, wiki_file, test_only, delay_ms):
    """
    Processes the input file and uploads to Wikidata given username and password.

    Args:
        username (str): The username for authentication.
        password (str): The password for authentication.
                lang (str): ISO 639 language code.
        input_file (str): The path to the input lexeme file.
                wiki_file (str): Path to Wikidata JSON file.
        test_only (bool): Upload if false, print out if true.
        delay_ms (int): Upload delay in ms, for rate limiting.
    """
    lexemes = load_lexemes(lang, input_file)
    wikidata = load_wikidata(lang, wiki_file)
    new_lexemes = filter_duplicates(lexemes, wikidata)
    tfsl_lexemes = build_tfsl_lexemes(new_lexemes)
    if test_only:
        # Dry run, doesn't print details like forms or statements.
        print(f"Lexeme:\n {tfsl_lexemes}")
    else:
        # Actual upload.
        upload_to_wikidata(tfsl_lexemes, username, password, delay_ms)


if __name__ == "__main__":
    parser = argparse.ArgumentParser(
        description="Upload data to Wikidata requiring authentication."
    )
    parser.add_argument("username", help="Username for authentication.")
    parser.add_argument("password", help="Password for authentication.")
    parser.add_argument("lang", help="ISO 639 language code, e.g. en, sr, es...")
    parser.add_argument("input_file", help="Path to the input file.")
    parser.add_argument("wiki_file", help="Path to the wikidata JSON file.")
    parser.add_argument(
        "--test", action="store_true", help="Test run, prints instead of upload."
    )
    parser.add_argument(
        "--delay",
        type=int,
        default=1000,
        help="Milliseconds between uploads for rate limiting.",
    )
    args = parser.parse_args()

    upload_data(
        args.username,
        args.password,
        args.lang,
        args.input_file,
        args.wiki_file,
        args.test,
        args.delay,
    )
