# Changelog {#about_changelog}

[TOC]

This changelog contains a top-level entry for each release with sections on new features, API changes and notable
bug-fixes (not all bug-fixes will be listed).

Get to know SeqAn3 with our [tutorials](https://docs.seqan.de/seqan/3-master-user/usergroup1.html).

Please see the release announcement: https://www.seqan.de/announcing-seqan3/

See the porting guide for some help on porting: https://docs.seqan.de/seqan/3-master-user/howto_porting.html

See the documentation on [API stability](https://docs.seqan.de/seqan/3-master-user/about_api.html) to learn about
when API changes are allowed.

<!--
The following API changes should be documented as such:
  * a previously experimental interface now being marked as stable
  * an interface being removed
  * syntactical changes to an interface (e.g. renaming or reordering of files, functions, parameters)
  * semantic changes to an interface (e.g. a function's result is now always one larger) [DANGEROUS!]

If possible, provide tooling that performs the changes, e.g. a shell-script.
-->

## API changes

#### I/O

* In order to avoid using the seqan3 I/O module, you now have to give a list of file extensions explicitly to
`sharg::input_file_validator` and `sharg::output_file_validator`:
For example `sharg::input_file_validator validator{std::vector<std::string>{{"exe"}, {"fasta"}}};`. Please follow
https://github.com/seqan/seqan3/issues/2927 to see how the list of file extensions can be extracted from seqan3 files.
We also removed the `default_extensions()` function, as we now can construct `output_file_validator` with just a given
mode: `output_file_validator(output_file_open_options const mode)`. The extensions will be an empty array in this case.
