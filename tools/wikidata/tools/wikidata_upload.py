"""
Sample script for uploading lexicon data to Wikidata using tfsl library.

Follow these instructions to get tfsl dependency:
https://phabricator.wikimedia.org/source/tool-twofivesixlex/browse/main/

git clone https://gitlab.wikimedia.org/toolforge-repos/twofivesixlex.git tfsl
pip3 install requests
cp tfsl/config.ini.example tfsl/config.ini

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
    "direct": "Q1751855",
    "oblique": "Q1233197",
    "benefactive": "Q664905",
    "sociative": "Q3773161",
    "definite": "Q53997851",
    "indefinite": "Q53997857",
    "animate": "Q51927507",
    "inanimate": "Q51927539",
    "human": "Q27918551",
    # Polish (and other Slavic) plural agreement is virile (masculine personal) vs non-virile.
    # Use these for plural adjective/noun forms instead of animate/inanimate (which are a
    # masculine *singular* accusative concern and do not apply to plural agreement).
    "virile": "Q27918551",
    "nonvirile": "Q54152717",
    "nonhuman": "Q67372837",
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
    "verb": "Q24905",
    "numeral": "Q63116",
    "ordinal": "Q191780",
    "pronoun": "Q36224",
    "interrogative pronoun": "Q54310231",
    "possesive pronouns": "Q1502460",
    "adjective": "Q34698",
    "personal pronoun": "Q468801",
    # Word or form that substitutes for another word, broader scope than pronoun.
    "pro-form": "Q2006180",
    # Person
    "first person": "Q21714344",
    "second person": "Q51929049",
    "third person": "Q51929074",
    # Tense
    "present tense": "Q192613",
    "past tense": "Q1994301",
    "future tense": "Q501405",
    # Mood
    "imperative": "Q22716",
    "subjunctive": "Q473746",
    "jussive": "Q462367",
    # Additional moods used by Finnish (and other) verb paradigms.
    "indicative": "Q682111",
    "conditional": "Q625581",
    # Participle marker (combined with tense + voice to form the four Finnish
    # participles, e.g. past passive participle "avattu" = passive + past tense + participle).
    "participle": "Q814722",
    # Voice
    "active": "Q1317831",
    "passive": "Q1194697",
    # Verb form
    "infinitive": "Q179230",
    "gerund": "Q1923028",
    "verbal noun": "Q1350145",
    "active participle": "Q72249355",
    "passive participle": "Q72249544",
    # Comparison degree
    "positive": "Q3482678",
    "comparative": "Q14169499",
    "superlative": "Q1817208",
    # Sizeness
    "diminutive": "Q108709",
    # Gender (common gender for Dutch de-words)
    "common gender": "Q1305037",
    # Case
    "prepositional": "Q2114906",
    "partitive": "Q857325",
    "inessive": "Q282031",
    "elative": "Q394253",
    "illative": "Q474668",
    "adessive": "Q281954",
    "ablative": "Q156986",
    "allative": "Q655020",
    "essive": "Q148465",
    "translative": "Q950170",
    # Number (dual, for Arabic and similar)
    "dual": "Q110022",
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


def resolve_language_item(language):
    """Returns the Wikidata Q-id for a language given as a code ('bn') or Q-id ('Q11051')."""
    if language.startswith("Q"):
        return language
    return tfsl.languages.get_first_lang(language).item


def entry_lemma(lexeme, lang):
    """Returns a representative lemma string for an entry (for dedup/keys).

    Single-script entries use "lemma"; multi-script entries (e.g. Hindustani,
    which combines Hindi and Urdu under one lexeme) use "lemmas": a mapping of
    representation language code to value. The requested language's lemma is
    preferred, falling back to any available one.
    """
    if "lemma" in lexeme:
        return lexeme["lemma"]
    lemmas = lexeme["lemmas"]
    return lemmas.get(lang, next(iter(lemmas.values())))


def monolingual_holder(mapping):
    """Builds a tfsl MonolingualTextHolder from a {langCode: value} mapping.

    Used for multi-script lemmas and form representations (e.g. Hindustani's
    Devanagari "hi" and Urdu "ur" written under the same Q11051 lexeme).
    """
    texts = [value @ tfsl.languages.get_first_lang(code) for code, value in mapping.items()]
    return tfsl.MonolingualTextHolder(texts)


def load_lexemes(lang, input_file):
    """
    Loads lexemes from the input_file.

    Entries with "lemma" are treated as new lexemes to create.
    Entries with "lexemeId" (e.g. "L21189") instead of "lemma" are treated
    as updates to existing Wikidata lexemes.

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
                has_lemma = "lemma" in lexeme or "lemmas" in lexeme
                has_id = "lexemeId" in lexeme
                if not has_lemma and not has_id:
                    print(f"Entry must have either 'lemma'/'lemmas' or 'lexemeId':\n{lexeme}")
                    exit(-1)
                if (
                    resolve_language_item(lexeme["language"]) != resolve_language_item(lang)
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


def filter_duplicates(lexemes, wikidata, lang):
    """Filters out lexemes already in wikidata.

    Lexemes with a lexemeId (updates) are never filtered out.

    Args:
        lexemes (list): List of loaded lexemes.
        wikidata (list): Wikidata dump.
        lang (str): Requested language (code or Q-id), matching load_wikidata's keys.
    """
    count = 0
    new_lexemes = []
    for lexeme in lexemes:
        if "lexemeId" in lexeme:
            count += 1
            new_lexemes.append(lexeme)
            continue
        key = make_search_key(
            entry_lemma(lexeme, lang),
            WIKI_TYPES[lexeme["grammaticalCategory"]],
            lang,
        )
        if key not in wikidata:
            count += 1
            new_lexemes.append(lexeme)
    print(f"New lexemes: {count}, duplicates: {len(lexemes)-count}.")
    return new_lexemes


def build_tfsl_lexemes(new_lexemes):
    """For each new_lexeme builds a corresponding tfsl one to create.

    Only entries that create a new lexeme (those with "lemma"/"lemmas") are
    built here. Entries with "lexemeId" add forms to an existing lexeme and are
    handled by add_forms_to_existing via wbladdform instead, because a
    full-entity push would corrupt the existing lexeme's statements/senses and
    fail with param-illegal.

    Args:
        new_lexemes (list): New lexemes from the input_file.
    """
    tfsl_lexemes = []
    for new_lexeme in new_lexemes:
        if "lexemeId" in new_lexeme:
            # Handled by add_forms_to_existing (wbladdform); skip here.
            continue
        multi_script = "lemmas" in new_lexeme
        # Language object used to attach single-value forms/lemmas. For multi-script
        # entries (e.g. Hindustani, Q11051) use the first lemma's language object,
        # whose .item is the shared lexeme language.
        if multi_script:
            wiki_lang = tfsl.languages.get_first_lang(next(iter(new_lexeme["lemmas"])))
        else:
            wiki_lang = tfsl.languages.get_first_lang(new_lexeme["language"])

        lemmata = (
            monolingual_holder(new_lexeme["lemmas"])
            if multi_script
            else new_lexeme["lemma"] @ wiki_lang
        )
        lexeme = tfsl.Lexeme(
            lemmata,
            wiki_lang,
            WIKI_TYPES[new_lexeme["grammaticalCategory"]],
        )

        # Adjectives don't have this, they carry gender in the form.
        if "grammaticalGender" in new_lexeme:
            lexeme += tfsl.Statement(
                WIKI_TYPES["gender"],
                tfsl.ItemValue(WIKI_TYPES[new_lexeme["grammaticalGender"]]),
            )

        if "animacy" in new_lexeme:
            lexeme += tfsl.Statement(
                "P1552",
                tfsl.ItemValue(WIKI_TYPES[new_lexeme["animacy"]]),
            )

        for form in new_lexeme["forms"]:
            features = []
            for feature in form["grammaticalFeatures"]:
                features.append(WIKI_TYPES[feature])
            # Multi-script forms carry a "representations" map; single-script a "value".
            representation = (
                monolingual_holder(form["representations"])
                if "representations" in form
                else form["value"] @ wiki_lang
            )
            lexeme += tfsl.LexemeForm(representation, features)

        if "descriptions" in new_lexeme:
            glosses = []
            for gloss in new_lexeme["descriptions"]:
                glosses.append(
                    gloss["value"] @ tfsl.languages.get_first_lang(gloss["language"])
                )
            lexeme += tfsl.LexemeSense(glosses)
        elif "lemma" in new_lexeme or "lemmas" in new_lexeme:
            print(f"WARNING: Description missing for {entry_lemma(new_lexeme, None)}.")

        tfsl_lexemes.append(lexeme)

    return tfsl_lexemes


def upload_to_wikidata(lexemes, session, delay):
    """Pushes newly-created lexemes to Wikidata.

    Args:
        lexemes (list): List of tfsl Lexemes to create.
        session: An authenticated tfsl WikibaseSession.
        delay (int): Delay between lexeme pushes (for rate limiting).
    """
    print("Pushing new lexemes to Wikidata:")
    for lexeme in lexemes:
        print(f"{lexeme}")
        session.push(lexeme)
        time.sleep(delay / 1000)


def build_form_addition(new_lexeme, form):
    """Builds the ``data`` payload for a single ``wbladdform`` request.

    Args:
        new_lexeme (dict): The lexemeId entry the form belongs to (used for its
            language when the form carries a single-script "value").
        form (dict): A single form with either "value" (single-script) or
            "representations" (multi-script) plus "grammaticalFeatures".
    """
    if "representations" in form:
        representations = {
            code: {"language": code, "value": value}
            for code, value in form["representations"].items()
        }
    else:
        lang = new_lexeme["language"]
        representations = {lang: {"language": lang, "value": form["value"]}}
    features = [WIKI_TYPES[feature] for feature in form["grammaticalFeatures"]]
    return {"representations": representations, "grammaticalFeatures": features}


def add_forms_to_existing(session, lexeme_entries, test_only, delay):
    """Adds forms to existing lexemes via the Wikibase Lexeme ``wbladdform`` action.

    Unlike ``WikibaseSession.push`` (a full-entity ``wbeditentity``), ``wbladdform``
    appends a single form without re-serializing the lexeme's existing statements,
    senses, or other forms. A full-entity re-push of a lexeme that already carries
    statements/senses corrupts them on tfsl's round-trip and the API rejects the
    edit with ``param-illegal``; ``wbladdform`` sidesteps that entirely.

    Args:
        session: An authenticated tfsl WikibaseSession (None when test_only).
        lexeme_entries (list): Entries with a "lexemeId" and "forms" to add.
        test_only (bool): If true, print the payloads instead of posting.
        delay (int): Delay between form additions (for rate limiting).
    """
    for entry in lexeme_entries:
        lexeme_id = entry["lexemeId"]
        print(f"Adding {len(entry['forms'])} forms to existing lexeme {lexeme_id}.")
        for form in entry["forms"]:
            data = build_form_addition(entry, form)
            if test_only:
                print(f"  wbladdform {lexeme_id}: {json.dumps(data, ensure_ascii=False)}")
                continue
            params = {
                "action": "wbladdform",
                "lexemeId": lexeme_id,
                "data": json.dumps(data),
                "token": "__AUTO__",
                "format": "json",
            }
            session.post(params)
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

    to_create = [lx for lx in lexemes if "lexemeId" not in lx]
    to_add_forms = [lx for lx in lexemes if "lexemeId" in lx]

    # The Wikidata dump is only needed to dedupe new-lexeme creations; adding
    # forms to existing lexemes (lexemeId entries) never consults it. Skip the
    # multi-GB read entirely when there is nothing to create.
    if to_create:
        wikidata = load_wikidata(lang, wiki_file)
        to_create = filter_duplicates(to_create, wikidata, lang)
    tfsl_lexemes = build_tfsl_lexemes(to_create)

    if test_only:
        # Dry run, doesn't print details like forms or statements.
        print(f"Lexeme:\n {tfsl_lexemes}")
        add_forms_to_existing(None, to_add_forms, True, delay_ms)
    else:
        # Actual upload. One authenticated session is shared by both paths:
        # new lexemes are created with wbeditentity (push), existing lexemes
        # get their forms appended with wbladdform.
        session = tfsl.WikibaseSession(username, password)
        if tfsl_lexemes:
            upload_to_wikidata(tfsl_lexemes, session, delay_ms)
        if to_add_forms:
            add_forms_to_existing(session, to_add_forms, False, delay_ms)


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
