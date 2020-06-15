#pragma once
#include "string_basic.h"
#include "repr.h"
#include <sstream>
template <>
struct e::system::repr_maker<e::system::string>
{
    static e::system::string of(const e::system::string &value)
    {
        auto length = value.len();
        std::ostringstream stream;
        stream << '\"';
        for (size_t i = 0; i < length; i++)
        {
            auto c = value.data[i];
            switch (c)
            {
            case '\r':
                stream << "\\r";
                break;
            case '\f':
                stream << "\\f";
                break;
            case '\n':
                stream << "\\n";
                break;
            case '\t':
                stream << "\\t";
                break;
            case '\"':
                stream << "\\\"";
                break;
            default:
                stream << c;
                break;
            }
        }
        stream << "\"";
        return e::system::string(stream.str().c_str());
    }
};
IMPLEMENT_DBG_INFO_VIA_REPR(e::system::string)