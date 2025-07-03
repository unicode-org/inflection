/*
 * Copyright 2024 Unicode Incorporated and others. All rights reserved.
 */
#pragma once
#include <inflection/api.h>
#include <unicode/utypes.h>
#include <inflection/dialog/SemanticFeatureModel.h>

INFLECTION_CBEGIN
/**
 * A constraint. The valid values for the key and value are typically
 * valid for only specific languages.
 */
typedef struct IDDisplayValue_Constraint {
    /**
     * The semantic feature name. This should be a valid name from
     * {@link inflection::lang::features::LanguageGrammarFeatures_GrammarCategory} or
     * {@link inflection::lang::features::LanguageGrammarFeatures_GrammarFeatures}. In all
     * languages, there is also a special key for the speak part
     * of a SpeakableString named "speak". When that is provided,
     * the associated value will be returned as a part of the
     * SpeakableString. This string must be null-terminated.
     */
    const char16_t* name;
    /**
     * The semantic feature name. This should be a valid value from
     * {@link inflection::lang::features::LanguageGrammarFeatures_GrammarCategory} or
     * {@link inflection::lang::features::LanguageGrammarFeatures_GrammarFeatures}. Some constraints
     * are bounded, and some are unbounded. Typically, grammar
     * categories are bounded. The grammar features may or may not be
     * bounded. This string must be null-terminated.
     */
    const char16_t* value;
} IDDisplayValue_Constraint;

/**
 * @brief Represents a display value with constraint values.
 * @details The display value is valid for the constraints provided as semantic
 * features. Typically, the order of display values is important for selecting
 * the string to display. For example, if a plural constraint is requested,
 * and no other constraints are provided for matching, then the first plural
 * display value will be used.
 */
typedef struct IDDisplayValue {
    /**
     * The value to display.
     */
    const char16_t* displayString;
    /**
     * The constraints for this displayString.
     */
    IDDisplayValue_Constraint* constraintMap;
    /**
     * The length of displayString. If it's -1, the string is null terminated.
     */
    int32_t displayStringLen;
    /**
     * The length of constraintMap. If it's -1, the key in the last element
     * in the array must be null to note the end of the array.
     */
    int32_t constraintMapLen;
} IDDisplayValue;
INFLECTION_CEND

/**
 * Validates the constraints for a model.
 * @param model The model to validate against.
 * @param constraints An array of constraints to validate.
 * @param constraintsLen The length of constraints.
 * @param status Must be a valid pointer to an error code value,
 *        which must not indicate a failure before the function call.
 *        This is set to a failure when a failure has occurred during execution.
 * @return true when all feature names are known to the model.
 *         If the feature values are bounded, then the value is validated as being a known value.
 */
INFLECTION_CAPI bool idv_isValidConstraints(const IDSemanticFeatureModel* model,
                                            const IDDisplayValue_Constraint* constraints,
                                            int32_t constraintsLen,
                                            UErrorCode* status);

/**
 * Validates the constraints of a DisplayValue.
 * @param model The model to validate against.
 * @param displayValue The single DisplayValue to validate.
 * @param status Must be a valid pointer to an error code value,
 *        which must not indicate a failure before the function call.
 *        This is set to a failure when a failure has occurred during execution.
 * @return true when all feature names are known to the model.
 *         If the feature values are bounded, then the value is validated as being a known value.
 */
INFLECTION_CAPI bool idv_isValidDisplayValue(const IDSemanticFeatureModel* model, const IDDisplayValue* displayValue, UErrorCode* status);
