#pragma once

#include <cstddef>      // size_t, nullptr_t
#include <stdexcept>    // exceptions
#include <climits>
#include <iostream>     // debugging
#include "Config.hpp"


#define CUSTOM_ASSERT(Expr, Msg) __Assert(Expr, Msg, #Expr, __FILE__, __LINE__)

inline void __Assert(bool expr, const char* msg, const char* exprStr, const char* file, int line)
{
    if (!expr)
    {
        std::cerr   << "Assert failed:\t"   << msg << "\n"
                    << "Expected:\t"        << exprStr << "\n"
                    << "Source:\t\t"        << file << ", line " << line << "\n";
        ::abort();
    }
}