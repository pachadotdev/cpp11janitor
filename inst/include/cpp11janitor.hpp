#ifndef TIDY_HPP
#define TIDY_HPP

#include <algorithm>
#include <cpp11.hpp>
#include <regex>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

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

// Remove \xXX
std::string remove_special_characters(const std::string& str) {
  std::string new_str;
  std::string::size_type pos = 0;

  while (pos < str.size()) {
    if (str[pos] == '\\' && pos + 3 < str.size() && str[pos + 1] == 'x') {
      // Skip past the \xXX sequence
      pos += 4;
    } else {
      // Append the current character if not part of a \x sequence
      new_str += str[pos];
      pos++;
    }
  }

  return new_str;
}

// Recode \xXX
std::string recode_special_characters(const std::string& str) {
  std::string new_str;
  std::string::size_type pos = 0;

  while (pos < str.size()) {
    // Look for the literal "\x" sequence in the string
    if (pos + 3 < str.size() && str[pos] == '\\' && str[pos + 1] == 'x') {
      // Extract the "XX" part after "\x"
      std::string hex = str.substr(pos + 2, 2);

      // Check if the extracted part is a valid hexadecimal number
      if (std::isxdigit(hex[0]) && std::isxdigit(hex[1])) {
        // Replace \xXX with \u00XX format
        new_str += "\\u00" + hex;

        // Skip past the "\xXX" sequence
        pos += 4;
      } else {
        // If not a valid hex, treat it as normal characters
        new_str += str[pos];
        pos++;
      }
    } else {
      // Append normal characters
      new_str += str[pos];
      pos++;
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

  // Remove starting spaces and punctuation +
  // Replace characters that are not letters, numbers, or underscores with a space +
  // Replace interior spaces and punctuation with underscores
  std::transform(names.begin(), names.end(), names.begin(), [](std::string& name) {
    name = remove_starting_spaces_and_punctuation(name);
    name = remove_special_characters(name);
    name = replace_non_alphanumeric_with_space(name);
    name = replace_interior_spaces_and_punctuation(name);
    return remove_ending_spaces_and_punctuation(name);
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

  // Replace multiple spaces with a single space
  std::transform(names.begin(), names.end(), names.begin(),
                 [](std::string& name) { return replace_multiple_spaces(name); });

  // Recode special characters
  std::transform(names.begin(), names.end(), names.begin(),
                 [](std::string& name) { return recode_special_characters(name); });

  // Remove leading and trailing spaces
  std::transform(names.begin(), names.end(), names.begin(),
                 [](std::string& name) { return remove_leading_trailing_spaces(name); });

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
