#ifndef _TYPECHECK_
#define _TYPECHECK_
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "stdafx.h"
#include <string>
#include <vector>
#include <stack>
#include <map>
#include <algorithm>
#include <assert.h>
#include <iostream>
#include <fstream>
#include "Token.h"
#include "ast.h"
#include "MemoryManager.h"
 

using namespace std;
 

class TypeCheck : public MemoryTool
{
	


public:
	TypeCheck(Fun* _fun) :fun(_fun){
	
		in.open("./out.txt"); //ios::trunc表示在打开文件前将文件清空,由于是写入,文件不存在则创建

		map<string, Type> st;
	 
		stackTable.push(st);


	}
	void funCheck();

	virtual ~TypeCheck()
	{
		//delete fun;已经在别处释放
		in.close();
	}

	void setParser(Parser *_parser)
	{
		parser = _parser;
	}


//private:
	typedef stack<map<string, Type>  > StackTable;
	Fun* fun;
	StackTable stackTable;//每个栈空间代表的是一个作用域，每个作用域有一个符号表，这个表用map来实现，里面是定义的变量，每定义一个变量会检查表里面有没有定义过。
	ofstream in;
	Parser *parser;

	void classfunCheck(ClassObject* _classobject );
	
	void generate(string str);//输出至文件
	void close();             //关闭文件
	void defVariable(string name, Type type);
	void defVariable2(string name, Type type);
	void assignVariable(string name, Type type);
	void stmtCheck(Stmt *stmt) ;
	Type judgeCheck(Judge* judge);
	void genwithoutendl(string str);
	void argCheck(Stmt* _arglist)  ;
};
#endif