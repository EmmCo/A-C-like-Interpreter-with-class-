
#include <stdio.h>
#include "stdafx.h"
#include <string>
#include <vector>
#include <stack>
#include <map>
#include <algorithm>
#include <assert.h>
#include "ast.h"
#include "Lexer.h"
#include "Token.h"
#include "TypeCheck.h"
#include "StackMachine.h"
#include "MemoryManager.h"
#include "MarkSweep.h"

using namespace std;
char*
re2post(char *re)
{
	int nalt, natom;
	static char buf[8000];
	char *dst;
	struct {
		int nalt;
		int natom;
	} paren[100], *p;

	p = paren;
	dst = buf;
	nalt = 0;
	natom = 0;
	if (strlen(re) >= sizeof buf / 2)
		return NULL;
	for (; *re; re++){
		switch (*re){
		case '(':
			if (natom > 1){
				--natom;
				*dst++ = '.';
			}
			if (p >= paren + 100)
				return NULL;
			p->nalt = nalt;
			p->natom = natom;
			p++;
			nalt = 0;
			natom = 0;
			break;
		case '|':
			if (natom == 0)
				return NULL;
			while (--natom > 0)
				*dst++ = '.';
			nalt++;
			break;
		case ')':
			if (p == paren)
				return NULL;
			if (natom == 0)
				return NULL;
			while (--natom > 0)
				*dst++ = '.';
			for (; nalt > 0; nalt--)
				*dst++ = '|';
			--p;
			nalt = p->nalt;
			natom = p->natom;
			natom++;
			break;
		case '*':
		case '+':
		case '?':
			if (natom == 0)
				return NULL;
			*dst++ = *re;
			break;
		default:
			if (natom > 1){
				--natom;
				*dst++ = '.';
			}
			*dst++ = *re;
			natom++;
			break;
		}
	}
	if (p != paren)
		return NULL;
	while (--natom > 0)
		*dst++ = '.';
	for (; nalt > 0; nalt--)
		*dst++ = '|';
	*dst = 0;
	return buf;
}

/*
* Represents an NFA state plus zero or one or two arrows exiting.
* if c == Match, no arrows out; matching state.
* If c == Split, unlabeled arrows to out and out1 (if != NULL).
* If c < 256, labeled arrow with character c to out.
*/
enum
{
	Match = 256,
	Split = 257
};
typedef struct State State;
struct State
{
	int c;
	State *out;
	State *out1;
	int lastlist;
};
State matchstate = { Match };	/* matching state */
int nstate;

/* Allocate and initialize State */
State*
state(int c, State *out, State *out1)
{
	State *s;

	nstate++;
	s = (State *)malloc(sizeof *s);
	s->lastlist = 0;
	s->c = c;
	s->out = out;
	s->out1 = out1;
	return s;
}

/*
* A partially built NFA without the matching state filled in.
* Frag.start points at the start state.
* Frag.out is a list of places that need to be set to the
* next state for this fragment.
*/
typedef struct Frag Frag;
typedef union Ptrlist Ptrlist;
struct Frag
{
	State *start;
	Ptrlist *out;
};

/* Initialize Frag struct. */
Frag
frag(State *start, Ptrlist *out)
{
	Frag n = { start, out };
	return n;
}

/*
* Since the out pointers in the list are always
* uninitialized, we use the pointers themselves
* as storage for the Ptrlists.
*/
union Ptrlist
{
	Ptrlist *next;
	State *s;
};

/* Create singleton list containing just outp. */
Ptrlist*
list1(State **outp)
{
	Ptrlist *l;

	l = (Ptrlist*)outp;
	l->next = NULL;
	return l;
}

/* Patch the list of states at out to point to start. */
void
patch(Ptrlist *l, State *s)
{
	Ptrlist *next;

	for (; l; l = next){
		next = l->next;
		l->s = s;
	}
}

/* Join the two lists l1 and l2, returning the combination. */
Ptrlist*
append(Ptrlist *l1, Ptrlist *l2)
{
	Ptrlist *oldl1;

	oldl1 = l1;
	while (l1->next)
		l1 = l1->next;
	l1->next = l2;
	return oldl1;
}


typedef vector<State*> Statelist;

