# Homework #2

## Play in a Sandbox!?

In this homework, we are going to practice library injection and API hijacking. Please implement a sandbox that confines file relevant operations to a user provided directory. You have to implement your sandbox as a <u>shared library (1)</u> (e.g., `sandbox.so`) and then inject the shared library into a process using using your customized <u>command launcher (2)</u> (e.g., `sandbox`). If an operation attempts to escape from the sandbox, e.g., open a file or a directory outside the sandbox, you have to reject the request by returning an error code to the function caller. In addition, please also output an error message to the user's <u>terminal</u>. **You should output error messages correctly even if the command line has redirected outputs/errors to a file.** Please check the functions listed in the section "Requirements" below.

You have to compile your source codes and generate 1) the shared object and 2) the launcher. You don't have to implement any test program by yourself. Instead, you have to work with those binaries already installed in the system.

## Requirements

* The list of monitored library calls is shown below. It covers several functions we have introduced in the class.

|           |           |           |           |           |           |
|:---------:|:---------:|:---------:|:---------:|:---------:|:---------:|
|chdir      |chmod      |chown      |creat      |fopen      |link       |
|mkdir      |open       |openat     |opendir    |readlink   |remove     |
|rename     |rmdir      |stat       |symlink    |unlink     |           |

* In addition to the aforementioned library calls, your sandbox **ALWAYS** has to reject the use of the following functions

|           |           |           |           |           |           |
|:---------:|:---------:|:---------:|:---------:|:---------:|:---------:|
|execl      |execle     |execlp     |execv      |execve     |execvp     |
|system     |           |           |           |           |           |

## Output Messages

If an invalid access is detected, your program has to output an error message to indicate the error. If an access to an invalid path is detected, please show the target pathname and the corresponding operation. If the use of `system` or `exec*` is rejected, please also show the function name and the command line (for `system`) or the executable name (for `exec*`).

## Grading Policy

* [10%] Use Makefile to manage the building process of your program. We will not grade your program if we cannot use make command to build your program.
* [30%] A sandboxed executable can work as usual. Your program cannot change the behavior of a sandboxed executable if the accesses are in the allowed base directory and its subdirectories.
* [30%] Implement file/directory-relevant functions listed in the requirements.
* [20%] Implement `system` and `exec*` functions listed in the requirements.
* [10%] Display error messages.

## Hints

Some hints that may simplify your work:

1. We will test your program on Ubuntu 18.04. For the best compatibility, you may develop your program on Ubuntu 18.04.
2. You may need to define macros to simplify your implementation.
3. You may consider working with # and ## operators in macros.
4. For variable-length function parameters, consider working with [stdarg.h](http://man7.org/linux/man-pages/man0/stdarg.h.0p.html).
5. You may consider working with **\_\_attribute\_\_((constructor))**. If you don't know what is that, please google for it!
6. The implementation for some library functions may be different from its well-known prototypes. For example, the actual implementation for **stat** in GNU C library is **__xstat**. Therefore, you may be not able to find symbol **stat** in the binary. In case that you are not sure about the ***real*** symbols used in C library, try to work with **readelf** or **nm** to get the symbol names.
7. Sample macro implementation is available [here](https://people.cs.nctu.edu.tw/~chuang/courses/unixprog/resources/hw2_sandbox/macro.txt).
8. You may use ltrace command to check what library functions are called by a program.

## Development Note

### Usage

```
usage: ./sandbox [-p sopath] [-d basedir] [--] cmd [cmd args ...]
        -p: set the path to sandbox.so, default = ./sandbox.so
        -d: the base directory that is allowed to access, default = .
        --: separate the arguments for sandbox and for the executed command
```

### Return value of injected function

If the function try to access invalid directory, the **return value** of function will be `-1` or `NULL`, and set **errno** to `EACCES`.
