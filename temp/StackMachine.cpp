#include <stdio.h>
#include "stdafx.h"
#include <string>
#include <vector>
#include <stack>
#include <map>
#include <algorithm>
#include <assert.h>
#include "ast.h"
#include "Token.h"
#include "TypeCheck.h"
#include "StackMachine.h"
using namespace std;

 
#define new new(__FILE__,__LINE__)

void  StackMachine::alu()
{
	string temp, temp1;
	static ifstream in("out.txt");
	whileflag = false;
	streampos pos,mainpos;
	stack<streampos > ebp;
	int     ebp_ptr;
	std::map<string, streampos> _functionmap;
	
	bool _run=false;
	
	while (getline(in, temp))
	{
		if (!_run)//遍历各个函数的地址
		{
		 if (temp.find(".fundefine") == 0)
		  {
			temp1 = temp.substr(10);
			_functionmap[temp1] = in.tellg();

			if (temp1.find("_sTaRtmain") != -1)
			{
				mainpos = _functionmap[temp1];
			}
		  }
		 if (temp.find(".fileend") == 0)
		 {
			 assert(mainpos.state()==0);
			 _run = true;
			 in.seekg(mainpos);
		 }
		 continue;
		}

		if (temp.find("push") == 0)//push为temp的起始位置，-1则没找到
		{
			temp1 = temp.substr(5);
			if (temp1[0] >= '0'&&temp1[0] <= '9')
			{
				aluStack.push(temp1);
			}
			else if (temp1 == "true" || temp1 == "True")
			{
				aluStack.push("true");
			}
			else if (temp1 == "false" || temp1 == "False")
			{
				aluStack.push("false");
			}
			else
			{
				aluStack.push(temp1);
			}
		}
		else if (temp.find("load") == 0)
		{
			temp1 = temp.substr(5);
			aluStack.push(memory[temp1]);//装载"temp1"变量到变量表里
		}
		else if (temp.find("store") == 0) //从栈顶读取一个值放入变量表里
		{
			temp1 = temp.substr(6);
			memory[temp1] =aluStack.top();
			aluStack.pop();
		}
		else if (temp == "add")
		{
			int t1 = atoi(aluStack.top().c_str());  aluStack.pop();
			int t2 = atoi(aluStack.top().c_str());  aluStack.pop();
			int t = t1 + t2;
			aluStack.push(itos(t));
		}
		else if (temp =="minus")
		{
			int t1 = atoi(aluStack.top().c_str());  aluStack.pop();
			int t2 = atoi(aluStack.top().c_str());  aluStack.pop();
			int t =  t2-t1;
			aluStack.push(itos(t));
		}
		else if (temp=="times")
		{
			int t1 = atoi(aluStack.top().c_str());  aluStack.pop();
			int t2 = atoi(aluStack.top().c_str());  aluStack.pop();
			int t = t1 * t2;
			aluStack.push(itos(t));
		}
		else if (temp=="division")
		{
			int t1 = atoi(aluStack.top().c_str());  aluStack.pop();
			int t2 = atoi(aluStack.top().c_str());  aluStack.pop();
			if (t1 == 0)
				wait_for_debug();
			int t =  t2/t1;
			aluStack.push(itos(t));
		}
		else if (temp=="ge")
		{
			int t1 = atoi(aluStack.top().c_str());  aluStack.pop();
			int t2 = atoi(aluStack.top().c_str());  aluStack.pop();
			if (t1<=t2)
				aluStack.push("true");
			else
				aluStack.push("false");
		}
		else if (temp == "le")
		{
			int t1 = atoi(aluStack.top().c_str());  aluStack.pop();
			int t2 = atoi(aluStack.top().c_str());  aluStack.pop();
			if (t1 >= t2)
				aluStack.push("true");
			else
				aluStack.push("false");
		}
		else if (temp == "g")
		{
			int t1 = atoi(aluStack.top().c_str());  aluStack.pop();
			int t2 = atoi(aluStack.top().c_str());  aluStack.pop();
			if (t1 < t2)
				aluStack.push("true");
			else
				aluStack.push("false");
		}
		else if (temp == "l")
		{
			int t1 = atoi(aluStack.top().c_str());  aluStack.pop();
			int t2 = atoi(aluStack.top().c_str());  aluStack.pop();
			if (t1 > t2)
				aluStack.push("true");
			else
				aluStack.push("false");
		}
		else if (temp == "equal")
		{
			string t1 = aluStack.top();  aluStack.pop();
			string t2 = aluStack.top();  aluStack.pop();
			if (t1 == t2)
				aluStack.push("true");
			else
				aluStack.push("false");
		}
		else if (temp == "and")
		{
			bool _bo,_bo2;
			string st = aluStack.top(); aluStack.pop();
			if (st == "true")
			{
				_bo = true;
			}
			else
			{
				_bo = false;
			}
			st = aluStack.top(); aluStack.pop();
			if (st == "true")
			{
				_bo2 = true;
			}
			else
			{
				_bo2 = false;
			}
			if (_bo&&_bo)
			{
				aluStack.push("true");
			}
			else
				aluStack.push("false");

		}
		else if (temp == "or")
		{
			bool _bo, _bo2;
			string st = aluStack.top(); aluStack.pop();
			if (st == "true")
			{
				_bo = true;
			}
			else
			{
				_bo = false;
			}
			st = aluStack.top(); aluStack.pop();
			if (st == "true")
			{
				_bo2 = true;
			}
			else
			{
				_bo2 = false;
			}
			if (_bo||_bo)
			{
				aluStack.push("true");
			}
			else
				aluStack.push("false");
		}
		else if (temp == "not")
		{
			bool _bo;
			string st = aluStack.top(); aluStack.pop();
			if (st == "true")
			{
				_bo = true;
			}
			else
			{
				_bo = false;
			}
		 
			if (!_bo)
			{
				aluStack.push("true");
			}
			else
				aluStack.push("false");
		}
		else if (temp == "negative")
		{
			int t1 = atoi(aluStack.top().c_str());  aluStack.pop();
			aluStack.push(itos(-t1));
		}
		else if (temp == "print")
		{

		}
		else if (temp == "if")
		{
			int counter = 1;
			bool _bo;
			string st = aluStack.top(); aluStack.pop();
			if (st == "false")
			{
				while (counter!=0)
				{
					getline(in, temp);
					if (temp == "if")
					{
						counter++;
					}
					else if (temp == "otherwise")
						counter--;
				}
			}
		}
		else if (temp == "otherwise")
		{
			while (true)
			{
				getline(in, temp);
				if (temp == "end")
					break;
			}
		}
		else if (temp == "while")
		{
			pos = in.tellg();
		}
		else if (temp == "then")
		{
			string st = aluStack.top(); aluStack.pop();
			if (st == "false")
			{
				whileflag = false;
				while (temp!="end")
				 getline(in, temp);
			}
			else if (st == "true")
			{
				whileflag = true;
			}
		}
		else if (temp == "end")
		{
			if (whileflag)
			{
				in.seekg(pos);
			}
		}
		
		else if (temp == "call")
		{
		 
		}
		else if (temp.find("callfun ") == 0)
		{
			temp1 = temp.substr(8);
			ebp.push(in.tellg());
			in.seekg(_functionmap[temp1]);//跳转
		}
		else if (temp.find(".functionend ") == 0)
		{
			aluStack.push("1");
			if (temp.find("_sTaRtmain") != -1)
			{
				continue;
			}
			in.seekg(ebp.top());
			ebp.pop();
		}
		else if (temp[0] == '.')
		{


		}
		else
		{
		//	wait_for_debug();
		}


	}

	 
} 