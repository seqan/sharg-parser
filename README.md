# <img src="https://user-images.githubusercontent.com/7955878/144013376-6fc96860-b30b-4c8d-8203-b5f98571a1ba.png" alt="drawing" style="width:80px;"/> Sharg -- hungrily eating away your arguments 
[![build status][1]][2] [![codecov][3]][4]

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

<!--
    This is the CI badge image:
        `https://img.shields.io/github/workflow/status/` - we do not use GitHub's badges as they are not customisable.
        `/seqan/app-template/` - owner/repository
        `CI%20on%20Linux` - name of the workflow as encoded URL (e.g., whitespace = %20)
        `master` - branch to show
        `?style=flat&logo=github` - use a GitHub-style badge
        `&label=App-Template%20CI` - text on the badge
        `"Open GitHub actions page"` - this text will be shown on hover
-->
[1]: https://img.shields.io/github/workflow/status/seqan/app-template/CI%20on%20Linux/master?style=flat&logo=github&label=App-Template%20CI "Open GitHub actions page"
<!--
    This is the CI badge link:
        `https://github.com/seqan/app-template/actions` - actions page of owner(seqan)/repository(app-template)
        `?query=branch%3Amaster` - only show actions that ran on the mater branch
-->
[2]: https://github.com/seqan/app-template/actions?query=branch%3Amaster
<!--
    This is the Codecov badge image:
        Codecov offers badges: https://app.codecov.io/gh/seqan/app-template/settings/badge
        While being logged in into Codecov, navigate to Settings->Badge and copy the markdown badge.
        Copy the image part of the markdown badge here.
    `"Open Codecov page"` - this text will be shown on hover
-->
[3]: https://codecov.io/gh/seqan/app-template/branch/master/graph/badge.svg?token=V82JRCXF0K "Open Codecov page"
<!--
    This is the Codecov badge link:
        Codecov offers badges: https://app.codecov.io/gh/seqan/app-template/settings/badge
        While being logged in into Codecov, navigate to Settings->Badge and copy the markdown badge.
        Copy the URL part of the markdown badge here.
-->
[4]: https://codecov.io/gh/seqan/app-template

you can add this library to your application or include the [seqan3 library](https://github.com/seqan/seqan3)
with even more bio c++ features.

More information and highlights will folllow soon.
