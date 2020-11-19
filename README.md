# CompileTheory
 flex & bison based Compiler ForeGround

## how to export binary

```
./compiler ./test/test.swiftl

llc -march=x86-64 ./test/test.ir --filetype=obj -o ./test/test.o

clang -v ./test/test.o -o ./test/test

```