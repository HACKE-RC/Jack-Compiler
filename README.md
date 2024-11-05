# Jack Compiler
Jack Programming Language Compiler written purely in C++, does not follow most of the compiler thoery and is therefore extremely complicated.
The purpose of the project was for me to have fun and build something functional, it does not therefore return errors in most cases, is not efficient, does not follow any guidelines, it's freestyle code and should not be used by anyone to do real compilation. There are other compilers which follow compiler thoery and can do that much more efficiently.

The code quality is terrible, please do not use this to judge my programming skills.

It passes all the test given in the Nand2Tetris course and can theoritically compile any piece of jack code. 

# Running the output
The compiler produces output in the Jack Virtual Machine (JVM, unrelated to java) code, so in order to run it you would have to get the software suite from [here](https://www.nand2tetris.org/software) and then open the folder which contains the output files and it will run it. 
The output JVM files are stored in tests/<test_name>/output folder.
