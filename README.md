# C-Like-Compiler
Components of a compiler for a simplified C family language

### Assembler

Convert MIPS language to its binary representation

### Scanner

Tokenize CLike program into pieces of tokens

### CLikeParse

Reads an LR1 File representing a context-free grammar, an LR(1) machine, and a sequence to be recognized. If the sequence is in the language, output unindented reversed rightmost derivation format

### CLikeTyping

The semantic analysis phase of the CLike compiler. In particular that catches all remaining compile-time errors in the source program; a program that makes it through this program of compilation is guaranteed to be free of compile-time errors

### Code Generation

Generates corresponding MIPS code of a CLIKE program

## Run the Compiler
There is a CLike program named `foo.CLike` that can be used to try out the compiler, first run:  

`cat foo.CLike | ./scan | ./parse | ./errorCheck`  

to check if there is any error, if the program prints nothing, then there is no error and we can compile the program by entering:

`cat foo.CLike | ./scan | ./parse | ./codeGen`

## Run the Assembler
There is a MIPS program named `treeHeight.asm` that can be used to try out the assembler, run:  

`./assemble < treeHeight.asm > treeHeight.mips`, then `treeHeight.mips` is a executable MIPS program
