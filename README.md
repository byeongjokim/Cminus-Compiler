# C-minus Compiler

Structure

 * scanner (only scanner)
 * parser (parser + scanner)
 * semantic (semantic analysis + parser + scanner)
 * assignment
 * report

Using

* Flex (scanner)
* Yacc (parser)
* gcc (semantic)

How to run

```
cd semantic
setting main.c (TraceScan, TraceParse, TraceAnalyze ...)
$make clean; make; 
$./cminus test.cm
```