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
	
		in.open("./out.txt"); //ios::trunc��ʾ�ڴ��ļ�ǰ���ļ����,������д��,�ļ��������򴴽�

		map<string, Type> st;
	 
		stackTable.push(st);


	}
	void funCheck();

	virtual ~TypeCheck()
	{
		//delete fun;�Ѿ��ڱ��ͷ�
		in.close();
	}

	void setParser(Parser *_parser)
	{
		parser = _parser;
	}


//private:
	typedef stack<map<string, Type>  > StackTable;
	Fun* fun;
	StackTable stackTable;//ÿ��ջ�ռ�������һ��������ÿ����������һ�����ű��������map��ʵ�֣������Ƕ���ı�����ÿ����һ�����������������û�ж������
	ofstream in;
	Parser *parser;

	void classfunCheck(ClassObject* _classobject );
	
	void generate(string str);//������ļ�
	void close();             //�ر��ļ�
	void defVariable(string name, Type type);
	void defVariable2(string name, Type type);
	void assignVariable(string name, Type type);
	void stmtCheck(Stmt *stmt) ;
	Type judgeCheck(Judge* judge);
	void genwithoutendl(string str);
	void argCheck(Stmt* _arglist)  ;
};
#endif