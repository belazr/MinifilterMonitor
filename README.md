# MinifilterMonitor
MinifilterMonitor is a Procmon-like file system activity tracer for Windows, built on the minifilter framework.

Unlike single-altitude tools, it can trace at multiple altitudes at the same time and aims to capture enough information to correlate records across altitudes, so the same logical I/O can be followed up and down the filter stack.

It consists of two parts:
- **MiniMonFlt**: a kernel-mode minifilter driver (`.sys`) that runs one instance per altitude, observes file system operations and streams tagged trace records to user mode.
- **MiniMonClient**: a user-mode command line client (`.exe`) that loads and attaches the driver, receives the trace stream and writes it as CSV.

## Requirements
- Windows 10 or 11 x64, tested on Windows 11
- Test signing enabled (`bcdedit /set testsigning on` and a reboot), since the driver is test-signed

## Installation
The driver installs from its INF file (`MiniMonFlt.inf`) through *Install* in the file's context menu. Nothing runs until a session loads it.

## Configuration
All settings are under `HKLM\SYSTEM\CurrentControlSet\Services\MiniMonFlt`:

| Key | Value | Type | Meaning |
|---|---|---|---|
| `Parameters` | `MaxMemoryKB` | `REG_DWORD` | The cap in KB on the records the driver buffers in memory until the client picks them up, read when the driver loads. Default `51200`. Once the cap is reached the driver drops further records. |
| `Parameters\Instances\<instance>` | `Altitude` | `REG_SZ` | The instance's altitude, one not used by another filter. |
| `Parameters\Instances\<instance>` | `Flags` | `REG_DWORD` | `1`, suppresses automatic attachment. |

For each key under `Parameters\Instances` the client attaches an instance at the given altitude, whatever its name. The next session picks up a new key with the listed values.

## Usage
Run the client from an elevated command prompt:

```
MiniMonClient /f <output_file> [/v <volume>...] [/s]
MiniMonClient /?
```

- `/f <output_file>`: Attaches to every volume that has a file system and writes the captured records to `<output_file>`. Named pipes and mailslots are left out.
- `/v <volume>...`: Attaches to the named volumes only. Each volume is given by its drive letter, e.g. `C:`, or by its device name, e.g. `\Device\Mup` for network files or `\Device\NamedPipe` for named pipes.
- `/s`: Writes each altitude to its own file (`<base>.<altitude>.<ext>`). Records without an altitude, such as volume mounts, go to `<output_file>` itself.
- `/?`: Prints the usage.

A typical session:

```
MiniMonClient /f trace.csv /v C:       trace C: at all altitudes until Ctrl+C
MiniMonClient /f trace.csv /v C: D:    trace C: and D: at all altitudes until Ctrl+C
MiniMonClient /f trace.csv             trace every volume until Ctrl+C
```

On Ctrl+C the client writes what is still queued, detaches and unloads the driver. A client ended without the chance to clean up, through Task Manager for example, leaves the driver loaded until `fltmc unload MiniMonFlt` or the next session unloads it.

## Output
Records are written as a semicolon-separated CSV with a header row. The single merged file interleaves all altitudes and is the better view for following one I/O through the stack. `/s` is an additional per-altitude export.

| Columns | Content |
|---|---|
| `SeqNum` | Position in the total order shared by all instances |
| `Altitude` | The instance that produced the record |
| `OperationId`, `TopLevelIrp` | The correlation tags, see How it works. `OperationId` is the address of the Filter Manager's callback data. |
| `PreOpTime`, `PostOpTime` | Timestamps before and after the operation |
| `ProcessId`, `ThreadId`, `RequestorMode`, `IoPriorityHint` | The caller |
| `PreOpIrql`, `PostOpIrql` | The IRQL at the pre- and post-operation callback |
| `Opr`, `Major`, `Minor`, `Name`, `Status`, `Information`, `Details` | The operation: its category, major and minor function, file name, result and parameters |
| `IrpFlags`, `OperationFlags`, `DevObj`, `FileObj`, `Arg1` to `Arg6` | The raw callback data |
| `ReparseTag` | The reparse point a create hit, if any |
| `Transaction`, `TransactionSeq`, `TransactionNotify` | The transaction, if any |
| `StackTrace` | The kernel stack at the pre-operation callback |

## How it works
The client attaches one instance per configured altitude to each volume it traces.

Every trace record is tagged with the altitude that produced it and carries data to correlate it across altitudes:

- a sequence number shared by all instances, giving the records a total order
- the address of the Filter Manager's callback data, which is shared by all instances handling one I/O
- the top-level IRP and the issuing thread, hints for linking filter-generated I/O back to its cause
- precise pre- and post-operation timestamps

Which combination of these reliably ties records together is still an open question. The correlation itself is meant to happen offline on the captured data, not in the driver.

The driver registers callbacks for nearly every operation Filter Manager can route to a minifilter (including named pipe and mailslot creates) and also reports transaction notifications.

The client polls the driver for batches of records over a filter communication port. The driver's sources are in `MiniMonFlt`, the client's in `MiniMonClient`, and both share one header, `inc/protocol.h`, which defines the records with fixed-width types. Static asserts pin the layout, so the x86 client reads what the x64 driver writes.

## Build
The solution `MinifilterMonitor.slnx` needs:
- Visual Studio 18
- Windows SDK
- Windows Driver Kit (WDK)

Tested with:
- Windows 11 x64
- MSVC v145 (client), WindowsKernelModeDriver10.0 (driver)
- Windows 11 SDK / WDK (10.0.28000)

The client (`MiniMonClient`) builds for x64 and x86 with a static runtime, so it runs with no runtime dependencies. The driver (`MiniMonFlt`) builds for x64 only.

The driver is test-signed with a certificate named `MinifilterMonitor TestCert` from the current user's personal store, and the build fails without one. A self-signed one is created in PowerShell with:

```powershell
New-SelfSignedCertificate -Type CodeSigningCert -Subject "CN=MinifilterMonitor TestCert" -CertStoreLocation Cert:\CurrentUser\My
```

## Roadmap
- Add a test executable that generates known file system activity
- Refine named pipe and mailslot tracing

## License
Copyright (C) 2026 belazr

Licensed under the GNU General Public License v3.0. See `LICENSE.txt` and `NOTICE`.

## References
- https://learn.microsoft.com/en-us/windows-hardware/drivers/ifs/
- https://github.com/microsoft/Windows-driver-samples/
