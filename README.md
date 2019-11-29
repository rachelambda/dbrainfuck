# dbrainfuck
depsterr - brainfuck, dbrainfuck for short is an interpreter and extention of the brainfuck language

## Installation and usage

To install dbrainfuck, run the following
```shell
git clone https://www.github.com/depsterr/dbrainfuck.git
cd dbrainfuck
sudo make install
```

To then use dbrainfuck with the dbrainfuck command
```shell
dbrainfuck <path-to-brainfuck-file>
```

## Features

Except for the normal brainfuck syntax dbrainfuck currently has the following features

### General

* Infinite\* memory array and infinite\* nested loops

* Error messages

* Supports tabs, newlines and spaces in input files

* Extended syntax and functionality

\**as long as your have ram to spare*

### Syntax

`\` - Breaks the current loop, if not in a loop it will exit the program with the current pointer value as status

`^FF` - Sets the value of the current byte to the hex value specified (note that whitespace is not supported here at the moment, the formatt must be `^FF`)

`(` - Saves the current memory pointer to a list

`)` - Loads the latest saved memory pointer

`p` - Pops the latest pointer from the pointer list

`$` - Saves the value of the current byte

`\`` - Loads saved value to the current byte

`}` - Moves memory pointer as many bytes forward as byte pointed at

`{` - Moves memory pointer as many bytes backwards as byte pointed at

## Future Plans

* Add more custom syntax
  * Set pointer to value
  * ... more to come!
* Improve whitespace filtering
* Create a vim syntax file for simplifying developing dbrainfuck (not that anyone is going to)
