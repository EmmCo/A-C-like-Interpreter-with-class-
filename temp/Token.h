#ifndef _TOKEN_
#define _TOKEN_
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
#include "MemoryManager.h"
 
using namespace std;
static std::hash_set<string> typeset;
static bool lastclass;
class Token :public MemoryTool
{
    public :
		enum Type
		{
			IF, ELSE, WHILE, ID, NUM,FLOAT, BOOL, STR, TYPE, ASSIGN, COM, DOT, LOP, AOP, ROP, BRA, SEMI, PRINT,EXTEND,NEW,RETURN,CLASS,FUN
		};
		Type type;
		string value;
		int line;
		int pos; 
		 
		Token(Type _type, string _value, int _line, int _pos) :value(_value), line(_line), pos(_pos - _value.length())
		{
			if (Type::ID == _type && (_value == "if"))
			{
				type = Type::IF;
			}
			else if (Type::ID == _type && (_value == "extend"))
			{
				type = Type::EXTEND;
			}
			else if (Type::ID == _type && (_value == "return"))
			{
				type = Type::RETURN;
			}
			else if (Type::ID == _type && (_value == "new"))
			{
				type = Type::NEW;
			}
			else if (Type::ID == _type && (_value == "else"))
			{
				type = Type::ELSE;
			}
			else if (Type::ID == _type && (_value == "while"))
			{
				type = Type::WHILE;
			}
			else if (Type::ID == _type && (_value == "class"))
			{
				type = Type::CLASS;
				lastclass = true;

			}
			else if (Type::ID == _type && (_value == "int" || _value == "bool" || _value == "string" || _value == "void" || _value == "float" || (lastclass) || (typeset.find(_value) != typeset.end())))
			{
				type = Type::TYPE;
				if (lastclass)
				{
					typeset.insert(_value);
					lastclass = false;
				}
			}
			else if (Type::ID == _type && (_value == "true" || _value == "false" || _value == "True" || _value == "False"))
			{
				type = Type::BOOL;
			}
			else if (Type::ID == _type && (_value == "print"))
			{
				type = Type::PRINT;
			}
			else
			{
				type = _type;
			}
		}
		string toString()
		{
			string _temp="";
			stringstream types, lines, poss;
			types << type;
			lines << line;
			poss <<  pos;
			
			return "Type :"+ types.str() + "Value :" + value + ", line :"+lines.str() + ", position :" + poss.str()  ;
		}
		~Token()
		{
			//value.~basic_string();
		}

};


 

#endif