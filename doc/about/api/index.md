# Stability and long-term promises {#about_api}

<!--
SPDX-FileCopyrightText: 2006-2025 Knut Reinert & Freie Universität Berlin
SPDX-FileCopyrightText: 2016-2025 Knut Reinert & MPI für molekulare Genetik
SPDX-License-Identifier: CC-BY-4.0
-->

[TOC]

# Application Programming Interface (API) stability {#api_stability}

Sharg adheres to [semantic versioning](https://semver.org) and provides a stable API within
one major release unless otherwise noted.

For details regarding API stability, please see the
[SeqAn documentation on API stability](https://docs.seqan.de/seqan3/main_user/about_api.html#api_stability).

# Application Binary Interface (ABI) stability

There is no A<strong>B</strong>I stability.

For details regarding ABI stability, please see the
[SeqAn documentation on ABI stability](https://docs.seqan.de/seqan3/main_user/about_api.html#autotoc_md47).

# Platform stability {#platform_stability}

The main requirement for Sharg is that your operating system provides one of the compilers supported by us.
In general, we only support the latest three major compiler versions.
We currently support the following compilers on 64-bit operating systems with little-endian CPU architectures:
  * GCC11, GCC12, GCC13
  * Clang 17

\note Only the most recent minor release of a compiler is guaranteed to be supported, e.g. when `gcc-11.4` is released,
we may drop support for `gcc-11.3`. Since all platforms with an older version receive minor release updates,
this should not be a problem.

We promise to support the above compilers in the latest release of Sharg, or until all the following
operating systems provide a newer supported compiler:

| Operating System             | Supported Releases¹                    |
|------------------------------|----------------------------------------|
| RedHat Enterprise Linux      | the latest release ²                   |
| CentOS Linux                 | the latest release ²                   |
| SUSE Linux Enterprise Server | the latest release                     |
| Debian GNU/Linux             | "stable" and "old-stable"              |
| Ubuntu Linux                 | the two latest LTS releases            |
|                              |                                        |
| macOS                        | the two latest releases                |
|                              |                                        |
| FreeBSD                      | the latest stable release              |

"Support" in this context does not imply that we package Sharg for these operating systems (although we do in some
cases), it merely states that you should be able to build applications that depend on Sharg on the given platforms.
This implies the availability of a supported compiler in the default package repositories or via easy-to-use
third party services.

**We promise to provide good forward-compatibility with the C++ standard.** And we will strive to fix any warnings that
are added by newer versions of a supported compiler.

<small>¹ [This site](https://linuxlifecycle.com) provides a good overview of what the current release and its
lifecycle is.</small><br>
<small>² We consider CentOS 7 / RedHat Enterprise Linux (RHEL) 7 as being community-supported. That means issues and
patches are welcome, but we do not actively test for those operating systems. See this related
[issue](https://github.com/seqan/seqan3/issues/2244).</small>

# Dependencies

We will keep Sharg independent of any external libraries, unless absolutely necessary.
