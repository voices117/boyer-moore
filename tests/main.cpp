/* include area */
#include "boiermur.hpp"
#include <exception>
#include <iostream>
#include <iterator>
#include <string.h>
#include <vector>

using std::size_t;
using std::vector;
using std::string;
using std::ostream;
using std::cout;
using std::endl;

/* auxiliary macros */
#define STR(s) #s
#define XSTR(s) STR(s)

#define GREEN_TEXT(text) "\x1B[1;92m" text "\x1B[0m"
#define RED_TEXT(text) "\x1B[1;31m" text "\x1B[0m"

/* test macros */
#define ASSERT(e)                     \
    if (!(e)) {                       \
        throw XSTR(__LINE__) ": " #e; \
    }

#define ASSERT_EQ(obtained, expected)                                                                \
    if (!((obtained) == (expected))) {                                                               \
        throw static_cast<string>(XSTR(__LINE__) ":\n\n" #obtained "=") + std::to_string(obtained) + \
            static_cast<string>("\nexpected=") + std::to_string(expected) + "\n";                    \
    }

#define Z_TEST(text, expected)                                         \
    do {                                                               \
        string s{text};                                                \
        vector<size_t> z_num;                                          \
        z_num.reserve(s.length());                                     \
        BM::Z(z_num, BM::RString{s});                                  \
        for (size_t i = 0; i < expected.size(); i++) {                 \
            if (z_num[i] != expected[i]) {                             \
                cout << "at iteration " << i << endl;                  \
                cout << "expected: " << expected << endl;              \
                cout << "obtained: " << z_num << endl;                 \
                ASSERT(z_num[i] == expected[expected.size() - i - 1]); \
            }                                                          \
        }                                                              \
    } while (0)

template <typename T>
std::ostream& operator<<(std::ostream& out, const std::vector<T>& v) {
    if (!v.empty()) {
        out << '[';
        std::copy(v.begin(), v.end(), std::ostream_iterator<T>(out, ", "));
        out << "\b\b]";
    }
    return out;
}

/**
 * Tests the Z algorithm implementation (on a reversed string).
 */
void test_z() {
    {
        const char* test = "abcabcd";
        vector<size_t> expected = {7, 0, 0, 0, 0, 0, 0};

        Z_TEST(test, expected);
    }
    {
        const char* test = "dcbacba";
        vector<size_t> expected = {7, 0, 0, 3, 0, 0, 0};

        Z_TEST(test, expected);
    }
    {
        const char* test = "banana";
        vector<size_t> expected = {6, 0, 3, 0, 1, 0};

        Z_TEST(test, expected);
    }
    {
        const char* test = "aaaaaaaaaabbbbb";
        vector<size_t> expected = {15, 4, 3, 2, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

        Z_TEST(test, expected);
    }
}

/**
 * Tests the final Boyer-Moore algorithm.
 */
void test_boiermur() {
#define TEST_MATCHES(P, T, ...)                        \
    do {                                               \
        vector<size_t> expected = {__VA_ARGS__};       \
        auto obtained = P.find(T);                     \
        ASSERT(expected.size() == obtained.size());    \
        for (size_t i = 0; i < expected.size(); i++) { \
            ASSERT(expected[i] == obtained[i]);        \
        }                                              \
    } while (0)

#define ASSERT_NO_MATCH(P, T)         \
    do {                              \
        auto obtained = P.find(T);    \
        ASSERT(obtained.size() == 0); \
    } while (0)

    /* tests the bad-character rule */
    {
        BM::Pattern p{"bananas"};

        /* if the letter is not in the pattern, the rest of it it should be skipped */
        ASSERT(p.bad_char_shift('c', 3) == 3);
        ASSERT(p.bad_char_shift('d', 1) == 1);
        ASSERT(p.bad_char_shift('d', 6) == 6);
        ASSERT(p.bad_char_shift('e', 5) == 5);

        /* if the character is not repeated *after* the index, skips the rest of the pattern */
        ASSERT(p.bad_char_shift('s', 6) == 6);
        ASSERT(p.bad_char_shift('s', 5) == 5);
        ASSERT(p.bad_char_shift('s', 4) == 4);
        ASSERT(p.bad_char_shift('n', 2) == 2);
        ASSERT(p.bad_char_shift('n', 1) == 1);

        /* checks that the jump is made to the next character */
        ASSERT(p.bad_char_shift('n', 6) == 2);
        ASSERT(p.bad_char_shift('n', 5) == 1);
        ASSERT(p.bad_char_shift('n', 4) == 2); // jumps to the next 'n'
        ASSERT(p.bad_char_shift('n', 3) == 1);
        ASSERT(p.bad_char_shift('n', 2) == 2); // jumps to the other next 'n'
        ASSERT(p.bad_char_shift('n', 1) == 1);

        /* if the last character mismatched, moves 1 place */
        ASSERT(p.bad_char_shift('n', 0) == 1);

        ASSERT(p.bad_char_shift('b', 6) == 6);
        ASSERT(p.bad_char_shift('b', 5) == 5);
        ASSERT(p.bad_char_shift('b', 4) == 4);
        ASSERT(p.bad_char_shift('b', 3) == 3);
        ASSERT(p.bad_char_shift('b', 2) == 2);
        ASSERT(p.bad_char_shift('b', 1) == 1);
        ASSERT(p.bad_char_shift('b', 0) == 1);
    }

    /* tests the good-suffix rule */
    {
        BM::Pattern p{"ana banana"};
        size_t len = string{"ana banana"}.length();

        /* if no character matched (i.e. the first comparisson failed), shifts 1 place */
        ASSERT(p.good_suffix_shift(len) == 1);

        /* tests when the "a" matched (i.e. the "a" suffix)
           the 'a' in position 7 is ignored because it's also preceded by an 'n'
           0123456789            0123456789
           ana banana   -shift-> ana banana
           ________xa__          ____xa______
                   ^                 ^        */
        ASSERT_EQ(p.good_suffix_shift(len - 1), 4);

        /* tests the "na" suffix (similar to the previous case):
           0123456789              0123456789
           ana banana   -shift->   ana banana
           _______xna__          xna_________
                  ^              ^            */
        ASSERT_EQ(p.good_suffix_shift(len - 2), 9);

        /* tests the "ana" suffix:
           0123456789            0123456789
           ana banana   -shift-> ana banana
           ______xana__          ____xana____
                 ^                   ^          */
        ASSERT_EQ(p.good_suffix_shift(len - 3), 2);

        /* tests the "nana" suffix:
           0123456789               0123456789
           ana banana   -shift->    ana banana
           _____xnana__           xnana_______
                 ^                   ^          */
        ASSERT_EQ(p.good_suffix_shift(len - 4), 7);

        /* tests the rest of the suffixes (similar to the previous case) */
        ASSERT_EQ(p.good_suffix_shift(len - 5), 7);
        ASSERT_EQ(p.good_suffix_shift(len - 6), 7);
        ASSERT_EQ(p.good_suffix_shift(len - 7), 7);
        ASSERT_EQ(p.good_suffix_shift(len - 8), 7);
        ASSERT_EQ(p.good_suffix_shift(len - 9), 7);
    }

    /* tests the boyer-moore algorithm with different patterns */
    {
        BM::Pattern p{"idiot"};

        ASSERT_NO_MATCH(p, "a man a plan");
        ASSERT_NO_MATCH(p, "it's me, Mario!");
        ASSERT_NO_MATCH(p, "FOL: Fart Out Loud");
        ASSERT_NO_MATCH(p, "idioidioidio");
        TEST_MATCHES(p, "adiotidiot", 5);
        TEST_MATCHES(p, "diotidiot", 4);
    }
    {
        BM::Pattern p{"hello"};

        ASSERT_NO_MATCH(p, "");
        ASSERT_NO_MATCH(p, "h");
        ASSERT_NO_MATCH(p, "he");
        ASSERT_NO_MATCH(p, "hel");
        ASSERT_NO_MATCH(p, "hell");

        TEST_MATCHES(p, "hello", 0);
        TEST_MATCHES(p, "hello world!", 0);
        TEST_MATCHES(p, "hello hello", 0, 6);
        TEST_MATCHES(p, "aello hello", 6);
        TEST_MATCHES(p, "hell hello", 5);
        TEST_MATCHES(p, "hellohellohello", 0, 5, 10);
    }
    {
        BM::Pattern p{"ere"};

        TEST_MATCHES(p, "there is a problem here where mere faces appear", 2, 20, 26, 31);
        TEST_MATCHES(p, "here", 1);
    }
    {
        BM::Pattern p{"hello hello"};

        TEST_MATCHES(p, "well, hello hello my friend... oh! hello bye bye LOL", 6);
        ASSERT_NO_MATCH(p, "hellohello heee llooo wehehe low hello hell oops");
    }
    {
        BM::Pattern p{"ana banana"};

        ASSERT_NO_MATCH(p, "banana");
        ASSERT_NO_MATCH(p, "ana banann");
        ASSERT_NO_MATCH(p, "xna banana");

        TEST_MATCHES(p, "ana banana", 0);
        TEST_MATCHES(p, "ana banaxaana banana", 10);
    }
    {
        BM::Pattern p{"ttttagagca"};

        TEST_MATCHES(p, "attttagagca", 1);
    }
    {
        BM::Pattern p{"aagcgaataccg"};

        TEST_MATCHES(p, "tggtacaaagcgaataccg", 7);
    }
}

#ifdef __TESTS__
int main(int argc, const char* argv[])
#else
int test_main(int argc, const char* argv[])
#endif
{
    try {
        test_z();
        test_boiermur();
    } catch (const char* e) {
        cout << RED_TEXT("[ FAILED ]") " at " << e << endl;
        return 1;
    } catch (const string& e) {
        cout << RED_TEXT("[ FAILED ]") " at " << e << endl;
        return 1;
    }

    cout << GREEN_TEXT("[ SUCCESS ]") << endl;
    return 0;
}
