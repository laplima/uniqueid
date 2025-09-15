#ifndef PRINTABLE_H
#define PRINTABLE_H

#include <iostream>
#include <string>
#include <sstream>

template <class T>
concept Printable = requires(std::ostream& os, T a) {
    os << a; // Requires that 'os << a' is a valid expression
};

template<Printable T>
std::string streamed(const T& t) { std::ostringstream ss; ss << t; return ss.str(); }

#endif
