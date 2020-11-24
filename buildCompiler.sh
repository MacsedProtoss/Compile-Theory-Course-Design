cd ./src && flex lex.l && bison -d -x parser.y
cd .. && g++ -g ./src/ast.cc ./src/lex.yy.c ./src/parser.tab.c ./src/main.cc ./src/analyzer.cc ./src/functions.cc ./src/variables.cc ./src/ir.cc -I/usr/lib/llvm-10/include -L/usr/lib/llvm-10/lib -lLLVM-10 -std=c++2a -o ./
