#' Vendor the cpp11 and cpp11janitor dependency
#'
#' Vendoring is the act of making your own copy of the 3rd party packages your
#' project is using. It is often used in the go language community.
#'
#' This function vendors cpp11 and cpp11janitor into your package by copying
#' the cpp11 and cpp11janitor headers into the `inst/include` folder and
#' adding 'cpp11 version: XYZ' and 'cpp11janitor version: XYZ' to the top of
#' the files, where XYZ is the version of cpp11 and cpp11janitor currently
#' installed on your machine.
#'
#' Vendoring places the responsibility of updating the code on
#' you. Bugfixes and new features in cpp11 and cpp11janitor will not be
#' available for your code until you run `cpp_vendor()` again.
#'
#' @param dir The directory to vendor the code into.
#' @param subdir The subdirectory to vendor the code into.
#' @return The file path to the vendored code (invisibly).
#' @export
#' @examples
#' # create a new directory
#' dir <- tempdir()
#' dir.create(dir)
#'
#' # vendor the cpp11 headers into the directory
#' cpp_vendor(dir)
cpp_vendor <- function(dir = NULL, subdir = "/inst/include") {
  if (is.null(dir)) {
    stop("You must provide a path to vendor the code into", call. = FALSE)
  }

  path <- paste0(dir, subdir)

  path2 <- file.path(path, "cpp11")
  if (dir.exists(path2)) {
    stop("'", path2, "' already exists\n * run unlink('", path2, "', recursive = TRUE)", call. = FALSE)
  }

  # Vendor cpp11 ----

  dir.create(
    path2,
    recursive = TRUE,
    showWarnings = FALSE
  )

  current_cpp11 <- system.file(
    "include",
    "cpp11",
    package = "cpp11"
  )

  if (!nzchar(current_cpp11)) {
    stop("cpp11 is not installed", call. = FALSE)
  }

  cpp11_version <- utils::packageVersion("cpp11")

  cpp11_header <- sprintf(
    "// cpp11 version: %s\n// vendored on: %s",
    cpp11_version,
    Sys.Date()
  )

  write_header(
    path, "cpp11.hpp", "cpp11",
    cpp11_header
  )

  copy_files(
    list.files(current_cpp11, full.names = TRUE),
    path, "cpp11", cpp11_header
  )

  # Vendor cpp11janitor ----

  dir.create(
    file.path(path, "janitor"),
    recursive = TRUE,
    showWarnings = FALSE
  )

  current_janitor <- system.file(
    "include",
    "cpp11janitor.hpp",
    package = "cpp11janitor"
  )

  if (!nzchar(current_janitor)) {
    stop("cpp11janitor is not installed", call. = FALSE)
  }

  janitor_version <- utils::packageVersion("cpp11janitor")

  janitor_header <- sprintf(
    "// cpp11janitor version: %s\n// vendored on: %s",
    janitor_version,
    Sys.Date()
  )

  write_header(
    path, "cpp11janitor.hpp", "cpp11janitor",
    janitor_header
  )

  # Additional steps to make vendoring work ----

  message(paste(
    "Makevars and/or Makevars.win should have a line such as",
    "'PKG_CPPFLAGS = -I../inst/include'"
  ))

  message("DESCRIPTION should not have lines such as 'LinkingTo: cpp11'")

  invisible(path)
}

write_header <- function(path, header, pkg, janitor_header) {
  writeLines(
    c(
      janitor_header,
      readLines(
        system.file("include", header, package = pkg)
      )
    ),
    file.path(path, header)
  )
}

copy_files <- function(files, path, out, janitor_header) {
  for (f in files) {
    writeLines(
      c(janitor_header, readLines(f)),
      file.path(path, out, basename(f))
    )
  }
}
