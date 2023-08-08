This misc directory is for showcasing how to compress data into binary and uncompressing it. It also contains the tasks.json for compiling on macbook and windows. 
- Note Windows compilation tasks.json requires clang++, and was installed with LLVM on windows 10 at C:\Program Files\LLVM\bin\clang++.exe in case it matters.
- To use the tasks.json for your OS switch it with the one currently in the .vscode folder and rename it to tasks.json. Compiled using the vscode CMAKE build tools, although you can plop the build command into a Makefile or run it manually if you want. 

How to compile and run the CompressionTest.cpp file

1. Run the command: 
    /usr/bin/clang++ CompressionTest.cpp
in the bin directory.

2. Run the compiled binary by running:
    ./a.out
in the bin directory.