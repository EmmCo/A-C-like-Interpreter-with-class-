#ifndef _AST_
#define _AST_
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
#include "Token.h"
#include "MemoryManager.h"
#include "ClassObject.h"
using namespace std;


enum Type //定义
{
	_Int, _Bool, _Str, _Void, _Class, _Float, _Unknown, _Fun, _ClassFun, _Object
};


static string TypeString(Type _type)
{
	switch (_type)
	{
	case Type::_Int:
		return "int";

	}
	

}

typedef stack<map<string, Type>  > StackSymbolTable;

typedef std::map<string, string>      SymbolTable ;

 
class Judge :public MemoryTool
{
public:
	enum Kind{
		ADD, MINUS, TIMES, DIVISION, NEGATIVE, NOT, OR, AND, EQUAL, GE, LE, G, L, ID, VAL, FUN, CLASSFUN
	};
	Kind kind;
	Judge(Kind _kind) :kind(_kind)
	{

	}
	virtual string toString()
	{
		return " ";
	}

	virtual ~Judge()
	{
	}

	Type returntype;
};
//  +
class Add : public Judge
{
public:
	Judge *left ;
	Judge *right;

	Add(Judge *_left, Judge *_right) :left(_left), right(_right), Judge(Kind::ADD)
	{
	}
	string toString()
	{
		return '(' + left->toString() + ')' + "+" + '(' + right->toString() + ')';
	}

	virtual ~Add()
	{
		delete left;
		delete right;

	}

};
//  &&
class And : public Judge
{
public:
	Judge *left;
	Judge *right;

	And(Judge *_left, Judge *_right) :left(_left), right(_right), Judge(Kind::AND)
	{
	}
	string toString()
	{
		return '(' + left->toString() + ')' + "&&" + '(' + right->toString() + ')';
	}
	virtual ~And()
	{
		delete left;
		delete right;

	}

};

//  /
class Division : public Judge
{
public:
	Judge *left;
	Judge *right;

	Division(Judge *_left, Judge *_right) :left(_left), right(_right), Judge(Kind::DIVISION)
	{
	}
	string toString()
	{
		return '(' + left->toString() + ')' + "/" + '(' + right->toString() + ')';
	}
	virtual ~Division()
	{
		delete left;
		delete right;

	}

};
// == 
class Equal : public Judge
{
public:
	Judge *left;
	Judge *right;

	Equal(Judge *_left, Judge *_right) :left(_left), right(_right), Judge(Kind::EQUAL)
	{
	}
	string toString()
	{
		return '(' + left->toString() + ')' + "==" + '(' + right->toString() + ')';
	}
	virtual ~Equal()
	{
		delete left;
		delete right;
	}
};


// >
class G : public Judge
{
public:
	Judge *left;
	Judge *right;

	G(Judge *_left, Judge *_right) :left(_left), right(_right), Judge(Kind::G)
	{
	}
	string toString()
	{
		return '(' + left->toString() + ')' + ">" + '(' + right->toString() + ')';
	}
	virtual ~G()
	{
		delete left;
		delete right;

	}
};
// >=
class Ge : public Judge
{
public:
	Judge *left;
	Judge *right;

	Ge(Judge *_left, Judge *_right) :left(_left), right(_right), Judge(Kind::GE)
	{
	}
	string toString()
	{
		return '(' + left->toString() + ')' + ">=" + '(' + right->toString() + ')';
	}
	virtual ~Ge()
	{
		delete left;
		delete right;

	}
};

// id
class Id : public Judge
{
public:
	Id(string _type, string _name) :type_name(_type), name(_name), Judge(Kind::ID), next(nullptr)
	{ 
	}
	Id(string _type, string _name, Kind _kind) :type_name(_type), name(_name), Judge(Kind::FUN), next(nullptr)
	{
		 
	}
	Type getType()
	{
		return type;
	}
	string getName()
	{
		return name;
	}
	void setName(string _name)
	{
		name = _name;
	}
	string getTypeName()
	{
		return type_name;
	}


	string toString()
	{
		return  type_name + " " + name;
	}
	void add(Id * _link)
	{
		Id *temp = this;
		while (temp->next)
		{
			temp = temp->next;
		}
		temp->next = _link;
	}
	Id * getNext()
	{
		return next;
	}
protected :
	string type_name;
	Type  type;
	string name;
	Id *next;
};


class Function : public Id
{
public:
	Function(string _type, string _name) :Id(_type, _name, Kind::FUN), argjudge(nullptr)
	{
		 
	}

