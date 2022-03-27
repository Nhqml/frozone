
# Frozone
*AppING 2022 - Final project*

## What

The goal of this project was to create 2 modules / libs in the C language that
would allow to retrieve live information on a system (running processes, logged
users, opened files, etc...), and freeze those resources.

This project is divided in 2 libs:

- `carto`, which contains code to retrieve information on a system
- `freezer`, which contains code to freeze some resources of a system

## Installation

TODO

### Build (debug)

```bash
meson build && meson compile -C build
```

### Test

```bash
meson test -C build
```

#### Verbose mode

```bash
meson test -C build -v
```

#### With valgrind

```bash
meson test -C build -v --wrap='valgrind'
```

or, for a more detailed output:

```bash
meson test -C build -v --wrap='valgrind --track-origins=yes --leak-check=full'
```

### Build (release)

```bash
meson --buildtype=release build-release && meson compile -C build-release
```
