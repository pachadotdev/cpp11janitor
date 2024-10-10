#ifndef TIDY_HPP
#define TIDY_HPP

#include <algorithm>
#include <map>
#include <regex>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

#include <cpp11.hpp>

// Remove starting spaces and punctuation
std::string remove_starting_spaces_and_punctuation(const std::string& str) {
  std::regex re("^\\s*[[:punct:]]*");
  return std::regex_replace(str, re, "");
}

// Remove ending spaces and punctuation
std::string remove_ending_spaces_and_punctuation(const std::string& str) {
  std::regex re("[[:punct:]]*\\s*$");
  return std::regex_replace(str, re, "");
}

// Remove leading and trailing underscores
std::string remove_edge_underscores(const std::string& str) {
  std::regex re("^_+|_+$");
  return std::regex_replace(str, re, "");
}

// Remove \xXX
std::string remove_special_characters(const std::string& str) {
  std::string new_str;
  std::string::size_type pos = 0;

  std::map<std::string, char> flatten_map = {
      {"\xc3\x80", 'A'},  // LATIN CAPITAL LETTER A WITH GRAVE
      {"\xc3\x81", 'A'},  // LATIN CAPITAL LETTER A WITH ACUTE
      {"\xc3\x82", 'A'},  // LATIN CAPITAL LETTER A WITH CIRCUMFLEX
      {"\xc3\x83", 'A'},  // LATIN CAPITAL LETTER A WITH TILDE
      {"\xc3\x84", 'A'},  // LATIN CAPITAL LETTER A WITH DIAERESIS
      {"\xc3\x85", 'A'},  // LATIN CAPITAL LETTER A WITH RING ABOVE
      {"\xc3\x86", 'A'},  // LATIN CAPITAL LETTER AE
      {"\xc3\x87", 'C'},  // LATIN CAPITAL LETTER C WITH CEDILLA
      {"\xc3\x88", 'E'},  // LATIN CAPITAL LETTER E WITH GRAVE
      {"\xc3\x89", 'E'},  // LATIN CAPITAL LETTER E WITH ACUTE
      {"\xc3\x8a", 'E'},  // LATIN CAPITAL LETTER E WITH CIRCUMFLEX
      {"\xc3\x8b", 'E'},  // LATIN CAPITAL LETTER E WITH DIAERESIS
      {"\xc3\x8c", 'I'},  // LATIN CAPITAL LETTER I WITH GRAVE
      {"\xc3\x8d", 'I'},  // LATIN CAPITAL LETTER I WITH ACUTE
      {"\xc3\x8e", 'I'},  // LATIN CAPITAL LETTER I WITH CIRCUMFLEX
      {"\xc3\x8f", 'I'},  // LATIN CAPITAL LETTER I WITH DIAERESIS
      {"\xc3\x91", 'N'},  // LATIN CAPITAL LETTER N WITH TILDE
      {"\xc3\x92", 'O'},  // LATIN CAPITAL LETTER O WITH GRAVE
      {"\xc3\x93", 'O'},  // LATIN CAPITAL LETTER O WITH ACUTE
      {"\xc3\x94", 'O'},  // LATIN CAPITAL LETTER O WITH CIRCUMFLEX
      {"\xc3\x95", 'O'},  // LATIN CAPITAL LETTER O WITH TILDE
      {"\xc3\x96", 'O'},  // LATIN CAPITAL LETTER O WITH DIAERESIS
      {"\xc3\x99", 'U'},  // LATIN CAPITAL LETTER U WITH GRAVE
      {"\xc3\x9a", 'U'},  // LATIN CAPITAL LETTER U WITH ACUTE
      {"\xc3\x9b", 'U'},  // LATIN CAPITAL LETTER U WITH CIRCUMFLEX
      {"\xc3\x9c", 'U'},  // LATIN CAPITAL LETTER U WITH DIAERESIS
      {"\xc3\x9d", 'Y'},  // LATIN CAPITAL LETTER Y WITH ACUTE
      {"\xc3\xa0", 'a'},  // LATIN SMALL LETTER A WITH GRAVE
      {"\xc3\xa1", 'a'},  // LATIN SMALL LETTER A WITH ACUTE
      {"\xc3\xa2", 'a'},  // LATIN SMALL LETTER A WITH CIRCUMFLEX
      {"\xc3\xa3", 'a'},  // LATIN SMALL LETTER A WITH TILDE
      {"\xc3\xa4", 'a'},  // LATIN SMALL LETTER A WITH DIAERESIS
      {"\xc3\xa5", 'a'},  // LATIN SMALL LETTER A WITH RING ABOVE
      {"\xc3\xa6", 'a'},  // LATIN SMALL LETTER AE
      {"\xc3\xa7", 'c'},  // LATIN SMALL LETTER C WITH CEDILLA
      {"\xc3\xa8", 'e'},  // LATIN SMALL LETTER E WITH GRAVE
      {"\xc3\xa9", 'e'},  // LATIN SMALL LETTER E WITH ACUTE
      {"\xc3\xaa", 'e'},  // LATIN SMALL LETTER E WITH CIRCUMFLEX
      {"\xc3\xab", 'e'},  // LATIN SMALL LETTER E WITH DIAERESIS
      {"\xc3\xac", 'i'},  // LATIN SMALL LETTER I WITH GRAVE
      {"\xc3\xad", 'i'},  // LATIN SMALL LETTER I WITH ACUTE
      {"\xc3\xae", 'i'},  // LATIN SMALL LETTER I WITH CIRCUMFLEX
      {"\xc3\xaf", 'i'},  // LATIN SMALL LETTER I WITH DIAERESIS
      {"\xc3\xb1", 'n'},  // LATIN SMALL LETTER N WITH TILDE
      {"\xc3\xb2", 'o'},  // LATIN SMALL LETTER O WITH GRAVE
      {"\xc3\xb3", 'o'},  // LATIN SMALL LETTER O WITH ACUTE
      {"\xc3\xb4", 'o'},  // LATIN SMALL LETTER O WITH CIRCUMFLEX
      {"\xc3\xb5", 'o'},  // LATIN SMALL LETTER O WITH TILDE
      {"\xc3\xb6", 'o'},  // LATIN SMALL LETTER O WITH DIAERESIS
      {"\xc3\xb9", 'u'},  // LATIN SMALL LETTER U WITH GRAVE
      {"\xc3\xba", 'u'},  // LATIN SMALL LETTER U WITH ACUTE
      {"\xc3\xbb", 'u'},  // LATIN SMALL LETTER U WITH CIRCUMFLEX
      {"\xc3\xbc", 'u'},  // LATIN SMALL LETTER U WITH DIAERESIS
      {"\xc3\xbd", 'y'},  // LATIN SMALL LETTER Y WITH ACUTE
      {"\xc3\xbf", 'y'}   // LATIN SMALL LETTER Y WITH DIAERESIS
  };

  while (pos < str.size()) {
    unsigned char byte = static_cast<unsigned char>(str[pos]);
    std::string utf8_char;

    if ((byte & 0x80) == 0) {
      // 1-byte character
      utf8_char = str.substr(pos, 1);
      pos += 1;
    } else if ((byte & 0xE0) == 0xC0) {
      // 2-byte character
      utf8_char = str.substr(pos, 2);
      pos += 2;
    } else if ((byte & 0xF0) == 0xE0) {
      // 3-byte character
      utf8_char = str.substr(pos, 3);
      pos += 3;
    } else if ((byte & 0xF8) == 0xF0) {
      // 4-byte character
      utf8_char = str.substr(pos, 4);
      pos += 4;
    } else {
      // Invalid UTF-8 byte, treat as a single character
      utf8_char = str.substr(pos, 1);
      pos += 1;
    }

    if (flatten_map.find(utf8_char) != flatten_map.end()) {
      new_str += flatten_map[utf8_char];
    } else {
      new_str += utf8_char;
    }
  }

  return new_str;
}

