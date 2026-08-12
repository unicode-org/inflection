#
# Copyright 2019-2024 Apple Inc. All rights reserved.
#
CPPFLAGS := $(CPPFLAGS) $(INCLUDES)
CFLAGS := $(CFLAGS) -g -pipe
CXXFLAGS := $(CFLAGS) -std=c++17
OBJROOT ?= ../../build
OBJROOT_BINDIR ?= $(OBJROOT)/bin
SRCROOT ?= ../..
DEST_DIR := ../../src/icu4cxx
SOURCE_HEADERS := $(shell fgrep -Rl "class INFLECTION_CLASS_API" $(SRCROOT)/src \
	| fgrep -v /exception/ \
	| fgrep -v /CommonConceptFactoryProvider.hpp \
	| fgrep -v /Object.hpp \
	| fgrep -v /ULocale.hpp \
	)
TARGET_HEADERS := $(SOURCE_HEADERS:%.hpp=%.h)

TARGET_TOOL := $(OBJROOT_BINDIR)/generateInflectionWrapper

$(TARGET_TOOL): generateInflectionWrapper.cpp
	@mkdir -p $(OBJROOT_BINDIR)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -o $@ $<

$(TARGET_HEADERS): $(TARGET_TOOL)

%.h:%.hpp
	$(TARGET_TOOL) $<

$(SRC_HEADERS): $(TARGET_TOOL)

clean:
	rm -rf $(OBJROOT_BINDIR) $(TARGET_HEADERS)

headers: $(TARGET_HEADERS) | $(TARGET_TOOL)

all: $(TARGET_TOOL)
.PHONY: clean target headers
