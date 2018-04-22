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
using namespace std; 

#define new new(__FILE__,__LINE__)  

StackSymbolTable stacksymboltable;

SymbolTable symboltable;
/*
class Judge
{
public :	
	enum Kind{
		ADD, MINUS, TIMES, DIVISION, NEGATIVE, NOT, OR, AND, EQUAL, GE, LE, G, L, ID, VAL
	};
	  Kind kind; 
	  Judge(Kind _kind) :kind(_kind)
	  {}
};
*/

Parser::Parser(string name, vector<Token*> _tokens) :BaseObject(name), tokens(_tokens), length(_tokens.size()), index(0), ErrorLog("")
{

}


Fun* Parser::parse()
{
	return parse(this);
	//return nullptr;
}

Fun* Parser::parse(BaseObject * _object) //���ǵݹ麯��
{
	Token* token = nextToken();
	Fun* _fun = nullptr;

	do
	{
	 if (token->type == Token::Type::CLASS)//class parse
	  {
		_object->AddClass(classparse(_object));
	  }
	 else if (token->type == Token::Type::TYPE)
	  {
		if (_fun != nullptr)
			_fun->add(funParse(_object));
		else
			_fun = funParse(_object);
	  }
	 else if (token->value == "}")
	 {
		 index--; break;
	 }
	} while ((token = nextToken())!= nullptr);

 
	_object->SetFun(_fun);
	return _fun;
}

ClassObject* Parser::classparse(BaseObject * _object)
{
	ClassObject *_classobject;
	Token* token = nextToken();
	string idname;

		if (token->type != Token::Type::ID)
		{
			//wait_for_debug();
		}
		idname = token->value;
		
		_classobject = new ClassObject(idname);

		_object->AddSymbol(idname, Type::_Class);

		token = nextToken();
		if (token->type == Token::Type::EXTEND) //class A extend B,C,D{ }
		{
			token = nextToken();
			
			if (token->type != Token::Type::TYPE)
			{
				wait_for_debug();
			}
			
			_classobject->ExtendAdd(token->value);

			while (nextToken()->type==Token::Type::COM)
			{
				_classobject->ExtendAdd(nextToken()->value);
			}	 
		}	 
		index--;	
		token = nextToken();
		if (token->value != "{")
		{
			ErrorLog += "expected function left bracket :" + token->toString() + "! \n";
			wait_for_debug();
			return nullptr;
		}

		parse(_classobject);
		
		token = nextToken();
		if (token->value != "}")
		{
			ErrorLog += "expected function right bracket :" + token->toString() + "! \n";
			wait_for_debug();
			return nullptr;
		}
		return _classobject;
}

Stmt* Parser::argsParse(BaseObject * _object)
{
	Stmt* stmt = argParse( _object);

	while (nextToken()->value != ")")
	{
		index--;
		stmt->add(argParse( _object));
	}
	index--;
	return stmt;
}

Stmt* Parser::argParse(BaseObject * _object)
{ 
	Token* token = nextToken();

	if (token == nullptr)
		return nullptr;

	if (token->type == Token::Type::TYPE)//int, bool ,string...
	{
		Type type = typeJudge(token, _object);
		Id *id;
		Judge *judge;
		token = nextToken();

		if (token->type != Token::Type::ID)
		{
			ErrorLog += "expected id :" + token->toString() + "! \n";
			wait_for_debug();
			return nullptr;
		}
		else
		{
			id = new Id(type, token->value);
			token = nextToken();
			if (token->type == Token::Type::COM) 
			{
				return new Def(id, nullptr);
			}
			else if (token->value == ")")
			{
				index--;
				return new Def(id, nullptr);
			}
			else
			{
				ErrorLog += "parsing error :" + token->toString() + "! \n";
				wait_for_debug();
				return nullptr;
			}
		}
	}
	else 
	{
		index--;
		return nullptr;
	}
}
Fun* Parser::funParse(BaseObject * _object)
{
	index--;
	Token* token = nextToken();
	Type type;
	string id;
	Stmt* stmt    = nullptr;
	Stmt* arglist = nullptr;
	//   func -> type id () block

	type = typeJudge(token, _object);

	token = nextToken(); //ID
	if (token->type != Token::Type::ID)
	{
		ErrorLog += "expected function name  :" + token->toString() + "! \n";
		wait_for_debug();
		return nullptr;
	}
	else
	{
		id = token->value;
	}
	token = nextToken();
	if (token->type == Token::Type::ASSIGN)
	{
		token = nextToken();
		if (token->type != type)
		{
			wait_for_debug();
		}

		_object->AddMember(new Def(new Id(type, id), judgeParse()));
		return nullptr;
	}
	if (token->type == Token::Type::SEMI)
	{
		_object->AddMember(new Def(new Id(type, id), nullptr));
		return nullptr;
	}
	 
	if (token->value != "(")
	{
		ErrorLog += "expected function left paren :" + token->toString() + "! \n";
		wait_for_debug();
		return nullptr;
	}

	arglist = argsParse( _object);

	token = nextToken();
	if (token->value != ")")
	{
		ErrorLog += "expected function right paren :" + token->toString() + "! \n";
		wait_for_debug();
		return nullptr;	
	}
	
	stmt = blockParse( _object);

	Fun *fun = new Fun(type, id, arglist, stmt);
	return fun;
}