// Recode \xXX as \u00XX

std::string recode_special_characters(const std::string& str) {
  std::string new_str;

  std::map<std::string, std::string> hex_to_unicode = {
      {"\x20", "\u0020"},      // SPACE
      {"\x21", "\u0021"},      // EXCLAMATION MARK
      {"\x22", "\u0022"},      // QUOTATION MARK
      {"\x23", "\u0023"},      // NUMBER SIGN
      {"\x24", "\u0024"},      // DOLLAR SIGN
      {"\x25", "\u0025"},      // PERCENT SIGN
      {"\x26", "\u0026"},      // AMPERSAND
      {"\x27", "\u0027"},      // APOSTROPHE
      {"\x28", "\u0028"},      // LEFT PARENTHESIS
      {"\x29", "\u0029"},      // RIGHT PARENTHESIS
      {"\x2a", "\u002A"},      // ASTERISK
      {"\x2b", "\u002B"},      // PLUS SIGN
      {"\x2c", "\u002C"},      // COMMA
      {"\x2d", "\u002D"},      // HYPHEN-MINUS
      {"\x2e", "\u002E"},      // FULL STOP
      {"\x2f", "\u002F"},      // SOLIDUS
      {"\x30", "\u0030"},      // DIGIT ZERO
      {"\x31", "\u0031"},      // DIGIT ONE
      {"\x32", "\u0032"},      // DIGIT TWO
      {"\x33", "\u0033"},      // DIGIT THREE
      {"\x34", "\u0034"},      // DIGIT FOUR
      {"\x35", "\u0035"},      // DIGIT FIVE
      {"\x36", "\u0036"},      // DIGIT SIX
      {"\x37", "\u0037"},      // DIGIT SEVEN
      {"\x38", "\u0038"},      // DIGIT EIGHT
      {"\x39", "\u0039"},      // DIGIT NINE
      {"\x3a", "\u003A"},      // COLON
      {"\x3b", "\u003B"},      // SEMICOLON
      {"\x3c", "\u003C"},      // LESS-THAN SIGN
      {"\x3d", "\u003D"},      // EQUALS SIGN
      {"\x3e", "\u003E"},      // GREATER-THAN SIGN
      {"\x3f", "\u003F"},      // QUESTION MARK
      {"\x40", "\u0040"},      // COMMERCIAL AT
      {"\x41", "\u0041"},      // LATIN CAPITAL LETTER A
      {"\x42", "\u0042"},      // LATIN CAPITAL LETTER B
      {"\x43", "\u0043"},      // LATIN CAPITAL LETTER C
      {"\x44", "\u0044"},      // LATIN CAPITAL LETTER D
      {"\x45", "\u0045"},      // LATIN CAPITAL LETTER E
      {"\x46", "\u0046"},      // LATIN CAPITAL LETTER F
      {"\x47", "\u0047"},      // LATIN CAPITAL LETTER G
      {"\x48", "\u0048"},      // LATIN CAPITAL LETTER H
      {"\x49", "\u0049"},      // LATIN CAPITAL LETTER I
      {"\x4a", "\u004A"},      // LATIN CAPITAL LETTER J
      {"\x4b", "\u004B"},      // LATIN CAPITAL LETTER K
      {"\x4c", "\u004C"},      // LATIN CAPITAL LETTER L
      {"\x4d", "\u004D"},      // LATIN CAPITAL LETTER M
      {"\x4e", "\u004E"},      // LATIN CAPITAL LETTER N
      {"\x4f", "\u004F"},      // LATIN CAPITAL LETTER O
      {"\x50", "\u0050"},      // LATIN CAPITAL LETTER P
      {"\x51", "\u0051"},      // LATIN CAPITAL LETTER Q
      {"\x52", "\u0052"},      // LATIN CAPITAL LETTER R
      {"\x53", "\u0053"},      // LATIN CAPITAL LETTER S
      {"\x54", "\u0054"},      // LATIN CAPITAL LETTER T
      {"\x55", "\u0055"},      // LATIN CAPITAL LETTER U
      {"\x56", "\u0056"},      // LATIN CAPITAL LETTER V
      {"\x57", "\u0057"},      // LATIN CAPITAL LETTER W
      {"\x58", "\u0058"},      // LATIN CAPITAL LETTER X
      {"\x59", "\u0059"},      // LATIN CAPITAL LETTER Y
      {"\x5a", "\u005A"},      // LATIN CAPITAL LETTER Z
      {"\x5b", "\u005B"},      // LEFT SQUARE BRACKET
      {"\x5c", "\u005C"},      // REVERSE SOLIDUS
      {"\x5d", "\u005D"},      // RIGHT SQUARE BRACKET
      {"\x5e", "\u005E"},      // CIRCUMFLEX ACCENT
      {"\x5f", "\u005F"},      // LOW LINE
      {"\x60", "\u0060"},      // GRAVE ACCENT
      {"\x61", "\u0061"},      // LATIN SMALL LETTER A
      {"\x62", "\u0062"},      // LATIN SMALL LETTER B
      {"\x63", "\u0063"},      // LATIN SMALL LETTER C
      {"\x64", "\u0064"},      // LATIN SMALL LETTER D
      {"\x65", "\u0065"},      // LATIN SMALL LETTER E
      {"\x66", "\u0066"},      // LATIN SMALL LETTER F
      {"\x67", "\u0067"},      // LATIN SMALL LETTER G
      {"\x68", "\u0068"},      // LATIN SMALL LETTER H
      {"\x69", "\u0069"},      // LATIN SMALL LETTER I
      {"\x6a", "\u006A"},      // LATIN SMALL LETTER J
      {"\x6b", "\u006B"},      // LATIN SMALL LETTER K
      {"\x6c", "\u006C"},      // LATIN SMALL LETTER L
      {"\x6d", "\u006D"},      // LATIN SMALL LETTER M
      {"\x6e", "\u006E"},      // LATIN SMALL LETTER N
      {"\x6f", "\u006F"},      // LATIN SMALL LETTER O
      {"\x70", "\u0070"},      // LATIN SMALL LETTER P
      {"\x71", "\u0071"},      // LATIN SMALL LETTER Q
      {"\x72", "\u0072"},      // LATIN SMALL LETTER R
      {"\x73", "\u0073"},      // LATIN SMALL LETTER S
      {"\x74", "\u0074"},      // LATIN SMALL LETTER T
      {"\x75", "\u0075"},      // LATIN SMALL LETTER U
      {"\x76", "\u0076"},      // LATIN SMALL LETTER V
      {"\x77", "\u0077"},      // LATIN SMALL LETTER W
      {"\x78", "\u0078"},      // LATIN SMALL LETTER X
      {"\x79", "\u0079"},      // LATIN SMALL LETTER Y
      {"\x7a", "\u007A"},      // LATIN SMALL LETTER Z
      {"\x7b", "\u007B"},      // LEFT CURLY BRACKET
      {"\x7c", "\u007C"},      // VERTICAL LINE
      {"\x7d", "\u007D"},      // RIGHT CURLY BRACKET
      {"\x7e", "\u007E"},      // TILDE
      {"\x7f", "\u007F"},      // DELETE
      {"\xc2\xa0", "\u00A0"},  // NO-BREAK SPACE
      {"\xc2\xa1", "\u00A1"},  // INVERTED EXCLAMATION MARK
      {"\xc2\xa2", "\u00A2"},  // CENT SIGN
      {"\xc2\xa3", "\u00A3"},  // POUND SIGN
      {"\xc2\xa4", "\u00A4"},  // CURRENCY SIGN
      {"\xc2\xa5", "\u00A5"},  // YEN SIGN
      {"\xc2\xa6", "\u00A6"},  // BROKEN BAR
      {"\xc2\xa7", "\u00A7"},  // SECTION SIGN
      {"\xc2\xa8", "\u00A8"},  // DIAERESIS
      {"\xc2\xa9", "\u00A9"},  // COPYRIGHT SIGN
      {"\xc2\xaa", "\u00AA"},  // FEMININE ORDINAL INDICATOR
      {"\xc2\xab", "\u00AB"},  // LEFT-POINTING DOUBLE ANGLE QUOTATION MARK
      {"\xc2\xac", "\u00AC"},  // NOT SIGN
      {"\xc2\xad", "\u00AD"},  // SOFT HYPHEN
      {"\xc2\xae", "\u00AE"},  // REGISTERED SIGN
      {"\xc2\xaf", "\u00AF"},  // MACRON
      {"\xc2\xb0", "\u00B0"},  // DEGREE SIGN
      {"\xc2\xb1", "\u00B1"},  // PLUS-MINUS SIGN
      {"\xc2\xb2", "\u00B2"},  // SUPERSCRIPT TWO
      {"\xc2\xb3", "\u00B3"},  // SUPERSCRIPT THREE
      {"\xc2\xb4", "\u00B4"},  // ACUTE ACCENT
      {"\xc2\xb5", "\u00B5"},  // MICRO SIGN
      {"\xc2\xb6", "\u00B6"},  // PILCROW SIGN
      {"\xc2\xb7", "\u00B7"},  // MIDDLE DOT
      {"\xc2\xb8", "\u00B8"},  // CEDILLA
      {"\xc2\xb9", "\u00B9"},  // SUPERSCRIPT ONE
      {"\xc2\xba", "\u00BA"},  // MASCULINE ORDINAL INDICATOR
      {"\xc2\xbb", "\u00BB"},  // RIGHT-POINTING DOUBLE ANGLE QUOTATION MARK
      {"\xc2\xbc", "\u00BC"},  // VULGAR FRACTION ONE QUARTER
      {"\xc2\xbd", "\u00BD"},  // VULGAR FRACTION ONE HALF
      {"\xc2\xbe", "\u00BE"},  // VULGAR FRACTION THREE QUARTERS
      {"\xc2\xbf", "\u00BF"},  // INVERTED QUESTION MARK
      {"\xc3\x80", "\u00C0"},  // LATIN CAPITAL LETTER A WITH GRAVE
      {"\xc3\x81", "\u00C1"},  // LATIN CAPITAL LETTER A WITH ACUTE
      {"\xc3\x82", "\u00C2"},  // LATIN CAPITAL LETTER A WITH CIRCUMFLEX
      {"\xc3\x83", "\u00C3"},  // LATIN CAPITAL LETTER A WITH TILDE
      {"\xc3\x84", "\u00C4"},  // LATIN CAPITAL LETTER A WITH DIAERESIS
      {"\xc3\x85", "\u00C5"},  // LATIN CAPITAL LETTER A WITH RING ABOVE
      {"\xc3\x86", "\u00C6"},  // LATIN CAPITAL LETTER AE
      {"\xc3\x87", "\u00C7"},  // LATIN CAPITAL LETTER C WITH CEDILLA
      {"\xc3\x88", "\u00C8"},  // LATIN CAPITAL LETTER E WITH GRAVE
      {"\xc3\x89", "\u00C9"},  // LATIN CAPITAL LETTER E WITH ACUTE
      {"\xc3\x8a", "\u00CA"},  // LATIN CAPITAL LETTER E WITH CIRCUMFLEX
      {"\xc3\x8b", "\u00CB"},  // LATIN CAPITAL LETTER E WITH DIAERESIS
      {"\xc3\x8c", "\u00CC"},  // LATIN CAPITAL LETTER I WITH GRAVE
      {"\xc3\x8d", "\u00CD"},  // LATIN CAPITAL LETTER I WITH ACUTE
      {"\xc3\x8e", "\u00CE"},  // LATIN CAPITAL LETTER I WITH CIRCUMFLEX
      {"\xc3\x8f", "\u00CF"},  // LATIN CAPITAL LETTER I WITH DIAERESIS
      {"\xc3\x90", "\u00D0"},  // LATIN CAPITAL LETTER ETH
      {"\xc3\x91", "\u00D1"},  // LATIN CAPITAL LETTER N WITH TILDE
      {"\xc3\x92", "\u00D2"},  // LATIN CAPITAL LETTER O WITH GRAVE
      {"\xc3\x93", "\u00D3"},  // LATIN CAPITAL LETTER O WITH ACUTE
      {"\xc3\x94", "\u00D4"},  // LATIN CAPITAL LETTER O WITH CIRCUMFLEX
      {"\xc3\x95", "\u00D5"},  // LATIN CAPITAL LETTER O WITH TILDE
      {"\xc3\x96", "\u00D6"},  // LATIN CAPITAL LETTER O WITH DIAERESIS
      {"\xc3\x97", "\u00D7"},  // MULTIPLICATION SIGN
      {"\xc3\x98", "\u00D8"},  // LATIN CAPITAL LETTER O WITH STROKE
      {"\xc3\x99", "\u00D9"},  // LATIN CAPITAL LETTER U WITH GRAVE
      {"\xc3\x9a", "\u00DA"},  // LATIN CAPITAL LETTER U WITH ACUTE
      {"\xc3\x9b", "\u00DB"},  // LATIN CAPITAL LETTER U WITH CIRCUMFLEX
      {"\xc3\x9c", "\u00DC"},  // LATIN CAPITAL LETTER U WITH DIAERESIS
      {"\xc3\x9d", "\u00DD"},  // LATIN CAPITAL LETTER Y WITH ACUTE
      {"\xc3\x9e", "\u00DE"},  // LATIN CAPITAL LETTER THORN
      {"\xc3\x9f", "\u00DF"},  // LATIN SMALL LETTER SHARP S
      {"\xc3\xa0", "\u00E0"},  // LATIN SMALL LETTER A WITH GRAVE
      {"\xc3\xa1", "\u00E1"},  // LATIN SMALL LETTER A WITH ACUTE
      {"\xc3\xa2", "\u00E2"},  // LATIN SMALL LETTER A WITH CIRCUMFLEX
      {"\xc3\xa3", "\u00E3"},  // LATIN SMALL LETTER A WITH TILDE
      {"\xc3\xa4", "\u00E4"},  // LATIN SMALL LETTER A WITH DIAERESIS
      {"\xc3\xa5", "\u00E5"},  // LATIN SMALL LETTER A WITH RING ABOVE
      {"\xc3\xa6", "\u00E6"},  // LATIN SMALL LETTER AE
      {"\xc3\xa7", "\u00E7"},  // LATIN SMALL LETTER C WITH CEDILLA
      {"\xc3\xa8", "\u00E8"},  // LATIN SMALL LETTER E WITH GRAVE
      {"\xc3\xa9", "\u00E9"},  // LATIN SMALL LETTER E WITH ACUTE
      {"\xc3\xaa", "\u00EA"},  // LATIN SMALL LETTER E WITH CIRCUMFLEX
      {"\xc3\xab", "\u00EB"},  // LATIN SMALL LETTER E WITH DIAERESIS
      {"\xc3\xac", "\u00EC"},  // LATIN SMALL LETTER I WITH GRAVE
      {"\xc3\xad", "\u00ED"},  // LATIN SMALL LETTER I WITH ACUTE
      {"\xc3\xae", "\u00EE"},  // LATIN SMALL LETTER I WITH CIRCUMFLEX
      {"\xc3\xaf", "\u00EF"},  // LATIN SMALL LETTER I WITH DIAERESIS
      {"\xc3\xb0", "\u00F0"},  // LATIN SMALL LETTER ETH
      {"\xc3\xb1", "\u00F1"},  // LATIN SMALL LETTER N WITH TILDE
      {"\xc3\xb2", "\u00F2"},  // LATIN SMALL LETTER O WITH GRAVE
      {"\xc3\xb3", "\u00F3"},  // LATIN SMALL LETTER O WITH ACUTE
      {"\xc3\xb4", "\u00F4"},  // LATIN SMALL LETTER O WITH CIRCUMFLEX
      {"\xc3\xb5", "\u00F5"},  // LATIN SMALL LETTER O WITH TILDE
      {"\xc3\xb6", "\u00F6"},  // LATIN SMALL LETTER O WITH DIAERESIS
      {"\xc3\xb7", "\u00F7"},  // DIVISION SIGN
      {"\xc3\xb8", "\u00F8"},  // LATIN SMALL LETTER O WITH STROKE
      {"\xc3\xb9", "\u00F9"},  // LATIN SMALL LETTER U WITH GRAVE
      {"\xc3\xba", "\u00FA"},  // LATIN SMALL LETTER U WITH ACUTE
      {"\xc3\xbb", "\u00FB"},  // LATIN SMALL LETTER U WITH CIRCUMFLEX
      {"\xc3\xbc", "\u00FC"},  // LATIN SMALL LETTER U WITH DIAERESIS
      {"\xc3\xbd", "\u00FD"},  // LATIN SMALL LETTER Y WITH ACUTE
      {"\xc3\xbe", "\u00FE"},  // LATIN SMALL LETTER THORN
      {"\xc3\xbf", "\u00FF"}   // LATIN SMALL LETTER Y WITH DIAERESIS
  };

  for (std::size_t i = 0; i < str.size(); ++i) {
    std::string c(1, str[i]);
    // If the character is in the map, replace it with its Unicode equivalent
    if (hex_to_unicode.find(c) != hex_to_unicode.end()) {
      new_str += hex_to_unicode[c];
    } else {
      // If not, just append the character as is
      new_str += c;
    }
  }

  return new_str;
}

