# Changelog {#about_changelog}

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
