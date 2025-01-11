#pragma once

#include <exception> 

class BadWeakPtr : public std::exception {
public:
    const char* what() noexcept
    { return "weak_ptr is expired!"; }
};
