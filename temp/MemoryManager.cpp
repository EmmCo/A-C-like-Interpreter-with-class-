#include <stdio.h>
#include "stdafx.h"
#include <string>
#include <vector>
#include <stack>
#include <map>
#include <algorithm>
#include <bitset>
#include <assert.h>
#include "ast.h"
#include "Token.h"
#include "TypeCheck.h"
#include "StackMachine.h"
#include "MemoryManager.h"
using namespace std;

MemoryPool* MemoryTool::_mpool = nullptr;
MemoryTool::NewMap* MemoryTool::_newmap=nullptr;

bool MemoryBlock::Init(Size_t blockSize, Size_t blocks)
{
	const Size_t allocSize = blockSize * blocks;   //allocSize = PageSize;

	_pData = static_cast< Data_t * >(::operator new (allocSize));//这里是真正的分配内存空间
	if (_pData == nullptr)
		return false;
   
	ResetBlock(blockSize, blocks);
	return true;
}
void MemoryBlock::ResetBlock(Size_t blockSize, Size_t blocks)
{
	_firstAvailableBlock = 0;
	_blocksAvailable     = blocks;
	
	Data_t i = 0;
	for (Data_t * p = _pData; i != blocks; p += blockSize)//这个标记表明下一块可用的位置。
	{
		*p = ++i;
	}
}
void MemoryBlock::Release()
{
	::operator delete (_pData);
}
void *MemoryBlock::GetBlock(Size_t blockSize)
{
	if (RunOut())
		return nullptr;

	Data_t * pResult     = _pData + (_firstAvailableBlock*blockSize);//获取最合适分配出去的block
	_firstAvailableBlock = *pResult;
	--_blocksAvailable;

	return pResult;

}
void MemoryBlock::ReleaseBlock(void * p ,Size_t blockSize)
{
	Data_t * toRelease = (Data_t *)p;

	Size_t index = (toRelease - _pData) / blockSize;

	*toRelease = _firstAvailableBlock;

	_firstAvailableBlock = index; //总是将最合适分配的块设置为最临近还回来的块，有利于提高缓存命中率

	++_blocksAvailable;
}

bool MemoryBlock::IsCorrupt(Data_t numBlocks, Size_t blockSize,bool checkIndexes) const
{
	if (numBlocks < _blocksAvailable)
	{
		assert(false);
		return true;
	}
	if (RunOut())
		return false;

	Data_t index = _firstAvailableBlock;
	if (numBlocks <= index)
	{
		 
		assert(false);
		return true;
	}
	if (!checkIndexes)
	 
		return false;

	 
	std::bitset< UCHAR_MAX > foundBlocks;
	Data_t * nextBlock = NULL;


	for (Data_t cc = 0;;)
	{
		nextBlock = _pData + (index * blockSize);
		foundBlocks.set(index, true);
		++cc;
		if (cc >= _blocksAvailable)
			 
			break;
		index = *nextBlock;
		if (numBlocks <= index)
		{ 
			assert(false);
			return true;
		}
		if (foundBlocks.test(index))
		{
			 
			assert(false);
			return true;
		}
	}
	if (foundBlocks.count() != _blocksAvailable)
	{ 
		assert(false);
		return true;
	}

	return false;

}
bool MemoryBlock::IsBlockAvailable(void * p, Data_t numBlocks,Size_t blockSize) const //判断一个指针指向的小block区域是否可拿来使用
{
	if (RunOut())
		return false;

	Data_t* place = (Data_t*) p;

	Data_t blockIndex = (place - _pData) / blockSize ;

	Data_t index = _firstAvailableBlock;

	if (index == blockIndex) //如果指向的Block正好是当前最适合分配出去的，则就返回true
		return true;
	
	std::bitset< UCHAR_MAX > foundBlocks;
	
	Data_t* nextBlock = NULL;

	for (Data_t cc = 0;;)
	{
		nextBlock = _pData + (index * blockSize);
		foundBlocks.set(index, true);
		++cc;
		if (cc >= _blocksAvailable)//找遍了所有可用小block区域
			break;
		index = *nextBlock;
		if (index == blockIndex)
			return true; 
	}
	return false;
}




