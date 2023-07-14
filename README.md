# build-tool

This project is an example project generated with [cmake-init][1] with the
purpose of showing off how to use a build tool that is also provided in the
source tree of the project.

## Purpose

This project was inspired by
[CMake: A Case Study - Hans Vredeveld - ACCU 2023][2] with the purpose of not
only showing off what the presentation shows off, but also how to deal with a
build tool similar to LLVM's tblgen.

There are some limitations as to what is possible with build time tools like
this due to generator expressions not being powerful enough to make things
work flawlessly for multi config generators. One should make sure that
building with at least one config using a multi config generator works to make
the project usable by anyone.

The included commands `prec_target_sources` and
`prec_target_include_directories` do not support support generator expressions
and `prec_target_include_directories` can only specify build requirements
(`PRIVATE`).

> **Note**
> CMake gained [path generator expressions][3] in 3.24, which do solve the
> issues above, but those are not shown off here in the interest of staying
> close to the presentation's content.

[1]: https://github.com/friendlyanon/cmake-init
[2]: https://www.youtube.com/watch?v=8l53O3FaJdM
[3]: https://cmake.org/cmake/help/latest/manual/cmake-generator-expressions.7.html#path-expressions
