#
# Copyright 2024-2024 Apple Inc. All rights reserved.
#
ORIGINAL_SRCROOT = $(dir $(realpath $(firstword $(MAKEFILE_LIST))))

-include $(ORIGINAL_SRCROOT)/../options.mk

print-% : ; @echo $($*)