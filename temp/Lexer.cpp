
#include "stdafx.h"
#include "Lexer.h"
using namespace std;
/*
class Lexer{
    
    public :
	vector<Token*> tokens;
private:
	  static const int BufMaxLen = 256;
	  int	line ;
	  int 	pos  ;
	  int  cursor;
	  int  bufLen;
	  bool   _EOF;
	  string  str;
	  char *buffer;  //= new char[BufMaxLen]; C++11 允许就地赋值
};*/

//#define new new(__FILE__,__LINE__)


#define new new(__FILE__,__LINE__)

void Lexer::refresh()//读取新的数据
{

	static ifstream in("text.txt");
	
	if (!in.is_open())
	{
		while (1);//Something error
		return;
	}
	
	if (!in.eof())
	{
		in.read(buffer, BufMaxLen);
		bufLen = in.gcount();
	}
	else
	{
		_EOF = true;
		in.close();
	}
	cursor = 0;
}

char Lexer::nextChar()
{
	if (cursor == bufLen)
		refresh();
	if (!_EOF)
		return buffer[cursor++];
	return -1;

}

void Lexer::eatSpace()
{
	char ch;
	while(true)
	{
		ch = nextChar();
		if (ch == -1)
		{
			_EOF = true;
			return;
		}
		else if (ch == '\n' )
		{
			line++;
			pos = 1;
		}
		else if (ch == ' ')
			pos++;
		else if (ch == '/')
		{
			ch = nextChar();
			if (ch == '/')        //检测到了注释
			{
				while (nextChar() != '\n');
				cursor--;
			}
			else if (ch == '*')   //检测到了注释
			{
				char _t = '0';
				while (_t != -1)
				{
					while (_t != '*')
						_t = nextChar();
					if (nextChar() == '/')
						return;
					cursor--;
					_t = nextChar();
				}
				 
			}
			else
			{
				cursor -= 2;
				return;
			}
		}
		else if (ch == '\t')
			pos+=8;
		
		else
		{
			cursor--;
			return;
		}
	}
}

bool Lexer::isIdentifier(char ch)
{
	return (ch >= 'a'&&ch <= 'z') || (ch >= 'A'&&ch <= 'Z') || isNumber(ch) || ch == '_';
}
bool Lexer::isNumber(char ch)
{
	return ch >= '0'&&ch <= '9';
}
bool Lexer::isLogicalOperator(char ch)
{
	return ch == '=' || ch == '>' || ch == '<' || ch == '!';
}
bool Lexer::isArithmeticOperator(char ch)
{
	return ch == '+' || ch == '-' || ch == '*' || ch == '/';
}


void Lexer::strGenerate()
{
	bool escape = false;
	nextChar();
	char ch;
	while (true)
	{
		ch = nextChar();
		pos++;
		if (ch == -1 || ch == '\n' )
		{
			while (1);//something error
			return;
		}
		else if (ch == '"'&&!escape)
		{
			tokens.push_back(new Token(Token::Type::STR, str, line, pos - 1));
			str = "";
			return;
		}
		else if (ch == '//')//注释
		{
			pos--;
			escape = true;
		}
		else
		{
			escape = false;
			str += ch;
		}
	}
}

void Lexer::numGenerate()
{
 
	char ch;
	bool _f = false;
	while (true)
	{
		ch = nextChar();
		if (ch == -1 )
		{
			while (1);//something error
			return;
		}
		else if (isNumber(ch) )
		{
			pos++;
			str += ch;
		}
		else if (ch == '.')
		{
			pos++;
			str += ch;
			_f = true;
		}
		else
		{
			cursor--;

			if (_f)
				tokens.push_back(new Token(Token::Type::FLOAT, str, line, pos));
			else
			    tokens.push_back(new Token(Token::Type::NUM, str, line, pos ));

			str ="";
			return;
		}
	}
}


void Lexer::idGenerate()
{

	char ch;
	while (true)
	{
		ch = nextChar();
		if (ch == -1)
		{
			while (1);//something error
			return;
		}
		else if (isIdentifier(ch))
		{
			pos++;
			str += ch;
		}
		else
		{
			cursor--;
			tokens.push_back(new Token(Token::Type::ID, str, line, pos));
			str = "";
			return;
		}
	}
}

