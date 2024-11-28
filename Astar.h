// Astar.h

#ifndef ASTAR_H
#define ASTAR_H

#include <stdlib.h>
#include <stdbool.h>
#include "Engine.h"
#include "AC3.h"

typedef struct TNode{
    struct TNode* up;
    struct TNode* left;
    struct TNode* down;
    struct TNode* right;
    struct TNode* parentNode;
    char* color;
    int x;
    int y;
    int fcost; // fcost = gcost + hcost
    int gcost; // 起點到目前格子的cost, hcost由heurisitc計算, 不儲存
}Path;

// 更新graph中各個格子的狀態
bool UpdateGraph(Node**, const CL*);

// 複製graph(中間解)
void CopyGraph(Node** , const Node**);

// 初始化tree
Path* PathsInitialize();

// 啟發式函數
int Heuristic(const Node*, const Node*, const Node*, const Node**);

// 初始化集合
void makeSet(int **, int **, int , int);

// 找到節點的根
int find(int **, int , int , int);

// 合併兩個集合
void unionSets(int **, int **, int , int , int , int , int);

// 確定新增節點為合法
bool IsPathLegal(Node**, int, int, char*, int, int, int, int, const CL*, int);

// 新增TNode
void AddTNode(Path**, int, int);

// 更新test
void UpdateTest(Node**, const Path*, int, int);

// 復原test
void RestoreTest(const Node**, Node**, int, int);

// 儲存最終解
void StoreFinalAns(Node**, const Node**);

// 刪除非法路徑
void FreePath(Path*);

// 尋找最底層節點
Path* FindPathsEnd(Path*);

// 釋放tree記憶體
void FreePaths(Path*);

// Astar演算法
bool Astar(Node**, CL**);

#endif