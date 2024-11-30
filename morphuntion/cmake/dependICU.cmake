#
# Copyright 2024-2024 Apple Inc. All rights reserved.
#
# Add -DICU_DEBUG=ON when calling cmake if you want to see all of the variable values.
get_version_from_makefile(ICU_VERSION_MINIMUM)

if(NOT DEFINED ICU_ROOT)
    get_option_from_makefile(ICU_ROOT)
endif()
find_package(ICU COMPONENTS uc i18n)

if(NOT ICU_FOUND)
    # TODO Download and/or build ICU.
    message(FATAL_ERROR "ICU not found. Specify the location with ICU_ROOT. ICU_ROOT=${ICU_ROOT}")
endif()

if(ICU_VERSION VERSION_LESS ICU_VERSION_MINIMUM)
    # TODO Download and/or build ICU.
    message(FATAL_ERROR "ICU version found ${ICU_VERSION}. Version ${ICU_VERSION_MINIMUM} is the minimum acceptable version.")
endif()

get_filename_component(ICU_LIB_DIRECTORY ${ICU_UC_LIBRARIES} DIRECTORY)
