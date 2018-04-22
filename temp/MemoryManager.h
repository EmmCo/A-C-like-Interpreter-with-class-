#ifndef  __MMANAGER
#define  __MMANAGER


#include <stdio.h>
#include "stdafx.h"
#include <string>
#include <string.h>
#include <vector>
#include <list>
#include <stack>
#include <map>
#include <algorithm>
#include <bitset>
#include <assert.h>
#include <iosfwd>
#include <fstream>

using namespace std;
typedef int Size_t;
typedef unsigned char Data_t;




class MemoryBlock
{
public:
	bool Init(Size_t blockSize, Size_t blocks);
	void ResetBlock(Size_t blockSize, Size_t blocks);
	Data_t * _pData;

	void * GetBlock(Size_t blockSize);
	void ReleaseBlock(void * p, Size_t blockSize);

	bool IsCorrupt(Data_t numBlocks, Size_t blockSize, bool checkIndexes) const;
	bool IsBlockAvailable(void * p, Data_t numBlocks, Size_t blockSize) const;



	void Release();

	inline bool HasBlock(void * p, Size_t chunkLength) const
	{
		Data_t* pc = (Data_t*)p;
		return (_pData <= pc) && (pc < _pData + chunkLength);
	}

	inline bool HasAvailable(Data_t numBlocks) const //是否有满足要求的Block块
	{
		return (_blocksAvailable == numBlocks);
	}

	inline bool RunOut(void) const
	{
		return (0 == _blocksAvailable);
	}



private:
	Size_t _blocksAvailable;
	Size_t _firstAvailableBlock;

};


class MemoryPager
{

public:

	void init(Size_t BlockSize, Size_t PageSize);
	
	MemoryPager() : _blockSize(0), _numBlocks(0), _blocks(0), _allocBlock(NULL), _deallocBlock(NULL), _emptyBlock(NULL)
	{}
	~MemoryPager()
	{
		for (BlockIter i(_blocks.begin()); i != _blocks.end(); ++i)
		{
			i->Release();	
		}
		_blocks.~vector();
	}	
	Size_t  CountEmptyBlocks() const;

	const MemoryBlock*  HasBlock(void * p) const;
	inline MemoryBlock * HasBlock(void * p)
	{
		return const_cast< MemoryBlock * >(
			const_cast< const MemoryPager * >(this)->HasBlock(p));
	}

	bool  TrimEmptyBlock(void);
	bool  TrimBlockList(void);

	void * GetPage(void);
	bool  ReleasePage(void * p, MemoryBlock * hint);
	void  ReleasePage(void* p);


	//private:
	Size_t  _blockSize, MaxPerBlockCount=255, MinPerBlockCount=8;

	typedef vector<MemoryBlock> Blocks;
	typedef Blocks::iterator BlockIter;
	typedef Blocks::const_iterator BlockCIter;
	Blocks _blocks;


	Data_t _numBlocks;

	MemoryBlock * _allocBlock;
	MemoryBlock * _deallocBlock;
	MemoryBlock * _emptyBlock;

	bool MakeNewBlock(void);
	MemoryBlock * Find(void * p) const;
	bool  IsCorrupt(void) const;

};

 
class MemoryPool
{
public:
	 
	 MemoryPool(Size_t PageSize, Size_t _maxAllowSize, Size_t _alignSize) :_maxAllowSize(_maxAllowSize), _alignSize(_alignSize), _pages(nullptr)// 初始化一个内存池，每一页的大小是PageSize字节																													   // 页内每一个bolck最大不能超过_maxAllowSize字节
	{
		const Size_t allocCount = _Align(_maxAllowSize, _alignSize)//向上取整
			;
		_pages = new MemoryPager[allocCount];
		for (int i = 0; i < allocCount; i++)
		{
			_pages[i].init((i + 1)*_alignSize, PageSize);
		}
	}
	void * GetBytesByNum(Size_t numBytes);

	Size_t MemoryPool::FindPtrGetNumBytes(void * p);
 
	bool FindCorrupt(void)const;
	Size_t _Align(Size_t numBytes, Size_t alignment);

	void  ReleasePointer(void * p, Size_t numBytes);
	void  ReleasePointer(void * p);


	~MemoryPool()
	{
		delete[]_pages;
	}
	bool TrimExcessMemory();
private:
	MemoryPager * _pages;
	const Size_t  _maxAllowSize;
	const Size_t  _alignSize;
};

typedef const char* FlieName_t;

class MemoryTool;

class NewObject
{

public:
	NewObject(string _filename, int _line, size_t _size, MemoryTool *_address) :filename(_filename), line(_line), size(_size), address(_address), color(0)
	{
	   
	}
	NewObject() :filename("None"), line(0), size(0), address(0), color(0){}
	 
//private :
	int line;
	string filename;
	size_t size;
	MemoryTool *address;
	unsigned char color;
};

class MemoryTool
{
 public :

	 
	 typedef int Line_t;
	 typedef std::list<NewObject> ObjectList;
	 typedef std::map<void*, NewObject> NewMap;
	 typedef NewObject NewObject;


	 static inline void * operator new(size_t sz ){
		 
		//int line = line;
		//const char *filename = filename;
		void * p = get_mpinstance().GetBytesByNum(sz);
		return p;
	 }
	 
	 static inline void * operator new(size_t sz, FlieName_t _filename, Line_t _line){
		 
		 static std::ofstream newout("./memory.txt");

		 Line_t     line = _line;
		 FlieName_t filename = _filename;
		 string filestr(filename);
		 string _tmp;
		 int i = filestr.size() - 1;
		 
		 while (filestr[i] != '\\')
			 _tmp += filestr[i--];
			
		 std::reverse(_tmp.begin(), _tmp.end());
        //get_newlinstance()[_tmp] = line;
		 
	     void * p = get_mpinstance().GetBytesByNum(sz);

		 
		 NewObject _newobject(_tmp, line, sz, (MemoryTool*)p);
		 
		 get_newlinstance()[p]=(_newobject);

		// newout << _tmp << "    " << line << "   Get  " << sz << " Bytes!  " <<"Address at: 0x"<<p<< endl;
		 return p;	 
	 }

	static void operator delete(void * p)
	 {
		get_mpinstance().ReleasePointer(p);
	 }

	static void operator delete(void * p, FlieName_t _filename, Line_t _line)
	{
		get_mpinstance().ReleasePointer(p);
	}


//private:
	static MemoryPool *_mpool;

	static NewMap *_newmap;
	
	static MemoryPool & get_mpinstance()
	{
		return *_mpool;
	}
	static void set_mpinstance(MemoryPool  *mpool)
	{
		_mpool = mpool;
	}

	static void set_newlinstance(NewMap  *newmap)
	{
		_newmap = newmap;
	}
	static NewMap & get_newlinstance()
	{
		return *_newmap;
	}

	virtual void _Non()
	{
	
	}

};

class MemoryTest :public MemoryTool
{
public:int j, k, l;
	   MemoryTest()
	   { 
		   j = k = l = 1;
	   }
	   void * operator new(size_t ,void*pp)
	   {
		   return nullptr;
	   }

};



#endif