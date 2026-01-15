# Changelog {#about_changelog}

<!--
SPDX-FileCopyrightText: 2006-2026 Knut Reinert & Freie Universität Berlin
SPDX-FileCopyrightText: 2016-2026 Knut Reinert & MPI für molekulare Genetik
SPDX-License-Identifier: CC-BY-4.0
-->

[TOC]

This changelog contains a top-level entry for each release with sections on new features, API changes and notable
bug-fixes (not all bug-fixes will be listed).

See the Sharg documentation on [API stability](https://docs.seqan.de/sharg/main_user/about_api.html) to learn about
when API changes are allowed in the Sharg-parser.

<!--
The following API changes should be documented as such:
  * a previously experimental interface now being marked as stable
  * an interface being removed
  * syntactical changes to an interface (e.g. renaming or reordering of files, functions, parameters)
  * semantic changes to an interface (e.g. a function's result is now always one larger) [DANGEROUS!]

If possible, provide tooling that performs the changes, e.g. a shell-script.
-->

# Release 1.2.0

## Features

* **Recursive subcommands**: Subcommands can now have their own subcommands, allowing for more complex command-line
  interfaces (e.g., `git remote add`) ([#233](https://github.com/seqan/sharg-parser/pull/233)).
* **Auto-generated synopsis**: The synopsis line shown in help pages is now automatically generated based on the
  configured options and flags. A synopsis is only generated if no synopsis is given. You can deactivate this by
  setting `parser.info.synopsis = {""};` ([#296](https://github.com/seqan/sharg-parser/pull/296)).
* **Subparser metadata propagation**: Some metadata (e.g., `version`, `app_name`) is now automatically copied to
  subparsers ([#285](https://github.com/seqan/sharg-parser/pull/285)).
* **Multiple citations**: Applications can now specify multiple citations instead of just one
  ([#280](https://github.com/seqan/sharg-parser/pull/280)).
* `sharg::parser_meta_data` now supports designated initializers
  ([#285](https://github.com/seqan/sharg-parser/pull/285)).

## Bug fixes

* Fixed `is_option_set()` to correctly match both long and short option identifiers
  ([#226](https://github.com/seqan/sharg-parser/pull/226)).
* Fixed an issue where having multiple flags refering to the same `bool value` (`add_flag(value, ...);`) resulted in
  not setting `value` to `true` ([#243](https://github.com/seqan/sharg-parser/pull/243)).
* Fixed CWL/CTD export to correctly mark positional options as required (positional list options remain optional)
  ([#237](https://github.com/seqan/sharg-parser/pull/237)).
* Fixed man page export to use lowercase application name ([#296](https://github.com/seqan/sharg-parser/pull/296)).

## API changes

#### Compiler

* We now use C++23.
* Compiler support (tested):
  * GCC 13, 14, 15
  * Clang 19, 20, 21
  * Intel oneAPI C++ Compiler 2025.0 (IntelLLVM)
  * Other compilers might work but are not tested, GCC < 12 and Clang < 17 are known to not work.

#### Dependencies

* We now use Doxygen version 1.10.0 to build our documentation
  ([#230](https://github.com/seqan/sharg-parser/pull/230)).
* Switched from git submodules to CPM (CMake Package Manager) for dependency management
  ([#260](https://github.com/seqan/sharg-parser/pull/260)).

# Release 1.1.2

## API changes

#### Dependencies
  * TDL is now an optional dependency and can be force deactivated via CMake (`-DSHARG_NO_TDL=ON`)
    ([#218](https://github.com/seqan/sharg-parser/pull/218)).
  * We now use Doxygen version 1.10.0 to build our documentation
    ([\#230](https://github.com/seqan/sharg-parser/pull/230)).

# Release 1.1.1

## Bug fixes

* Fixed installation of Sharg via `make install` ([#202](https://github.com/seqan/sharg-parser/pull/202)).

## API changes

#### Compiler

* Dropped support for gcc-10 ([#199](https://github.com/seqan/sharg-parser/pull/199)).
* Added support for gcc-13 ([#199](https://github.com/seqan/sharg-parser/pull/199)).
* Added support for clang-17 ([#197](https://github.com/seqan/sharg-parser/pull/197),
  [#198](https://github.com/seqan/sharg-parser/pull/198)).

# Release 1.1.0

## Features

* We support Gitpod. [Click here](https://gitpod.io/#https://github.com/seqan/sharg-parser/)
  to try it out.
* **CWL support** (Common Workflow Language): An app using Sharg can now automatically export a CWL tool
  description file via `--export-help cwl` ([#94](https://github.com/seqan/sharg-parser/pull/94)).

## Bug fixes

* When using subcommand parsers, e.g. `git push`, typing `git puhs -h` will raise an exception that the user misspelled
  the subcommand instead of printing the help page of `git` ([\#172](https://github.com/seqan/sharg-parser/pull/172)).
* Fixed an issue that caused the validation of a directory via the `sharg::output_file_validator` to delete the
  directory's contents ([\#175](https://github.com/seqan/sharg-parser/pull/175)).
* Segmentation fault when using `sharg::value_list_validator` in conjuction with a `std::filesystem::path` option
  ([\#179](https://github.com/seqan/sharg-parser/pull/179)).

## API changes

#### Dependencies
  * We now use Doxygen version 1.9.5 to build our documentation ([\#145](https://github.com/seqan/sharg-parser/pull/145)).
  * We require at least CMake 3.16 for our test suite. Note that the minimum requirement for using Sharg is unchanged
    ([\#135](https://github.com/seqan/sharg-parser/pull/135)).

# Release 1.0.0

We are happy to release the first version of the SeqAn Sharg parser!

The Sharg parser succeeds the former `seqan3::argument_parser` with a few new features in a light-weight repository.
Most notably, we switched to
[*Designated initializers*](https://en.cppreference.com/w/cpp/language/aggregate_initialization#Designated_initializers)
for configuring (positional) options and flags when adding them to the parser. See `API changes` for more details.

From this release on, most of the **API is now stable**. Stable entities are marked as such in our
[online documentation](https://docs.seqan.de/sharg/main_user/classsharg_1_1parser.html).

## New features

Besides the new `sharg::config` API using designated initializers, which is described in `API changes`,
you can now alter the default message printed on the help page. E.g.
```cpp
int i{0};
parser.add_option(val, sharg::config{.short_id = 'i', .default_message = "Calculated from your data"});
```
prints
```
    -i (signed 32 bit integer)
    Default: Calculated from your data.
```
instead of `Default: 0.`. See our online
[documentation](https://docs.seqan.de/sharg/main_user/structsharg_1_1config.html#aec21e88c7a32f4c0cfab9970de89df71)
for more details.

## API changes

#### Name changes

If you are switching form the `seqan3::argument_parser` to the `sharg::parser` there are several name changes.
All of them can be fixed with a simple search & replace:
* The namespace of all entities is now `sharg` instead of `seqan3`
* Every occurrence of `argument_parser` has been replaced with `parser`
* The concept `seqan::parser_compatible_option` has been renamed to `sharg::parsable`

#### add_option/add_flag/add_positional_option

**!Important!** New API of `add_option()/add_flag()/add_positional_option()` calls that is more descriptive and flexible.
An option flag or positional option is added with only two parameters:
(1) Its value that stores the command line parameter (nothing changed here)
(2) A `sharg::config` object (NEW)

Before:
```cpp
parser.add_option(val, 'i', "int", "some int");
```
Now:
```cpp
parser.add_option(val, sharg::config{.short_id = 'i', .long_id = "int", .description = "some int"});
```
We take advantage of [*Designated initializers*](https://en.cppreference.com/w/cpp/language/aggregate_initialization#Designated_initializers)
that make the call much more descriptive and flexible.
E.g., you can leave out parameters you don't need, but beware that the order must be as specified in `sharg::config`.

You can now set an option as required without the need of the `sharg::option_spec`
```cpp
parser.add_option(val, sharg::config{.short_id = 'i', .required = true});
```

**!Important!** We removed the `sharg::option_spec` as it is obsolete in the new API.

#### Concepts

* Custom option types must not only model `sharg::istreamable` (`stream >> option`)
  but must also model `sharg::ostreamable` in order to be used in `parser.add_option()` calls.
  All standard types as well as types that overload `sharg::named_enumeration` are not affected.

#### Validators

* In order to avoid using the seqan3 I/O module, you now have to give a list of file extensions explicitly to
`sharg::input_file_validator` and `sharg::output_file_validator`:
For example `sharg::input_file_validator validator{std::vector<std::string>{{"exe"}, {"fasta"}}};`. Please follow
https://github.com/seqan/seqan3/issues/2927 to see how the list of file extensions can be extracted from seqan3 files.
We also removed the `default_extensions()` function, as we now can construct `output_file_validator` with just a given
mode: `output_file_validator(output_file_open_options const mode)`. The extensions will be an empty array in this case.