// Replace characters that are not letters, numbers, or underscores with a space
std::string replace_non_alphanumeric_with_space(const std::string& str) {
  std::regex re("[^a-zA-Z0-9_]");
  return std::regex_replace(str, re, " ");
}

// Replace interior spaces and punctuation with underscores
std::string replace_interior_spaces_and_punctuation(const std::string& str) {
  std::regex re("[\\s[:punct:]]+");
  std::regex re2("_+");
  return std::regex_replace(std::regex_replace(str, re, "_"), re2, "_");
}

// Replace multiple spaces with a single space
std::string replace_multiple_spaces(const std::string& str) {
  std::regex re("\\s+");
  return std::regex_replace(str, re, " ");
}

// Remove leading and trailing spaces
std::string remove_leading_trailing_spaces(const std::string& str) {
  std::regex re("^\\s+|\\s+$");
  return std::regex_replace(str, re, "");
}

// Ensure lowercase
std::string to_lower(const std::string& str) {
  std::string lower_str = str;
  std::transform(lower_str.begin(), lower_str.end(), lower_str.begin(), ::tolower);
  return lower_str;
}

// Ensure unique names
std::vector<std::string> unique_names(const std::vector<std::string>& names) {
  std::unordered_map<std::string, int> name_count;
  std::vector<std::string> unique_names;

  for (const auto& name : names) {
    if (name_count[name] == 0) {
      unique_names.push_back(name);
    } else {
      std::ostringstream oss;
      oss << name << "_" << name_count[name];
      unique_names.push_back(oss.str());
    }
    name_count[name]++;
  }

  return unique_names;
}

