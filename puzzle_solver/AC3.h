// AC3.h

#ifndef SOLUTION_H
#define SOLUTION_H

#include "Engine.h"
#include "Interface.h"
#include <stdlib.h>
#include <stdbool.h>

typedef struct ARC{
    Node* node1; // flowfree題目上相鄰的兩個位置
    Node* node2;
} ARC;

// 建立Queue節點
typedef struct QueueNode{
    ARC arc;
    struct QueueNode* next;
} QueueNode;

// 用LinkedList實作Queue
typedef struct Queue{
    QueueNode* front;
    QueueNode* rear;
} Queue;

// 初始化queue
Queue* InitializeQueue();

// 檢查queue是否為空
bool IsEmptyQueue(Queue*);

// 將元素排入queue
void Enqueue(Queue**, ARC);

// 將元素取出queue
ARC Dequeue(Queue*);

// 將queue清空釋放記憶體
void ClearQueue(Queue*);

// 初始化AC-3 演算法的約束條件
void InitializeConstraintQueue(Node**, Queue**);

// 格子在邊界(四個角)
bool IsEdge(const Node*);

// 集合1是否有完全包括集合2
bool IsIncluded(const char*, const char* const);

// 是否為唯一解
int IsUnique(const Node**, int, int, int, int);

// 是否被獨立(四周皆為單色)
bool IsIsolated(const Node**, int, int);

// 決定被獨立格子的顏色，回傳字串表示有結果，否則為NULL
char* IsolatedBlock(const Node**, int, int);

// 去除非解顏色
void RemoveChar(char*, char*);

// 該格子是否只剩一邊未確定
bool IsSemiIsolated(const Node**, int, int, int*);

// 透過從IsSemiIsolated得到的state決定該格子的顏色交集集合
char* SemiIsolated(const Node**, int, int, int*);

// 找兩字串集合比修改node1字串
bool FindIntersection(Node*, char*);

// 在更動node1的值後，找出與他相關的Arc重新檢查
void findRevisedArc(Node**, Queue**, Node*);

// AC-3 revise extension
bool IsRevise(Node**, Node*, Node*);

// AC-3演算法
bool AC3(Node** game);

// AC-3直接找到最終解
bool IsSolutionFound(const Node**);

// 輸出簡化結果
void PrintReducedResult(const Node**);

#endif