void Closure(Statelist &_Slist, State *start)//不动点算法获取start状态的闭包
{
	if (start == NULL)
		return;

	_Slist.push_back(start);

	if (start->out != NULL)
	{
		if (start->out->c == Split)
			Closure(_Slist, start->out);
	}
	if (start->out1 != NULL)
	{
		if (start->out1->c == Split)
			Closure(_Slist, start->out1);
	}	 

	return;
}


 
State*
post2nfa(char *postfix)
{
	char *p;
	Frag stack[1000], *stackp, e1, e2, e;
	State *s;

	// fprintf(stderr, "postfix: %s\n", postfix);

	if (postfix == NULL)
		return NULL;

#define push(s) *stackp++ = s
#define pop() *--stackp

	stackp = stack;
	for (p = postfix; *p; p++){
		switch (*p){
		default:
			s = state(*p, NULL, NULL);
			push(frag(s, list1(&s->out)));
			break;
		case '.':	/* catenate */
			e2 = pop();
			e1 = pop();
			patch(e1.out, e2.start);
			/*
			void
			patch(Ptrlist *l, State *s)
            {
	            Ptrlist *next;
	           for (; l; l = next){
		         next = l->next;
		         l->s = s;
	           }
            }*/
			push(frag(e1.start, e2.out));
			break;
		case '|':	/* alternate */
			e2 = pop();
			e1 = pop();
			s = state(Split, e1.start, e2.start);
			push(frag(s, append(e1.out, e2.out)));
			break;
		case '?':	/* zero or one */
			e = pop();
			s = state(Split, e.start, NULL);
			push(frag(s, append(e.out, list1(&s->out1))));
			break;
		case '*':	/* zero or more */
			e = pop();
			s = state(Split, e.start, NULL);
			patch(e.out, s);
			push(frag(s, list1(&s->out1)));
			break;
		case '+':	/* one or more */
			e = pop();
			s = state(Split, e.start, NULL);
			patch(e.out, s);
			push(frag(e.start, list1(&s->out1)));
			break;
		}
	}

	e = pop();
	if (stackp != stack)
		return NULL;

	patch(e.out, &matchstate);
	vector<State*> _Slist;
	Closure(_Slist, e.start);


	return e.start;
#undef pop
#undef push
}

typedef struct List List;
struct List
{
	State **s;
	int n;
};
List l1, l2;
static int listid;

void addstate(List*, State*);
void step(List*, int, List*);

/* Compute initial state list */
List*
startlist(State *start, List *l)
{
	l->n = 0;
	listid++;
	addstate(l, start);
	return l;
}

/* Check whether state list contains a match. */
int
ismatch(List *l)
{
	int i;

	for (i = 0; i<l->n; i++)
	if (l->s[i] == &matchstate)
		return 1;
	return 0;
}

/* Add s to l, following unlabeled arrows. */
void
addstate(List *l, State *s)
{
	if (s == NULL || s->lastlist == listid)
		return;
	s->lastlist = listid;
	if (s->c == Split){
		/* follow unlabeled arrows */
		addstate(l, s->out);
		addstate(l, s->out1);
		return;
	}
	l->s[l->n++] = s;
}

/*
* Step the NFA from the states in clist
* past the character c,
* to create next NFA state set nlist.
*/
void
step(List *clist, int c, List *nlist)
{
	int i;
	State *s;

	listid++;
	nlist->n = 0;
	for (i = 0; i<clist->n; i++){
		s = clist->s[i];
		if (s->c == c)
			addstate(nlist, s->out);
	}
}
/* Run NFA to determine whether it matches s. */
int
match(State *start, char *s)
{
	int i, c;
	List *clist, *nlist, *t;

	clist = startlist(start, &l1);
	nlist = &l2;
	for (; *s; s++){
		c = *s & 0xFF;
		step(clist, c, nlist);
		t = clist; clist = nlist; nlist = t;	/* swap clist, nlist */
	}
	return ismatch(clist);
}

void Nfa2Dfa();
void GetNextStalist(Statelist & _currstalist, Statelist &_nextstalist, char i);

#include<windows.h>  
 

struct mmm
{
	char a, b;
	short c;
	int d;

};
struct ZeroMemory {
	int *a[0];
};

void * tempaddress;

int kkk()
{
	return 2;
}
long long  kkk(int k)
{
	return 2;
}


 

 


