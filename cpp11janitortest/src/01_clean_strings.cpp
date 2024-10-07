#include "00_main.h"
#include "cpp11janitor.hpp"

[[cpp11::register]] cpp11::writable::strings test_tidy_r_names(const cpp11::strings& x) {
  cpp11::writable::strings res = tidy_r_names(x);
  return res;
}

[[cpp11::register]] cpp11::writable::strings test_tidy_r_vars(const cpp11::strings& x) {
  cpp11::writable::strings res = tidy_r_vars(x);
  return res;
}
