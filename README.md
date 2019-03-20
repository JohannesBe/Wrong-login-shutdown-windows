# Auto-shutdown after a number of invalid windows logins

## Why

It is well known that any running operating system has a myriad of security vulnerabilities.
Furthermore, tinkering users might have the intention to execute a [cold boot attack](https://en.wikipedia.org/wiki/Cold_boot_attack),
but first try to play the guessing game to get into your machine.

In any case, it is clear that an encrypted system might be at risk when left
locked, in a running state. In order to be fully secure, users **must always shutdown their computer when left behind**.

Sometimes however, this is just not convenient: from a bathroom break, to going to grab a coffee; 
unlocking a fully disk encrypted device simply takes too long to do that in some situations.
If in these cases a colleague, friend, acquaintance or non-intended person would try to crack
your computer, security might be improved by this tool.

## How

The tool automatically checks for windows logon events. *Note that your group policies must enable auditing of logon events in order for this tool to work.*
When three invalid logins are found since the last successfull login, the tool instantly shuts down your computer.
In order for the program to be able to read these windows logon events, the tool requires administrator elevation.

## License

GPLv3
(c) Johannes Bergé

## Contributors

Johannes Bergé