	string toString()
	{
		return  type_name + " " + name;
	}
	std::stack<Judge *> GetArgstack()
	{
		return argstack;
	}
	void AddJudgeTostack(Judge * _judge)
	{
		argstack.push(_judge);
	}
	string getidName()
	{
		return idname;
	}
private:
 
	 
	string idname;
	Judge *argjudge;
	std::stack<Judge *> argstack;
};


 
 

// <
class L : public Judge
{
public:
	Judge *left;
	Judge *right;

	L(Judge *_left, Judge *_right) :left(_left), right(_right), Judge(Kind::L)
	{
	}
	string toString()
	{
		return '(' + left->toString() + ')' + "<" + '(' + right->toString() + ')';
	}
	virtual ~L()
	{
		delete left;
		delete right;

	}
};
// <=
class Le : public Judge
{
public:
	Judge *left;
	Judge *right;

	Le(Judge *_left, Judge *_right) :left(_left), right(_right), Judge(Kind::LE)
	{
	}
	string toString()
	{
		return '(' + left->toString() + ')' + "<=" + '(' + right->toString() + ')';
	}

	virtual ~Le()
	{
		delete left;
		delete right;

	}

};

// -
class Minus : public Judge
{
public:
	Judge *left;
	Judge *right;

	Minus(Judge *_left, Judge *_right) :left(_left), right(_right), Judge(Kind::MINUS)
	{
	}
	string toString()
	{
		return '(' + left->toString() + ')' + "-" + '(' + right->toString() + ')';
	}

	virtual ~Minus()
	{
		delete left;
		delete right;

	}

};
 

// - 负号
class Negative : public Judge
{
public:
	Judge *follow;
 
	Negative(Judge *_follow) :follow(_follow) , Judge(Kind::NEGATIVE)
	{
	}
	string toString()
	{
		return "-(" + follow->toString() + ')';
	}

	virtual ~Negative()
	{
		delete follow;
 

	}

};

// ! 取反
class Not : public Judge
{
public:
	Judge *follow;


	Not(Judge *_follow) :follow(_follow), Judge(Kind::NOT)
	{
	}
	string toString()
	{
		return "!(" + follow->toString() + ')';
	}

	virtual ~Not()
	{
		delete follow;


	}

};

// || 
class Or : public Judge
{
public:
	Judge *left;
	Judge *right;

	Or(Judge *_left, Judge *_right) :left(_left), right(_right), Judge(Kind::OR)
	{
	}
	string toString()
	{
		return '(' + left->toString() + ')' + "||" + '(' + right->toString() + ')';
	}
	virtual ~Or()
	{
		delete left;
		delete right;

	}

};

// * 
class Times : public Judge
{
public:
	Judge *left;
	Judge *right;

	Times(Judge *_left, Judge *_right) :left(_left), right(_right), Judge(Kind::TIMES)
	{
	}
	string toString()
	{
		return '(' + left->toString() + ')' + "*" + '(' + right->toString() + ')';
	}
	virtual ~Times()
	{
		delete left;
		delete right;
	}

};



class Value : public Judge
{
public:
	Value(string _type) :Judge(Kind::VAL), type(_type)
	{}
	string type;
	 
};


class Str : public Value
{
public:
	Str(string _value) :value(_value), Value("string")
	{
	}
	string value;
	string toString()
	{
		return value;
	}
};

class Int : public Value
{
public:
	Int(int _value) :value(_value), Value("int")
	{

	}
	Int(string _value) : value(atoi(_value.c_str())), Value("int")
	{

	}

	int value;
	string toString()
	{

		stringstream stream;
		stream << value;
		return stream.str();	 
	}
};


class Float : public Value
{
public:
	Float(int _value) :value(_value), Value("float")
	{

	}
	Float(string _value) : value(atoi(_value.c_str())), Value("float")
	{

	}

	float value;
	string toString()
	{

		stringstream stream;
		stream << value;
		return stream.str();
	}
};


class Bool : public Value
{
public:
	Bool(string _value) : Value("bool")
	{
		if (_value == "true" || _value == "True")
		{
			value = true;
		}
		else
			value = false;
	}
	bool value;
	string toString()
	{
		if (value)
			return "true";
		return "false";
	}
};



#define wait_for_debug() do{}while(1) 

class Stmt :public MemoryTool
{
public:
	enum Kind{
		DefStmt, AssignStmt, IfStmt, WhileStmt, PrintStmt, FunStmt, ClassFunStmt, JudgeStmt
	};
	Kind kind;
	Stmt *next;

	Stmt()
	{
		next = nullptr;
	}
	void add(Stmt *newstmt)
	{
		Stmt *temp = this;
		while (temp->next)
		{
			temp = temp->next;
		}
		temp->next = newstmt;
	}

