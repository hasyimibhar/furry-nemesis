#ifndef HASHEDSTRING_H_INCLUDED
#define HASHEDSTRING_H_INCLUDED

// I didn't write the code below. I copy-pasted it from here:
// http://seanmiddleditch.com/journal/2011/05/compile-time-string-hashing-in-c0x/

// FNV-1a constants
static constexpr unsigned long long basis = 14695981039346656037ULL;
static constexpr unsigned long long prime = 1099511628211ULL;

// compile-time hash helper function
constexpr static unsigned long long hash_one(char c, const char* remain, unsigned long long value)
{
    return c == 0 ? value : hash_one(remain[0], remain + 1, (value ^ c) * prime);
}

// compile-time hash
constexpr static unsigned long long hash_ct(const char* str)
{
    return hash_one(str[0], str + 1, basis);
}

// run-time hash
static unsigned long long hash_rt(const char* str)
{
    unsigned long long hash = basis;
    while (*str != 0) {
        hash ^= str[0];
        hash *= prime;
        ++str;
    }
    return hash;
}

#endif