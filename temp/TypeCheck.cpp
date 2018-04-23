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
using namespace std;
#define new new(__FILE__,__LINE__)


#define GLOBAL "._global."

static string GetTypeString(Type _type)
{
	switch (_type)
	{
	case Type::_Int:
		return "int";
	case Type::_Bool:
		return "bool";
	case Type::_Str:
		return "string";
	case Type::_Float:
		return "float";
	case Type::_Class:
		return "class";
	case Type::_Object:
		return "object";
	case Type::_Void:
		return "void";
	}
}

void TypeCheck::argCheck(Stmt* _arglist) 
{
	stmtCheck(_arglist);
	generate("argend");
	while (_arglist)
	{
		generate("store " + ((Def *)_arglist)->getID()->getName());
		_arglist = _arglist->next;
	}
}

void TypeCheck::funCheck()
{

	 Parser::Classestype::iterator       classit  = parser->GetChildClasses().begin();
	 Parser::Classestype::iterator const classend = parser->GetChildClasses().end();
	 while (classit != classend)
	 {
		 classfunCheck(*classit);
		 classit++;
	 }

	 Fun* cur = fun;
	 while (cur)//全局函数	 
	 {
		 string _funstart;
		 if (cur->GetidName() == "main")
		 {
			 _funstart = GLOBAL;
			 _funstart += "_sTaRt" + cur->GetFunName();
		 }
		 else
			 _funstart = GLOBAL + cur->GetFunName();


		 generate(".fundefine" + _funstart);

		 argCheck(cur->arglist);

		 stmtCheck(cur->stmt);
		 generate(".functionend " + _funstart);

		 cur = cur->next;
	 }



	 generate(".fileend");
	 close();
	 return;
 }




void TypeCheck::classfunCheck( ClassObject* _classobject )
{
	if (_classobject == nullptr)
		return;
	Fun* cur = _classobject->GetFun();
	
	while (cur)
	{
		string _funstart = ".fundefine." + _classobject->GetClassName() + "." + cur->GetFunName();
		generate(_funstart);
		 argCheck(cur->arglist);
		stmtCheck(cur->stmt);
		generate("functionend " + _funstart);
		cur = cur->next;
	}

	Parser::Classestype::iterator        classit = _classobject->GetChildClasses().begin();
	Parser::Classestype::iterator const classend = _classobject->GetChildClasses().end();
	while (classit != classend)
	{
		classfunCheck(*classit);	
		classit++;
	}
	return;
}


void TypeCheck::generate(string str)//输出至文件
{
	in << str << endl;
}

void TypeCheck::genwithoutendl(string str)//输出至文件
{
	in << str ;
}

void TypeCheck::close()             //关闭文件
{
	in.close();
}
void TypeCheck::defVariable(string name, string type)
{
	defVariable2(name, type);
	generate("store " + name);
}

void TypeCheck::defVariable2(string name, string type)
{
	if (stackTable.top().find(name) != stackTable.top().end())
	{
		//重复定义了变量
	//	wait_for_debug();
	}
	stackTable.top()[name] = type;
	/*
	switch (type)
	{
	case Type::_Bool:
		generate(".bool " + name);
		break;
	case Type::_Int:
		generate(".int " + name);
		break;
	case Type::_Str:
		generate(".string " + name);
		break;
	case Type::_Float:
		generate(".float " + name);
		break;
	case Type::_Unknown:
		generate(".unknown " + name);
		break;

	case Type::_Class:
		generate(".class " + name);
		break;
	}
	 */
}