void MemoryPager::init(Size_t BlockSize, Size_t PageSize)
{
	_blockSize = BlockSize; //页表内每个block的大小为_blockSize字节

	Size_t numBlocks = PageSize / BlockSize;  // 页表内有_numBlocks个block 
	                                          // PageSize = numBlocks * BlockSize

	if (numBlocks > MaxPerBlockCount)
		numBlocks = MaxPerBlockCount;
	else if (numBlocks < MinPerBlockCount)
		numBlocks = MinPerBlockCount;

	_numBlocks = numBlocks;
}



Size_t MemoryPager::CountEmptyBlocks( ) const
{
 
	return (nullptr == _emptyBlock) ? 0 : 1;
 
}

/*
bool MemoryPager::IsCorrupt(void) const
{
	const bool isEmpty = _blocks.empty();

	BlockCIter start(_blocks.begin());
	BlockCIter last(_blocks.end());
	
	const size_t emptyBlockCount = CountEmptyBlocks();

	if (isEmpty)
	{
		if (start != last)
		{
			assert(false);
			return true;
		}
		if (0 < emptyBlockCount)
		{
			assert(false);
			return true;
		}
		if (nullptr != _deallocBlock)
		{
			assert(false);
			return true;
		}
		if (nullptr != _allocBlock)
		{
			assert(false);
			return true;
		}
		if (nullptr != _emptyBlock)
		{
			assert(false);
			return true;
		}
	}

	else
	{
		const MemoryBlock * front = &_blocks.front();
		const MemoryBlock * back  = &_blocks.back();
		
		if (start >= last)
		{
			assert(false);
			return true;
		}
		if (back < _deallocBlock)
		{
			assert(false);
			return true;
		}
		if (back < _allocBlock)
		{
			assert(false);
			return true;
		}
		if (front > _deallocBlock)
		{
			assert(false);
			return true;
		}
		if (front > _allocBlock)
		{
			assert(false);
			return true;
		}

		switch (emptyBlockCount)
		{
		case 0:
			if (_emptyBlock != NULL)
			{
				assert(false);
				return true;
			}
			break;
		case 1:
			if (_emptyBlock == NULL)
			{
				assert(false);
				return true;
			}
			if (back < _emptyBlock)
			{
				assert(false);
				return true;
			}
			if (front > _emptyBlock)
			{
				assert(false);
				return true;
			}
			if (!_emptyBlock->HasAvailable(_numBlocks))
			{
				// This may imply somebody tried to delete a block twice.
				assert(false);
				return true;
			}
			break;
		default:
			assert(false);
			return true;
		}
		for (BlockCIter it(start); it != last; ++it)
		{
			const MemoryBlock & block = *it;
			if (block.IsCorrupt(_numBlocks, _blockSize, true))
				return true;
		}
	}

	return false;
}
*/
const MemoryBlock* MemoryPager::HasBlock(void * p) const//寻找到可用的大Block块
{
	const Size_t blockLength = _numBlocks * _blockSize;
	for (BlockCIter it = _blocks.begin(); it != _blocks.end(); ++it)
	{
		const MemoryBlock & block = *it;
		if (block.HasBlock(p, blockLength))
			return &block;
	}
	return NULL;
}

 

bool MemoryPager::TrimEmptyBlock(void)
{
	if (nullptr == _emptyBlock) return false;

 
	assert(!_blocks.empty());
 
	assert(1 == CountEmptyBlocks());

	MemoryBlock * lastBlock = &_blocks.back();
	if (lastBlock != _emptyBlock)
		std::swap(*_emptyBlock, *lastBlock);
	
	lastBlock->Release();
	_blocks.pop_back();

	if (_blocks.empty())
	{
		_allocBlock = nullptr;
		_deallocBlock = nullptr;
	}
	else
	{
		if (_deallocBlock == _emptyBlock)
		{
			_deallocBlock = &_blocks.front();
		}
		if (_allocBlock == _emptyBlock)
		{
			_allocBlock = &_blocks.back();	 
		}
	}

	_emptyBlock = nullptr;
 
	return true;
}

 

