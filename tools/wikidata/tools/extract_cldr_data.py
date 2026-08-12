"""
Extracts data from CLDR-JSON repository, e.g. nouns like month or day names.
Script either creates a new inflection file, or appends data to existing one.

Part of Speech, Gender, Case, Number. Animacy

Run script from data folder.

Before running the script clone cldr-json repository:

gh repo clone unicode-org/cldr-json

and install jsonpath-ng package:

pip install jsonpath-ng
"""

import argparse
import json
import os

from jsonpath_ng import jsonpath, parse


def load_json(filename):
    """Loads JSON data from the specified file.

    Args:
        filename: The name of the JSON file.

    Returns:
        The parsed JSON data.
    """

    try:
        with open(filename, 'r', encoding='utf-8') as file:
            return json.load(file)
    except FileNotFoundError:
        print(f"Error: File '{filename}' not found.")
        return None


def write_to_lexicon(output_file, language, json_data):
    """Extracts specified data from cldr-json file
       and writes it to the lexicon file.

    Args:
        output_file: name of the lexicon.
        language: cldr-json file language.
        json_data: parsed cldr-json data.
    """
    MONTH_NAMES_EXPRESSION = parse('main..dates.calendars.gregorian.months.format.wide.*')
    DAY_NAMES_EXPRESSION = parse('main..dates.calendars.gregorian.days.format.wide.*')
    EXPRESSIONS = [MONTH_NAMES_EXPRESSION, DAY_NAMES_EXPRESSION]

    results = []
    for expression in EXPRESSIONS:
        match = expression.find(json_data)
        for m in match:
            results.append(m.value + ': noun masculine nominative singular inanimate\n')

    full_filename = os.path.join(language, output_file)
    try:
        os.makedirs(os.path.dirname(full_filename), exist_ok=True)
        with open(full_filename, 'a', encoding='utf-8') as file:
            file.writelines(results)
    except FileNotFoundError:
        print(f"Error: file '{output_file}' can't be created.")


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description='Load and process CLDR-JSON files for given languages.')
    parser.add_argument('--cldr_root', help='The path to CLDR-JSON data.', default='../../cldr-json/cldr-json/cldr-dates-full/main')
    parser.add_argument('--input_file', help='Data file to read from, e.g. ca-gregorian.json.', default='ca-gregorian.json')
    parser.add_argument('--output_file', help='Data file to create/append to, e.g. lexicon.txt.', default='lexicon.txt')
    parser.add_argument('--language_list', nargs='+', default=['sr', 'en', 'de', 'es', 'fr'])
    args = parser.parse_args()

    for language in args.language_list:
        full_filename = os.path.join(args.cldr_root, language, args.input_file)
        data = load_json(full_filename)

        if data:
            write_to_lexicon(args.output_file, language, data)
