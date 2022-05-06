# Update Notifications {#about_update_notifications}

[TOC]

Outdated software is a huge issue in science workflows. Especially when the software is not part of the packaging
provided by the operating system, it tends to be installed once and never updated.

We provide update notifications as a convenient service to app users so that they immediately see when a new version is
available. When we assume that an application is being run by its developer we nag the developer to also keep their
checkout of the Sharg library up-to-date. This ensures new features get to app developers faster, applications have
fewer bugs, and we receive fewer bug reports on outdated versions.

## How it works

Upon starting an application using the `sharg::argument_parser`, it is determined whether the app is built in release
mode (we assume a user is running it) or in debug mode (we assume the developer is running it).

We then send a web-request to our server with the following:

 * The application name and its version
 * The Sharg version that the application was built with
 * The operating system type (Linux, macOS, Windows or BSD)
 * The CPU type (32 or 64bit)

But we send at most one request per day [we keep track of this by saving a timestamp to `.config/seqan/app_name`].

If a newer app version is registered in our database (or can be automatically determined), we inform the user about it.
If the app is running in debug mode and the Sharg version is outdated, we assume the user is the app developer and
inform them about it.

## Privacy Implications

Only the information listed above is transmitted; no data is gathered about the usage of the app, and, of course, no
information regarding your input/output files is transmitted. All our code is open source so this can be verified at
any time.

The server does a simple form of non-personalised data aggregation that gives us a better overview of which apps are
most commonly used and how well new Sharg versions are picked up. This helps the SeqAn project (www.seqan.de) a lot, also in acquiring
funding. The web-server that receives the requests currently logs IP-Addresses as part of its standard behaviour,
however, we are working on removing this behaviour.

We may publish aggregated and anonymised usage data irregularly, but the original data is never shared with a third
party.

## How to turn it on/off

You are asked about your preference in regard to the version check the first time you run an application.
You may choose to always or never perform checks, or be asked every time.
You can also specify that you don't (or do) want to perform the check (regardless of any previous choice)
by passing this argument to your program: `--version-check 0` (or `1` to activate it).

Users may opt-out of the version check globally (for all apps on their computer) by setting the environment variable
`SEQAN3_NO_VERSION_CHECK`.

Application developers may opt-out of the version check for their app permanently (independent of user choice) by
passing `seqan3::update_notifications::off` as the fourth argument to seqan3::argument_parser.
See the respective API documentation of the `sharg::argument_parser`.