	virtual ~Stmt()
	{
		delete next;
	} 
};

class Objectlink :public MemoryTool
{
/*	void add(Judge *newstmt)
	{
		Judge *temp = this;
		while (temp->next)
		{
			temp = temp->next;
		}
		temp->next = newstmt;
	}
	*/
public:
	void add(Judge *_newjudge){}
};


class Assign : public Stmt
{
public:
	Assign(Id *_id, Judge *_judge) :id(_id), judge(_judge)
	{
		kind = Kind::AssignStmt;
	}

	Judge* getJudge()
	{
		return judge;
	}
	Id* getID()
	{
		return id;
	}

	virtual ~Assign()
	{
		delete id;
		delete judge;
	}


private:
	Id *id;
	Judge *judge;
};

class Def : public Stmt
{
public:
	Def(Id *_id, Judge *_judge) :id(_id), judge(_judge)
	{
		kind = Kind::DefStmt;
	}
	Judge* getJudge()
	{
		return judge;
	}
	Id* getID()
	{
		return id;
	}
	virtual ~Def()
	{
		delete id;
		delete judge;
	}
private:
	Id *id;
	Judge *judge;
};


class If : public Stmt
{
public:
	If(Judge * _condition, Stmt * _then, Stmt *_otherwise) :condition(_condition), then(_then), otherwise(_otherwise)
	{
		kind = Kind::IfStmt;
	}

	Stmt* getOtherwise()
	{
		return otherwise;
	}
	Judge* getCondition()
	{
		return condition;
	}
	Stmt* getThen()
	{
		return then;
	}
	virtual ~If()
	{
		delete otherwise;
		delete then;
		delete condition;
	}

private:
	Stmt *otherwise;
	Stmt *then;
	Judge *condition;
};

class Print : public Stmt
{
public:
	enum InnerType{
		Pid, Pvalue
	};

	Print(InnerType _innertype, string _name) :innertype(_innertype), name(_name)
	{
		kind = Kind::PrintStmt;
	}

	InnerType getInnerType()
	{
		return innertype;
	}
	string  getName()
	{
		return name;
	}

private:
	InnerType innertype;
	string name;
};


class While : public Stmt
{
public:

	While(Judge *_condition, Stmt* _then) :condition(_condition), then(_then)
	{
		kind = Kind::WhileStmt;
	}

	Judge* getCondition()
	{
		return condition;
	}
	Stmt*  getThen()
	{
		return then;
	}
	virtual ~While()
	{
		delete then;
		delete condition;
	}
private:
	Stmt*  then;
	Judge* condition;
};

class Fun :public MemoryTool
{
public:
	Fun* next;
	string type;
	string id;
	Stmt* stmt, *arglist;
	Fun(string _type, string _id, Stmt* _arglist, Stmt* _stmt) :type(_type), id(_id), arglist(_arglist), stmt(_stmt), next(nullptr)
	{
		/*
		Stmt* _arglast = arglist;
		if (_arglast == nullptr)
			return;
		Stmt* sthead = stmt;
		
		stmt = _arglast;
		
		while (_arglast->next != nullptr)
		{   
			_arglast = _arglast->next;
		}

		_arglast->next = sthead;*/
	}
	string GetidName() const
	{
		return id;
	}
	string GetFunName() const
	{
		Def*  _arg = (Def*)arglist;
		string _argname;
		while (_arg != nullptr)
		{
		   _argname+=(_arg->getID()->getTypeName());
		   _arg = (Def*)_arg->next;
		}

		return id + _argname;
		
	}
	bool add(Fun* newfun)
	{
		if (newfun == nullptr)
			return false;
		Fun* temp = this;
		while (temp->next)
		{
			temp = temp->next;
		}
		temp->next = newfun;
		return true;
	}
	virtual ~Fun()
	{
		delete next;
		delete stmt;
	}
};


class ClassObject;
class BaseObject :public MemoryTool
{
public :
	typedef std::list<ClassObject *> Classestype;
	BaseObject(string _name) :fun(nullptr), Member(nullptr), name(_name)
	{
		 
	}
	virtual ~BaseObject()
	{
	
	}

	void SetFun(Fun *_fun)
	{
		fun = _fun;
		Fun *cur=fun;
		Def *arglist = (Def *)cur->arglist;
		while (cur != nullptr)
		{
			string argname="";
			while (arglist!=nullptr)
			{
				argname += arglist->getID()->getTypeName();
				arglist = (Def *)arglist->next;
			}
			AddSymbol(cur->id + argname, "Fun");
			cur = cur->next;
		}
	}



