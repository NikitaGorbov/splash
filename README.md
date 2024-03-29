# SPLASH

<img src="images/logo.png" align="right" width="200" heigth="200">

#### Super Powerful Lightweight Advanced SHell <sub>yes, it's ridiculous on purpose.</sub>
A command-line shell for Linux made as part of my university studies.

## Features:
- Executing programs with or without arguments
- Redirecting input/output with '<' and '>' accordingly
- Executing commands in a pipeline. For example:
```sh
grep include < input.txt | sort > output.txt
```
- Changing directory with `cd`
- Running programs in background. For example:
```sh
okular &
```
- Terminating programs with CTRL + C

## Installation:
Requires `make`.
It is possible to compile with `GCC` or `Clang`.

- GCC is used by default. Run:
```sh
make
```

- To compile with Clang instead run:
```sh
make clang
```

After compiling you can simply run the created executable `splash` from the `bin` directory:
```sh
bin/splash
```