Stmt* Parser::blockParse(  BaseObject * _object)
{
	Stmt  *stmt=nullptr;
	Token *token = nextToken();

	if (token->value != "{")
	{
		ErrorLog += "expected function left bracket :" + token->toString() + "! \n";
		wait_for_debug();
		return nullptr;
	}
	if (nextToken()->value == "}")
	{
		index--;
	}
	else
	{
		index--;
		stmt = stmtsParse(  _object);
	}
	  
	token = nextToken();
	if (token->value != "}")
	{
		ErrorLog += "expected function right bracket :" + token->toString() + "! \n";
		wait_for_debug();
		return nullptr;
	}
	return stmt;
}

Stmt* Parser::stmtsParse(BaseObject * _object)
{
	Stmt* stmt = stmtParse(  _object);
	 
	while (nextToken()->value != "}")
	{
		index--;
		stmt->add(stmtParse(  _object));
	}
	index--;
	return stmt;
}

Stmt* Parser::stmtParse(BaseObject * _object)
{
	Token* token = nextToken();

	if (token == nullptr)
		return nullptr;

	if (token->type == Token::Type::TYPE)//int, bool ,string...
	{
		Type type = typeJudge(token, _object);
		Id *id;
		Judge *judge;
		token = nextToken();

		// int id; or int id = num ;
		if (token->type != Token::Type::ID)
		{
			ErrorLog += "expected id :" + token->toString() + "! \n";
			wait_for_debug();
			return nullptr;
		}
		else
		{
			id = new Id(type, token->value);
			token = nextToken();
			if (token->type == Token::Type::SEMI)// just like "int id ;" then do nothing
			{
				return new Def(id, nullptr);
			}
			else if (token->value == "=") //int id = 0; 
			{
				judge = judgeParse();
				token = nextToken();
				if (token->type != Token::Type::SEMI)
				{
					ErrorLog += "expected ; :" + token->toString() + "! \n";
					wait_for_debug();
					return nullptr;
				}
				return new Def(id, judge);
			}
			else
			{
				ErrorLog += "parsing error :" + token->toString() + "! \n";
				wait_for_debug();
				return nullptr;
			}
		}
	}
	else if (token->type == Token::Type::ID)//
	{
		Id *id = new Id(Type::_Unknown, token->value);
		token = nextToken();

			if (token->value == "(")
			{
				index -= 2;//Function
				id->setName("_fun_implicit" + id->getName());	        
				return new Assign(id,judgeParse());
			}
			else if (token->type == Token::Type::DOT)//id.id.id = id;
			{
				index--;
				id->add(classlinknParse());
				token = nextToken();
			}
			else if (token->value!="=")
			{
				ErrorLog += "expected = :" + token->toString() + "! \n";
				wait_for_debug();
				return nullptr;
			}
		 
		Assign* assign = new Assign(id, judgeParse());
		token = nextToken();

		if (token->value != ";")
		{
			ErrorLog += "expected ; :" + token->toString() + "! \n";
			wait_for_debug();
			return nullptr;
		}
		return assign;
	}
	else if (token->value == "while")
	{
		token = nextToken();
		if (token->value != "(")
		{
			ErrorLog += "expected function left bracket :" + token->toString() + "! \n";
			wait_for_debug();
			return nullptr;
		}
		Judge * condition = judgeParse();
		token = nextToken();
		if (token->value != ")")
		{
			ErrorLog += "expected function right bracket :" + token->toString() + "! \n";
			wait_for_debug();
			return nullptr;
		}
		return new While(condition, blockParse(  _object));

	}
	else if (token->value == "print")
	{
		token = nextToken();
		if (token->type == Token::Type::ID)
		{
			Id *id = new Id(typeJudge(token, _object), token->value);
			token = nextToken();
			if (token->value != ";")
			{
				ErrorLog += "expected ; :" + token->toString() + "! \n";
				wait_for_debug();
				return nullptr;
			}
			return new Print(Print::InnerType::Pid, id->getName());
		}
		else if (token->type == Token::Type::NUM || token->type == Token::Type::STR || token->type == Token::Type::BOOL)
		{
			token = nextToken();
			if (token->value != ";")
			{
				ErrorLog += "expected ; :" + token->toString() + "! \n";
				wait_for_debug();
				return nullptr;
			}
			return new Print(Print::InnerType::Pvalue, token->value);
		}
	}
	else if (token->value == "if")
	{
		token = nextToken();
		if (token->value != "(")
		{
			ErrorLog += "expected function left bracket :" + token->toString() + "! \n";
			wait_for_debug();
			return nullptr;
		}

		Judge *condition = judgeParse();

		token = nextToken();
		if (token->value != ")")
		{
			ErrorLog += "expected function right bracket :" + token->toString() + "! \n";
			wait_for_debug();
			return nullptr;
		}

		Stmt *then = blockParse( _object);

		token = nextToken();
		if (token->value == "else")
		{
			return new If(condition, then, blockParse( _object));
		}
		index--;
		return new If(condition, then, nullptr);
	}
	//else if (token->value == "."){}
	else
	{
		judgeParse();

		return nullptr;
	}
	ErrorLog += "Parsing error: " + token->toString() + "! \n";
	wait_for_debug();
	return nullptr;
}