// Convert cpp11::strings to std::vector<std::string>
std::vector<std::string> cpp11strings_to_stdvector(const cpp11::strings& input) {
  std::vector<std::string> output;
  output.reserve(input.size());
  for (const auto& s : input) {
    output.push_back(static_cast<std::string>(s));
  }
  return output;
}

// Main clean_names functions

std::vector<std::string> tidy_std_names(std::vector<std::string>& input) {
  std::vector<std::string> names(input.begin(), input.end());

  std::transform(names.begin(), names.end(), names.begin(), [](std::string& name) {
    name = remove_starting_spaces_and_punctuation(name);
    name = remove_special_characters(name);
    name = replace_non_alphanumeric_with_space(name);
    name = replace_interior_spaces_and_punctuation(name);
    name = remove_ending_spaces_and_punctuation(name);
    return remove_edge_underscores(name);
  });

  // Ensure lowercase
  std::transform(names.begin(), names.end(), names.begin(),
                 [](std::string& name) { return to_lower(name); });

  // Ensure unique names
  names = unique_names(names);

  return names;
}

std::vector<std::string> tidy_std_vars(std::vector<std::string>& input) {
  std::vector<std::string> names(input.begin(), input.end());

  std::transform(names.begin(), names.end(), names.begin(), [](std::string& name) {
    name = replace_multiple_spaces(name);
    name = recode_special_characters(name);
    name = remove_leading_trailing_spaces(name);
    return name;
  });

  return names;
}

// Export to R

cpp11::strings tidy_r_names(const cpp11::writable::strings& input) {
  // cpp11::strings -> std::vector<std::string>
  std::vector<std::string> names = cpp11strings_to_stdvector(input);

  names = tidy_std_names(names);

  // std::vector<std::string> -> cpp11::strings
  cpp11::writable::strings result(names.size());
  std::transform(names.begin(), names.end(), result.begin(),
                 [](const std::string& s) { return s; });

  return result;
}

cpp11::strings tidy_r_vars(const cpp11::writable::strings& input) {
  // cpp11::strings -> std::vector<std::string>
  std::vector<std::string> names = cpp11strings_to_stdvector(input);

  names = tidy_std_vars(names);

  // std::vector<std::string> -> cpp11::strings
  cpp11::writable::strings result(names.size());
  std::transform(names.begin(), names.end(), result.begin(),
                 [](const std::string& s) { return s; });

  return result;
}

#endif  // TIDY_HPP
