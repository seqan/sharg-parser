# SeqAn3 App Template [![build status][1]][2] [![codecov][3]][4]
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

This is a template for app developers with SeqAn3.
You can easily clone this repository and modify the existing code to your needs.
It provides the elementary set-up for all SeqAn3 applications.

The example application is a FastQ to FastA file format converter.
It demonstrates exemplarily the set-up of test cases, documentation, and build infrastructure.
Probably you want to name your app differently — simply replace `app-template` with your app name in the following.
Please note that the command line interface tests fail if you use an individual project name without adapting the
name in the test file.

Instructions:
1. clone this repository: `git clone --recurse-submodules https://github.com/seqan/app-template.git app-template`
2. edit the project name in the *project* command of `app-template/CMakeLists.txt`
3. create a build directory and visit it: `mkdir build && cd build`
4. run cmake: `cmake ../app-template`
5. build the application: `make`
6. optional: build and run the tests: `make test`
7. optional: build the api documentation: `make doc`
8. execute the app: `./bin/app-template`
