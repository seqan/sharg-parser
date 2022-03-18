# [![sharg_logo][1]][2] Sharg -- hungrily eating away your arguments
[![build status][3]][4] [![codecov][5]][6]

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
[1]: ./test/documentation/sharg_logo.svg "Open documentation"
<!-- Link the logo to the documentation website. --->
[2]: https://sharg.vercel.app/

<!--
    This is the CI badge image:
        `https://img.shields.io/github/workflow/status/` - we do not use GitHub's badges as they are not customisable.
        `/seqan/sharg-parser/` - owner/repository
        `CI%20on%20Linux` - name of the workflow as encoded URL (e.g., whitespace = %20)
        `master` - branch to show
        `?style=flat&logo=github` - use a GitHub-style badge
        `&label=sharg-parser%20CI` - text on the badge
        `"Open GitHub actions page"` - this text will be shown on hover
-->
[3]: https://img.shields.io/github/workflow/status/seqan/sharg-parser/CI%20on%20Linux/master?style=flat&logo=github&label=Sharg%20CI "Open GitHub actions page"
<!--
    This is the CI badge link:
        `https://github.com/seqan/sharg-parser/actions` - actions page of owner(seqan)/repository(sharg-parser)
        `?query=branch%3Amaster` - only show actions that ran on the mater branch
-->
[4]: https://github.com/seqan/sharg-parser/actions?query=branch%3Amaster
<!--
    This is the Codecov badge image:
        Codecov offers badges: https://app.codecov.io/gh/seqan/sharg-parser/settings/badge
        While being logged in into Codecov, navigate to Settings->Badge and copy the markdown badge.
        Copy the image part of the markdown badge here.
    `"Open Codecov page"` - this text will be shown on hover
-->
[5]: https://codecov.io/gh/seqan/sharg-parser/branch/master/graph/badge.svg?token=KIdo8b91jb "Open Codecov page"
<!--
    This is the Codecov badge link:
        Codecov offers badges: https://app.codecov.io/gh/seqan/sharg-parser/settings/badge
        While being logged in into Codecov, navigate to Settings->Badge and copy the markdown badge.
        Copy the URL part of the markdown badge here.
-->
[6]: https://codecov.io/gh/seqan/sharg-parser

you can add this library to your application or include the [seqan3 library](https://github.com/seqan/seqan3)
with even more bio c++ features.

More information and highlights will follow soon.
