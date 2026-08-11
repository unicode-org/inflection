/*
 * Copyright 2017-2024 Apple Inc. All rights reserved.
 */
#include <inflection/dialog/SemanticFeature.hpp>

#include <inflection/exception/IllegalArgumentException.hpp>
#include <inflection/util/StringUtils.hpp>

namespace inflection::dialog {


SemanticFeature::SemanticFeature(const ::std::u16string& name, Type type, const ::std::set<::std::u16string>& boundedValues, bool aliased)
    : super()
{
    if (type != Type::BoundedValue && !boundedValues.empty()) {
        throw ::inflection::exception::IllegalArgumentException(::std::u16string(u"Features of type ") + inflection::util::StringUtils::to_u16string((int32_t) type)
            + u" do not support bounded values!");
    }
    if (type != Type::BoundedValue && aliased) {
        throw ::inflection::exception::IllegalArgumentException(u"Only bounded features can have aliases");
    }
    this->name = name;
    this->type = type;
    this->boundedValues = boundedValues;
    this->aliased = aliased;
}

SemanticFeature::SemanticFeature(const SemanticFeature& other)
    : type(other.type)
    , name(other.name)
    , boundedValues(other.boundedValues)
    , aliased(other.aliased)
{
}

SemanticFeature::~SemanticFeature()
{
}

SemanticFeature* SemanticFeature::noValue(const ::std::u16string& name)
{
    return new SemanticFeature(name, Type::NoValue, {}, false);
}

SemanticFeature* SemanticFeature::boundedValue(const ::std::u16string& name, const ::std::set<::std::u16string>& values)
{
    return new SemanticFeature(name, Type::BoundedValue, values, false);
}

SemanticFeature* SemanticFeature::unboundedValue(const ::std::u16string& name)
{
    return new SemanticFeature(name, Type::UnboundedValue, {}, false);
}

SemanticFeature::Type SemanticFeature::getType() const
{
    return type;
}

const ::std::u16string& SemanticFeature::getName() const
{
    return name;
}

const ::std::set<::std::u16string>& SemanticFeature::getBoundedValues() const
{
    return boundedValues;
}

bool SemanticFeature::isAliased() const
{
    return aliased;
}

std::weak_ordering SemanticFeature::operator<=>(const SemanticFeature& other) const
{
    return name <=> other.name;
}

bool SemanticFeature::operator==(const SemanticFeature& other) const
{
    return (*this <=> other) == 0;
}

} // namespace inflection::dialog