void TypeCheck::assignVariable(string name, string type)
{
	generate("store " + name);
}
void TypeCheck::stmtCheck(Stmt *stmt) //这是递归函数
{
	string rhs;
	bool flag = true;
	Stmt* cur = stmt;
	while (cur)//遍历每一个语句
	{
		switch (cur->kind)
		{
		  case Stmt::Kind::DefStmt: //定义式
		  {
			  Def *def = static_cast<Def*>(cur);
			  Id  *id = def->getID();//获取这个定义式中的变量的id

			  if (def->getJudge() != nullptr)//只是纯粹的定义，定义时没有赋值
			  {
				  rhs = judgeCheck(def->getJudge());//判断Judge是否合法
				  if (id->getTypeName() != rhs)
				  {
					  wait_for_debug();
				  }
				  else
				  {
					  defVariable(id->getName(), rhs);//定义这个变量，放入map里
				  }
			  }
			  else
			  {
				  rhs = id->getType();
				  defVariable2(id->getName(), rhs);//定义这个变量，放入map里,不产生store语句。
			  }
			  break;
		   }
		  case Stmt::Kind::AssignStmt:
		  { 
			  Assign *assign = static_cast<Assign*>(cur);
		      Id  *id = assign->getID();
	    	  rhs = judgeCheck(assign->getJudge());
     		  assignVariable(id->getName(), rhs);

		      break;
		  }
		  case Stmt::Kind::IfStmt:
		  {
			If *iff = static_cast<If *>(cur);
			string type = judgeCheck(iff->getCondition());
			if (type != "bool")
			{
				wait_for_debug();
			}
			else
			{
				generate("if");
				stmtCheck(iff->getThen());
				generate("otherwise");
				stmtCheck(iff->getOtherwise());
				generate("end");
			}
			break; 
		  }
		  case Stmt::Kind::PrintStmt:
		  {


			break; 
		  }

		  case Stmt::Kind::FunStmt:
		  {


			break; 
		  }
		case Stmt::Kind::JudgeStmt://Judge产生的临时语句变量
		  {


			break; 
		  }

		  case Stmt::Kind::WhileStmt:
		  {  
			 While *wh = static_cast<While *>(cur);
			 generate("while");
			 string type = judgeCheck(wh->getCondition());
			 if (type != "bool")
			 {
				 wait_for_debug();
			 }
			 else
			 {
				 generate("then");
				 stmtCheck(wh->getThen());
				 generate("end");
			 }

		     break; 
		  }
		}
		cur = cur->next;
	}
	 
}
string TypeCheck::judgeCheck(Judge* judge)
{
	string left, right;
	switch (judge->kind)
	{
	    case Judge::Kind::ADD:
	  {
        Add *add = static_cast<Add*>(judge);
	    left = judgeCheck(add->left);
	    right = judgeCheck(add->right);
		if (left != "int" || right != "int")
		   wait_for_debug();
	       else
	       {
		     generate("add");
	       }
	       return "int";
	   }
		case Judge::Kind::MINUS:
		{
           Minus *minus = static_cast<Minus*>(judge);
		   left = judgeCheck(minus->left);
		   right = judgeCheck(minus->right);
		   if (left !=  "int" || right != "int")
		   {
			wait_for_debug();
		   }
		   else
		   {
			generate("minus");
		    }
		   return "int";
		}
		
		case Judge::Kind::TIMES:
		{ 
           Times *times = static_cast<Times*>(judge);
		   left = judgeCheck(times->left);
		   right = judgeCheck(times->right);
		   if (left != "int" || right != "int")
		   {
			wait_for_debug();
		   }
		   else
	       {
			generate("times");
		   }
		   return "int";
		}

		case Judge::Kind::DIVISION:
		{
	      Division *division = static_cast<Division*>(judge);
		  left = judgeCheck(division->left);
		  right = judgeCheck(division->right);
		  if (left != "int" || right != "int")
		  {
			wait_for_debug();
		  }
		  else
		  {
			generate("division");
		   }
		  return "int";
		}

		 case Judge::Kind::GE:
		{
          Ge *ge = static_cast<Ge*>(judge);
		  left = judgeCheck(ge->left);
		  right = judgeCheck(ge->right);
		  if (left != "int" || right != "int")
		  {
			wait_for_debug();
		   }
		  else
		  {
			generate("ge");
		   }
		   return "bool"; 
		 }

		case Judge::Kind::LE:
		{
		  Le *le = static_cast<Le*>(judge);
		  left = judgeCheck(le->left);
		  right = judgeCheck(le->right);
		 if (left != "int" || right != "int")
		  {
			wait_for_debug();
		  }
		 else
		  {
			generate("le");
		  }
		 return "bool";
		}

		case Judge::Kind::G:
		{G *g = static_cast<G*>(judge);
		left = judgeCheck(g->left);
		right = judgeCheck(g->right);
		if (left != "int" || right != "int")
		{
			wait_for_debug();
		}
		else
		{
			generate("g");
		}
		return "bool"; 
		}
		case Judge::Kind::L:
		{L *l = static_cast<L*>(judge);
		left = judgeCheck(l->left);
		right = judgeCheck(l->right);
		if (left != "int" || right != "int")
		{
			wait_for_debug();
		}
		else
		{
			generate("l");
		}
		return "bool"; 
		}
		case Judge::Kind::EQUAL:
		{
		Equal *equal = static_cast<Equal*>(judge);
		left = judgeCheck(equal->left);
		right = judgeCheck(equal->right);
		if (left!=right  )
		{
			wait_for_debug();
		}
		else
		{
			generate("equal");
		}
		return "bool";
		}
		case Judge::Kind::AND:
		{And *and = static_cast<And*>(judge);
		left = judgeCheck(and->left);
		right = judgeCheck(and->right);
		if (left != "bool" || right != "bool")
		{
			wait_for_debug();
		}
		else
		{
			generate("and");
		}
		return "bool"; 
		}
		case Judge::Kind::OR:
		{Or *or = static_cast<Or*>(judge);
		left = judgeCheck(or->left);
		right = judgeCheck(or->right);
		if (left != "bool" || right != "bool")
		{
			wait_for_debug();
		}
		else
		{
			generate("or");
		}
		return "bool";  }
		case Judge::Kind::NEGATIVE:
		{Negative *negative = static_cast<Negative*>(judge);
		right = judgeCheck(negative->follow);
		if (right != "int")
		{
			wait_for_debug();
		}
		else
		{
			generate("negative");
		}
		return "int"; }

		case Judge::Kind::NOT:
		{Not *not = static_cast<Not*>(judge);
		right = judgeCheck(not->follow);
		if (right != "bool")
		{
			wait_for_debug();
		}
		else
		{
			generate("not");
		}
		return "bool";
		}

		case Judge::Kind::ID:
		{
		  Id *id = static_cast<Id*>(judge);
		  
		  generate("load " + id->getName());

		  if (stackTable.top().find(id->getName()) == stackTable.top().end())
		  {
			   wait_for_debug();//没有定义变量
		  }
		  
		  if (id->getNext() != nullptr)
		  {   //全局变量
			  if (stackTable.top().find(id->getName()) == stackTable.top().end())
			  {
				  
				  
			  }
			  else//局部变量
			  {

			  }
		  }



		  if (id->getNext() != nullptr)
			  return judgeCheck(id->getNext());
		  else
		  return ((stackTable.top()))[id->getName()];
		}
		case Judge::Kind::FUN:
		{
          Function *func = static_cast<Function*>(judge);

		  std::stack<Judge*> argstack = func->GetArgstack();

		  generate("call ");

		  //从右到左的方式压入栈内
		  string argname="";
		  while (!argstack.empty())
		  {
			  string result = judgeCheck(argstack.top()); argstack.pop();
			  argname +=  (result);
		  }

		  if (parser->isSymbol(func->getidName() + argname, Type::_Fun))
		  {
			  string namestr = "callfun ";
			  namestr += GLOBAL;
			  namestr += (func->getidName() + argname);
			  generate(namestr);
		  }
		  else
		  {
			  wait_for_debug();//没有定义这个函数
		  }
		  if (func->getNext()!=nullptr)
			  return judgeCheck(func->getNext());
		  else
		  return  parser->GetFunType(func->getidName() + argname);
		}

		case Judge::Kind::VAL:
		{	
		   Value *value = dynamic_cast<Value*>(judge);
		   string out = value->toString();
		   generate("push " + out);
		   return value->type;
		}
	}
	 
}
 