void Lexer::logGenerate()
{
	char temp1, temp2;
	temp1 = nextChar();
	temp2 = nextChar();

	if (temp1 == '=')
	{
		if (temp2 == '=')
		{
			pos += 2;
			tokens.push_back(new Token(Token::Type::LOP, "==", line, pos));
		}
		else
		{
			cursor--; pos++;
			tokens.push_back(new Token(Token::Type::ASSIGN, "=", line, pos));
		}

	}
	else if (temp1 == '>')
	{
		if (temp2 == '=')
		{
			pos += 2;
			tokens.push_back(new Token(Token::Type::ROP, ">=", line, pos));
		}
		else
		{
			cursor--; pos++;
			tokens.push_back(new Token(Token::Type::ROP, ">", line, pos));
		}

	}
	else if (temp1 == '<')
	{
		if (temp2 == '=')
		{
			pos += 2;
			tokens.push_back(new Token(Token::Type::ROP, "<=", line, pos));
		}
		else
		{
			cursor--; pos++;
			tokens.push_back(new Token(Token::Type::ROP, "<", line, pos));
		}

	}
	else if (temp1 == '!')
	{
		if (temp2 == '=')
		{
			pos += 2;
			tokens.push_back(new Token(Token::Type::LOP, "!=", line, pos));
		}
		else
		{
			cursor--; pos++;
			tokens.push_back(new Token(Token::Type::LOP, "!", line, pos));
		}

	}

	else if (temp1 == '|')
	{

		if (temp2 == '|')
		{
			pos+=2;
			tokens.push_back(new Token(Token::Type::LOP, "||", line, pos));
		}
		else
		{
			cursor--; pos++;
			tokens.push_back(new Token(Token::Type::LOP, "|", line, pos));

		}

	}

	else if (temp1 == '&')
	{

		if (temp2 == '&')
		{
			pos+=2;
			tokens.push_back(new Token(Token::Type::LOP, "&&", line, pos));
		}
		else
		{
			cursor--; pos++;
			tokens.push_back(new Token(Token::Type::LOP, "&", line, pos));
		}

	}




}
void Lexer::ariGenerate()
{
	char ch=nextChar();
	pos++;
	switch (ch)
	{
	   case '+':
		   ch = nextChar();

		   if (ch == '=')
		   tokens.push_back(new Token(Token::Type::AOP, "+=", line, pos));

		   else
		   {
			   cursor--;
			   tokens.push_back(new Token(Token::Type::AOP, "+", line, pos));
		   }
		return;
	   case '-':
		   ch = nextChar();

		   if (ch == '=')
		   tokens.push_back(new Token(Token::Type::AOP, "-=", line, pos));

		   else
		   {
		    cursor--;
		    tokens.push_back(new Token(Token::Type::AOP, "-", line, pos));
		   }
		   return;
	   case '*':
		   ch = nextChar();
		   if (ch == '=')
			   tokens.push_back(new Token(Token::Type::AOP, "*=", line, pos));
		   else
		   {
			   cursor--;
			   tokens.push_back(new Token(Token::Type::AOP, "*", line, pos));
		   }
		   return;
	   case '/':
		   ch = nextChar();
		   if (ch=='=')
			   tokens.push_back(new Token(Token::Type::AOP, "/=", line, pos));
		   else
		   {
			   cursor--;
			   tokens.push_back(new Token(Token::Type::AOP, "/", line, pos));
		   }
		   return;
	}

}

void Lexer::lex()
{
	char ch;
	while (true)
	{
		eatSpace();
		ch = nextChar();
		if (ch == -1)
			return; 
		switch (ch)
		{
		case ';':
			pos++;
			tokens.push_back(new Token(Token::Type::SEMI, ";", line, pos));
		break;
		
		case '{':
			pos++;
			tokens.push_back(new Token(Token::Type::BRA, "{", line, pos));
		break;
		case '}':
			pos++;
			tokens.push_back(new Token(Token::Type::BRA, "}", line, pos));
		break;

		case '[':
			pos++;
			tokens.push_back(new Token(Token::Type::BRA, "[", line, pos));
			break;

		case ']':
			pos++;
			tokens.push_back(new Token(Token::Type::BRA, "]", line, pos));
			break;

		case '(':
			pos++;
			tokens.push_back(new Token(Token::Type::BRA, "(", line, pos));
		break;
		case ')':
			pos++;
			tokens.push_back(new Token(Token::Type::BRA, ")", line, pos));
		break;

		break;

		case ',':
			pos++;
			tokens.push_back(new Token(Token::Type::COM, ",", line, pos));
		break;

		case '.':
			pos++;
			tokens.push_back(new Token(Token::Type::DOT, ".", line, pos));
			break;

			//string
		case '"':
			cursor--;
			strGenerate();
		break;


		default :
			cursor--;
			if (isLogicalOperator(ch))
			{
				logGenerate();
			}
			else if (isArithmeticOperator(ch))
			{
				ariGenerate();
			}
			else if (isNumber(ch))
			{
				numGenerate();
			}
			else if (isIdentifier(ch))
			{
				idGenerate();
			}
			else
			{

				//while (true);
				//return;
			}
	   	break;
		}
	}
}

void Lexer::print()
{
	vector<Token*>::iterator tokenit = tokens.begin();
	for (; tokenit < tokens.end(); tokenit++)
	{
		 cout<< (*tokenit)->toString();
	}

}