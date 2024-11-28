// Interface.h

#ifndef INTERFACE_H
#define INTERFACE_H

#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>
#include "Engine.h"

// 讀取題目內容
Node** ReadMap(char*, CL**);

// 輸出題目內容
void PrintMap(Node**);

// 輸出各顏色基本資料
void PrintCL(CL*);

#endif
