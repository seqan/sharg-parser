---
name: Release Checklist
about: A checklist used internally for the release process.
title: "#.#.# Release"
labels: ''
assignees: ''

---

Pre-release Tasks:

- [ ] Update copyright year. ([Script](https://github.com/seqan/sharg-parser/blob/main/test/scripts/update_copyright.sh))
- [ ] Check whether the [directory structure](https://github.com/seqan/sharg-parser/blob/main/doc/setup/quickstart_cmake/index.md) is valid.
- [ ] Update the cookbook. [Instructions](#cookbook))
- [ ] Check whether supported compilers are listed correctly:
  - [ ] In [README](https://github.com/seqan/sharg-parser/blob/main/README.md)
  - [ ] In [doc/about/api](https://github.com/seqan/sharg-parser/blob/main/doc/about/api/index.md)
- [ ] Check workarounds in `platform.hpp`. Are they still valid, or can they be limited to specific compiler versions?
- [ ] Update the index from cppreference.com so that up-to-date documentation links are generated. ([Instructions](#cppreference))
- [ ] Check the [Changelog.md](https://github.com/seqan/sharg-parser/blob/main/CHANGELOG.md) for completeness (including changed headers). ([Instructions](#changelog))
- [ ] Check whether all links are valid using [this script](https://github.com/seqan/sharg-parser/blob/main/test/scripts/link_check.sh).
<br>

- [ ] Create at least one pre-release s.t. our package maintainers can try out a new release of our library. ([Instructions](#prerelease))
- [ ] Check [Nightlies](https://cdash.seqan.de/index.php?project=Sharg) for critical build failures.

---

- [ ] Freeze `main` branch and set `SHARG_RELEASE_CANDIDATE` to `0`. ([Instructions](#freeze))
- [ ] Add versioned documentation to docs.seqan.de. ([Instructions](#versioned-docs))
- [ ] Prepare `sharg-[VERSION]-{Linux,Source}.tar.xz{,.sha265}`. ([Instructions](#packaging))
- [ ] Prepare a release note with notable features, API changes, bugs, and external dependency updates.

---

- [ ] **Tag release on GitHub and attach `sharg-[VERSION]-{Linux,Source}.tar.xz{,.sha265}` to the release. ([Instructions](#release))**

---

- [ ] Bump version and set `SHARG_RELEASE_CANDIDATE` to `1`. ([Instructions](#version-bump))
- [ ] Update the version used for [update notifications](https://github.com/OpenMS/usage_plots/blob/master/seqan_versions.txt).
- [ ] Announce release on [Twitter](https://twitter.com/seqanlib).
- [ ] Announce release on [website](https://www.seqan.de) ([Instructions](#website-post)).
- [ ] Announce release on mailing list `seqan-dev@lists.fu-berlin.de`.
- [ ] Announce release on [public Gitter channel](https://gitter.im/seqan/Lobby).
- [ ] Update release template with the current release tasks.
- [ ] Celebrate :tada: :beer:

---

#### Instructions

<a name="cookbook"></a>
<details><summary>Updating cppreference index</summary><br>

Execute `/path/to/sharg/test/scripts/add_snippets_to_cookbook.sh`
Create a PR if there are any changes in the cookbook because new snippets have been added.

</details>
<a name="prerelease"></a>
<details><summary>Creating a pre-release</summary><br>

GitHub is not able to create annotated releases (https://github.com/seqan/product_backlog/issues/159), so we have to manually sign the release.
Make sure you have set up [signed commits](https://docs.github.com/en/authentication/managing-commit-signature-verification/signing-commits).
```bash
git checkout release-[VERSION]
git tag -s [VERSION]-rc.[RC] # e.g. 3.1.0-rc.1
git push upstream [VERSION]-rc.[RC]
```

You will need to provide a tag message. Since this is a pre-release, it can be as simple as `Tag 3.1.0-rc.1`.

Now follow the [packaging instructions](#packaging) to create `sharg-[VERSION]-rc.[RC]-{Linux,Source}.tar.xz{,.sha265}`.

Go to https://github.com/seqan/sharg-parser/releases and create a new release using the created tag and attach the source packages.

:warning: **Make sure to set the tick for "This is a pre-release"** :warning:

Once again, the release message can be simply something along the lines of:
```
This is the first release candidate for Sharg 1.0.0

You can find a list of changes in our [changelog](https://github.com/seqan/sharg-parser/blob/1.0.0/CHANGELOG.md).
```

Afterwards, bump the succeeding release candidate number in the `main`` branch: [include/sharg/version.hpp](https://github.com/seqan/sharg-parser/blob/1.0.0/include/sharg/version.hpp#L17-L22).

</details>
<a name="cppreference"></a>
<details><summary>Updating cppreference index</summary><br>

Check for [new releases](https://github.com/PeterFeicht/cppreference-doc/releases) and update the link and hash in [test/documentation/sharg-doxygen.cmake](https://github.com/seqan/sharg-parser/blob/1.0.0/test/documentation/sharg-doxygen.cmake#L55).
You can compute the hash via `wget -O- <link to html book> | sha256sum`.

</details>
<a name="freeze"></a>
<details><summary>Freezing the `main` branch</summary><br>

- Make sure all PRs that should be merged are merged.
- Set `SHARG_RELEASE_CANDIDATE` to `0` [include/sharg/version.hpp](https://github.com/seqan/sharg-parser/blob/1.0.0/include/sharg/version.hpp#L24).
- This should be the last commit before the release.

</details>
<a name="versioned-docs"></a>
<details><summary>Creating versioned documentation</summary><br>

1. Checkout the release tag and build documentation.
2. Create a `#.#.#` directory for the release in `/web/docs.seqan.de/htdocs/sharg/`
3. Copy everything from the build (`doc_usr/html/*`) into the `#.#.#` directory.
4. Alter the file `/web/docs.seqan.de/htdocs/sharg.html` with a link to the new documentation build.

</details>
<a name="packaging"></a>
<details><summary>Creating source packages</summary><br>

Use a new clone of the repository.
```bash
git clone https://github.com/seqan/sharg-parser.git
cd sharg-parser
git checkout release-[VERSION] # version/branch to pack

mkdir ../package-build
cd ../package-build

cmake ../sharg-parser # configure
cpack # builds binary package, e.g. sharg-[VERSION]-Linux.tar.xz{,.sha265}
cmake --build . --target package_source # builds source package, e.g. sharg-[VERSION]-Source.tar.xz{,.sha265}
```

</details>
<a name="changelog"></a>
<details><summary>Checking the changelog</summary><br>

- List all newly supported and dropped compilers.
- Check that all PR links are consistent, e.g., `[\#2](https://github.com/seqan/sharg-parser/pull/2)`:
  - Search `(\[\\#)(\d+)(\]\(.+?)(\d+)(\))` and replace `$1$2$3$2$5` (i.e., replace link issue-id by the displayed id).

</details>
<a name="release"></a>
<details><summary>Creating a release</summary><br>

GitHub is not able to create annotated releases (https://github.com/seqan/product_backlog/issues/159), so we have to manually sign the release.
Make sure you have set up [signed commits](https://docs.github.com/en/authentication/managing-commit-signature-verification/signing-commits).
```bash
git checkout release-[VERSION]
git tag -s [VERSION]
git push upstream [VERSION]
```

You will need to provide a tag message. We use the first sentences of the release note:

E.g. see previous tag messages (see https://github.com/seqan/sharg-parser/tags)
```
Sharg 1.0.0 Release

Sharg is awesome!
```

</details>
<a name="version-bump"></a>
<details><summary>Bumping the version</summary><br>

- Bump succeeding version number in the main branch: [include/sharg/version.hpp](https://github.com/seqan/sharg-parser/blob/1.0.0/include/sharg/version.hpp#L17-L22).
- The [`SHARG_RELEASE_CANDIDATE`](https://github.com/seqan/sharg-parser/blob/1.0.0/include/sharg/version.hpp#L24) must be set to `1` as `0` indicates a stable release.
- Bump the [latest stable version number](https://github.com/seqan/sharg-parser/blob/main/test/api_stability/CMakeLists.txt#L10) and [checksum](https://github.com/seqan/sharg-parser/blob/main/test/api_stability/CMakeLists.txt#L14) of the API-Stability test on main.

</details>

</details>
<a name="website-post"></a>
<details><summary>Posting on the Website</summary><br>

* Go to [the website repository](https://github.com/seqan/www.seqan.de).
* Add a new file in the `_posts` directory named "YYYY-MM-DD-sharg-X-X-X-release.md".
* Alternatively, you can copy and rename an old release post.
* The file should contain:
  * ```
    ---
    layout: post
    title: "Sharg 1.0.0 released"
    categories: release
    excerpt_separator: <!--more-->
    ---
    ```
  * Followed by a short release announcement.
  * Followed by the keyword `<!--more-->`.
  * Followed by the full release note.

</details>
