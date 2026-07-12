# Security Policy

MinifilterMonitor consists of a Windows kernel-mode minifilter driver (`MiniMonFlt.sys`) and a
user-mode client (`MiniMonClient.exe`). Because the driver runs in kernel space (ring 0), defects
in it can affect overall system stability and integrity.

## Intended use

This is a research and diagnostic tool. Loading the driver requires administrator rights and
test-signing to be enabled (or driver-signature enforcement otherwise relaxed). It is **not**
hardened for production or for use on untrusted, multi-tenant systems.

## Supported versions

This is pre-1.0 software; only the latest release receives fixes, and the shared protocol, command
line, and behavior may change between releases.

## Reporting a vulnerability

Please report security issues privately through GitHub's private vulnerability reporting (the
**"Report a vulnerability"** button on the repository's **Security** tab) rather than opening a
public issue. This keeps the report confidential until a fix is available.

Where possible, include the affected version or commit, the impact, and steps to reproduce or a
proof of concept. This is an unfunded personal project, so there is no formal response-time
guarantee. Reports will be answered on a best-effort basis and disclosure will be coordinated with
the reporter.
