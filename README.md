# pineapple

Pineapple的C++实现。

原项目地址：https://github.com/karminski/pineapple

## How to Run

```bash
$ xmake build
$ ./pineapple hello-world.pineapple
pen pineapple apple pen.
pen pineapple
apple pen.overwrite test
with new line
```

内存泄漏检测
```
$ xmake f -m valgrind
$ xmake build
$ valgrind --tool=memcheck --leak-check=full ./pineapple
==1901== Memcheck, a memory error detector
==1901== Copyright (C) 2002-2022, and GNU GPL'd, by Julian Seward et al.
==1901== Using Valgrind-3.19.0 and LibVEX; rerun with -h for copyright info
==1901== Command: ./pineapple
==1901==
### unhandled dwarf2 abbrev form code 0x25
### unhandled dwarf2 abbrev form code 0x25
### unhandled dwarf2 abbrev form code 0x25
### unhandled dwarf2 abbrev form code 0x23
get_Form_szB: unhandled 35 (DW_FORM_rnglistx)
--1901-- WARNING: Serious error when reading debug info
--1901-- When reading debug info from /home/Source/pineapple/pineapple:
--1901-- get_Form_contents: unhandled DW_FORM
File not found.
==1901==
==1901== HEAP SUMMARY:
==1901==     in use at exit: 0 bytes in 0 blocks
==1901==   total heap usage: 2 allocs, 2 frees, 73,800 bytes allocated
==1901==
==1901== All heap blocks were freed -- no leaks are possible
==1901==
==1901== For lists of detected and suppressed errors, rerun with: -s
==1901== ERROR SUMMARY: 0 errors from 0 contexts (suppressed: 0 from 0)
```
