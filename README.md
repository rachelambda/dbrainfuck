# dbrainfuck
depsterr - brainfuck, dbrainfuck for short, is an interpreter and extention of the brainfuck language

## Installation and Usage

To install dbrainfuck, run the following
```shell
git clone https://www.github.com/depsterr/dbrainfuck.git
cd dbrainfuck
sudo make install
```

Then use dbrainfuck with the dbrainfuck command
```shell
dbrainfuck <path-to-brainfuck-file>
```

## Features

Except for the normal brainfuck syntax dbrainfuck currently has the following features

### General

* Infinite\* memory array and infinite\* nested loops

* Error messages

* Supports tabs, newlines and spaces in input files

* Supports comments

* Extended syntax and functionality

\**as long as your have ram to spare*

### New Syntax

`#` - Treats the rest of the line as a comment

`\` - Breaks the all loops, if not in a loop it will exit the program with the current pointer value as status

`^FF` - Sets the value of the current byte to the hex value specified

`(` - Saves the current memory pointer to a list

`)` - Loads the latest saved memory pointer

`/` - Pops the latest pointer from the pointer list

`$` - Saves the value of the current byte

`@` - Loads saved value to the current byte

`}` - Moves memory pointer as many bytes forward as byte pointed at

`{` - Moves memory pointer as many bytes backwards as byte pointed at

## Future Plans

* Add more custom syntax
  * Set pointer to value
  * ... more to come!
* Create a vim syntax file for simplifying developing dbrainfuck (not that anyone is going to)
