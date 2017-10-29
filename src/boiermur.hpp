#ifndef BOIERMUR_HPP
#define BOIERMUR_HPP

/* include area */
#include <iostream>
#include <string>
#include <vector>

namespace BM
{

class Pattern
{
  public:
    Pattern(const char *pattern);
    Pattern(const char *pattern, std::size_t len);
    Pattern(const std::string &pattern);
    ~Pattern();

    size_t bad_char_shift(unsigned char c, size_t index) const;

    std::vector<size_t> find(const char *T) const;
    std::vector<size_t> find(const std::string &T) const;

    friend std::ostream &operator<<(std::ostream &os, const Pattern &p);

  private:
    std::vector<size_t> bad_char_table[256];
    int *Lp{nullptr};
    int *lp{nullptr};

    const char *pattern;
    std::size_t len; // pattern length
};
std::ostream &operator<<(std::ostream &os, const Pattern &p);

class RString
{
  public:
    RString(std::string s) : s(s.c_str()), last_index(s.length() - 1) {}
    RString(const char *s, size_t len) : s(s), last_index(len - 1) {}

    std::size_t length() const { return this->last_index + 1; }
    char operator[](size_t i) const { return s[this->last_index - i]; }

  private:
    const char *s;
    size_t last_index;
};

template <typename S>
void Z(std::size_t *z, S s);
}

#endif
