#!/bin/sh
#
# Copyright 2023-2024 Apple Inc. All rights reserved.
#
git tag -l | grep "^Inflection-[0-9\.]*$" | sed "s/Inflection-//" | grep "^[0-9.]*$" | sort --version-sort | tail -1
