# A-C-like-Interpreter-with-class. This is a interpreter based on C++, inputing source code ( C with class ) for interpretation and execution

IDE -->MicroSoft Visual Studio 2013

This is a interpreter based on C++, inputing source code ( C with class ) for interpretation and execution, use DFA to construct lexical analyzer to get word Token, then use recursive descent method to generate syntax analysis tree, and then generate target assembler code by type check system, and finally get the final calculation result by the stack machine analysising target assembly code.The memory manager is used for memory allocation, and memory allocation records are output. Mark&Sweep is used to garbage collect.



---------------------
- [x] 03/2/2018  Start work
- [x] 03/4/2018  Complete Lexical analyzer     ---> Lexer.cpp
- [x] 03/7/2018  Complete Syntax analysis Tree ---> ast.cpp
- [x] 03/18/2018 Complete Generating target assembler code by type check--> TypeCheck.cpp
- [x] 03/22/2018 Complete Stack machine analysis assembly code     --->StcakMachine.cpp
- [x] 03/28/2018 Complete Memory manager and Garbage collect system--->MemoryManager.cpp 
                                                                   --->MarkSweep.cpp
---------------------
- [x] 04/5/2018  Start work for supporting Object-oriented
- [x] 04/10/2018 Complete supporting for Encapsulation of object-oriented
- [x] 04/20/2018 Complete supporting for Polymorphism of object-oriented（80%）
- [ ] 04/30/2018 Complete supporting for Inheritance of object-oriented （10%）
