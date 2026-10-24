#!/usr/bin/env python3
#
# Copyright 2026 Unicode Incorporated and others. All rights reserved.
#
"""Generates icu_filters.json from icu_filters.json.in and supported-locales.properties."""

import argparse
import json
from pathlib import Path

REPO_ROOT = Path(__file__).resolve().parents[2]
DEFAULT_TEMPLATE = REPO_ROOT / "icu_filters.json.in"
DEFAULT_PROPERTIES = (
    REPO_ROOT
    / "resources"
    / "org"
    / "unicode"
    / "inflection"
    / "locale"
    / "supported-locales.properties"
)
DEFAULT_OUTPUT = REPO_ROOT / "icu_filters.json"
LOCALE_GROUP_PREFIX = "locale.group."


def parse_supported_locales(properties_path: Path) -> list[str]:
    locales = []
    with properties_path.open(encoding="utf-8") as f:
        for line in f:
            line = line.strip()
            if not line or line.startswith(("#", "!")) or "=" not in line:
                continue
            key, _ = line.split("=", 1)
            key = key.strip()
            if key.startswith(LOCALE_GROUP_PREFIX):
                locale = key[len(LOCALE_GROUP_PREFIX) :].strip()
                if locale:
                    locales.append(locale)
    if not locales:
        raise ValueError(f"No '{LOCALE_GROUP_PREFIX}*' entries found in {properties_path}")
    return sorted(set(locales))


def main() -> None:
    parser = argparse.ArgumentParser(
        description="Generate icu_filters.json from icu_filters.json.in and supported-locales.properties."
    )
    parser.add_argument(
        "-o",
        "--output",
        type=Path,
        default=DEFAULT_OUTPUT,
        help="Output path for generated icu_filters.json (default: %(default)s)",
    )
    parser.add_argument(
        "--template",
        type=Path,
        default=DEFAULT_TEMPLATE,
        help="Path to icu_filters.json.in template (default: %(default)s)",
    )
    parser.add_argument(
        "--locales",
        type=Path,
        default=DEFAULT_PROPERTIES,
        help="Path to supported-locales.properties (default: %(default)s)",
    )
    args = parser.parse_args()

    with args.template.open(encoding="utf-8") as f:
        config = json.load(f)

    config["localeFilter"]["includelist"] = parse_supported_locales(args.locales)

    args.output.parent.mkdir(parents=True, exist_ok=True)
    with args.output.open("w", encoding="utf-8") as f:
        json.dump(config, f, indent=2)
        f.write("\n")


if __name__ == "__main__":
    main()
