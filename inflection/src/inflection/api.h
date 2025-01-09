/*
 * Copyright 2018-2024 Apple Inc. All rights reserved.
 */
#pragma once

/**
 * Version of Inflection
 */
#include <inflection/version.h>

#if !INFLECTION_STATIC
#define INFLECTION_EXPORT_API __attribute__((visibility("default")))
#else
#define INFLECTION_EXPORT_API
#endif

/**
 * The header and symbols of the class are exported from the library.
 */
#define INFLECTION_CLASS_API INFLECTION_EXPORT_API
/**
 * The symbols of the class are exported from the library, but the header is not exported.
 */
#define INFLECTION_INTERNAL_API INFLECTION_EXPORT_API
/**
 * Defines an enumeration.
 */
#define INFLECTION_ENUM(_type, _name) enum _name : _type; typedef enum _name _name; enum _name : _type

/**
 * The C symbol is exported from the library.
 */
#ifdef __cplusplus
#define INFLECTION_CAPI extern "C" INFLECTION_EXPORT_API
#define INFLECTION_CTYPE(name) extern "C" {typedef struct name name;}
#define INFLECTION_CBEGIN extern "C" {
#define INFLECTION_CEND }
#else
#define INFLECTION_CAPI extern INFLECTION_EXPORT_API
#define INFLECTION_CTYPE(name) typedef struct name name;
#define INFLECTION_CBEGIN
#define INFLECTION_CEND
#include <stdint.h>

#if (defined(__clang__) && defined(__CHAR16_TYPE__) && !defined(char16_t))
typedef __CHAR16_TYPE__ char16_t;
#else
// This is typically gcc
#include <uchar.h>
#endif

#endif
