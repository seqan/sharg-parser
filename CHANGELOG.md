# Changelog {#about_changelog}

[TOC]

This changelog contains a top-level entry for each release with sections on new features, API changes and notable
bug-fixes (not all bug-fixes will be listed).

See the SeqAn documentation on [API stability](https://docs.seqan.de/seqan/3-master-user/about_api.html) to learn about
when API changes are allowed in the Sharg-parser.

<!--
The following API changes should be documented as such:
  * a previously experimental interface now being marked as stable
  * an interface being removed
  * syntactical changes to an interface (e.g. renaming or reordering of files, functions, parameters)
  * semantic changes to an interface (e.g. a function's result is now always one larger) [DANGEROUS!]

If possible, provide tooling that performs the changes, e.g. a shell-script.
-->

## API changes

#### add_option/add_flag/add_positional_option

**!Important!** New API of `add_option/add_flag/add_positional_option` calls that is more descriptive and flexible.
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
