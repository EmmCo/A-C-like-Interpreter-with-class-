# A-C-like-Interpreter-with-class-
IDE-- MicroSoft Visual Studio 2013

This is a interpreter based on C++, input source code( C with class ) interpretation and execution, use DFA to construct lexical analyzer to get word Token, then use recursive descent method to generate syntax analysis tree, and then generate target assembler code by type check, and finally get the final calculation result by the stack machine analysis target assembly code.The memory manager is used for memory allocation, and memory allocation records are output. Mark&Sweep is used to garbage collect.



---------------------
- [ ] 03/2/2018  Start work
- [x] 03/4/2018  Complete Lexical analyzer     ---> Lexer.cpp
- [x] 03/7/2018  Complete Syntax analysis Tree ---> ast.cpp
- [x] 03/18/2018 Complete Generate target assembler code by type check ---> TypeCheck.cpp
- [x] 03/22/2018 Complete Stack machine analysis target assembly code  ---> StcakMachine.cpp
- [x] 03/28/2018 Complete Memory manager and Garbage collect system --->MemoryManager.cpp 
  ---> MarkSweep.cpp
---------------------
- [ ] 04/5/2018  Start work for Object-oriented
- [x] 04/10/2018 Complete Encapsulation of object-oriented
- [x] 04/20/2018 Complete Polymorphism  of object-oriented 
- [ ] 04/30/2018 Complete Inheritance of object-oriented 