Judge* Parser::judgeParse()   //judge -> judge \| join //���ǵݹ麯��
{                             //                | join
	
	
	
	Judge *re = joinParse();
	Token *token = nextToken();

	while (token->value == "|")
	{
		re = new Or(re, joinParse());
		token = nextToken();
	}
	index--;
	return re;
}

Judge* Parser::joinParse()
{
	Judge *re = equalityParse();
	Token *token = nextToken();

	while (token->value == "&")
	{
		re = new And(re, joinParse());
		token = nextToken();
	}
	index--;
	return re;
}
Judge* Parser::equalityParse()
{
	Judge *re = relParse();
	Token *token = nextToken();

	while (token->value == "==" || token->value == "!=")
	{
		if (token->value == "==")
			re = new Equal(re, relParse());
		else
			re = new Not(new Equal(re, relParse()));

		token = nextToken();
	}
	index--;
	return re;
}
Judge* Parser::relParse()
{
	Judge *re = exprParse();
	Token *token = nextToken();

	if (token->type == Token::Type::ROP)
	{
		if (token->value == ">=")
			return new Ge(re, exprParse());
		if (token->value == ">")
			return new G(re, exprParse());
		if (token->value == "<=")
			return new Le(re, exprParse());
		if (token->value == "<")
			return new L(re, exprParse());

		ErrorLog += "parsing error :" + token->toString() + "! \n";
		wait_for_debug();
		return nullptr;
	}
	else
	{
		index--;
		return re;
	}
}
Judge* Parser::exprParse()
{
	Judge *re = termParse();
	Token *token = nextToken();

	while (token->value == "+" || token->value == "-")
	{
		if (token->value == "+")
			re = new Add(re, termParse());
		else
			re = new Minus(re, termParse());

		token = nextToken();
	}
	index--;
	return re;
}
Judge* Parser::termParse()
{
	Judge *re = unaryParse();
	Token *token = nextToken();

	while (token->value == "*" || token->value == "/")
	{
		if (token->value == "*")
			re = new Times(re, unaryParse());
		else
			re = new Division(re, unaryParse());

		token = nextToken();
	}
	index--;
	return re;
}
Judge* Parser::unaryParse()
{
	Judge *re;
	Token *token = nextToken();

	if (token->value == "!")
	{
		re = new Not(unaryParse());
	}
	else if (token->value == "-")
	{
		re = new Negative(unaryParse());
	}
	else
	{
		index--;
		re = factorParse();
	}
	return re;
}


