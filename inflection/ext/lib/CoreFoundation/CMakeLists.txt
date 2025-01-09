#
# Copyright 2024-2024 Apple Inc. All rights reserved.
#
cmake_minimum_required(VERSION 3.24)
get_version_from_makefile(CF_VERSION)
inflection_debug_vars(CF_VERSION)

if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")

if(DEFINED ENV{CORE_FOUNDATION_ROOT})
    message("Building with local copy of core foundation. CORE_FOUNDATION_ROOT: " $ENV{CORE_FOUNDATION_ROOT})
    set_directory_properties(PROPERTIES EP_BASE ${CMAKE_CURRENT_BINARY_DIR})
    add_library(CoreFoundation INTERFACE IMPORTED GLOBAL)
    set_target_properties(CoreFoundation PROPERTIES IMPORTED_LIBNAME CoreFoundation)
    add_custom_target(CoreFoundation_dev)
    target_include_directories(CoreFoundation INTERFACE $ENV{CORE_FOUNDATION_ROOT}/usr/local/include)
    target_link_directories(CoreFoundation INTERFACE $ENV{CORE_FOUNDATION_ROOT}/usr/local/lib)
else()
    set_directory_properties(PROPERTIES EP_BASE ${CMAKE_CURRENT_BINARY_DIR})
    add_custom_target(CoreFoundation_artifacts)

    # Core foundation artifacts
    if ("${CMAKE_SYSTEM_PROCESSOR}" STREQUAL "aarch64")
        message("CMAKE_SYSTEM_PROCESSOR: ${CMAKE_SYSTEM_PROCESSOR}. Using aarch64 dependency for core-foundation")
        set(SWIFT_ARCH "-aarch64")
    else()
        message("CMAKE_SYSTEM_PROCESSOR: ${CMAKE_SYSTEM_PROCESSOR}. Using x86 dependency for core-foundation")
        set(SWIFT_ARCH "")
    endif() #"${CMAKE_SYSTEM_PROCESSOR}" STREQUAL "aarch64"
    set(BASE_NAME "swift-${CF_VERSION}-RELEASE-ubi9${SWIFT_ARCH}")
    set(ARTIFACT "${BASE_NAME}.tar.gz")
    ExternalProject_Add(CoreFoundation_dev
        URL https://download.swift.org/swift-${CF_VERSION}-release/ubi9${SWIFT_ARCH}/swift-${CF_VERSION}-RELEASE/${ARTIFACT}
        BUILD_COMMAND tar xzof <DOWNLOAD_DIR>/${ARTIFACT} -C <SOURCE_DIR> --strip-components=2 --exclude=lib/swift/clang --exclude=lib/swift/linux/lib_InternalSwiftScan.so --exclude=lib/swift_static/clang ${BASE_NAME}/usr/lib ${BASE_NAME}/usr/include
        CONFIGURE_COMMAND "" INSTALL_COMMAND "" LOG_DOWNLOAD ON EXCLUDE_FROM_ALL TRUE DOWNLOAD_NO_EXTRACT TRUE
    )

#    Adding CoreFoundation library as imported
    add_library(CoreFoundation INTERFACE IMPORTED GLOBAL)
    set_target_properties(CoreFoundation PROPERTIES IMPORTED_LIBNAME Foundation)

#    Setting include path
    ExternalProject_Get_Property(CoreFoundation_dev SOURCE_DIR)
    file(MAKE_DIRECTORY ${SOURCE_DIR}/lib/swift)
    target_include_directories(CoreFoundation INTERFACE ${SOURCE_DIR}/lib/swift)
    file(MAKE_DIRECTORY ${SOURCE_DIR}/include)
    target_include_directories(CoreFoundation INTERFACE ${SOURCE_DIR}/include)

#    Setting linker path
    file(MAKE_DIRECTORY ${SOURCE_DIR}/lib/swift/linux)
    target_link_directories(CoreFoundation INTERFACE ${SOURCE_DIR}/lib/swift/linux)

    add_dependencies(CoreFoundation CoreFoundation_dev)
endif() # (DEFINED ENV{CORE_FOUNDATION_ROOT})

else()
    find_library(COREFOUNDATION_FRAMEWORK CoreFoundation)
    inflection_debug_vars(COREFOUNDATION_FRAMEWORK)

    add_library(CoreFoundation SHARED IMPORTED GLOBAL)
    set_target_properties(CoreFoundation PROPERTIES IMPORTED_LOCATION ${COREFOUNDATION_FRAMEWORK}/CoreFoundation.tbd)
endif()
