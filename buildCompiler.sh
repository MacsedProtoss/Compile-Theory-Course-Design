echo "compiler build start"
path=`pwd`
cd $path/src
echo "running flex[1/3]"
flex lex.l
echo "done"
echo "running bison[2/3]"
bison -d -x parser.y
echo "done"
cd $path
echo "running g++[3/3]"
g++ -g $path/src/ast.cc $path/src/lex.yy.c $path/src/parser.tab.c $path/src/main.cc $path/src/analyzer.cc $path/src/functions.cc $path/src/variables.cc $path/src/ir.cc -I/usr/lib/llvm-10/include -L/usr/lib/llvm-10/lib -lLLVM-10 -std=c++2a -o $path/compiler
echo "done"
echo "compiler build finshed"