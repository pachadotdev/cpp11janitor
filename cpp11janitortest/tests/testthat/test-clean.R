test_that("return clean strings", {
  # setwd("~/github/cpp11janitor/"); install()
  # setwd("~/github/cpp11janitor/cpp11janitortest/"); cpp11::cpp_register(); clean_dll(); load_all()

  cols <- c("DEPTO_REF_ID", "CPV2011_REF_ID", "IDDEPTO", "DEPTO", "NDEPTO", "REDCODEN")

  res1 <- test_tidy_r_names(cols)

  expect_equal(res1[1], "depto_ref_id")

  # intentionally mess the colnames
  cols[1] <- "  DEPTO. .REF_ID_ "

  res2 <- test_tidy_r_names(cols)

  expect_equal(res2[1], "depto_ref_id")

  vars <- c("Montevideo", "Paysand\xfa ", " San Jos\xe9")

  res3 <- test_tidy_r_names(vars)

  expect_equal(res3[3], "san_jos")

  res4 <- test_tidy_r_vars(vars)

  expect_equal(res4[3], "San Jos\xe9")
})
