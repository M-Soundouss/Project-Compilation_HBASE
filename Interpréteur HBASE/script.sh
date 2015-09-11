#Ị/bin/bash
# sh exec 1
flex -i $1.l
mv lex.yy.c $1.yy.c
gcc -c $1.yy.c -lfl
gcc -c $1.c
gcc -o parser $1.yy.o $1.o
./parser
exit 0
