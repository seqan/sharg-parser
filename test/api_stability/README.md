<!--
SPDX-FileCopyrightText: 2006-2025 Knut Reinert & Freie Universität Berlin
SPDX-FileCopyrightText: 2016-2025 Knut Reinert & MPI für molekulare Genetik
SPDX-License-Identifier: BSD-3-Clause
-->

# Sharg API Stability

This test suite will check whether the current version of Sharg (e.g. git checkout) can be used to build the test cases
of the latest stable version of Sharg (https://github.com/seqan/sharg-parser/releases/latest).

By seeing which of our own previous test cases break, we can estimate how difficult it is to upgrade an external app and
adapt our changelog accordingly.

### Usage

```bash
# Note that the build directory must not be a subdirectory of a git repository (a directory containing .git)
# Otherwise, the patches cannot be applied correctly
cd <build_dir> # e.g. sharg-builds/api_stability/

# we recommend to disable deprecation warnings in CI
cmake -DCMAKE_CXX_FLAGS="-DSHARG_DISABLE_DEPRECATED_WARNINGS=1" <sharg_git_checkout>

# if you want to build with 40 threads, you need to use CMAKE_BUILD_PARALLEL_LEVEL to specify the threads
# a normal `make -j 40` or `cmake --build . -j 40` will not work
CMAKE_BUILD_PARALLEL_LEVEL=40 cmake --build .
```

### How does this work?

In the following we will use `1.1.1` as the latest stable release (LSR).

The workflow entails these steps:
1. Download and unzip the LSR.
2. Remove everything from the LSR except the tests in `<sharg_lsr_source>/test/`.
3. Apply patches from the current git version on the LSR, i.e., patches found in
   `<sharg_git_source>/test/api_stability/1.1.1`.
  * These patches will only apply changes on the tests.
  * This step is necessary as some of our tests also test non-public / non-stable API.
3. Use the current version of `find_package (Sharg)` found in `<sharg_git_source>/cmake`.
4. Configure the tests `<sharg_lsr_source>/test/unit` with our current sharg header-library
   (e.g. `<sharg_git_source>/include`).
  * But use the test cases from the LSR.
5. Build all tests.
6. Run all tests.
7. If any of the above steps fail, the complete build will fail.

We can apply this workflow to every kind of test (e.g., `test/unit`, `test/snippet`) that we have.
Currently, we only test `test/unit` and `test/snippet` as both of them test most of our public API.

### When to create patches and what is the naming scheme?

Patches should only be created if API changed in a way that we cannot give an "automatic" upgrade path by, for example,
defining a type alias, a compatible function that perfectly forwards to the new API or some other means that provide the
old API which then uses the new API. Please try your hardest to provide "compatible" API, as this is the easiest for the
user.

There are two categories of API changes that are reflected in the patches (prefixes in the commit message):

* `[API]` are patches that changed the public API in some way (regardless of whether this is `stable` or `experimental`
  API), like the behaviour, the return type or the call convention. These patches should always have a `CHANGELOG.md`
  entry explaining what the change did. `[API]` changes are further divided into these subcategories:
  * `[API] [BREAKAGE]` An API that existed and has a completely different meaning now, such a change MUST have a really
    good reason and should be avoided or documented extremely well.
  * `[API] [FIX]` The change was necessary, as it fixed a defect. Some examples:
    * A function does not throw anymore / will throw now.
    * Format of output (e.g., `std::cout`) changed.
    * CTAD changed (e.g., `some_class{0}` will now be of type `some_class<int>` instead of `some_class<double>`).
    * The return type changed / order within template parameters changed.
  * `[API] [WRONG-USAGE]` An API was misused in the test or tested some invalid scenarios.
* `[NOAPI]` are patches that change internal (e.g., `sharg::detail` or `private` member) or non-public API (e.g.,
  marked `\noapi`). These changes do not need an upgrade path. We differentiate between the following subcategories:
  * `[NOAPI] [INCLUDE]` Some header file was moved, or an unnecessary include was removed in some header,
    which caused a test to explicitly include that header. As there are tools like [Include What You
    Use](https://github.com/include-what-you-use/include-what-you-use), we do not consider these changes as API
    stability issues. We will still try to add an upgrade path if possible and add `CHANGELOG.md` entries
    for every header change.
  * `[NOAPI] [DETAIL]` Some `sharg::detail` code changed, like a rename, or template parameter change.
  * `[NOAPI] [DEPRECATED]` Some deprecated code was removed.
  * `[NOAPI] [BREAKAGE]` The same API has a considerable different behaviour. E.g., it accepted some
    input before, but now it would segfault on the same input.


### How to create patches?

In the following, we will use `1.1.1` as the latest stable release (LSR).

Create a new branch based on the LSR and apply all existing patches in `<sharg_git_source>/test/api_stability/1.1.1`.

```
cd <sharg_git_source>

# copy over patches to a tmp directory (`git am` seems to not support applying patches onto a different branch)
mkdir -p /tmp/sharg-api-stability-patches
cp test/api_stability/1.1.1/*.patch /tmp/sharg-api-stability-patches

# create a new branch based on the LSR and switch to it
git checkout -b api-stability-patches 1.1.1

# apply all patches onto 1.1.1 (--keep-non-patch will keep `[NOAPI]` tags in the commit message)
git am --keep-non-patch /tmp/sharg-api-stability-patches/*.patch

# clean up applied patches
rm /tmp/sharg-api-stability-patches/*.patch
```

Now re-apply the commit(s) that changed the API.

```
git cherry-pick <commit>

# fix possible merge conflicts and continue cherry-picking
git cherry-pick --continue

# remove everything except test/unit and test/snippet

# commit everything to the current cherry-picked commit
git commit --amend
```

You will most likely get merge conflicts, you can ignore all merge conflicts in any folder other than
`<sharg_lsr_source>/test/unit` or `<sharg_lsr_source>/test/snippet` (as only those kinds of tests are currently
tested by the api_stability test).

It is also important that you double check if the patch only contains changes that apply to
`<sharg_lsr_source>/test/{unit, snippet}`, any other change must be discarded.

After that, we can export all patches.

```
# export all patches since 1.1.1
git format-patch 1.1.1

# move them to tmp directory
mv *.patch /tmp/sharg-api-stability-patches
```

Now change to your branch that you were working on and check-in the patches.

```
git checkout -

cp /tmp/sharg-api-stability-patches/*.patch test/api_stability/1.1.1/

rm -rf /tmp/sharg-api-stability-patches

# add new patches
git add test/api_stability/1.1.1/

# commit changes
git commit
```

Before pushing, try whether the patches work as intended.