Judge* Parser::factorParse()
{
	Token *token = nextToken();
	if (token->type == Token::Type::NUM)
	{
		return new Int(token->value);
	}
	else if (token->type == Token::Type::BOOL)
	{
		return new Bool(token->value);
	}
	else if (token->type == Token::Type::STR)
	{
		return new Str(token->value);
	}
	else if (token->type == Token::Type::FLOAT)
	{
		return new Float(token->value);
	}
	else if (token->type == Token::Type::ID)
	{
		Function *func = new Function(typeJudge(token, nullptr),token->value);
		 
		token = nextToken();
		if (token->value != "(")
		{
			delete func;
			index-=2;
			token = nextToken();
			Id *id = new Id(typeJudge(token, nullptr), token->value);
			
			id->add(classlinknParse());
			
			return id;
		}
		token = nextToken();
		if (token->value == ")")
		{
			token = nextToken();
			if (token->type == Token::Type::SEMI)
				return func;
			else if (token->type == Token::Type::DOT)
			{
				index--;
				func->add(classlinknParse());
				return func;
			}
				wait_for_debug();
		}
		else
		{   
			index--;
			argadd:
			func->AddJudgeTostack(judgeParse());

			token = nextToken();
			if (token->value==",")
			 goto argadd;
			else if (token->value == ")")
			{
				token = nextToken();
				if (token->type == Token::Type::SEMI)
					return func; 
				else if (token->type == Token::Type::DOT)
				{
					index--;
					func->add(classlinknParse());
					return func;
				}
				else
					wait_for_debug();
			}
			else
				wait_for_debug();
		}
	}
	else if (token->type == Token::Type::DOT)
	{


	}
	else if (token->value == "(")
	{
		Judge *judge = judgeParse();
		token = nextToken();
		if (token->value != ")")
		{
			ErrorLog += "expected function right bracket :" + token->toString() + "! \n";
			wait_for_debug();
			return nullptr;
		}
		return judge;
	}
	else
	{
		ErrorLog += "parsing error :" + token->toString() + "! \n";
		wait_for_debug();
		return nullptr;
	}
}

Judge* Parser::functionParse()
{
	return nullptr;
}
Id* Parser::classlinknParse()
{
	Token *token = nextToken();
	Id *id=nullptr;
	if (token->type == Token::Type::DOT)
	{
		if (id == nullptr)
			id = (Id*)factorParse();
		else
			id->add((Id*)factorParse());
	}
	else
		index--;
	return id;
}



void ClassObject::ExtendAdd(string basename)
{
	extendlist.push_back(basename);
}
 
 
 


/*

class ->id extends classblock
extends->extend class


prog -> func funcs
      | 
func -> type id ( arglist ) block

block -> { stmts }

stmts -> stmt stmts
 �� �� | 

stmt -> type id;
      | type id = judge;
      | id = judge;
      | if(judge) block
      | if(judge) block else block
      | while(judge) block
      | print judge;
	  | judge
	  | id.fun.fun
	  | id.fun.id
	  | id.id
	  

type -> int
      | bool
      | string


judges-> judge 
       | judge , judges


judge -> judge \| join
       | join
	   


join  -> join & equality
       | equality

equality -> equality == rel
          | equality != rel
          | rel

rel -> expr < expr 
     | expr <= expr
     | expr >= expr
     | expr > expr
     | expr

expr -> expr + term
      | expr - term
      | term

term -> term * unary
      | term / unary
      | unary

unary -> !unary
       | -unary
       | factor

factor -> num
        | boolean
        | str
        | id
        | (judge)
		| fun(judges)

boolean -> True | False*/
 