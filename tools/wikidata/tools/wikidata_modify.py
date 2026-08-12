"""
Script for modifying grammatical features on existing Wikidata lexeme forms.

Unlike wikidata_upload.py (which creates new lexemes or adds new forms), this
tool edits the grammatical features of forms that already exist on a lexeme,
adding and/or removing feature Q-ids. It is driven entirely by lexeme ids and
does not read a Wikidata dump.

Follow these instructions to get tfsl dependency:
https://phabricator.wikimedia.org/source/tool-twofivesixlex/browse/main/

git clone https://gitlab.wikimedia.org/toolforge-repos/twofivesixlex.git tfsl
pip3 install requests
cp tfsl/config.ini.example tfsl/config.ini

Code expects tfsl checkout location next to inflection repository.

Input file is a JSON array of per-lexeme modifications, e.g.:

[
  {
    "lexemeId": "L1493026",
    "modifications": [
      { "formId": "L1493026-F9", "add": ["Q65048529"] },
      { "form": "ai-udviklingen", "add": ["Q65048529"], "remove": [] }
    ]
  }
]

A form is identified either by "formId" (preferred, unambiguous) or by "form"
(the surface string, matched against the representation in the requested
language). "add" and "remove" hold raw feature Q-ids.

Run with --self-check to exercise the pure logic without any network access.
"""

import argparse
import json
import os
import re
import sys
import time

LEXEME_ID_RE = re.compile(r"^L\d+$")
FORM_ID_RE = re.compile(r"^L\d+-F\d+$")
QID_RE = re.compile(r"^Q\d+$")


def _import_tfsl():
    """Imports the tfsl library, expected next to the inflection repository.

    Deferred so that pure-logic paths (validation, --self-check) work without a
    tfsl checkout or network access.
    """
    sys.path.append(os.path.join(os.path.dirname(__file__), "..", "..", "..", "tfsl"))
    import tfsl

    return tfsl


def validate_modifications(data):
    """Validates the structure of a loaded modifications document.

    Args:
        data: Parsed JSON (expected: list of per-lexeme modification entries).

    Returns:
        The validated data.

    Raises:
        ValueError: if the document does not match the expected schema.
    """
    if not isinstance(data, list):
        raise ValueError("Top-level JSON must be an array of lexeme entries.")
    for entry in data:
        lexeme_id = entry.get("lexemeId")
        if not isinstance(lexeme_id, str) or not LEXEME_ID_RE.match(lexeme_id):
            raise ValueError(f"Invalid or missing 'lexemeId' in entry:\n{entry}")
        mods = entry.get("modifications")
        if not isinstance(mods, list) or not mods:
            raise ValueError(f"'modifications' must be a non-empty array in:\n{entry}")
        for mod in mods:
            has_form_id = "formId" in mod
            has_form = "form" in mod
            if has_form_id == has_form:
                raise ValueError(
                    f"Each modification needs exactly one of 'formId' or 'form':\n{mod}"
                )
            if has_form_id and not FORM_ID_RE.match(str(mod["formId"])):
                raise ValueError(f"Invalid 'formId' in:\n{mod}")
            if has_form and not isinstance(mod["form"], str):
                raise ValueError(f"'form' must be a string in:\n{mod}")
            add = mod.get("add", [])
            remove = mod.get("remove", [])
            if not isinstance(add, list) or not isinstance(remove, list):
                raise ValueError(f"'add' and 'remove' must be arrays in:\n{mod}")
            if not add and not remove:
                raise ValueError(
                    f"Modification must have a non-empty 'add' or 'remove':\n{mod}"
                )
            for qid in add + remove:
                if not isinstance(qid, str) or not QID_RE.match(qid):
                    raise ValueError(f"Invalid feature Q-id '{qid}' in:\n{mod}")
    return data


