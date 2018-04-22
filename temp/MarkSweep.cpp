#include <list>
#include "stdafx.h"
#include "MarkSweep.h"

using namespace std;

struct ImageSectionInfo
{
	char SectionName[IMAGE_SIZEOF_SHORT_NAME];//the macro is defined WinNT.h
	char *SectionAddress;
	int SectionSize;
	ImageSectionInfo(const char* name)
	{
		strcpy_s(SectionName, name);
	}
};

typedef unsigned int ptr;
 static int *  argnumadress;
static DWORD database, dataend, rdatabase, rdataend;
extern void * tempaddress;

static int countc = 0;



static void mark(ptr p) //���ǵݹ麯��
{	
	ptr tmpp   ;
    unsigned int len  =0  ;
	ptr _p = p ;
	unsigned int i  = 0 ;
	ptr base;

	 std::list<void*> cptr;
	 std::list<int>   ci;
	 std::list<int>   clen;


	if ((void*)_p == nullptr)
		return;
	//cptr.push_back((void *)_p);
	/**/
	do
	{
		//MemoryTool::NewMap ___t=MemoryTool::get_newlinstance();
		MemoryTool::NewMap::iterator &_findresult = MemoryTool::get_newlinstance().find((void*)_p);
		
		if (MemoryTool::get_newlinstance().end() == _findresult)//ָ�벻�ڷ�������
		{		 
			if (cptr.empty())
				return ;

			base = (ptr)(cptr.back());
			cptr.pop_back();

			i = ci.back();
			ci.pop_back();

			len = clen.back();
			clen.pop_back();
			_p = *((unsigned int *)(base + i));
		//	break;
		}
		else
		{
			MemoryTool::NewObject &_newobject = _findresult->second;
			if (_newobject.color == 1)
			{
				if (cptr.empty())
					return;

				base = (ptr)(cptr.back());
				cptr.pop_back();

				i = ci.back();
				ci.pop_back();

				len = clen.back();
				clen.pop_back();

				_p = *((unsigned int *)(base + i));
			//	break;
			}
			else
			{
				i = 0;
				base = (ptr)_newobject.address;
				_newobject.color = 1;//Mark
				countc++;
				len = _newobject.size;
				_p = *((unsigned int *)(unsigned int)(_newobject.address + i / sizeof(MemoryTool)));			
			}

		}	

		if (i >= len)
			continue;
		cptr.push_back((void *)base);
		ci  .push_back(i + 4);
		clen.push_back(len);
 

	} while (!cptr.empty());
	
	/*
	MemoryTool::NewMap::iterator &_findresult  = MemoryTool::get_newlinstance().find((void*)p) ;

	if (MemoryTool::get_newlinstance().end() == _findresult)//ָ�벻�ڷ�������
	{
		return;                   
	}   
	MemoryTool::NewObject &_newobject=_findresult->second;

	if (_newobject.color==1)
	return;
	_newobject.color = 1;//Mark
	countc++;
	len = _newobject.size;


	for (int i = 0; i < (len); i += 4)
	{
	    tmpp = *((unsigned int *)(unsigned int)( _newobject.address + i/sizeof(MemoryTool)));
		mark(tmpp);
	}
	*/
	return;
}


static void mark_from_region(ptr *begin, ptr *end)
{
	ptr *curr = begin;
	ptr tofind = 0;
	for (; curr < end; curr += 1)
	{
	    if ((tofind = *curr) == 0)	
			continue;
		mark(tofind);
	}
}
static void sweep()
{
	MemoryTool::NewMap::iterator &_Addressstart = MemoryTool::get_newlinstance().begin();
	MemoryTool::NewMap::iterator &_Addressend   = MemoryTool::get_newlinstance().end();
	while (_Addressstart != _Addressend)
	{
		if (_Addressstart->second.color == 1)
		{
			_Addressstart->second.color = 0;
		}
		else
		{
			//while (true);//�ڴ������������
		}
		_Addressstart++;
	}
}

