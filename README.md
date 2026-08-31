# MinifilterMonitor
MinifilterMonitor is a Procmon-like file system activity tracer for Windows, built on the minifilter framework.

Unlike single-altitude tools, it can trace at multiple altitudes at the same time and aims to capture enough information to correlate records across altitudes, so the same logical I/O can be followed up and down the filter stack.

It consists of two parts:
- **MiniMonFlt**: a kernel-mode minifilter driver (`.sys`) that runs one instance per altitude, observes file system operations and streams tagged trace records to user mode.
- **MiniMonClient**: a user-mode command line client (`.exe`) that loads and attaches the driver, receives the trace stream and writes it as CSV.

Both projects share a single protocol header (`inc/protocol.h`).

## Multi-altitude tracing and correlation
The driver installs two instances at different altitudes and the client can attach all of them to the same volume. More can be added via the registry.

Every trace record is tagged with the altitude that produced it and carries data to correlate it across altitudes:

- a sequence number shared by all instances, giving the records a total order
- the address of the Filter Manager's callback data, which is shared by all instances handling one I/O
- the top-level IRP and the issuing thread, hints for linking filter-generated I/O back to its cause
- precise pre- and post-operation timestamps

Which combination of these reliably ties records together is still an open question. The correlation itself is meant to happen offline on the captured data, not in the driver.

## What is captured
The driver registers callbacks for nearly every operation Filter Manager can route to a minifilter (including named pipe and mailslot creates) and also reports transaction notifications. Each record holds, among other fields, the operation category (IRP / fast I/O / FS filter), major and minor function, process and thread id, status and information, the objects involved, a short stack trace and the file name.

## Requirements
- Windows 10/11 64bit
- Visual Studio
- C++ 20 compliant compiler
- Windows SDK
- Windows Driver Kit (WDK)

Build tested with:
- Windows 11 64bit
- Visual Studio 18
- MSVC v145 (client), WindowsKernelModeDriver10.0 (driver)
- Windows 11 SDK / WDK (10.0.28000)

Usage tested with:
- Windows 11 64bit

Running an unsigned kernel driver requires test signing to be enabled (`bcdedit /set testsigning on`, then reboot) and the driver to be test-signed.

## Build
Open the solution file (`MinifilterMonitor.slnx`) with Visual Studio and build from there.

The client (`MiniMonClient`) builds for x64 and x86 and links the runtime statically (`/MT` and `/MTd`), so it runs with no runtime dependencies. The driver (`MiniMonFlt`) builds for x64 and requires the WDK.

## Installation
Install the driver from its INF file (`MiniMonFlt.inf`): right click it and choose *Install*. This registers the `MiniMonFlt` service and its instances but does not start anything. The client loads and attaches the driver on demand. The instance altitudes are stored in the registry under `HKLM\SYSTEM\CurrentControlSet\Services\MiniMonFlt\Parameters\Instances\<instance>\Altitude` (older Windows versions use `Instances` instead of `Parameters\Instances`). They can be adjusted after installation and take effect the next time the client attaches.

The driver buffers captured records in memory until the client picks them up, capped at 50 MB by default. The cap can be changed by editing the `MaxMemoryKB` value (`REG_DWORD`) under `HKLM\SYSTEM\CurrentControlSet\Services\MiniMonFlt\Parameters`, read the next time the driver loads. Once the cap is reached the driver drops further records and the client reports how many were lost.

### Adding instances
To trace at an additional altitude, create a new key under `HKLM\SYSTEM\CurrentControlSet\Services\MiniMonFlt\Parameters\Instances` (the key name is the instance name) with two values:

- `Altitude` (`REG_SZ`): an altitude not used by another filter, e.g. `375000`
- `Flags` (`REG_DWORD`): `1` (suppress automatic attachment)

The client picks it up the next time `/m` attaches.

## Usage
Run the client from an elevated command prompt:

```
MiniMonClient [/a <volume> | /m <volume> | /u | /f <output_file> [/s]]
```

- `/a <volume>`: loads the driver and attaches the default instance to `<volume>`
- `/m <volume>`: loads the driver and attaches every installed instance (all altitudes) to `<volume>`
- `/u`: unloads the driver
- `/f <output_file>`: writes captured records to `<output_file>` (without `/f`, records go to stdout)
- `/s`: with `/f`, writes each altitude to its own file (`<base>.<altitude>.<ext>`)

A volume is given by its drive letter, e.g. `C:`. Without an action the client connects to the driver and captures the live trace to stdout. Press Ctrl+C to stop. To inspect which volumes the filter is attached to, use `fltmc instances`.

A typical session:

```
MiniMonClient /m C:           attach all altitudes to C:
MiniMonClient /f trace.csv    capture the trace until Ctrl+C
MiniMonClient /u              detach and unload the driver
```

## Output
Records are written as a semicolon-separated CSV with a header row. The single merged file interleaves all altitudes and is the better view for following one I/O through the stack. `/s` is an additional per-altitude export.

## Roadmap
- Trace operation parameters with richer semantics (e.g. the information class and buffer for query/set information, offsets and lengths for reads and writes) instead of only the raw callback arguments
- Version handshake between client and driver (the driver already answers a version query but the client does not send it yet)

## License
Copyright (C) 2026 belazr

Licensed under the GNU General Public License v3.0. See `LICENSE.txt` and `NOTICE`.

## References
- https://learn.microsoft.com/en-us/windows-hardware/drivers/ifs/
- https://github.com/microsoft/Windows-driver-samples/
