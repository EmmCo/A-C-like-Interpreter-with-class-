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

	_pData = static_cast< Data_t * >(::operator new (allocSize));//�����������ķ����ڴ�ռ�
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
	for (Data_t * p = _pData; i != blocks; p += blockSize)//�����Ǳ�����һ����õ�λ�á�
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

	Data_t * pResult     = _pData + (_firstAvailableBlock*blockSize);//��ȡ����ʷ����ȥ��block
	_firstAvailableBlock = *pResult;
	--_blocksAvailable;

	return pResult;

}
void MemoryBlock::ReleaseBlock(void * p ,Size_t blockSize)
{
	Data_t * toRelease = (Data_t *)p;

	Size_t index = (toRelease - _pData) / blockSize;

	*toRelease = _firstAvailableBlock;

	_firstAvailableBlock = index; //���ǽ�����ʷ���Ŀ�����Ϊ���ٽ��������Ŀ飬��������߻���������

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
bool MemoryBlock::IsBlockAvailable(void * p, Data_t numBlocks,Size_t blockSize) const //�ж�һ��ָ��ָ���Сblock�����Ƿ������ʹ��
{
	if (RunOut())
		return false;

	Data_t* place = (Data_t*) p;

	Data_t blockIndex = (place - _pData) / blockSize ;

	Data_t index = _firstAvailableBlock;

	if (index == blockIndex) //���ָ���Block�����ǵ�ǰ���ʺϷ����ȥ�ģ���ͷ���true
		return true;
	
	std::bitset< UCHAR_MAX > foundBlocks;
	
	Data_t* nextBlock = NULL;

	for (Data_t cc = 0;;)
	{
		nextBlock = _pData + (index * blockSize);
		foundBlocks.set(index, true);
		++cc;
		if (cc >= _blocksAvailable)//�ұ������п���Сblock����
			break;
		index = *nextBlock;
		if (index == blockIndex)
			return true; 
	}
	return false;
}




void MemoryPager::init(Size_t BlockSize, Size_t PageSize)
{
	_blockSize = BlockSize; //ҳ����ÿ��block�Ĵ�СΪ_blockSize�ֽ�

	Size_t numBlocks = PageSize / BlockSize;  // ҳ������_numBlocks��block 
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
const MemoryBlock* MemoryPager::HasBlock(void * p) const//Ѱ�ҵ����õĴ�Block��
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
	 
	Blocks(_blocks).swap(_blocks); //�������capacityȥ��

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

	_allocBlock = &_blocks.back();      // _allocBlock  ʼ��ָ��Blocks��β��  ��Ϊ��ǰ���ڽ��з����Block
	_deallocBlock = &_blocks.front();   //_deallocBlock ʼ��ָ��Blocks��ͷ��  ����Vector���ݻ�ʹ������ʧЧ������ÿ����һ��Block���ͻ����¸�ֵһ�顣
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
	else if (_allocBlock == _emptyBlock) //�ͷ�֮ǰ
		_emptyBlock = nullptr;
 
	void * place = _allocBlock->GetBlock(_blockSize);//_blockSize �ֽڵ�Сblock��
	 
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

	_deallocBlock = foundBlock; //��Ҫ׼���ͷŵ�Block
	ReleasePage(p);
	 
	return true;
}

 

MemoryBlock * MemoryPager::Find(void * p) const //Ѱ��ָ��p����������һ��Block
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

void MemoryPager::ReleasePage(void* p) //��ָ��pָ��Ŀռ仹��ҳ��
{
	assert(_deallocBlock->HasBlock(p, _numBlocks * _blockSize));
	 
	assert(_emptyBlock != _deallocBlock);
	assert(!_deallocBlock->HasAvailable(_numBlocks));
	 
	assert((nullptr == _emptyBlock) || (_emptyBlock->HasAvailable(_numBlocks)));

	 
	_deallocBlock->ReleaseBlock(p, _blockSize); //��ָ��ָ��Ŀռ仹��Block

	if (_deallocBlock->HasAvailable(_numBlocks))//������Block�ڴ�ȫ�ջ�����
	{
		assert(_emptyBlock != _deallocBlock);
		 
		if (NULL != _emptyBlock) //�����ʱ_emptyBlock��Ϊ�յĻ���˵���ڴ�֮ǰ�Ѿ�����һ��Block���ڴ��Ѿ����ջ����ˣ���ʱ��Ϳ��Խ�һ��Block����������ϵͳ��
		{
			 
			MemoryBlock * lastBlock = &_blocks.back();// ��ȡҳ������һ��Block
			if (lastBlock == _deallocBlock)           // �����ǰ�ͷŵ�Block����ҳ��β��
				_deallocBlock = _emptyBlock;          // ��ô�ѵ�ǰ�ͷŵ�Blockָ��֮ǰ��Block

			else if (lastBlock != _emptyBlock)        
				std::swap(*_emptyBlock, *lastBlock);  // ʹҳ���β�����ǿյ�
			
			assert(lastBlock->HasAvailable(_numBlocks));
			
			lastBlock->Release();//��_emptyBlock�ڴ淵�ظ�����ϵͳ
			
			_blocks.pop_back(); // ����ҳ���β��
			if ((_allocBlock == lastBlock) || _allocBlock->RunOut())
				_allocBlock = _deallocBlock;
		}
		_emptyBlock = _deallocBlock;//�ڴ�ȫ�ջ�����Block
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

	if ((nullptr == place) && TrimExcessMemory())//TrimExcessMemory �޲��޲�
		place = _page.GetPage();
	
	assert(nullptr != place); //important

	return place;
}

void MemoryPool::ReleasePointer(void * p, Size_t numBytes) //ͨ��numBytes�ҵ���Ӧ��ҳ
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

void MemoryPool::ReleasePointer(void * p) //ͨ��ָ�������Ҷ�Ӧ��ҳ��
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
	if (NULL == pAllocator) //û���ҵ��Ļ���˵�����ָ��ָ��Ŀռ䲻��ͨ���ڴ������ȡ�ģ�ֱ���ͷ�֮
	{
		::operator delete(p);
		return;
	}
	const bool found = pAllocator->ReleasePage(p, block); 
}


int MemoryPool::FindPtrGetNumBytes(void * p) //ͨ��ָ�����ҵ����ָ���ǲ����ڴ������ģ����������ָ��ָ���Сblock��С
{
	return 0;
}



 