void Garbage_Collect()
{
	unsigned int stack_top;//  ָ֡��
	unsigned int _eax_,    //  �ۼ���
		_ebx_,    //  ����ַ (base)�Ĵ���, ���ڴ�Ѱַʱ��Ż���ַ
		_ecx_,    //  ������(counter), ���ظ�(REP)ǰ׺ָ���LOOPָ����ڶ�������
		_edx_,    //  ��������������������������
		_esi_,    //  �ֱ����"Դ/Ŀ�������Ĵ���"(source/destination index)
		_edi_;    //  ��Ϊ�ںܶ��ַ�������ָ����, DS:ESIָ��Դ��,��ES:EDIָ��Ŀ�괮
	_asm
	{
		mov  dword ptr[stack_top], ebp
			mov  dword ptr[_eax_], eax
			mov  dword ptr[_ebx_], ebx
			mov  dword ptr[_ecx_], ecx
			mov  dword ptr[_edx_], edx
			mov  dword ptr[_esi_], esi
			mov  dword ptr[_edi_], edi
	};
	mark(_eax_);
	mark(_ebx_);
	mark(_ecx_);
	mark(_edx_);
	mark(_esi_);
	mark(_edi_);

    mark_from_region((ptr *)((char *)stack_top + 4), (ptr *)(argnumadress - 1));  // ջ�ϵı���	
	mark_from_region((ptr *)((char *)rdatabase), (ptr *)(rdataend));              // rodata;
	mark_from_region((ptr *)((char *)database), (ptr *)(dataend));                // data  ;

	sweep();
} 

void Garbage_Init(int *argnumadd)//ջ��
{
	argnumadress = argnumadd;
	
	HANDLE hModule = GetModuleHandle(NULL);
	// get the location of the module's IMAGE_NT_HEADERS structure
	IMAGE_NT_HEADERS *pNtHdr = ImageNtHeader(hModule);

	// section table immediately follows the IMAGE_NT_HEADERS
	IMAGE_SECTION_HEADER *pSectionHdr = (IMAGE_SECTION_HEADER *)(pNtHdr + 1);

	const char* imageBase = (const char*)hModule;
	char scnName[sizeof(pSectionHdr->Name) + 1];
	scnName[sizeof(scnName)-1] = '\0'; // enforce nul-termination for scn names that are the whole length of pSectionHdr->Name[]

	for (int scn = 0; scn < pNtHdr->FileHeader.NumberOfSections; ++scn)
	{
		// Note: pSectionHdr->Name[] is 8 bytes long. If the scn name is 8 bytes long, ->Name[] will
		// not be nul-terminated. For this reason, copy it to a local buffer that's nul-terminated
		// to be sure we only print the real scn name, and no extra garbage beyond it.
		strncpy_s(scnName, (const char*)pSectionHdr->Name, sizeof(pSectionHdr->Name));

		int equal = strcmp(".data", scnName);
		if (!equal)
		{
			database = (DWORD)imageBase + pSectionHdr->VirtualAddress;
			dataend = (DWORD)imageBase + pSectionHdr->VirtualAddress + pSectionHdr->Misc.VirtualSize - 1;
		}
		equal = strcmp(".rdata", scnName);
		if (!equal)
		{
			rdatabase = (DWORD)(imageBase + pSectionHdr->VirtualAddress);
			rdataend = (DWORD)imageBase + pSectionHdr->VirtualAddress + pSectionHdr->Misc.VirtualSize - 1;
		}

		printf("  Section %3d: %p...%p %-10s (%u bytes)\n",
			scn,
			imageBase + pSectionHdr->VirtualAddress,
			imageBase + pSectionHdr->VirtualAddress + pSectionHdr->Misc.VirtualSize - 1,
			scnName,
			pSectionHdr->Misc.VirtualSize);

		++pSectionHdr;
	}
}
 