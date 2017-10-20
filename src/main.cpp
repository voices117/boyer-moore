/* include area */
#include "boiermur.hpp"
#include <string>
#include <string.h>
#include <vector>
#include <iostream>
#include <iomanip>

using std::cin;
using std::cout;
using std::endl;
using std::string;
using std::vector;


void display_usage( const string& bin_name ) {
    cout << "Usage: " << bin_name << " PATTERN TEXT" << endl;
    cout << "or echo text | " << bin_name << " PATTERN" << endl;
}


#ifdef __TESTS__
int dummy_main(int argc, const char *argv[])
#else
int main(int argc, const char *argv[])
#endif
{
    try
    {
        string pattern;

        if( argc == 2 ) {
            /* expects a pattern in the cmd line and input through STDIN */
            pattern = argv[1];
        } else if( argc == 3 ) {
            /* reads the pattern first, and then the text */
            pattern = argv[1];
        } else {
            display_usage( argv[0] );
            return 1;
        }

        BM::Pattern p{ pattern };

        cout << p << endl;

        size_t buffer_size = 1 << 20;
        char input[buffer_size];
        size_t bytes_read = 0;
        size_t base_index = 0;
        bool first = true;

        do {
            /* reads from STDIN into a buffer */
            if( first ) {
                cin.read( input, buffer_size );
                bytes_read = cin.gcount();
            } else {
                memmove( input, input + buffer_size - pattern.length() + 1, pattern.length() - 1 );
                cin.read( input + pattern.length() - 1, buffer_size - pattern.length() + 1 );
                bytes_read = cin.gcount() + pattern.length() - 1;
            }

            if( !bytes_read ) {
                break;
            }

            string in_str{ input, bytes_read };

            /* search */
            auto matches = p.find( in_str );
    
            /* prints the matches */
            for( auto match: matches ) {
                size_t base = match;
                if( base < 40 ) {
                    base = 0; 
                } else {
                    base -= 40;
                }
                cout << "\x1B[1;32m" << base_index + match << "\x1B[1;34m" << ": " << "\x1B[0m"
                     << in_str.substr(base, match - base) << "\x1B[1;31m" << in_str.substr(match, pattern.length())
                     << "\x1B[0m" << in_str.substr(match + pattern.length(), 40) << endl;
            }

            base_index += bytes_read;
            if( first ) {
                first = false;
            }
            base_index -= pattern.length() - 1;

        } while( bytes_read == buffer_size );
        
    } catch( ... ) {
        cout << "Unexpected error" << endl;
        return 1;
    }

    return 0;
}