def load_modifications(input_file):
    """Loads and validates modifications from the input file.

    Args:
        input_file (str): Path to the JSON modifications file.
    """
    print("Processing modifications...")
    with open(input_file, "r", encoding="utf-8") as file:
        data = json.load(file)
    validate_modifications(data)
    count = sum(len(entry["modifications"]) for entry in data)
    print(f"Collected {count} form modifications across {len(data)} lexemes.")
    return data


def compute_new_features(current, add, remove):
    """Computes the resulting feature set and any no-op warnings.

    Args:
        current (iterable): Current feature Q-ids on the form.
        add (list): Feature Q-ids to add.
        remove (list): Feature Q-ids to remove.

    Returns:
        Tuple of (new feature set, list of warning strings).
    """
    current_set = set(current)
    warnings = []
    for qid in add:
        if qid in current_set:
            warnings.append(f"feature {qid} already present (add is a no-op)")
    for qid in remove:
        if qid not in current_set:
            warnings.append(f"feature {qid} not present (remove is a no-op)")
    new_set = (current_set | set(add)) - set(remove)
    return new_set, warnings


def _form_rep_value(form, lang):
    """Returns the form's representation value for the given language, or None."""
    reps = form.__jsonout__().get("representations", {})
    if lang in reps:
        return reps[lang]["value"]
    return None


def locate_form(lexeme, mod, lang):
    """Finds the single form on a lexeme targeted by a modification.

    Args:
        lexeme: An editable tfsl Lexeme.
        mod (dict): A single modification (with 'formId' or 'form').
        lang (str): Language code used for surface-string matching.

    Returns:
        The matching tfsl LexemeForm.

    Raises:
        ValueError: if zero or multiple forms match.
    """
    if "formId" in mod:
        matches = [f for f in lexeme.forms if getattr(f, "id", None) == mod["formId"]]
        descriptor = f"formId '{mod['formId']}'"
    else:
        matches = [f for f in lexeme.forms if _form_rep_value(f, lang) == mod["form"]]
        descriptor = f"form '{mod['form']}' ({lang})"
    if not matches:
        raise ValueError(f"No form matched {descriptor} on {lexeme.id}.")
    if len(matches) > 1:
        raise ValueError(f"{len(matches)} forms matched {descriptor} on {lexeme.id}.")
    return matches[0]


def apply_to_lexeme(lexeme, mods, lang):
    """Applies modifications to an editable lexeme in place.

    Mutates each target form's feature set directly, preserving the form id so
    the eventual push edits the existing form rather than adding a new one.

    Args:
        lexeme: An editable tfsl Lexeme.
        mods (list): Modifications for this lexeme.
        lang (str): Language code for surface-string matching.

    Returns:
        List of (form_id, surface, before_features, after_features) tuples.
    """
    changes = []
    for mod in mods:
        form = locate_form(lexeme, mod, lang)
        before = set(form.features)
        new_features, warnings = compute_new_features(
            before, mod.get("add", []), mod.get("remove", [])
        )
        surface = _form_rep_value(form, lang)
        for warning in warnings:
            print(f"  WARNING ({getattr(form, 'id', '?')} '{surface}'): {warning}")
        form.features.clear()
        form.features.update(new_features)
        changes.append(
            (getattr(form, "id", None), surface, sorted(before), sorted(new_features))
        )
    return changes


def _print_changes(lexeme_id, changes):
    """Prints a per-form before -> after feature diff."""
    print(f"{lexeme_id}:")
    for form_id, surface, before, after in changes:
        print(f"  {form_id} '{surface}'")
        print(f"    before: {before}")
        print(f"    after:  {after}")


