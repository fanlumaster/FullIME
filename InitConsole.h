#pragma once
#include <cstdio>
#include <fstream>

// For debugging
#include <fcntl.h>
#include <io.h>

#define UNUSED(x) (void)(x)  // Unused param (C compatible - not applicable to expressions)

class outbuf : public std::streambuf {
   public:
    outbuf() { setp(0, 0); }

    virtual int_type overflow(int_type c = traits_type::eof()) { return fputc(c, stdout) == EOF ? traits_type::eof() : c; }
};