	void AddSymbol(string _idname,string type)
	{   
		if (symbollist.find(_idname) != symbollist.end())
		{
		}
		else
		  symbollist[_idname] = type;
	}



	bool AddClass(ClassObject *_classobject)
	{
		if (_classobject == nullptr)
			return false;
		childs.push_back(_classobject);

		//AddSymbol(_classobject->GetClassName(), Type::_Class);

		return true;
	}


	void AddMember(Stmt * _member)
	{
		if (Member==nullptr)
		{
		   Member = _member;
		}
		else 
		   Member->add(_member);
	  
		Def * defmem = dynamic_cast<Def *>(_member);
		
		if (defmem != nullptr)AddSymbol(defmem->getID()->getName(), defmem->getID()->getTypeName());
	}
	string GetClassName()
	{
		return name;
	}

	Classestype& GetChildClasses()
	{
		return childs;
	}
	Fun* GetFun()
	{
		return fun;
	}
	bool isSymbol(string _name,Type type)
	{
		return symbollist.find(_name) != symbollist.end();// && symbollist.find(_name)->second == type;
		//return symbollist[_name] == type;
	}
	string SymbolType(string _name)
	{
		 if(symbollist.find(_name)->second=="class")return _name;
		 return symbollist.find(_name)->second;
	}


	string GetFunType(string _funname)const
	{
		const Fun * _fun = fun;
		while (_fun != nullptr)
		{
			if (_fun->GetFunName() == _funname)
				return _fun->type;

			_fun = _fun->next;
		}
		return "unknow";
	}
	Classestype childs;
	SymbolTable symbollist;
	Fun  *fun;
	Stmt *Member;
	string name;
}; 

class ClassObject :public BaseObject
{
public:
	void ExtendAdd(string basename);
	void FunAdd(Fun * fun);
	void SymbolAdd(string idname, Type type);
	void ClassAdd(ClassObject*);
	ClassObject(string _name, string _pack) :BaseObject(_name), packname(_pack)
	{

	}
	virtual ~ClassObject()
	{

	}

private:
	std::list<string> extendlist; 
	string packname;

};

 


class Parser :public BaseObject
{
public:
	 
	Parser(string name, vector<Token*> _tokens);
	Fun* parse();
	string ErrorLog;

	virtual ~Parser()
	{

		for (int i = 0; i < tokens.size(); i++)
		{
			//get_mpinstance().ReleasePointer(tokens[i]);//返回给空间

			//tokens[i]->~Token();
		}


		delete fun;
	}

private:

	vector<Token*> tokens;
	int length;
	int index;
	 

	Token* nextToken()
	{
		if (index == length)
			return (nullptr);
		else
		{
			return tokens[index++];
		}
	}

	string typeJudge(Token* token, BaseObject * _object)
	{
		if (token->value == "int" || token->value == "bool" || token->value == "float" || token->value == "string" || token->value=="void")
 
		return token->value;
		 
			if (_object != nullptr&&_object->isSymbol(token->value, Type::_Class))
				return _object->SymbolType(token->value);
			if (isSymbol(token->value,Type::_Class))
				return SymbolType(token->value);
		 
		//wait_for_debug();
		return "unknown";//局部变量
	}
	string typeJudge2(string _Fatype ,Token* token, BaseObject * _object)
	{
		if (token->value == "int" || token->value == "bool" || token->value == "float" || token->value == "string" || token->value == "void")

			return token->value;

		if (_object != nullptr&&_object->isSymbol(token->value, Type::_Class))
			return token->value;
		if (isSymbol(token->value, Type::_Class))
			return token->value;

		wait_for_debug();
		return "unknown";
	}
	BaseObject * currentclass;
	ClassObject* classparse(BaseObject * _object);
	Fun*   funParse(BaseObject * _object);
	Stmt*  argsParse(BaseObject * _object);
	Stmt*  argParse(BaseObject * _object);
	Stmt*  blockParse(BaseObject * _object);
	Stmt*  stmtsParse(BaseObject * _object);
	Stmt*  stmtParse(BaseObject * _object);
	Judge* judgeParse(BaseObject * _object);
	Judge* joinParse(BaseObject * _object);
	Judge* equalityParse(BaseObject * _object);
	Judge* relParse(BaseObject * _object);
	Judge* exprParse(BaseObject * _object);
	Judge* termParse(BaseObject * _object);
	Judge* unaryParse(BaseObject * _object);
	Judge* factorParse(BaseObject * _object,string Fatype="");
	Id* classlinknParse(BaseObject * _object);
	Judge* functionParse(BaseObject * _object);
	Fun*   parse(BaseObject * _object);
	Type   FunType(string funname);

};

#endif