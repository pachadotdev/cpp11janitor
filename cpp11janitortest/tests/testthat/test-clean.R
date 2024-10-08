test_that("return clean strings", {
  setwd("~/github/cpp11janitor/"); install()
  setwd("~/github/cpp11janitor/cpp11janitortest/"); cpp11::cpp_register(); clean_dll(); load_all()

  cols <- c("DEPTO_REF_ID", "CPV2011_REF_ID", "IDDEPTO", "DEPTO", "NDEPTO", "REDCODEN")

  res1 <- test_tidy_r_names(cols)

  expect_equal(res1[1], "depto_ref_id")

  # intentionally mess the colnames
  cols[1] <- "  DEPTO. .REF_ID_ "

  res2 <- test_tidy_r_names(cols)

  expect_equal(res2[1], "depto_ref_id")

  vars <- c("Montevideo", "Paysand\xc3\xba ", " San Jos\xc3\xa9")

  res3 <- test_tidy_r_names(vars)

  expect_equal(res3[3], "san_jose")

  res4 <- test_tidy_r_vars(vars)

  expect_equal(res4[3], "San Jos\u00e9")

  # German
  vars <- "Gau\xc3\x9f"
  expect_equal(test_tidy_r_names(vars), "gau")
  expect_equal(test_tidy_r_vars(vars), "Gau\u00df")

  # French
  vars <- "c\xc2\xb4est-\xc3\xa0-dire"
  expect_equal(test_tidy_r_names(vars), "c_est_a_dire")
  expect_equal(test_tidy_r_vars(vars), "c\u00b4est-\u00e0-dire")

  # Spanish
  vars <- "\xc2\xbfC\xc3\xb3mo est\xc3\xa1s\x3f"
  expect_equal(test_tidy_r_names(vars), "como_estas")
  expect_equal(test_tidy_r_vars(vars), "\u00bfC\u00f3mo est\u00e1s\u003f")

  # Japanese
  vars <- "Konnichiwa \xe3\x81\x93\xe3\x82\x93\xe3\x81\xab\xe3\x81\xa1\xe3\x81\xaf"
  expect_equal(test_tidy_r_names(vars), "konnichiwa")
  expect_equal(test_tidy_r_vars(vars), "Konnichiwa \u3053\u3093\u306b\u3061\u306f")
})
