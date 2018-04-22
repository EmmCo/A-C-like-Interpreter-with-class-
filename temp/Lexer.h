#ifndef _LEXER_
#define _LEXER_
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
#include <sstream>
#include <iostream>
#include <fstream>  
#include <hash_set>
#include "Token.h"
#include "MemoryManager.h"
class Lexer :public MemoryTool{

public:
	vector<Token*> tokens;
	Lexer() :line(1), pos(1), cursor(0), _EOF(false), buffer(new char[BufMaxLen]), bufLen(0)
	{
		 
	}
	void lex();
	void print();
	virtual ~Lexer()
	{
		free(buffer);
		
		for (int i = 0; i < tokens.size(); i++)
		{
			get_mpinstance().ReleasePointer(tokens[i]);//返回给空间
			tokens[i]->~Token();
		}
		//tokens.~vector();
		//str.~basic_string();
	}
private:
	static const int BufMaxLen = 256;
	std::hash_set<string> classset;
	int	line;
	int 	pos;
	int  cursor;
	int  bufLen;
	bool   _EOF;
	string  str;
	string filename;
	char *buffer;  //= new char[BufMaxLen]; C++11 允许就地赋值

	void refresh();
	char nextChar();
	void eatSpace();
	
	bool isIdentifier(char ch);
	bool isNumber(char ch);
	bool isLogicalOperator(char ch);
	bool isArithmeticOperator(char ch);

	void strGenerate();
	void numGenerate();
	void idGenerate();
	void logGenerate();
	void ariGenerate();
};



#endif