int
main(int argc, char **argv)
{
	unsigned int kjkj = 25u-50;
	 
	//char kjkj[3] = {'2'};
	//int jkj = sizeof(kjkj);

	allocator<int>all;
	double time = 0;
	double counts = 0;
	LARGE_INTEGER nFreq;
	LARGE_INTEGER nBeginTime;
	LARGE_INTEGER nEndTime;
	QueryPerformanceFrequency(&nFreq);
	QueryPerformanceCounter(&nBeginTime);//开始计时 
	Garbage_Init(&argc);

	MemoryPool  GlobalMemoryPool(4096,256,8);       // 初始化内存池
	MemoryTool::set_mpinstance(&GlobalMemoryPool);  // 设置内存池

	MemoryTool::NewMap GlobalNewList;
	MemoryTool::set_newlinstance(&GlobalNewList);
	
	Lexer *_lex = new(__FILE__, __LINE__)Lexer();
	int size1 = sizeof(_lex->tokens);
	_lex->lex();
	int size2 = sizeof(_lex->tokens);
	tempaddress= &(_lex->tokens.begin());

    Parser *parser = new(__FILE__, __LINE__)Parser("text",_lex->tokens);
	Fun *fun = parser->parse();


	TypeCheck *typecheck = new(__FILE__, __LINE__)TypeCheck(fun);
	typecheck->setParser(parser);
	typecheck->funCheck();
		 
	StackMachine stm;
	stm.alu();

	Garbage_Collect();

	map<string, string>::iterator memoryit = stm.memory.begin();
	cout << endl << endl;
	cout << "\tThere are " + stm.itos(stm.memory.size()) + " variables in total program." << endl << endl;
	cout << "\tName:\t\tType:\t\tValue:" << endl << endl;
   
	for (; memoryit != stm.memory.end();  memoryit++)
	{
		string Typename = memoryit->first;
		string Typevalue = memoryit->second;
		Type type = typecheck->stackTable.top()[Typename];
		
		string stype="Unknown";
		if (type == Type::_Bool)
			stype = "bool";
		if (type == Type::_Int)
			stype = "int";
		if (type == Type::_Str)
			stype = "string";
		cout << "\t"+Typename + "\t\t" + stype + "\t\t" + Typevalue << endl;
	}


	QueryPerformanceCounter(&nEndTime);//停止计时  
	time = (double)(nEndTime.QuadPart - nBeginTime.QuadPart) / (double)nFreq.QuadPart;//计算程序执行时间单位为s  
	cout << "运行时间：" << time * 1000 << "ms" << endl;

	/* 

	for (MemoryTool::NewMap::iterator mapit = GlobalNewList.begin(); mapit != GlobalNewList.end(); mapit++)
	{
	for (MemoryTool::ObjectList::iterator listit = mapit->second.begin(); listit != mapit->second.end(); listit++)
	{
	int    _line     = listit->line;
	string _filename = listit->filename;
	void  *_address  = listit->address;
	size_t  _size    = listit->size;
	string _typename = typeid(*(listit->address)).name();

	cout << _filename << '\t' << _line << '\t' << _typename << '\t' << _size << '\t' << _address<<endl;
	}
	}

	MemoryTest::get_mpinstance();

	

	char _t = '\2';
	int i;
	char *post;
	State *start;
 
	Statelist pc, pnext,_temp;
	string wh = "a";
	const char *cptr =  wh.c_str();

	post = re2post("a");
	start = post2nfa(post);
	Closure(pc,start);
	while (*cptr)
	{
		GetNextStalist(pc,pnext , *cptr);
	
		pc = pnext;
		pnext.clear();

		cptr++;
	}

	if (start == NULL){
		fprintf(stderr, "error in post2nfa %s\n", post);
		return 1;
	}
	l1.s = (State **)malloc(nstate*sizeof l1.s[0]);
	l2.s = (State **)malloc(nstate*sizeof l2.s[0]);
	for (i = 2; i<argc; i++)
	if (match(start, argv[i]))
		printf("%s\n", argv[i]);
	*/
	//system("pause");

	delete _lex;
	delete parser;
	//delete fun;//已经被析构
	delete typecheck;
	return 0;
	 
}


void GetNextStalist(Statelist & _currstalist, Statelist &_nextstalist, char c)//获取以i点为转移的状态闭包
{
	if (_currstalist.size() < 1)
		   return;

	Statelist::iterator Stateptr = _currstalist.begin();
    
	for (Statelist::size_type i = 0; i < _currstalist.size(); i++)
	{
		State* cur = Stateptr[i];
			
		if (cur->out != NULL)
		if (cur->out->c ==c )
     		Closure(_nextstalist, cur->out);

		if (cur->out1 != NULL)
		if (cur->out1->c == c)
			Closure(_nextstalist, cur->out1);
	}
	return;
}

map<Statelist, int> DState;		//DFA中的状态集合,int代表dstate编号
void Nfa2Dfa()
{
	State *_start = post2nfa("");//得到了NFA的第一个状态量

	Statelist _oristate;
	
	map<Statelist, int>::iterator mapiter;

	Closure(_oristate,_start);//获取起始位置的闭包

	int dstate_num = 0;

	stack< Statelist > stk;
	stk.push(_oristate);

	DState.insert(pair<vector<State*>, int>(_oristate, dstate_num++));//插入到map里，作为计数

	while (!stk.empty())
	{
		Statelist _curstalist = stk.top();
		stk.pop();

	    for (int i = 1; i < 256; i++)//255个字符集
		{
			Statelist _nextstalist;
			
			GetNextStalist(_curstalist,_nextstalist,char(i)); //从当前的闭包集合得到以字符i为转移的闭包集合

			if (_nextstalist.size() < 1)//没有找到诶 没有闭包
			{
				continue;
			}

			sort(_nextstalist.begin(), _nextstalist.end());//排序是为了让集合有序，好让Map具有唯一

			mapiter = DState.find(_nextstalist);

			if (mapiter == DState.end())//新集合没有出现过插入map里并放入栈中
			{
				DState.insert(pair<Statelist, int>(_nextstalist, dstate_num++));
				stk.push(_nextstalist);
			}
		}
	}//栈空，所有集合找寻完毕
	map<Statelist, int>::iterator iter;
	/*
	for (iter = DState.begin(); iter != DState.end(); ++iter)
	{
		for (size_t i = 0; i<rex.size(); ++i)
		{
			if (find(iter->first.begin(), iter->first.end(), match[i]) != iter->first.end())
			{
				DMatch.insert(pair<int, int>(iter->second, rex[i].second));
				break;
			}
		}
	}
	*/

}







