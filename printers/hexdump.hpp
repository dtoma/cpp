/**
 * HexDump
 * 
 * Take a string and print it in hexdump format.
 * For example, running it on this file would output:
 * ```
 * 000000 | 2f 2a 2a                                         | /**
 * 000000 | 20 2a 20 48 65 78 44 75 6d 70                    |  * HexDump
 * 000000 | 20 2a 20                                         |  *
 * 000000 | 20 2a 20 54 61 6b 65 20 61 20 73 74 72 69 6e 67  |  * Take a string
 * 000010 | 20 61 6e 64 20 70 72 69 6e 74 20 69 74 20 69 6e  |  and print it in
 * 000020 | 20 68 65 78 64 75 6d 70 20 66 6f 72 6d 61 74 2e  |  hexdump format.
 * 000000 | 20 2a 20 46 6f 72 20 65 78 61 6d 70 6c 65 2c 20  |  * For example,
 * 000010 | 72 75 6e 6e 69 6e 67 20 69 74 20 6f 6e 20 74 68  | running it on th
 * 000020 | 69 73 20 66 69 6c 65 20 77 6f 75 6c 64 20 6f 75  | is file would ou
 * 000030 | 74 70 75 74 3a                                   | tput:
 * etc.
 * ```
 * 
 * Inspiration: http://stackoverflow.com/a/29865
 * Todo:
 * - return strings instead of printing
 * - use `fmt` to format the output
 */

#include <algorithm>
#include <string>
#include <vector>

#include "fmt/printf.h"

char get_printable_char(char c)
{
  return isprint(c) ? c : '.';
}

void hexdump(std::string const &str)
{
  auto const len = str.size();
  size_t const display_size = 16;

  for (size_t i = 0; i < len; i += display_size)
  {
    auto const line = str.substr(i, display_size);
    auto const line_size = line.size();

    // First column: hexadecimal line number
    fmt::print("{:06x} | ", i);

    // Second column: hexadecimal bytes
    std::for_each(
        std::cbegin(line),
        std::cend(line),
        [](auto const &c) { fmt::print("{:x} ", c); });

    for (auto j = display_size - line_size; j > 0; j--)
    { // Pad with spaces if we have fewer than 16 characters
      fmt::print("   ");
    }
    fmt::print("| ");

    // Third column: readable text
    std::for_each(
        std::cbegin(line),
        std::cend(line),
        [](auto const &c) { fmt::print("{}", get_printable_char(c)); });
    fmt::print("\n");
  }
}