def modify_data(username, password, lang, input_file, test_only, delay_ms):
    """Processes the input file and applies form modifications to Wikidata.

    Args:
        username (str): Wikidata username.
        password (str): Wikidata password.
        lang (str): Language code used for surface-string matching.
        input_file (str): Path to the JSON modifications file.
        test_only (bool): If true, print diffs instead of pushing.
        delay_ms (int): Delay between pushes in milliseconds, for rate limiting.
    """
    tfsl = _import_tfsl()
    data = load_modifications(input_file)

    session = None
    if not test_only:
        session = tfsl.WikibaseSession(username, password)

    for entry in data:
        lexeme_id = entry["lexemeId"]
        lexeme = tfsl.L(lexeme_id).to_editable()
        changes = apply_to_lexeme(lexeme, entry["modifications"], lang)
        if test_only:
            _print_changes(lexeme_id, changes)
        else:
            print(f"Pushing {lexeme_id} ({len(changes)} forms modified).")
            session.push(lexeme)
            time.sleep(delay_ms / 1000)


def self_check():
    """Exercises the pure logic without any network access."""
    failures = 0

    def check(name, condition):
        nonlocal failures
        status = "ok" if condition else "FAIL"
        if not condition:
            failures += 1
        print(f"  [{status}] {name}")

    # compute_new_features
    new, warns = compute_new_features(["Q1", "Q2"], ["Q3"], ["Q2"])
    check("add and remove", new == {"Q1", "Q3"} and not warns)
    new, warns = compute_new_features(["Q1"], ["Q1"], [])
    check("add no-op warns", new == {"Q1"} and len(warns) == 1)
    new, warns = compute_new_features(["Q1"], [], ["Q9"])
    check("remove no-op warns", new == {"Q1"} and len(warns) == 1)

    # validate_modifications: valid
    ok = [
        {
            "lexemeId": "L1",
            "modifications": [{"formId": "L1-F2", "add": ["Q65048529"]}],
        }
    ]
    try:
        validate_modifications(ok)
        check("valid document accepted", True)
    except ValueError:
        check("valid document accepted", False)

    # validate_modifications: rejections
    bad_cases = [
        {"not": "a list"},
        [{"lexemeId": "X", "modifications": [{"formId": "L1-F2", "add": ["Q1"]}]}],
        [{"lexemeId": "L1", "modifications": []}],
        [{"lexemeId": "L1", "modifications": [{"add": ["Q1"]}]}],
        [{"lexemeId": "L1", "modifications": [{"formId": "L1-F2", "form": "x", "add": ["Q1"]}]}],
        [{"lexemeId": "L1", "modifications": [{"formId": "L1-F2"}]}],
        [{"lexemeId": "L1", "modifications": [{"formId": "L1-F2", "add": ["bad"]}]}],
    ]
    rejected = 0
    for case in bad_cases:
        try:
            validate_modifications(case)
        except ValueError:
            rejected += 1
    check("invalid documents rejected", rejected == len(bad_cases))

    print("Self-check:", "PASSED" if failures == 0 else f"{failures} FAILURE(S)")
    return failures == 0


if __name__ == "__main__":
    parser = argparse.ArgumentParser(
        description="Modify grammatical features on existing Wikidata lexeme forms."
    )
    parser.add_argument("username", nargs="?", help="Username for authentication.")
    parser.add_argument("password", nargs="?", help="Password for authentication.")
    parser.add_argument("lang", nargs="?", help="ISO 639 language code, e.g. da, en.")
    parser.add_argument("input_file", nargs="?", help="Path to the input JSON file.")
    parser.add_argument(
        "--test", action="store_true", help="Test run, prints diffs instead of pushing."
    )
    parser.add_argument(
        "--delay",
        type=int,
        default=1000,
        help="Milliseconds between pushes for rate limiting.",
    )
    parser.add_argument(
        "--self-check",
        action="store_true",
        help="Run pure-logic self-checks and exit (no network).",
    )
    args = parser.parse_args()

    if args.self_check:
        sys.exit(0 if self_check() else 1)

    if not all([args.username, args.password, args.lang, args.input_file]):
        parser.error("username, password, lang, and input_file are required.")

    modify_data(
        args.username,
        args.password,
        args.lang,
        args.input_file,
        args.test,
        args.delay,
    )
