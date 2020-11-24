# CompileTheory
 flex & bison based Compiler ForeGround

## usage

this will clean rebuild Compiler
```
./buildCompiler.sh
```

this will compile ./test/test.swiftl & export binary automatically
```
./runCompiler.sh
```

## how to export binary manually

```
./compiler ./test/test.swiftl

llc -march=x86-64 ./test/test.ir --filetype=obj -o ./test/test.o

clang -v ./test/test.o -o ./test/test

```