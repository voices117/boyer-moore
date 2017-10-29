#ifndef BOIERMUR_HPP
#define BOIERMUR_HPP

/* include area */
#include <iostream>
#include <string>
#include <vector>

namespace BM {

    class Pattern {
      public:
        Pattern(const char* pattern);
        Pattern(const char* pattern, std::size_t len);
        Pattern(const std::string& pattern);
        ~Pattern();

        inline size_t bad_char_shift(unsigned char c, size_t index) const;
        inline size_t good_suffix_shift(size_t index) const;

        std::vector<size_t> find(const char* T) const;
        std::vector<size_t> find(const std::string& T) const;

        friend std::ostream& operator<<(std::ostream& os, const Pattern& p);

      private:
        std::vector<size_t> bad_char_table[256];
        int* Lp{nullptr};
        int* lp{nullptr};

        const char* pattern;
        std::size_t len; // pattern length
    };
    std::ostream& operator<<(std::ostream& os, const Pattern& p);

    class RString {
      public:
        RString(std::string s) : s(s.c_str()), last_index(s.length() - 1) {}
        RString(const char* s, size_t len) : s(s), last_index(len - 1) {}

        std::size_t length() const { return this->last_index + 1; }
        char operator[](size_t i) const { return s[this->last_index - i]; }

      private:
        const char* s;
        size_t last_index;
    };

    template <typename S>
    void Z(std::vector<std::size_t>& z, S s);
}

/**
 * @brief Returns the shift calculated by using the bad-character rule in the
 * given index.
 *
 * @param c The character in T that did not match.
 * @param index Index where the rule is applied.
 * @return size_t The amount to shift.
 */
inline size_t BM::Pattern::bad_char_shift(unsigned char c, size_t index) const {
    size_t shift = 0;

    /* finds the index of the next character c in the pattern left to the current position */
    size_t cc = static_cast<size_t>(c);
    for (auto i : this->bad_char_table[cc]) {
        if (i < index) {
            shift = i;
            break;
        }
    }
    return std::max(index - shift, size_t(1));
}

/**
* @brief Applies the good suffix rule at a given index and returns the amount to shift.
* 
* @param index Index of the last character matched.
* @return size_t Amount to shift.
*/
inline size_t BM::Pattern::good_suffix_shift(size_t index) const {
    size_t gs_shift = this->Lp[index];
    if (gs_shift == 0) {
        gs_shift = this->lp[index];
    }
    return gs_shift;
}

#endif