bool MemoryPager::TrimBlockList(void)
{
	if (_blocks.empty())
	{
		assert(NULL == _allocBlock);
		assert(NULL == _deallocBlock);
	}

	if (_blocks.size() == _blocks.capacity())
		return false;
	 
	Blocks(_blocks).swap(_blocks); //将额外的capacity去掉

	return true;
}

 

bool MemoryPager::MakeNewBlock(void)
{
	bool allocated = false;
	try
	{
		Size_t size = _blocks.size();
		 
		if (_blocks.capacity() == size)
		{
			if (0 == size) size = 4;
			_blocks.reserve(size * 2);
		}
		MemoryBlock newBlock;
		allocated = newBlock.Init(_blockSize, _numBlocks);
		if (allocated)
			_blocks.push_back(newBlock);
	}
	catch (...)
	{
		allocated = false;
	}
	if (!allocated) return false;

	_allocBlock = &_blocks.back();      // _allocBlock  始终指向Blocks的尾部  意为当前正在进行分配的Block
	_deallocBlock = &_blocks.front();   //_deallocBlock 始终指向Blocks的头部  由于Vector扩容会使迭代器失效，所以每加入一个Block，就会重新赋值一遍。
	return true;
}


void * MemoryPager::GetPage(void)
{
	if ((nullptr == _allocBlock) || _allocBlock->RunOut())
	{
		if (nullptr != _emptyBlock)
		{
			_allocBlock = _emptyBlock;
			_emptyBlock = nullptr;
		}
		else
		{
			for (BlockIter i = _blocks.begin();; ++i)
			{
				if (_blocks.end() == i)
				{
					if (!MakeNewBlock())
						return nullptr;
					break;
				}
				if (!i->RunOut())
				{
					_allocBlock = &*i;
					break;
				}
			}
		}
	}
	else if (_allocBlock == _emptyBlock) //释放之前
		_emptyBlock = nullptr;
 
	void * place = _allocBlock->GetBlock(_blockSize);//_blockSize 字节的小block块
	 
	return place;
}

 

bool MemoryPager::ReleasePage(void * p, MemoryBlock* hint)
{
	assert(!_blocks.empty());
	assert(&_blocks.front() <= _deallocBlock);
	assert(&_blocks.back() >= _deallocBlock);
	assert(&_blocks.front() <= _allocBlock);
	assert(&_blocks.back() >= _allocBlock);


	MemoryBlock * foundBlock = (nullptr == hint) ? Find(p) : hint;
	if (NULL == foundBlock)
		return false;

	_deallocBlock = foundBlock; //是要准备释放的Block
	ReleasePage(p);
	 
	return true;
}

 

MemoryBlock * MemoryPager::Find(void * p) const //寻找指针p到底属于哪一个Block
{
	if (_blocks.empty()) return NULL;
	assert(_deallocBlock);

	const std::size_t blockLength = _numBlocks * _blockSize;
	MemoryBlock * lo = _deallocBlock;
	MemoryBlock * hi = _deallocBlock + 1;
	const MemoryBlock * loBound = &_blocks.front();
	const MemoryBlock * hiBound = &_blocks.back() + 1;

	// Special case: deallocChunk_ is the last in the array
	if (hi == hiBound) hi = nullptr;
	for (;;)
	{
		if (lo)
		{
			if (lo->HasBlock(p, blockLength)) return lo;
			if (lo == loBound)
			{
				lo = nullptr;
				if (nullptr == hi) break;
			}
			else --lo;
		}
		if (hi)
		{
			if (hi->HasBlock(p, blockLength)) return hi;
			if (++hi == hiBound)
			{
				hi = nullptr;
				if (nullptr == lo) break;
			}
		}
	}
	return nullptr;
}

