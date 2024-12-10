#!/bin/bash
#
# Copyright 2016-2024 Apple Inc. All rights reserved.
#

set -e

DATA_RESOURCE_PATH=$1
RESOURCES="${2:-.}"/resources
RESOURCE_PROPS=$(find $RESOURCES/org -type f -name \*.properties | sort)
RESOURCE_DICTS=$(find $RESOURCES/org -type f -not -name \*.properties -not -name .\* -not -name \*.sdict -not -name \*.lst -not -name \*.dtd -not -name \*.xml -not -name transliteration_map\*.* -not -name CMakeLists.txt -not -name tokenizer.dictionary | sort)
RESOURCES_SED=$(echo $RESOURCES | sed 's/\//\\\//g')

echo "Generating $DATA_RESOURCE_PATH"

mkdir -p $(dirname $DATA_RESOURCE_PATH)

echo '#include <unordered_map>' > $DATA_RESOURCE_PATH
echo '#include <morphuntion/resources/DataResource.hpp>' >> $DATA_RESOURCE_PATH
echo '#include <morphuntion/exception/FileNotFoundException.hpp>' >> $DATA_RESOURCE_PATH

for RESOURCE_PROP in $RESOURCE_PROPS; do
    echo "#include <$(echo $RESOURCE_PROP | sed s/$RESOURCES_SED\\///g | sed 's/.properties/.hpp/g')>" >> $DATA_RESOURCE_PATH
done

for RESOURCE_DICT in $RESOURCE_DICTS; do
    echo "#include <$(echo $RESOURCE_DICT | sed s/$RESOURCES_SED\\///g).hpp>" >> $DATA_RESOURCE_PATH
done

echo '::std::map<::std::u16string_view, const char16_t*> morphuntion::resources::DataResource::getProperties(::std::u16string_view resourceName, bool returnEmptyOnFailure) {' >> $DATA_RESOURCE_PATH
echo '    static const auto resources = new ::std::unordered_map<::std::u16string_view, std::map<std::u16string_view, const char16_t*> (*)()>{' >> $DATA_RESOURCE_PATH
for RESOURCE_PROP in $RESOURCE_PROPS; do
    RELATIVE_RESOURCE_PROP=$(echo $RESOURCE_PROP | sed s/$RESOURCES_SED\\///g)
    echo "        {u\"/$RELATIVE_RESOURCE_PROP\", $(echo $RELATIVE_RESOURCE_PROP | sed 's/.properties//g;s/\//::/g;s/-/_/g')::getContent}," >> $DATA_RESOURCE_PATH
done
echo '    };' >> $DATA_RESOURCE_PATH
echo '    auto entry = resources->find(resourceName);' >> $DATA_RESOURCE_PATH
echo '    if (entry == resources->end()) {' >> $DATA_RESOURCE_PATH
echo '        if (returnEmptyOnFailure) {' >> $DATA_RESOURCE_PATH
echo '            return {};' >> $DATA_RESOURCE_PATH
echo '        }' >> $DATA_RESOURCE_PATH
echo '        throw ::morphuntion::exception::FileNotFoundException(::std::u16string(resourceName));' >> $DATA_RESOURCE_PATH
echo '    }' >> $DATA_RESOURCE_PATH
echo '    return entry->second();' >> $DATA_RESOURCE_PATH
echo '}' >> $DATA_RESOURCE_PATH
echo 'const char16_t *morphuntion::resources::DataResource::getString(::std::u16string_view resourceName, bool returnNullOnFailure) {' >> $DATA_RESOURCE_PATH
echo '    static const auto resources = new ::std::unordered_map<::std::u16string_view, const char16_t *>{' >> $DATA_RESOURCE_PATH
for RESOURCE_DICT in $RESOURCE_DICTS; do
    RELATIVE_RESOURCE_DICT=$(echo $RESOURCE_DICT | sed s/$RESOURCES_SED\\///g)
    echo "        {u\"/$RELATIVE_RESOURCE_DICT\", $(echo $RELATIVE_RESOURCE_DICT | sed 's/.properties//g;s/\//::/g;s/-/_/g;s/\./_/g')::content}," >> $DATA_RESOURCE_PATH
done
echo '    };' >> $DATA_RESOURCE_PATH
echo '    auto entry = resources->find(resourceName);' >> $DATA_RESOURCE_PATH
echo '    if (entry == resources->end()) {' >> $DATA_RESOURCE_PATH
echo '        if (returnNullOnFailure) {' >> $DATA_RESOURCE_PATH
echo '            return nullptr;' >> $DATA_RESOURCE_PATH
echo '        }' >> $DATA_RESOURCE_PATH
echo '        throw ::morphuntion::exception::FileNotFoundException(::std::u16string(resourceName));' >> $DATA_RESOURCE_PATH
echo '    }' >> $DATA_RESOURCE_PATH
echo '    return entry->second;' >> $DATA_RESOURCE_PATH
echo '}' >> $DATA_RESOURCE_PATH
