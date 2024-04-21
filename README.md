
# Wish

**Wish** is a simple Unix shell. A shell is the heart of the command-line interface, and thus is central to the Unix/C programming environment. 

## Overview

Wish operate in this basic way: when you type in a command (in response to its prompt), the shell creates a child process that executes the command you entered and then prompts
for more user input when it has finished.

## Program Specifications

### Basic Shell: `wish`

**Wish** is basically an interactive loop: it repeatedly prints a prompt `wish> `, parses the input, executes the command specified on that line of input, and waits for the command to finish. This is repeated until the user types `exit`. 

The shell can be invoked with either no arguments or a single argument; anything else is an error. Here is the no-argument way:

```
prompt> ./wish
wish> 
```

At this point, `wish` is running, and ready to accept commands. Type away!

The mode above is called *interactive* mode, and allows the user to type commands directly. The shell also supports a *batch mode*, which instead reads input from a batch file, executes commands from therein, and exit. Here is how you run the shell with a batch file named `batch.txt`:

```
prompt> ./wish batch.txt
```

One difference between batch and interactive modes: in interactive mode, a prompt is printed (`wish> `). In batch mode, no prompt should be printed.

## Structure
### Paths

In the following example, the user types `ls` but the shell knew to execute the
program `/bin/ls`. How does your shell know this?

It turns out that the user must specify a **path** variable to describe the set of directories to search for executables; the set of directories that comprise the path are sometimes called the *search path* of the shell. The path variable contains the list of all directories to search, in order, when the user types a command. 

To check if a particular file exists in a directory and is executable, `access()` system call was considered. 

The initial shell path contains one directory: `/bin`.

### Built-in Commands

Whenever **Wish** accepts a command, it should check whether the command is a **built-in command** or not. If it is, it should not be executed like other programs. Instead, **Wish** will invoke your implementation of the built-in
command.

* `exit`: When the user types `exit`, wish simply call the `exit` system call with 0 as a parameter. It is an error to pass any arguments to `exit`. 

* `cd`: `cd` always take one argument (0 or >1 args is signaled as an error). To change directories, wish use the `chdir()` system call with the argument supplied by the user; if `chdir` fails, that is also an error.

* `path`: The `path` command takes 0 or more arguments, with each argument separated by white space from the others. A typical usage would be like this: `wish> path /bin /usr/bin`, which would add `/bin` and `/usr/bin` to the search path of the shell. If the  user sets path to be empty, then the shell can not run any programs (except built-in commands). The `path` command always overwrites the old path with the newly specified path. 

### Redirection

Many times, a shell user prefers to send the output of a program to a file rather than to the screen. Usually, a shell provides this nice feature with the `>` character. Formally this is named as redirection of standard output. To make your shell users happy, wish also include this feature, but with a slight twist (explained below).

For example, if a user types `ls -la /tmp > output`, nothing should be printed on the screen. Instead, the standard output of the `ls` program should be rerouted to the file `output`. In addition, the standard error output of the program should be rerouted to the file `output` (the twist is that this is a little different than standard redirection).

If the `output` file exists before you run your program, wish overwrite it (after truncating it).  

The exact format of redirection is a command (and possibly some arguments) followed by the redirection symbol followed by a filename. Multiple redirection operators or multiple files to the right of the redirection sign are errors.

### Parallel Commands

Wish also allow the user to launch parallel commands. This is accomplished with the ampersand operator as follows:

```
wish> cmd1 & cmd2 args1 args2 & cmd3 args1
```

In this case, instead of running `cmd1` and then waiting for it to finish, shell run `cmd1`, `cmd2`, and `cmd3` (each with whatever arguments the user has passed to it) in parallel, *before* waiting for any of them to complete. 

Then, after starting all such processes, wish make sure to use `waitpid` to wait for them to complete. After all processes are done, wish return control to the user as usual (or, if in batch mode, move on to the next line).

### Program Errors

**The one and only error message.** Wish shell prints this one and only error message whenever it encounter an error of any type:

```
    char error_message[30] = "An error has occurred\n";
    write(STDERR_FILENO, error_message, strlen(error_message)); 
```

The error message is printed to `stderr` (standard error), as shown above. 

After most errors, shell simply *continue processing* after printing the one and only error message. However, if the shell is invoked with more than one file, or if the shell is passed a bad batch file, it should exit by calling `exit(1)`.

There is a difference between errors that wish catches and those that the program catches. wish catch all the syntax errors specified in this project page. If the syntax of the command looks perfect, wish simply run the specified program. If there are any program-related errors (e.g., invalid arguments to `ls` when you run it, for example), the shell does not
deal with that (rather, the program will print its own error messages and exit).