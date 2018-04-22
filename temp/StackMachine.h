#ifndef _SM_
#define _SM_
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


class StackMachine :public MemoryTool
{


public:
	void alu();
	map<string, string> memory;
	// private:
	stack<string> aluStack;
	bool whileflag;
	string itos(int i)
	{
		ostringstream stream;
		stream << i;  //nÎªintÀàÐÍ
		return stream.str();
	}


	~StackMachine()
	{
		//this->memory.clear();
	     
	}

};
#endif