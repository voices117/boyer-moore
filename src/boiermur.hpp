#ifndef BOIERMUR_HPP
#define BOIERMUR_HPP

/* include area */
#include <vector>
#include <string>
#include <iostream>

namespace BM
{

class Pattern
{
  public:
    Pattern(const char *pattern);
    Pattern(const char *pattern, std::size_t len);
    Pattern(const std::string &pattern);
    ~Pattern();

    size_t bad_char_shift( char c, size_t index ) const;
    
    std::vector<size_t> find(const char *T) const;
    std::vector<size_t> find(const std::string &T) const;

    friend std::ostream &operator<<(std::ostream &os, const Pattern &p)
    {
        //os << "[";
        //for (size_t i = 0; i < 256; i++) {
            size_t i = static_cast<size_t>('e');
            os << "[";
            for( size_t j; j < p.bad_char_table[i].size() - 1; j++ ) {
                os << p.bad_char_table[i][j] << ", ";
            }
            os << p.bad_char_table[i][p.bad_char_table[i].size()-1];
            os << "]" << std::endl;
        //}
        //os << "]" << std::endl;

        os << "[";
        for (size_t i = 0; i < p.len - 1; i++)
        {
            os << p.Lp[i] << ", ";
        }
        os << p.Lp[p.len - 1] << "]" << std::endl;

        os << "[";
        for (size_t i = 0; i < p.len - 1; i++)
        {
            os << p.lp[i] << ", ";
        }
        os << p.lp[p.len - 1] << "]" << std::endl;
        return os;
    }

  private:
    std::vector<size_t> bad_char_table[256];
    int *Lp{nullptr};
    int *lp{nullptr};

    const char *pattern;
    std::size_t len; // pattern length
};

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
