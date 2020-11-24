./compiler ./test/test.swiftl >> ./test/test.ir

llc -march=x86-64 ./test/test.ir --filetype=obj -o ./test/test.o

clang -v ./test/test.o -o ./test/test