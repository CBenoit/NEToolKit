NEToolKit
=========
A toolkit for NeuroEvolution featuring NEAT and rtNEAT (work in progress).

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

By default, the examples project is not built. If you want to build it, run cmake like so:
```
$ cmake .. -D"NETKIT_EXAMPLES=1"
```

## Building on Windows

You can probably use the `CMakeLists.txt` file too, but I didn't tested it.
However, you can use the Visual Studio C++ 2017 project files.

