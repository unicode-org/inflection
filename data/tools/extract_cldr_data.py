"""
Extracts data from CLDR-JSON repository, e.g. nouns like month or day names.
Script either creates a new inflection file, or appends data to existing one.
Run script from data folder.

Before running the script clone cldr-json repository:

gh repo clone unicode-org/cldr-json
"""

import argparse
import json
import os

def load_json(filename):
    """Loads JSON data from the specified file.

    Args:
        filename: The name of the JSON file.

    Returns:
        The parsed JSON data.
    """

    try:
        with open(filename, 'r') as file:
            return json.load(file)
    except FileNotFoundError:
        print(f"Error: File '{filename}' not found.")
        return None

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
            # Do something with the loaded JSON data
            print(data) 
