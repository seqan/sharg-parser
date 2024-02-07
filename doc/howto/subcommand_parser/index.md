# How to write a parser with subcommands {#subcommand_parse}

<!--
SPDX-FileCopyrightText: 2006-2024 Knut Reinert & Freie Universität Berlin
SPDX-FileCopyrightText: 2016-2024 Knut Reinert & MPI für molekulare Genetik
SPDX-License-Identifier: CC-BY-4.0
-->

[TOC]

This HowTo shows you how to write a parser with subcommands like `git push` using Sharg.

\tutorial_head{Easy, 15 min, \ref tutorial_parser, }

# Motivation

A common use case for command line tools, e.g. `git`, is to have multiple subcommands, e.g. `git fetch` or `git push`.
Each subcommand has its own set of options and its own help page.
This HowTo explains how this can be done with the sharg::parser and serves as a copy'n'paste source.
If you are new to Sharg, we recommend to do the basic
\link tutorial_parser parser tutorial \endlink before you read further.

# A subcommand parser

In order to keep parsing with subcommands straightforward and simple,
the sharg::parser provides an advanced interface that internally takes care of the correct input parsing.

You simply need to specify the names of the subcommands when constructing your top-level parser:

\snippet doc/howto/subcommand_parser/subcommand_parse.cpp construction

\attention You can still add flags to your top-level parser if needed but **no (positional) options**.
This avoids ambiguous parsing (e.g. subcommand fasta given file extension fasta
`./myfasta_parser --filext fasta fasta ...`).

After calling sharg::parser::parse() on your top-level parser,
you can then access the sub-parser via the function sharg::parser::get_sub_parser():

\snippet doc/howto/subcommand_parser/subcommand_parse.cpp get_sub_parser

The sub-parser's **sharg::parser::info::app_name will be set to the user chosen sub command**.
For example, if the user calls

```
max$ ./mygit push -h
```

then the sub-parser will be named `mygit-push` and will be instantiated with all arguments
followed by the keyword `push` which in this case triggers printing the help page (`-h`).

That's it. Here is a full example of a subcommand parser you can try and adjust to your needs:

\include doc/howto/subcommand_parser/subcommand_parse.cpp
