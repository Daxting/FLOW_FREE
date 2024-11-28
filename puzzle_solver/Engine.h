// Engine.h

#ifndef ENGINE_H
#define ENGINE_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

typedef struct {
    int x; // x座標
    int y; // y座標
    bool visited; // 是否已被訪問
    bool isDesignated; // 是否為起終點或已連線
    int numPossibleColors; // 可填入顏色數量
    char* possibleColors; // 可填入顏色選項
} Node;

typedef struct ColorLine{
    char* color;
    Node* start;
    Node* end;
    bool isConnected;
} CL;

// 要記得row和col的大小方便釋放記憶體
int Row, Col, Colors;
char* ColorNames;

// 建立題目的graph
Node** CreateGraph(int , int);

// 建立各顏色的起終點資料
CL* CreateColorLines();

// 檢查連線情況
bool IsConnected(const Node**, Node*, Node*);

// 更新各顏色的連線情況(檢查已連線之顏色)
void LinesConnectedCheck(const Node**, CL**);

// 拓展各顏色連線情況
bool ExpandAdjacent(Node*, Node**, Node**, int*, int*, int*, int*);

// 更新各顏色的連線情況(檢查未連線之顏色)
void LinesReducedCheck(Node**, CL**);

// 釋放graph記憶體
void FreeGraph(Node**);

// 釋放CL記憶體
void FreeCL(CL*);


#endif