void MemoryPager::ReleasePage(void* p) //将指针p指向的空间还给页表
{
	assert(_deallocBlock->HasBlock(p, _numBlocks * _blockSize));
	 
	assert(_emptyBlock != _deallocBlock);
	assert(!_deallocBlock->HasAvailable(_numBlocks));
	 
	assert((nullptr == _emptyBlock) || (_emptyBlock->HasAvailable(_numBlocks)));

	 
	_deallocBlock->ReleaseBlock(p, _blockSize); //将指针指向的空间还给Block

	if (_deallocBlock->HasAvailable(_numBlocks))//如果这个Block内存全收回来了
	{
		assert(_emptyBlock != _deallocBlock);
		 
		if (NULL != _emptyBlock) //如果此时_emptyBlock不为空的话，说明在此之前已经有了一个Block的内存已经回收回来了，这时候就可以将一个Block返还给操作系统了
		{
			 
			MemoryBlock * lastBlock = &_blocks.back();// 获取页表的最后一个Block
			if (lastBlock == _deallocBlock)           // 如果当前释放的Block处于页表尾部
				_deallocBlock = _emptyBlock;          // 那么把当前释放的Block指向之前的Block

			else if (lastBlock != _emptyBlock)        
				std::swap(*_emptyBlock, *lastBlock);  // 使页表的尾部总是空的
			
			assert(lastBlock->HasAvailable(_numBlocks));
			
			lastBlock->Release();//将_emptyBlock内存返回给操作系统
			
			_blocks.pop_back(); // 弹出页表的尾部
			if ((_allocBlock == lastBlock) || _allocBlock->RunOut())
				_allocBlock = _deallocBlock;
		}
		_emptyBlock = _deallocBlock;//内存全收回来的Block
	}  
}



Size_t MemoryPool::_Align(Size_t numBytes, Size_t alignment)
{
	const Size_t   alignExtra = alignment - 1  ;
	return (numBytes + alignExtra) / alignment ;
}


bool MemoryPool::TrimExcessMemory(void)
{
	bool found = false;
	const Size_t allocCount = _Align(_maxAllowSize, _alignSize);
	Size_t i = 0;
	for (; i < allocCount; ++i)
	{
		if (_pages[i].TrimEmptyBlock())
			found = true;
	}
	for (i = 0; i < allocCount; ++i)
	{
		if (_pages[i].TrimBlockList())
			found = true;
	}

	return found;
}


void * MemoryPool::GetBytesByNum(Size_t numBytes)
{
	//assert();
	if (numBytes > _maxAllowSize)
		return ::operator new(numBytes) ;

	if (0 == numBytes)
		numBytes = 1;

	const Size_t index      =  _Align(numBytes, _alignSize) - 1  ;
	const Size_t allocCount =  _Align(_maxAllowSize, _alignSize) ;

	MemoryPager &_page = _pages[index];
	void * place = _page.GetPage();

	if ((nullptr == place) && TrimExcessMemory())//TrimExcessMemory 修补修补
		place = _page.GetPage();
	
	assert(nullptr != place); //important

	return place;
}

void MemoryPool::ReleasePointer(void * p, Size_t numBytes) //通过numBytes找到对应的页
{
	if (nullptr == p)
		return;
	if (numBytes > _maxAllowSize)
	{
		  ::operator delete(p);
		  return;
	}
	if (0 == numBytes) numBytes = 1;
	const std::size_t index = _Align(numBytes, _alignSize ) - 1;
	const std::size_t allocCount = _Align(_maxAllowSize, _alignSize);
	 
	
	MemoryPager & page = _pages[index];
	 
	const bool found = page.ReleasePage(p, NULL); 
}

void MemoryPool::ReleasePointer(void * p) //通过指针来查找对应的页表
{
	if (NULL == p) return;
	assert(NULL != _pages);
	MemoryPager * pAllocator = NULL;
	const std::size_t allocCount = _Align(_maxAllowSize, _alignSize);
	MemoryBlock * block = NULL;

	for (Size_t ii = 0; ii < allocCount; ++ii)
	{
		block = _pages[ii].HasBlock(p);    //Data_t* pc = (Data_t*)p;
		                                   //return (_pData <= pc) && (pc < _pData + chunkLength);
		if (NULL != block)
		{
			pAllocator = &_pages[ii];
			break;
		}
	}
	if (NULL == pAllocator) //没有找到的话，说明这个指针指向的空间不是通过内存池来获取的，直接释放之
	{
		::operator delete(p);
		return;
	}
	const bool found = pAllocator->ReleasePage(p, block); 
}


int MemoryPool::FindPtrGetNumBytes(void * p) //通过指针来找到这个指针是不是内存池里面的，并返回这个指针指向的小block大小
{
	return 0;
}



 