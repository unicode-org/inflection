#
# Copyright 2016-2024 Apple Inc. All rights reserved.
#
CPPFLAGS := $(CPPFLAGS) $(INCLUDES)
CFLAGS := $(CFLAGS) -g -pipe
CXXFLAGS := $(CFLAGS) -std=c++14
OBJROOT ?= ../../build
OBJROOT_BINDIR ?= $(OBJROOT)/bin
ICU_INCLUDE_DIR := $(SDKROOT)/usr/local/include/unicode
ICU_PUBLIC_INCLUDE_DIR := $(SDKROOT)/usr/include/unicode
DEST_DIR := ../../src/icu4cxx
SRC_HEADERS := \
    $(DEST_DIR)/BreakIterator.hpp \
    $(DEST_DIR)/Normalizer2.hpp \
    $(DEST_DIR)/NumberFormat.hpp \
    $(DEST_DIR)/RegularExpression.hpp \
    $(DEST_DIR)/UnicodeSet.hpp \

TARGET := $(OBJROOT_BINDIR)/generateICUWrapper

define generateHeader
$(shell echo $(notdir $(basename $1)) $2 $1)
endef

$(TARGET): generateICUWrapper.cpp
	mkdir -p $(OBJROOT_BINDIR)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -o $@ $<

$(DEST_DIR)/BreakIterator.hpp: $(ICU_INCLUDE_DIR)/ubrk.h
	$(TARGET) $(call generateHeader,$@,$<)
$(DEST_DIR)/Normalizer2.hpp: $(ICU_INCLUDE_DIR)/unorm2.h
	$(TARGET) $(call generateHeader,$@,$<)
$(DEST_DIR)/NumberFormat.hpp: $(ICU_INCLUDE_DIR)/unum.h
	$(TARGET) $(call generateHeader,$@,$<)
$(DEST_DIR)/RegularExpression.hpp: $(ICU_PUBLIC_INCLUDE_DIR)/uregex.h
	$(TARGET) $(call generateHeader,$@,$<)
$(DEST_DIR)/UnicodeSet.hpp: $(ICU_INCLUDE_DIR)/uset.h
	$(TARGET) $(call generateHeader,$@,$<)

$(DEST_DIR)/%.hpp: $(ICU_INCLUDE_DIR)/%.h
	$(TARGET) $(call generateHeader,$@,$<)

$(SRC_HEADERS): $(TARGET)

clean:
	rm -rf $(OBJROOT_BINDIR)

headers: $(SRC_HEADERS)

all: $(TARGET)
.PHONY: clean target headers
