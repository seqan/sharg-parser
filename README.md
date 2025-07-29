# [![sharg_logo][sharg_logo_link]][sharg_link] Sharg -- hungrily eating away your arguments
[![build status][ci_badge]][github_actions]
[![codecov][codecov_badge]][codecov_link]
[![license][copy_badge]][copy_link]
[![platforms][api_badge]][api_link]
[![twitter][twitter_badge]][twitter_link]

<!--
    SPDX-FileCopyrightText: 2006-2025 Knut Reinert & Freie Universität Berlin
    SPDX-FileCopyrightText: 2016-2025 Knut Reinert & MPI für molekulare Genetik
    SPDX-License-Identifier: CC-BY-4.0
-->

<!--
    Above uses reference-style links with numbers.
    See also https://github.com/adam-p/markdown-here/wiki/Markdown-Cheatsheet#links.

    For example, `[![build status][1]][2]` evaluates to the following:
        `[link_text][2]`
        `[2]` is a reference to a link, i.e. `[link_text](https://...)`

        `[link_text]` = `[![build status][1]]`
        `[1]` is once again a reference to a link - this time an image, i.e. `[![build status](https://...)]
        `![build status]` is the text that should be displayed if the linked resource (`[1]`) is not available

    `[![build status][1]][2]` hence means:
    Show the picture linked under `[1]`. In case it cannot be displayed, show the text "build status" instead.
    The picture, or alternative text, should link to `[2]`.
-->

<!-- Use the Sharg logo within the repository. --->
[sharg_logo_link]: ./test/documentation/sharg_logo.svg "Open documentation"
<!-- Link the logo to the documentation website. --->
[sharg_link]: https://docs.seqan.de/sharg.html

<!--
    This is the CI badge image:
        `https://img.shields.io/github/workflow/status/` - we do not use GitHub's badges as they are not customisable.
        `/seqan/sharg-parser/` - owner/repository
        `CI%20on%20Linux` - name of the workflow as encoded URL (e.g., whitespace = %20)
        `main` - branch to show
        `?style=flat&logo=github` - use a GitHub-style badge
        `&label=sharg-parser%20CI` - text on the badge
        `"Open GitHub actions page"` - this text will be shown on hover
-->
[ci_badge]: https://img.shields.io/github/actions/workflow/status/seqan/sharg-parser/ci_linux.yml?branch=main&style=flat&logo=github&label=Sharg%20CI "Open GitHub actions page"
<!--
    This is the CI badge link:
        `https://github.com/seqan/sharg-parser/actions` - actions page of owner(seqan)/repository(sharg-parser)
        `?query=branch%3Amain` - only show actions that ran on the mater branch
-->
[github_actions]: https://github.com/seqan/sharg-parser/actions?query=branch%3Amain
<!--
    This is the Codecov badge image:
        Codecov offers badges: https://app.codecov.io/gh/seqan/sharg-parser/settings/badge
        While being logged in into Codecov, navigate to Settings->Badge and copy the markdown badge.
        Copy the image part of the markdown badge here.
    `"Open Codecov page"` - this text will be shown on hover
-->
[codecov_badge]: https://codecov.io/gh/seqan/sharg-parser/branch/main/graph/badge.svg?token=KIdo8b91jb "Open Codecov page"
<!--
    This is the Codecov badge link:
        Codecov offers badges: https://app.codecov.io/gh/seqan/sharg-parser/settings/badge
        While being logged in into Codecov, navigate to Settings->Badge and copy the markdown badge.
        Copy the URL part of the markdown badge here.
-->
[codecov_link]: https://codecov.io/gh/seqan/sharg-parser

[copy_badge]: https://img.shields.io/badge/license-BSD-green.svg "Open Copyright page"
<!--
    This is the Copyright badge link:
        `"Open Copyright page"` - this text will be shown on hover
-->
[copy_link]: https://docs.seqan.de/sharg/main_user/about_copyright.html

[api_badge]: https://img.shields.io/badge/platform-linux%20%7C%20bsd%20%7C%20osx-informational.svg "Read more about our API"
<!--
    This is the API badge link:
        Redirects to seqan's copyright page.
        `"Read more about our API"` - this text will be shown on hover
-->
[api_link]: https://docs.seqan.de/sharg/main_user/about_api.html

[twitter_badge]: https://img.shields.io/twitter/follow/SeqAnLib.svg?label=follow&style=social "Follow us on Twitter"
<!--
    This is the API badge link:
        Redirects to seqan's copyright page.
        `"Follow us on Twitter"` - this text will be shown on hover
-->
[twitter_link]: https://twitter.com/seqanlib

The Sharg parser offers a neat and easy-to-use header-only library for argument parsing in C++. With Sharg, your command
line interface can be customized to your specific needs and validated in a single line. Furthermore, the library provides
helpful templates and makes even subcommands feel effortless. For more information, details, and a tutorial on how to
use our features, please see our [online documentation](https://docs.seqan.de/sharg/main_user/index.html).

If you are working in the field of sequence analysis, we recommend using the
[SeqAn3 library](https://github.com/seqan/seqan3), which offers a wide range of bioinformatics-related C++ features.

## Sneak Peek:

An application with one option parsing an integer from the command line can be written in only 5 lines of code:

<!-- MARKDOWN-AUTO-DOCS:START (CODE:src=./test/snippet/readme_sneak_peek.cpp&lines=7-19) -->
<!-- The below code snippet is automatically added from ./test/snippet/readme_sneak_peek.cpp -->
```cpp
#include <sharg/all.hpp>

int main(int argc, char ** argv)
{
    int val{};

    sharg::parser parser{"Eat-Me-App", argc, argv};
    parser.add_subsection("Eating Numbers");
    parser.add_option(val, sharg::config{.short_id = 'i', .long_id = "int", .description = "Desc."});
    parser.parse();

    return 0;
}
```
<!-- MARKDOWN-AUTO-DOCS:END -->

Done. `val` is automatically filled with the value provided by `-i` or `--int`.

You also have a pretty help page and many more features:

<!-- MARKDOWN-AUTO-DOCS:START (CODE:src=./test/snippet/readme_sneak_peek.out&header=user$ ./Eat-Me-App -h) -->
<!-- The below code snippet is automatically added from ./test/snippet/readme_sneak_peek.out -->
```out
user$ ./Eat-Me-App -h
Eat-Me-App
==========

OPTIONS

  Eating Numbers
    -i, --int (signed 32 bit integer)
          Desc. Default: 0

  Common options
    -h, --help
          Prints the help page.
    -hh, --advanced-help
          Prints the help page including advanced options.
    --version
          Prints the version information.
    --copyright
          Prints the copyright/license information.
    --export-help (std::string)
          Export the help page information. Value must be one of [html, man,
          ctd, cwl].
    --version-check (bool)
          Whether to check for the newest app version. Default: true

VERSION
    Last update:
    Eat-Me-App version:
    Sharg version: 1.2.0-rc.1
```
<!-- MARKDOWN-AUTO-DOCS:END -->

## Dependencies

|                   | requirement                                               | version  | comment                                     |
|-------------------|-----------------------------------------------------------|----------|---------------------------------------------|
|**compiler**       | [GCC](https://gcc.gnu.org)                                | ≥ 12     |                                             |
|**compiler**       | [Clang](https://clang.llvm.org/)                          | ≥ 17     |                                             |
|**build system**   | [CMake](https://cmake.org)                                | ≥ 3.16   | optional, but recommended                   |
|**optional libs**  | [TDL](https://github.com/deNBI-cibi/tool_description_lib) | ≥ 1.0.0  | required for CWL and CTD export             |
