echo "compiler run start"
path=`pwd`
echo "generating ir[1/3]"
$path/compiler $path/test/test.swiftl >> $path/test/test.ir
echo "done"
echo "llc compiling[2/3]"
llc -march=x86-64 $path/test/test.ir --filetype=obj -o $path/test/test.o
echo "done"
echo "clang linking[3/3]"
clang -v $path/test/test.o -o $path/test/test
echo "done"
echo "compiler run finished"