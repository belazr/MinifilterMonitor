# MinifilterMonitor
MinifilterMonitor is a Procmon-like file system activity tracer for Windows, built on the minifilter framework.

Unlike single-altitude tools, it can **trace at multiple altitudes at the same time** and aims to capture **enough information to make correlation across altitudes possible**, so the same logical I/O can be followed as it travels up and down the filter stack.

It consists of two parts:
- **MiniMonFlt**: a kernel-mode minifilter driver (`.sys`) that attaches one instance per altitude, observes file system operations and streams tagged trace records to user mode.
- **MiniMonClient**: a user-mode command line client (`.exe`) that loads and attaches the driver, receives the trace stream and writes it as CSV.

Both projects share a single protocol header (`inc/protocol.h`).

## Multi-altitude tracing and correlation
The driver installs instances at different altitudes (a top and a bottom one by default, forming the upper and lower boundary of a monitoring window; more can be added via the registry) and the client attaches all of them to the same volume. Because it is one driver living in one Filter Manager frame at several altitudes, sharing global state, every trace record is tagged with the altitude that produced it and carries additional data intended to help correlate it across altitudes, such as:

- a single global sequence number, giving every record across all altitudes a total order
- an operation id from the callback data the Filter Manager reuses across the stack's instances, to identify one I/O across altitudes
- the top-level IRP and the issuing (current) thread, as hints for linking filter-generated I/O back to the operation that caused it
- high-precision pre- and post-operation timestamps bounding each operation's lifetime

Exactly which of these values, and in which combination, can reliably tie records together across altitudes is still an open question. The correlation itself is meant to be done offline on the captured data; the driver's job is to capture enough to make it possible. This cross-altitude correlation is the project's main aim.

## What is captured
The driver registers callbacks for the full set of file system operations (create, read, write, query/set information, directory control, FS/device control, cleanup, close, ...) and also reports transaction notifications. For each operation the record holds, among other fields: the operation category (IRP / fast I/O / FS filter), major and minor function, process and thread id, status and information, IRP flags, device, file and transaction objects, up to eight stack frames and the file name.

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
Open the solution file (`MinifilterMonitor.slnx`) with Visual Studio and run the desired builds from there.

The client (`MiniMonClient`) builds for x64 and x86 and links the runtime statically (`/MT` and `/MTd`), so it runs with no runtime dependencies. The driver (`MiniMonFlt`) builds for x64 and requires the WDK.

## Installation
Install the driver from its INF file (`MiniMonFlt.inf`): right click it and choose *Install*. This registers the `MiniMonFlt` service and its instances (a top and a bottom altitude by default) but does not start it. The client loads and attaches the driver on demand, so no manual service start is needed. The instance altitudes are stored in the registry under `HKLM\SYSTEM\CurrentControlSet\Services\MiniMonFlt\Parameters\Instances\<instance>\Altitude` and can be adjusted after installation to match your needs. Changes take effect the next time the client attaches.

The driver buffers captured records in memory until the client picks them up. This memory is capped at 50 MB by default. The cap can be adjusted via a `MaxMemoryKB` value (`REG_DWORD`) under `HKLM\SYSTEM\CurrentControlSet\Services\MiniMonFlt\Parameters`, read the next time the driver loads. Once the cap is reached the driver drops further records and the client reports how many were lost.

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

A volume is given by its drive letter, e.g. `C:`. Without an action the client connects to the driver and captures the live trace to stdout; press Ctrl+C to stop. To inspect which volumes the filter is attached to, use Windows' built-in `fltmc instances`.

A typical session:

```
MiniMonClient /m C:           attach all altitudes to C:
MiniMonClient /f trace.csv    capture the trace until Ctrl+C
MiniMonClient /u              detach and unload the driver
```

## Output
Records are written as a semicolon separated CSV with a header row. Alongside the per-operation details it includes columns such as `Altitude`, `SeqNum`, `OperationId`, `TopLevelIrp`, `Transaction` and the pre-/post-operation timestamps, all captured with offline cross-altitude correlation in mind. The merged single file interleaves all altitudes and is the better view for following one I/O through the stack; `/s` is an additional per-altitude export.

## Roadmap
- Trace operation parameters with richer semantics (e.g. the information class and buffer for query/set information, offsets and lengths for reads and writes) instead of only the raw callback arguments
- Version handshake between client and driver (the driver already answers a version query; the client does not perform the handshake yet)

## License
Copyright (C) 2026 belazr

Licensed under the GNU General Public License v3.0; see `LICENSE.txt` and `NOTICE`.

## References
- https://learn.microsoft.com/en-us/windows-hardware/drivers/ifs/
- https://github.com/microsoft/Windows-driver-samples/
