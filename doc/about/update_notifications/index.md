# Update Notifications {#about_update_notifications}

<!--
SPDX-FileCopyrightText: 2006-2025 Knut Reinert & Freie Universität Berlin
SPDX-FileCopyrightText: 2016-2025 Knut Reinert & MPI für molekulare Genetik
SPDX-License-Identifier: CC-BY-4.0
-->

[TOC]

Outdated software is a huge issue in scientific workflows. Especially when the software is not part of the packaging
provided by the operating system, it tends to be installed once and never updated.

We provide update notifications as a convenient service to app users so that they immediately see when a new version is
available. When we infer that an application is being run by its developer, we additionally remind them to keep their
checkout of the Sharg library up-to-date. This ensures that new features arrive at app developers faster, applications
have fewer bugs, and we receive fewer bug reports on outdated versions.

## How it works

Upon starting an application using the `sharg::parser`, it is determined whether the app was built in release
mode (we assume a user is running it) or in debug mode (we assume the developer is running it).

We then send a web-request to our server with the following information:

 * The application name
 * The application version
 * The Sharg version that the application was built with
 * The operating system type (Linux, macOS, Windows, or BSD)
 * The CPU type (32 or 64bit)

However, we send at most one request per day [we keep track of this by saving a timestamp to `~/.config/seqan/app_name`].

We inform the user about available updates, if a newer app version is registered in our database (or can be
automatically determined).
We inform the developer about new Sharg versions, if the app is running in debug mode.
Additionally, we recommend registering your app with us (support@seqan.de).

## Privacy Implications

Only the information listed above is transmitted; no data is gathered about the usage of the app, and, of course, no
information regarding your input/output files is transmitted. All our code is open source, and hence this can be
verified at any time.

The server does a simple form of non-personalised data aggregation that gives us a better overview of which apps are
most commonly used and how well new Sharg versions are picked up. This helps the [SeqAn project](https://www.seqan.de)
a lot, for example, in acquiring funding. The web server that receives the requests currently logs IP addresses as
part of its standard behaviour. However, we are working on removing this behaviour.

We may publish aggregated and anonymised usage data irregularly, but the original data is never shared with a third
party.

## How to turn it on/off

You are asked about your preference regarding the version check the first time you run an application.
You may choose to always or never perform checks, or be asked every time.
Furthermore, you can specify whether you want to perform the check regardless of any previous choice.
To perform this, pass `--version-check 0` (no check) or `--version-check 1` (run check) as argument to your program.

Users may opt out of the version check globally, i.e., for all apps on their computer, by setting the environment
variable `SHARG_NO_VERSION_CHECK` to any value.

Application developers may opt out of the version check for their app permanently (independent of user choice) by
passing `sharg::update_notifications::off` as the fourth argument to sharg::parser.
See the respective API documentation of the `sharg::parser`.
