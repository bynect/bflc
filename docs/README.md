# The bflc framework

## Overview

The Brain Fuck Like Compiler (bflc) is a compiler framework that targets
[Brainfuck][bf-wikipedia] and closely [related languages](bf-family).

## Installation

```sh
    git clone git@github.com:bynect/bflc.git
    cmake -B build
    make -C build
```

This will create both a static and a dynamic version of the
[bflc library](/libbflc "libbflc"), and the reference
[bflc compiler](/bflc "bflc") implementation.

## Design

The framework is made of many component, which we call **unit**,
which can be a backend, a frontend or a middle pass.
Adding new units is easy thanks to the modular structure of the project.

### Backend units

Backend units are made of two functions with the following signatures.

```c
    error_t emit_asm_ARCH(context_t *ctx, bytebuffer_t *buf, ir_t *ir);
    error_t emit_mach_ARCH(context_t *ctx, bytebuffer_t *buf, ir_t *ir);
```

The `emit_asm_ARCH` functions emit assembly for ARCH.
The `emit_mach_ARCH` functions emit machine code for ARCH.
These functions can report errors based on incorrect context or instructions.

### Frontend units

Frontend units are made of one functions with the following signature.

```c
    error_t scan_LANG(const char *src, size_t len, ir_t *ir);
```

The `scan_LANG` functions generate and intermediate representation from a source
language, reporting errors if necessary.

### Middle pass units

Middle pass units are made of one function with the following signature.

```c
    error_t pass_PASS(context_t *ctx, ir_t *ir);
```

The `pass_PASS` functions pass on the intermediate representation and can change
it if appropriate.

## Units

As of today the following units are available.

### Backends

- x86_64
  - Assembly output in GAS syntax.
  - Assembly output in Intel Nasm syntax.
  - Assembly output compatible with Nasm `-fbin` option.

### Frontends

- [Brainfuck][bf-wikipedia]
- [FuckbeEs][fuckbees]

### Middle passes

- Validation
  - Check cell overflow.
  - Check pointer overflow.
  - Check unmatched loops.

- Folding
  - Fold similar instructions.
  - Remove nop instructions.

### Planned units

An assembly backend for x86 is being actively developed.
A pseudo-backend for the [Graphviz Dot][graphviz-dot] language is planned.
Machine code output for backends will be added in the future.

Frontends for other Brainfuck-like languages are planned.

## Compiler usage

The [reference compiler](/bflc "bflc") requires a lot of flags to be passed,
because it can be used with multiple frontend and backend.

```sh
    bflc --front=Brainfuck --back=x86_64 myfile.bf -o outfile.asm
```

This will compile a Brainfuck input file `myfile.bf` to x86_64 assembly
in Intel Nasm syntax and store it in `outfile.asm`.

GAS assembly syntax is set by default.
The Validation and Folding passes are enabled by default.
`flibc` is set when neither `fwrite` or `fread` are set.

## History

A very early and incomplete implementation of the project can be found on the
`early` branch of this repository.

### Motivation

Why this project was created?

![motivation](/docs/motive.svg)

## Contributing

Contributions are welcome.
Please open an issue first for major changes.

Before opening a pull request please check for memory leaks, errors and
other memory problems with either [Asan][asan] or [Valgrind][valgrind].

## License

The bflc framework is licensed under the terms and conditions of the
[Apache-2.0 License](LICENSE).

[bf-wikipedia]: https://en.wikipedia.org/wiki/Brainfuck
[bf-family]: https://esolangs.org/wiki/Trivial_brainfuck_substitution
[fuckbees]: https://esolangs.org/wiki/FuckbeEs
[asan]: https://en.wikipedia.org/wiki/AddressSanitizer
[valgrind]: https://valgrind.org/
[graphviz-dot]: https://graphviz.org/doc/info/lang.html
