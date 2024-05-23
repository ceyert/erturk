#ifndef ERTURK_STRING_LITERAL_H
#define ERTURK_STRING_LITERAL_H

#include <cstddef>
#include <cstdint>
#include <stdexcept>

namespace erturk::string_literal
{

inline constexpr bool isalnum(char c)
{
    return (c >= '0' && c <= '9') || (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z');
}

inline constexpr bool isalpha(char c)
{
    return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z');
}

inline constexpr bool islower(char c)
{
    return c >= 'a' && c <= 'z';
}

inline constexpr bool isupper(char c)
{
    return c >= 'A' && c <= 'Z';
}

inline constexpr bool isdigit(char c)
{
    return c >= '0' && c <= '9';
}

inline constexpr bool isspace(char c)
{
    return c == ' ' || c == '\n' || c == '\t' || c == '\r' || c == '\f' || c == '\v';
}

inline constexpr bool isblank(char c)
{
    return c == ' ' || c == '\t';
}

inline constexpr char tolower(char c)
{
    if (isupper(c))
    {
        return c + 'a' - 'A';
    }
    return c;
}

inline constexpr char toupper(char c)
{
    if (islower(c))
    {
        return c - 'a' + 'A';
    }
    return c;
}

inline constexpr int atoi(const char* str_literal)
{
    if (str_literal == nullptr)
    {
        return 0;
    }

    // Skip leading whitespace
    while (isspace(*str_literal))
    {
        str_literal++;
    }

    int res = 0;
    int sign = 1;
    int i = 0;

    // Check for sign
    if (str_literal[i] == '-')
    {
        sign = -1;
        i++;
    }
    else if (str_literal[i] == '+')
    {
        i++;
    }

    // Convert digits to integer
    while (str_literal[i] != '\0')
    {
        if (!isdigit(str_literal[i]))
        {
            break;
        }
        res = res * 10 + str_literal[i] - '0';
        i++;
    }

    return sign * res;
}

inline char* strcat(char* dest, const char* src)
{
    if (dest == nullptr || src == nullptr)
    {
        return nullptr;
    }

    char* ret = dest;

    // Move dest to the end of the string
    while (*dest != '\0')
    {
        dest++;
    }

    // Copy src to dest
    while (*src != '\0')
    {
        *dest = *src;
        dest++;
        src++;
    }

    // Add null terminator at the end of the concatenated string
    *dest = '\0';

    return ret;
}

inline constexpr int strcmp(const char* s1, const char* s2)
{
    if (s1 == nullptr || s2 == nullptr)
    {
        return (s1 == nullptr) - (s2 == nullptr);
    }

    while (*s1 != '\0' && (*s1 == *s2))
    {
        s1++;
        s2++;
    }
    return *(const unsigned char*)s1 - *(const unsigned char*)s2;
}

inline void reverseStr(char* str, int length)
{
    if (str == nullptr || length <= 0)
    {
        return;
    }

    int start = 0;
    int end = length - 1;
    while (start < end)
    {
        char temp = str[start];
        str[start] = str[end];
        str[end] = temp;
        start++;
        end--;
    }
}

}  // namespace erturk::string_literal

#endif  // ERTURK_STRING_LITERAL_H