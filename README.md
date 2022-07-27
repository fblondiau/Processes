# Processes

A simple MPW tool (for Mac OS Classic), used in the [ToolDaemon](https://github.com/fblondiau/ToolDaemon) documentation.

To recreate the tool easily, using MPW, enter these two commands.

```
Rez Processes_resourceFork.r -o Processes
SetFile Processes -c 'MPS ' -t 'MPST'
```

You can also restore a full development environment (as it was in 1994, using Mac OS 8.5) ¹ and use the makefile.

```
Make -f Processes.make
```

¹ *In my development environment, some headers and/or the C compiler are missing (replaced by MrC)... so, this last method fails. You’ll be luckier.*
