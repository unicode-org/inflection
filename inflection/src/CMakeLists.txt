#
# Copyright 2021-2024 Apple Inc. All rights reserved.
#
set(inflection_compile_definitions INFLECTION_DATA_ROOT_DIR=\"${CMAKE_INSTALL_FULL_DATAROOTDIR}\" ENABLE_ALL_FEATURES=1)

# inflection and icu4cxx definitions
set(ICU4CXX_INCLUDE_PATH ${CMAKE_CURRENT_SOURCE_DIR})
set(INFLECTION_INCLUDE_PATH ${CMAKE_CURRENT_SOURCE_DIR})

file(GLOB_RECURSE INFLECTION_SOURCES CONFIGURE_DEPENDS "*.c" "*.cpp")
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${INFLECTION_SOURCES})

file(GLOB_RECURSE TOOLS_SUBSET
	${CMAKE_CURRENT_SOURCE_DIR}/inflection/dictionary/*.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/inflection/exception/*.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/inflection/resources/*.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/inflection/lang/features/*.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/inflection/tokenizer/trie/*.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/inflection/util/*.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/inflection/Object.cpp
)
list(FILTER TOOLS_SUBSET EXCLUDE REGEX "-c.cpp$")

set(INFLECTION_OTHER_SOURCES ${INFLECTION_SOURCES})
foreach(TOOL_SOURCE IN LISTS TOOLS_SUBSET)
    list(REMOVE_ITEM INFLECTION_OTHER_SOURCES ${TOOL_SOURCE})
endforeach()

# Inflection tool objects target
if(XCODE)
	add_library(inflection_tool_objs INTERFACE)
	target_sources(inflection_tool_objs INTERFACE ${TOOLS_SUBSET})
	target_include_directories(inflection_tool_objs INTERFACE ${INFLECTION_INCLUDE_PATH} ${ICU_INCLUDE_DIR})
	target_compile_definitions(inflection_tool_objs INTERFACE ${inflection_compile_definitions})
else()
	add_library(inflection_tool_objs OBJECT)
	target_sources(inflection_tool_objs PRIVATE ${TOOLS_SUBSET})
	set_property(TARGET inflection_tool_objs PROPERTY POSITION_INDEPENDENT_CODE ON)
	target_include_directories(inflection_tool_objs PUBLIC ${INFLECTION_INCLUDE_PATH} ${ICU_INCLUDE_DIR})
	target_link_libraries(inflection_tool_objs PRIVATE resource_objs marisa_objs ICU::uc ICU::i18n)
	target_compile_definitions(inflection_tool_objs PUBLIC ${inflection_compile_definitions})
endif()
add_dependencies(inflection_tool_objs generated_src)
#End section

# libinflection target
add_library(inflection SHARED ${INFLECTION_OTHER_SOURCES})
target_include_directories(inflection PUBLIC ${INFLECTION_INCLUDE_PATH} ${ICU_INCLUDE_DIR})
target_link_options(inflection PRIVATE $<$<PLATFORM_ID:Darwin>:-dead_strip>)
target_compile_definitions(inflection PUBLIC ${inflection_compile_definitions})
target_link_libraries(inflection
		PRIVATE
			ICU::uc ICU::i18n
			marisa_objs resource_objs inflection_tool_objs
)
add_link_options(-no_inits)

if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
	set_target_properties(inflection PROPERTIES
		BUILD_WITH_INSTALL_RPATH TRUE
		BUILD_WITH_INSTALL_NAME_DIR TRUE
		INSTALL_NAME_DIR ${CMAKE_INSTALL_FULL_LIBDIR}
	)
else()
	set_target_properties(inflection PROPERTIES
		BUILD_WITH_INSTALL_RPATH TRUE
		BUILD_RPATH_USE_ORIGIN TRUE
		INSTALL_RPATH $ORIGIN
	)
endif()

add_custom_command(
	OUTPUT ${INFLECTION_INCLUDE_ROOT}/inflection/version.h
	COMMAND ${CMAKE_COMMAND} -DBINARY_DIR=${CMAKE_BINARY_DIR} -DSOURCE_DIR=${CMAKE_CURRENT_SOURCE_DIR} -DPACKAGE_DIR=${INFLECTION_INCLUDE_ROOT} -P ${CMAKE_SOURCE_DIR}/cmake/installPublicHeaders.cmake
	DEPENDS generated_version_header
)
add_custom_target(inflection-headers ALL DEPENDS ${INFLECTION_INCLUDE_ROOT}/inflection/version.h)

set(CXX_CHECK_HEADER_FLAGS "${CXX_STD_LIB_FLAG}")
if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
	get_target_property(ICU_INCLUDES ICU::uc INTERFACE_INCLUDE_DIRECTORIES)
	set(ICU_INCLUDES "-I${ICU_INCLUDES}")
endif()

add_custom_target(check-headers
	COMMAND ${CMAKE_COMMAND} -DCPP_COMPILER_BIN=${CMAKE_CXX_COMPILER} -DC_COMPILER_BIN=${CMAKE_C_COMPILER} -DHEADER_DIR=${INFLECTION_INCLUDE_ROOT} "-DCXX_CHECK_HEADER_FLAGS=${CXX_CHECK_HEADER_FLAGS}" -DCXX_STANDARD=${CMAKE_CXX_STANDARD} -DICU_INCLUDES=${ICU_INCLUDES} -P ${CMAKE_SOURCE_DIR}/cmake/checkCompileHeaders.cmake
	VERBATIM
	DEPENDS inflection-headers
)

set(ARCHS_DEFAULT_VAL "x86_64")
if(DEFINED ENV{ARCHS})
	set(ARCHS_DEFAULT_VAL $ENV{ARCHS})
endif()
set(ARCHS ${ARCHS_DEFAULT_VAL} CACHE STRING "Used in tapi install api" FORCE)
string(REPLACE " " ";" ARCH_LIST ${ARCHS})
foreach(ARCH IN LISTS ARCH_LIST)
	list(APPEND ARCH_TARGET_FLAG_LIST "--target=${ARCH}-$ENV{LLVM_TARGET_TRIPLE_VENDOR}-$ENV{LLVM_TARGET_TRIPLE_OS_VERSION}$ENV{LLVM_TARGET_TRIPLE_SUFFIX}")
endforeach()
list(JOIN ARCH_TARGET_FLAG_LIST " " ARCH_TARGET_FLAGS)
get_target_property(INFLECTION_INSTALL_NAME_DIR inflection INSTALL_NAME_DIR)
add_custom_target(install_api
	COMMAND
		sh -c "xcrun --sdk ${BUILD_SDKROOT} tapi installapi -dynamiclib -xc++ -std=c++${CMAKE_CXX_STANDARD} ${ARCH_TARGET_FLAGS} -install_name ${INFLECTION_INSTALL_NAME_DIR}/libinflection.dylib -current_version 1 -compatibility_version 1 -extra-private-header $ENV{DSTROOT}${CMAKE_INSTALL_FULL_INCLUDEDIR} -extra-public-header $ENV{DSTROOT}${CMAKE_INSTALL_FULL_INCLUDEDIR} -o $ENV{BUILT_PRODUCTS_DIR}/libinflection.tbd $ENV{DSTROOT}"
	VERBATIM
)
add_dependencies(install_api inflection-headers)