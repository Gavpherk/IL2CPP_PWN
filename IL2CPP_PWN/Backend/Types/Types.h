#pragma once
#include <string>

struct JFunction
{
    int Address;
    std::string Class;
    std::string Method;
    std::string TypeSignature;
};

struct JTypeInfo
{
    int Address;
    std::string Name;
};
