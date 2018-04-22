#ifndef __MARKSWEEP_H
#define __MARKSWEEP_H

#include <stdio.h>
#include <winsock2.h>
#include <stdlib.h>
#include <Winbase.h>
#include <winnt.h>
#include <Dbghelp.h>
#include <string.h>
#include <string>

#pragma comment(lib,"ws2_32.lib")
#pragma comment( lib, "dbghelp.lib" )
#include "MemoryManager.h" 

void Garbage_Init(int *argnumadd);
void Garbage_Collect();

#endif