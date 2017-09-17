NEToolKit
=========
A toolkit for NeuroEvolution featuring NEAT and rtNEAT **(work in progress)**.

## Building on Linux

You can build with the provided `CMakeLists.txt` file.

For example:
```
$ cd /path/to/repo/
$ mkdir build
$ cd build
$ cmake ..
$ make
```
And you're good to go.

By default, the examples project is not built. If you want to build it, add `-D"NETKIT_EXAMPLES=1"`
to the cmake command.

For development, you may at least enable the warnings by adding `-D"NETKIT_WITH_WARNINGS=1"` and even
enable suggestions by adding `-D"NETKIT_WITH_SUGGESTIONS=1"` (only *suggestions* and they don't apply
every times).

Furthermore, the library can either be built to be *dynamic / shared* (default behavior) or *static*.
To get the static version, just add `-D"NETKIT_SHARED=0"`.

## Building on Windows

You can probably use the `CMakeLists.txt` file too, but I didn't tested it.
However, you can use the Visual Studio C++ 2017 project files.

