// AC3.c

#include "AC3.h"


// 初始化queue
Queue* InitializeQueue(){
    Queue *q = (Queue*)malloc(sizeof(Queue)); // 分配記憶體

    if (q == NULL) {
        // 記憶體分配失敗
        printf("Memory allocation failed\n");
        return NULL;
    }
    q->front = q->rear = NULL;
    return q;
}
// 檢查queue是否為空
bool IsEmptyQueue(Queue* q){
    return q->front == NULL;
}
// 將元素排入queue
void Enqueue(Queue** q, ARC arc){
    // 創建新的queue節點
    QueueNode* newNode = (QueueNode*)malloc(sizeof(QueueNode));
    newNode->arc = arc;
    newNode->next = NULL;

    // 如果佇列是空的，前後端都指向新節點
    if((*q)->rear == NULL){
        (*q)->front = (*q)->rear = newNode;
        return;
    }
    
    // 否則，將新節點連接到後端，並更新 rear
    (*q)->rear->next = newNode;
    (*q)->rear = newNode;
}
// 將元素取出queue
ARC Dequeue(Queue* q){
    // 如果佇列為空，回傳空的Arc
    if(q->front == NULL){
        ARC arc = {NULL, NULL};
        return arc;
    }

    // 將最前端節點輸出
    QueueNode* tempNode = q->front;
    ARC tempArc = tempNode->arc;
    q->front = q->front->next;

    // 如果最前端為最後一個節點
    if(q->front == NULL)
        q->rear = NULL;

    // 釋放已移除的節點
    free(tempNode);
    return tempArc;
}
// 將queue清空釋放記憶體
void ClearQueue(Queue* q){
    while(!IsEmptyQueue(q))
        Dequeue(q);
}
// 初始化AC-3 演算法的約束條件
void InitializeConstraintQueue(Node** game, Queue **q){
    
    int Redge = Row - 1, Cedge = Col - 1;

    for(int i = 0; i< Row; i++){
        for(int j = 0; j< Col; j++){
            if(i > 0){
                ARC arc = {&game[i][j], &game[i - 1][j]};
                Enqueue(q, arc);
            }
            if(i < Redge){
                ARC arc = {&game[i][j], &game[i + 1][j]};
                Enqueue(q, arc);
            }
            if(j > 0){
                ARC arc = {&game[i][j], &game[i][j - 1]};
                Enqueue(q, arc);
            }
            if(j < Cedge){
                ARC arc = {&game[i][j], &game[i][j + 1]};
                Enqueue(q, arc);
            }
        }
    }
}

// 格子在邊界(四個角)
bool IsEdge(const Node* n){
    if((n->x == 0 || n->x == (Col - 1)) && (n->y == 0  || n->y == (Row - 1)))
        return true;
    else
        return false;
}

// 是否為唯一邊界解
bool IsUniqueEdge(const Node** game, int x2, int y2){
    bool con1 = false, con2 = false, con3 = false, con4 = false;
    if(x2 == 0){
        con1 = strcmp(game[y2][x2].possibleColors, game[y2][x2 + 1].possibleColors) == 0;
        con3 = game[y2][x2 + 1].numPossibleColors == 1;
        if(y2 == 0){
            con2 = strcmp(game[y2][x2].possibleColors, game[y2 + 1][x2].possibleColors) == 0;
            con4 = game[y2 + 1][x2].numPossibleColors == 1;
        }
        else{
            con2 = strcmp(game[y2][x2].possibleColors, game[y2 - 1][x2].possibleColors) == 0;
            con4 = game[y2 - 1][x2].numPossibleColors == 1;
        }
    }
    else{
        con1 = strcmp(game[y2][x2].possibleColors, game[y2][x2 - 1].possibleColors) == 0;
        con3 = game[y2][x2 - 1].numPossibleColors == 1;
        if(y2 == 0){
            con2 = strcmp(game[y2][x2].possibleColors, game[y2 + 1][x2].possibleColors) == 0;
            con4 = game[y2 + 1][x2].numPossibleColors == 1;
        }
        else{
            con2 = strcmp(game[y2][x2].possibleColors, game[y2 - 1][x2].possibleColors) == 0;
            con4 = game[y2 - 1][x2].numPossibleColors == 1;
        }
    }
    if((!con1 && con3) || (!con2 && con4))
        return true;
    else
        return false;
}

// 集合1是否有完全包括集合2
bool IsIncluded(const char* str1, const char* str2){
    int len1 = strlen(str1), len2 = strlen(str2);
    for(int i = 0; i< len2; i++){
        for(int j = 0; j< len1; j++){
            if(str1[j] == str2[i])
                break;
            else if(j == (len1 - 1))
                return false;
        }
    }
    return true;
}

// 是否為唯一解
int IsUnique(const Node** game, int x1, int y1, int x2, int y2){
    if(x2 == 0){
        bool con1 = false, con2 = false, con3 = false, con4 = false;
        bool con5 = game[y2][x2].isDesignated == 1;
        if(y1 > y2){ // node1 在 node2 下面
            con1 = strcmp(game[y2][x2].possibleColors, game[y2][x2 + 1].possibleColors) == 0;
            con2 = strcmp(game[y2 - 1][x2].possibleColors, game[y2][x2].possibleColors) == 0;
            con3 = game[y2][x2 + 1].numPossibleColors == 1;
            con4 = game[y2 - 1][x2].numPossibleColors == 1;
        }
        else if(y1 < y2){ // node1 在 node2 上面
            con1 = strcmp(game[y2][x2].possibleColors, game[y2][x2 + 1].possibleColors) == 0;
            con2 = strcmp(game[y2 + 1][x2].possibleColors, game[y2][x2].possibleColors) == 0;
            con3 = game[y2][x2 + 1].numPossibleColors == 1;
            con4 = game[y2 + 1][x2].numPossibleColors == 1; 
        }
        else{ // node1 在 node2 右邊
            con1 = strcmp(game[y2 - 1][x2].possibleColors, game[y2][x2].possibleColors) == 0;
            con2 = strcmp(game[y2 + 1][x2].possibleColors, game[y2][x2].possibleColors) == 0;
            con3 = game[y2 - 1][x2].numPossibleColors == 1;
            con4 = game[y2 + 1][x2].numPossibleColors == 1;
        }
        if((con1 && con2) || ((con1 || con2) && con5))
            return 2;
        else if(con3 && con4)
            return 1;
        else
            return 0;
    }
    else if(x2 == (Col - 1)){
        bool con1 = false, con2 = false, con3 = false, con4 = false;
        bool con5 = (game[y2][x2].isDesignated == 1);
        if(y1 > y2){ // node1 在 node2 下面
            con1 = strcmp(game[y2][x2].possibleColors, game[y2][x2 - 1].possibleColors) == 0;
            con2 = strcmp(game[y2 - 1][x2].possibleColors, game[y2][x2].possibleColors) == 0;
            con3 = game[y2][x2 - 1].numPossibleColors == 1;
            con4 = game[y2 - 1][x2].numPossibleColors == 1;
        }
        else if(y1 < y2){ // node1 在 node2 上面
            con1 = strcmp(game[y2][x2].possibleColors, game[y2][x2 - 1].possibleColors) == 0;
            con2 = strcmp(game[y2 + 1][x2].possibleColors, game[y2][x2].possibleColors) == 0;
            con3 = game[y2][x2 - 1].numPossibleColors == 1;
            con4 = game[y2 + 1][x2].numPossibleColors == 1;
        }
        else{ // node1 在 node2 右邊
            con1 = strcmp(game[y2 - 1][x2].possibleColors, game[y2][x2].possibleColors) == 0;
            con2 = strcmp(game[y2 + 1][x2].possibleColors, game[y2][x2].possibleColors) == 0;
            con3 = game[y2 - 1][x2].numPossibleColors == 1;
            con4 = game[y2 + 1][x2].numPossibleColors == 1;
        }
        if((con1 && con2) || ((con1 || con2) && con5))
            return 2;
        else if(con3 && con4)
            return 1;
        else
            return 0;
    }
    else if(y2 == 0){
        bool con1 = false, con2 = false, con3 = false, con4 = false;
        bool con5 = (game[y2][x2].isDesignated == 1);
        if(x1 > x2){ // node1 在 node2 右邊
            con1 = strcmp(game[y2][x2].possibleColors, game[y2][x2 - 1].possibleColors) == 0;
            con2 = strcmp(game[y2 + 1][x2].possibleColors, game[y2][x2].possibleColors) == 0;
            con3 = game[y2][x2 - 1].numPossibleColors == 1;
            con4 = game[y2 + 1][x2].numPossibleColors == 1;
        }
        else if(x1 < x2){ // node1 在 node2 左邊
            con1 = strcmp(game[y2][x2].possibleColors, game[y2][x2 + 1].possibleColors) == 0;
            con2 = strcmp(game[y2 + 1][x2].possibleColors, game[y2][x2].possibleColors) == 0;
            con3 = game[y2][x2 + 1].numPossibleColors == 1;
            con4 = game[y2 + 1][x2].numPossibleColors == 1; 
        }
        else{ // node1 在 node2 下面
            con1 = strcmp(game[y2][x2 - 1].possibleColors, game[y2][x2].possibleColors) == 0;
            con2 = strcmp(game[y2][x2 + 1].possibleColors, game[y2][x2].possibleColors) == 0;
            con3 = game[y2][x2 - 1].numPossibleColors == 1;
            con4 = game[y2][x2 + 1].numPossibleColors == 1;
        }
        if((con1 && con2) || ((con1 || con2) && con5))
            return 2;
        else if(con3 && con4)
            return 1;
        else
            return 0;
    }
    else if(y2 == (Row - 1)){
        bool con1 = false, con2 = false, con3 = false, con4 = false;
        bool con5 = (game[y2][x2].isDesignated == 1);
        if(x1 > x2){ // node1 在 node2 右邊
            con1 = strcmp(game[y2][x2].possibleColors, game[y2][x2 - 1].possibleColors) == 0;
            con2 = strcmp(game[y2 - 1][x2].possibleColors, game[y2][x2].possibleColors) == 0;
            con3 = game[y2][x2 - 1].numPossibleColors == 1;
            con4 = game[y2 - 1][x2].numPossibleColors == 1;

        }
        else if(x1 < x2){ // node1 在 node2 左邊
            con1 = strcmp(game[y2][x2].possibleColors, game[y2][x2 + 1].possibleColors) == 0;
            con2 = strcmp(game[y2 - 1][x2].possibleColors, game[y2][x2].possibleColors) == 0;
            con3 = game[y2][x2 + 1].numPossibleColors == 1;
            con4 = game[y2 - 1][x2].numPossibleColors == 1;
        }
        else{ // node1 在 node2 上面
            con1 = strcmp(game[y2][x2 - 1].possibleColors, game[y2][x2].possibleColors) == 0;
            con2 = strcmp(game[y2][x2 + 1].possibleColors, game[y2][x2].possibleColors) == 0;
            con3 = game[y2][x2 - 1].numPossibleColors == 1;
            con4 = game[y2][x2 + 1].numPossibleColors == 1;
        }
        if((con1 && con2) || ((con1 || con2) && con5))
            return 2;
        else if(con3 && con4)
            return 1;
        else
            return 0;
    }
    else{
        bool con1 = false, con2 = false, con3 = false, con4 = false, con5 = false, con6 = false;
        bool con7 = game[y2][x2].isDesignated == true;
        if(x1 > x2){ // node1 在 node2 右邊
            con1 = strcmp(game[y2][x2].possibleColors, game[y2][x2 - 1].possibleColors) == 0;
            con2 = strcmp(game[y2][x2].possibleColors, game[y2 - 1][x2].possibleColors) == 0;
            con3 = strcmp(game[y2][x2].possibleColors, game[y2 + 1][x2].possibleColors) == 0;
            con4 = IsIncluded((const char*) game[y2][x2 - 1].possibleColors, (const char*) game[y2][x2].possibleColors);
            con5 = IsIncluded((const char*) game[y2 - 1][x2].possibleColors, (const char*) game[y2][x2].possibleColors);
            con6 = IsIncluded((const char*) game[y2 + 1][x2].possibleColors, (const char*) game[y2][x2].possibleColors);
        }
        else if(x1 < x2){ // node1 在 node2 左邊
            con1 = strcmp(game[y2][x2].possibleColors, game[y2][x2 + 1].possibleColors) == 0;
            con2 = strcmp(game[y2][x2].possibleColors, game[y2 - 1][x2].possibleColors) == 0;
            con3 = strcmp(game[y2][x2].possibleColors, game[y2 + 1][x2].possibleColors) == 0;
            con4 = IsIncluded((const char*) game[y2][x2 + 1].possibleColors, (const char*) game[y2][x2].possibleColors);
            con5 = IsIncluded((const char*) game[y2 - 1][x2].possibleColors, (const char*) game[y2][x2].possibleColors);
            con6 = IsIncluded((const char*) game[y2 + 1][x2].possibleColors, (const char*) game[y2][x2].possibleColors);
        }
        else if(y1 > y2){ // node1 在 node2 下面
            con1 = strcmp(game[y2][x2].possibleColors, game[y2][x2 + 1].possibleColors) == 0;
            con2 = strcmp(game[y2][x2].possibleColors, game[y2][x2 - 1].possibleColors) == 0;
            con3 = strcmp(game[y2][x2].possibleColors, game[y2 - 1][x2].possibleColors) == 0;
            con4 = IsIncluded((const char*) game[y2][x2 + 1].possibleColors, (const char*) game[y2][x2].possibleColors);
            con5 = IsIncluded((const char*) game[y2][x2 - 1].possibleColors, (const char*) game[y2][x2].possibleColors);
            con6 = IsIncluded((const char*) game[y2 - 1][x2].possibleColors, (const char*) game[y2][x2].possibleColors);
        }
        else{ // node1 在 node2 上面
            con1 = strcmp(game[y2][x2].possibleColors, game[y2][x2 + 1].possibleColors) == 0;
            con2 = strcmp(game[y2][x2].possibleColors, game[y2][x2 - 1].possibleColors) == 0;
            con3 = strcmp(game[y2][x2].possibleColors, game[y2 + 1][x2].possibleColors) == 0;
            con4 = IsIncluded((const char*) game[y2][x2 + 1].possibleColors, (const char*) game[y2][x2].possibleColors);
            con5 = IsIncluded((const char*) game[y2][x2 - 1].possibleColors, (const char*) game[y2][x2].possibleColors);
            con6 = IsIncluded((const char*) game[y2 + 1][x2].possibleColors, (const char*) game[y2][x2].possibleColors);
        }
        if((con1 && con2) || (con2 && con3) || (con1 && con3))
            return 2;
        else if((con4 && !con5 && !con6 && !con7) || (!con4 && con5 && !con6 && !con7) || (!con4 && !con5 && con6 && !con7) || (!con4 && !con5 && !con6))
            return 1;
        else
            return 0;
    }
}

// 是否被隔離(四周皆為單色)
bool IsIsolated(const Node** game, int x1, int y1){
    bool con1 = false, con2 = false, con3 = false, con4 = false;
    if(x1 == 0){
        con1 = game[y1-1][x1].numPossibleColors == 1;
        con2 = game[y1+1][x1].numPossibleColors == 1;
        con3 = game[y1][x1+1].numPossibleColors == 1;
        con4 = true;
    }
    else if(x1 == (Col - 1)){
        con1 = game[y1-1][x1].numPossibleColors == 1;
        con2 = game[y1+1][x1].numPossibleColors == 1;
        con3 = game[y1][x1-1].numPossibleColors == 1;
        con4 = true;
    }
    else if(y1 == 0){
        con1 = game[y1+1][x1].numPossibleColors == 1;
        con2 = game[y1][x1-1].numPossibleColors == 1;
        con3 = game[y1][x1+1].numPossibleColors == 1;
        con4 = true;
    }
    else if(y1 == (Row - 1)){
        con1 = game[y1-1][x1].numPossibleColors == 1;
        con2 = game[y1][x1-1].numPossibleColors == 1;
        con3 = game[y1][x1+1].numPossibleColors == 1;
        con4 = true;
    }
    else{
        con1 = game[y1-1][x1].numPossibleColors == 1;
        con2 = game[y1+1][x1].numPossibleColors == 1;
        con3 = game[y1][x1-1].numPossibleColors == 1;
        con4 = game[y1][x1+1].numPossibleColors == 1;
    }
    if(con1 && con2 && con3 && con4)
        return true;
    else
        return false;
}

// 決定被獨立格子的顏色，回傳字串表示有結果，否則為NULL
char* IsolatedBlock(const Node** game, int x1, int y1){
    char* answer = NULL;
    if(x1 == 0){ // node1在最左側
        bool con1 = strcmp(game[y1-1][x1].possibleColors, game[y1+1][x1].possibleColors) == 0;
        bool con2 = strcmp(game[y1-1][x1].possibleColors, game[y1][x1+1].possibleColors) == 0;
        answer = (char*)malloc(2*sizeof(char));
        if(con1 || con2)
            strcpy(answer, game[y1-1][x1].possibleColors);
        else
            strcpy(answer, game[y1+1][x1].possibleColors);
        return answer;
    }
    else if(x1 == (Col - 1)){ // node1在最右側
        bool con1 = strcmp(game[y1-1][x1].possibleColors, game[y1+1][x1].possibleColors) == 0;
        bool con2 = strcmp(game[y1-1][x1].possibleColors, game[y1][x1-1].possibleColors) == 0;
        answer = (char*)malloc(2*sizeof(char));
        if(con1 || con2)
            strcpy(answer, game[y1-1][x1].possibleColors);
        else
            strcpy(answer, game[y1+1][x1].possibleColors);
        return answer;
    }
    else if(y1 == 0){ // node1在最上側
        bool con1 = strcmp(game[y1+1][x1].possibleColors, game[y1][x1-1].possibleColors) == 0;
        bool con2 = strcmp(game[y1][x1+1].possibleColors, game[y1][x1-1].possibleColors) == 0;
        answer = (char*)malloc(2*sizeof(char));
        if(con1 || con2)
            strcpy(answer, game[y1][x1-1].possibleColors);
        else
            strcpy(answer, game[y1][x1+1].possibleColors);
        return answer;
    }
    else if(y1 == (Row - 1)){ // node1在最下側
            bool con1 = strcmp(game[y1-1][x1].possibleColors, game[y1][x1-1].possibleColors) == 0;
            bool con2 = strcmp(game[y1][x1+1].possibleColors, game[y1][x1-1].possibleColors) == 0;
            answer = (char*)malloc(2*sizeof(char));
            if(con1 || con2)
                strcpy(answer, game[y1][x1-1].possibleColors);
            else
                strcpy(answer, game[y1][x1+1].possibleColors);
            return answer;
    }
    else{ // node1不在側邊
        // 先決定四周有2種或3種顏色
        int orderType = 0;
        // 1: 3Color且目標格子(左及上) || (左及右) || (左及下)為同色
        // 2: 3Color且目標格子(上及右) || (上及下)為同色
        // 3: 3Color且目標格子(右及下)為同色
        // 4: 2Color且目標格子(左及上)為同色
        // 5: 2Color且目標格子(左及右)為同色
        // 6: 2Color且目標格子(左及下)為同色
        if(game[y1][x1-1].possibleColors[0] == game[y1-1][x1].possibleColors[0]){
            orderType = 1;
            if(game[y1][x1+1].possibleColors[0] == game[y1-1][x1].possibleColors[0])
                orderType = 4;
        }
        else if(game[y1][x1-1].possibleColors[0] == game[y1][x1+1].possibleColors[0]){
            orderType = 1;
            if(game[y1-1][x1].possibleColors[0] == game[y1+1][x1].possibleColors[0])
                orderType = 5;
        }
        else if(game[y1][x1-1].possibleColors[0] == game[y1+1][x1].possibleColors[0]){
            orderType = 1;
            if(game[y1-1][x1].possibleColors[0] == game[y1][x1+1].possibleColors[0])
                orderType = 6;
        }
        else if(game[y1-1][x1].possibleColors[0] == game[y1][x1+1].possibleColors[0] || game[y1-1][x1].possibleColors[0] == game[y1+1][x1].possibleColors[0])
            orderType = 2;
        else if(game[y1][x1+1].possibleColors[0] == game[y1+1][x1].possibleColors[0])
            orderType = 3;
        else{
            printf("\nWRONG!!!\n");
            return answer;
        }

        answer = (char*)malloc(2*sizeof(char));
        switch(orderType){
            case 1:
                strcpy(answer, game[y1][x1-1].possibleColors);
                break;
            case 2:
                strcpy(answer, game[y1-1][x1].possibleColors);
                break;
            case 3:
                strcpy(answer, game[y1][x1+1].possibleColors);
                break;
            case 4:
                if(strcmp(game[y1-1][x1-1].possibleColors, game[y1][x1-1].possibleColors) == 0)
                    strcpy(answer, game[y1][x1+1].possibleColors);
                else if(strcmp(game[y1+1][x1+1].possibleColors, game[y1][x1+1].possibleColors) == 0)
                    strcpy(answer, game[y1][x1-1].possibleColors);
                else{
                    answer = (char*)malloc(3*sizeof(char));
                    answer[0] = game[y1][x1+1].possibleColors[0];
                    answer[1] = game[y1][x1-1].possibleColors[0];
                    answer[2] = '\0';
                }
                break;
            case 5:
                answer = (char*)malloc(3*sizeof(char));
                answer[0] = game[y1][x1-1].possibleColors[0];
                answer[1] = game[y1-1][x1].possibleColors[0];
                answer[2] = '\0';
                break;
            case 6:
                if(strcmp(game[y1+1][x1-1].possibleColors, game[y1][x1-1].possibleColors) == 0)
                    strcpy(answer, game[y1][x1+1].possibleColors);
                else if(strcmp(game[y1-1][x1+1].possibleColors, game[y1][x1+1].possibleColors) == 0)
                    strcpy(answer, game[y1][x1-1].possibleColors);
                else{
                    answer = (char*)malloc(3*sizeof(char));
                    answer[0] = game[y1][x1+1].possibleColors[0];
                    answer[1] = game[y1][x1-1].possibleColors[0];
                    answer[2] = '\0';
                }
                break;
            default:
                printf("\nWRONG in case!!!\n");
                break;
        }
        return answer;
    }
}

// 除去四角可能性
char* EdgeReduced(const Node** game, int x1, int y1){
    if(x1 == 0){
        if(y1 == 0){
            if(game[y1+1][x1+1].numPossibleColors == 1 && IsIncluded((const char*) game[y1][x1].possibleColors, (const char*) game[y1+1][x1+1].possibleColors))
                return game[y1+1][x1+1].possibleColors;
            else
                return NULL;
        }
        else{
            if(game[y1-1][x1+1].numPossibleColors == 1 && IsIncluded((const char*) game[y1][x1].possibleColors, (const char*) game[y1-1][x1+1].possibleColors))
                return game[y1-1][x1+1].possibleColors;
            else
                return NULL;
        }
    }
    else{
        if(y1 == 0){
            if(game[y1+1][x1-1].numPossibleColors == 1 && IsIncluded((const char*) game[y1][x1].possibleColors, (const char*) game[y1+1][x1-1].possibleColors))
                return game[y1+1][x1-1].possibleColors;
            else
                return NULL;
        }
        else{
            if(game[y1-1][x1-1].numPossibleColors == 1 && IsIncluded((const char*) game[y1][x1].possibleColors, (const char*) game[y1-1][x1-1].possibleColors))
                return game[y1-1][x1-1].possibleColors;
            else
                return NULL;
        }
    }
}

// 去除非解顏色
void RemoveChar(char* s1, char* s2){
    int len = strlen(s1);
    char c = s2[0];
    bool isFound = false;
    for(int i = 0; i< len; i++){
        if(s1[i] == c || isFound){
            s1[i] = s1[i+1];
            isFound = true;
        }
    }
}

// 該格子是否只剩一邊未確定
bool IsSemiIsolated(const Node** game, int x1, int y1, int* state){
    
    if(x1 == 0){
        bool con1 = game[y1-1][x1].numPossibleColors == 1;
        bool con2 = game[y1][x1+1].numPossibleColors == 1;
        bool con3 = game[y1+1][x1].numPossibleColors == 1;
        if(con1 && con2)
            *state = 1;
        else if(con1 && con3)
            *state = 2;
        else if(con2 && con3)
            *state = 3;
        else
            return false;
    }
    else if(x1 == (Col - 1)){
        bool con1 = game[y1-1][x1].numPossibleColors == 1;
        bool con2 = game[y1][x1-1].numPossibleColors == 1;
        bool con3 = game[y1+1][x1].numPossibleColors == 1;
        if(con1 && con2)
            *state = 4;
        else if(con1 && con3)
            *state = 2;
        else if(con2 && con3)
            *state = 5;
        else
            return false;
    }
    else if(y1 == 0){
        bool con1 = game[y1][x1-1].numPossibleColors == 1;
        bool con2 = game[y1+1][x1].numPossibleColors == 1;
        bool con3 = game[y1][x1+1].numPossibleColors == 1;
        if(con1 && con2)
            *state = 5;
        else if(con1 && con3)
            *state = 6;
        else if(con2 && con3)
            *state = 3;
        else
            return false;
    }
    else if(y1 == (Row - 1)){
        bool con1 = game[y1][x1-1].numPossibleColors == 1;
        bool con2 = game[y1-1][x1].numPossibleColors == 1;
        bool con3 = game[y1][x1+1].numPossibleColors == 1;
        if(con1 && con2)
            *state = 4;
        else if(con1 && con3)
            *state = 6;
        else if(con2 && con3)
            *state = 1;
        else
            return false;
    }
    else{
        bool con1 = game[y1-1][x1].numPossibleColors == 1; // 上
        bool con2 = game[y1][x1+1].numPossibleColors == 1; // 右
        bool con3 = game[y1+1][x1].numPossibleColors == 1; // 下
        bool con4 = game[y1][x1-1].numPossibleColors == 1; // 左
        if(con1 && con2 && con3)
            *state = 7;
        else if(con1 && con2 && con4)
            *state = 8;
        else if(con1 && con3 && con4)
            *state = 9;
        else if(con2 && con3 && con4)
            *state = 10;
        else
            return false;
    }
    return true;
}

// 透過從IsSemiIsolated得到的state決定該格子的顏色交集集合
char* SemiIsolated(const Node** game, int x1, int y1, int* state){
    
    char* temp = (char*)malloc(4*sizeof(char));
    switch(*state){
        case 1:
            temp[0] = game[y1-1][x1].possibleColors[0];
            temp[1] = game[y1][x1+1].possibleColors[0];
            temp[2] = '\0';
            break;
        case 2:
            temp[0] = game[y1-1][x1].possibleColors[0];
            temp[1] = game[y1+1][x1].possibleColors[0];
            temp[2] = '\0';            
            break;
        case 3:
            temp[0] = game[y1][x1+1].possibleColors[0];
            temp[1] = game[y1+1][x1].possibleColors[0];
            temp[2] = '\0';              
            break;
        case 4:
            temp[0] = game[y1-1][x1].possibleColors[0];
            temp[1] = game[y1][x1-1].possibleColors[0];
            temp[2] = '\0';               
            break;
        case 5:
            temp[0] = game[y1][x1-1].possibleColors[0];
            temp[1] = game[y1+1][x1].possibleColors[0];
            temp[2] = '\0';             
            break;
        case 6:
            temp[0] = game[y1][x1-1].possibleColors[0];
            temp[1] = game[y1][x1+1].possibleColors[0];
            temp[2] = '\0';  
            break;
        case 7:
            temp[0] = game[y1-1][x1].possibleColors[0];
            temp[1] = game[y1][x1+1].possibleColors[0];
            temp[2] = game[y1+1][x1].possibleColors[0];
            temp[3] = '\0';  
            break;
        case 8:
            temp[0] = game[y1-1][x1].possibleColors[0];
            temp[1] = game[y1][x1+1].possibleColors[0];
            temp[2] = game[y1][x1-1].possibleColors[0];
            temp[3] = '\0';  
            break;
        case 9:
            temp[0] = game[y1-1][x1].possibleColors[0];
            temp[1] = game[y1+1][x1].possibleColors[0];
            temp[2] = game[y1][x1-1].possibleColors[0];
            temp[3] = '\0';  
            break;
        case 10:
            temp[0] = game[y1][x1+1].possibleColors[0];
            temp[1] = game[y1+1][x1].possibleColors[0];
            temp[2] = game[y1][x1-1].possibleColors[0];
            temp[3] = '\0';  
            break;
        default:
            break;
    }
    return temp;
}

// 找兩字串集合比修改node1字串
bool FindIntersection(Node* n, char* t){
    int len1 = strlen(n->possibleColors), len2 = strlen(t), count = 0;
    char* temp = (char*)malloc(3*sizeof(char));
    for(int i = 0; i< len2; i++){
        for(int j = 0; j< len1; j++){
            if(n->possibleColors[j] == t[i]){
                if(count == 0)
                    n->numPossibleColors = 0;
                temp[count] = t[i];
                n->numPossibleColors += 1;
                count++;
                break;
            }
        }
    }
    if(count == 0){
        free(temp);
        return false;
    }
    else{
        temp[count] = '\0';
        strcpy(n->possibleColors, temp);
        free(temp);
        return true;
    }
        
}

// AC-3 revise extension
bool IsRevise(Node** game, Node* node1, Node* node2){
    
    bool revised = false;
    int x1 = node1->x,  y1 = node1->y, x2 = node2->x, y2 = node2->y;
    int state = 0, *stateptr = &state; // 條件5使用
    
    // 0. 若目前格子為單色，則為最終結果
    // 1. 如果相鄰格子已確定為單色，且目前的格子為矩形的四個角，則目前的格子與相鄰格子的顏色相同
    // 2. 若相鄰格子為矩形的四個角，其為單色，則目前的格子與相鄰格子的顏色相同
    // 3. 若目前格子非矩形的四個角且四周皆為單色，則根據四周的格子決定目前格子的顏色(單色)
    // 4. 若相鄰格子的只剩唯一邊未確定，則根據相鄰格子四周的結果決定目前格子"與相鄰格子的顏色相同"或"與相鄰格子的顏色不相同"
    // 5. 若目前格子只剩唯一邊非單色，則目前格子中的可能性為周圍單色格子的聯集
    // 6. 若目前格子為矩形的四個角，且其對角為單色，則該顏色不會出現在目前的格子中
    if(node1->numPossibleColors == 1)
        return revised;
    else if(node2->numPossibleColors == 1 && IsEdge((const Node*)node1)){ 
        node1->possibleColors = (char*)malloc(2 * sizeof(char));
        strcpy(node1->possibleColors, node2->possibleColors);
        node1->numPossibleColors = 1;
        revised = true;
    }
    else if(node2->numPossibleColors == 1 && IsEdge((const Node*)node2)){
        if(node2->isDesignated == false){
            node1->possibleColors = (char*)malloc(2 * sizeof(char));
            strcpy(node1->possibleColors, node2->possibleColors);
            node1->numPossibleColors = 1;
            revised = true;
        }
        else if(IsUniqueEdge((const Node**)game, x2, y2)){
            node1->possibleColors = (char*)malloc(2 * sizeof(char));
            strcpy(node1->possibleColors, node2->possibleColors);
            node1->numPossibleColors = 1;
            revised = true;
        }
    }
    else if(!IsEdge((const Node*)node1) && IsIsolated((const Node**)game, x1, y1)){
        char* temp = (char*)malloc(3 * sizeof(char));
        temp = IsolatedBlock((const Node**)game, x1, y1);
        int len = strlen(temp);
        if(temp != NULL){
            if(len == 1){
                node1->possibleColors = (char*)malloc(2 * sizeof(char));
                strcpy(node1->possibleColors, temp);
                node1->numPossibleColors = 1;
            }
            else{
                node1->possibleColors = (char*)malloc(3 * sizeof(char));
                strcpy(node1->possibleColors, temp);
                node1->numPossibleColors = 2;
            }
            revised = true;
        }
        free(temp);
    }
    else if(node2->numPossibleColors == 1 && !IsEdge((const Node*)node2)){
        int u = IsUnique((const Node**)game, x1, y1, x2, y2);
        if(u == 1){
            node1->possibleColors = (char*)malloc(2 * sizeof(char));
            strcpy(node1->possibleColors, node2->possibleColors);
            node1->numPossibleColors = 1;
            revised = true;
        }
        else if(u == 2 && IsIncluded((const char*) node1->possibleColors, (const char*) node2->possibleColors)){
            RemoveChar(node1->possibleColors, node2->possibleColors);
            node1->numPossibleColors -= 1;
            revised = true;
        }
    }
    else if(!IsEdge((const Node*)node1) && IsSemiIsolated((const Node**)game, x1, y1, stateptr)){
        char* temp = SemiIsolated((const Node**)game, x1, y1, stateptr);
        int len = strlen(temp);
        if(len == 2){ // 字串長度2
            if(IsIncluded((const char*) node1->possibleColors, (const char*) temp)){
                strcpy(node1->possibleColors, temp);
                node1->numPossibleColors = 2;
                revised = true;
            }
            else if(FindIntersection(node1, temp)) // 在findintersection中修改
                revised = true;
        }
        else{ // 字串長度3
            if(IsIncluded((const char*) node1->possibleColors, (const char*) temp)){
                strcpy(node1->possibleColors, temp);
                node1->numPossibleColors = 3;
                revised = true;
            }
            else if(FindIntersection(node1, temp)) // 在findintersection中修改
                revised = true;
        }
        free(temp);
    }
    else if(IsEdge((const Node*)node1)){
        char* temp = EdgeReduced((const Node**)game, x1, y1);
        if(temp != NULL){
            RemoveChar(node1->possibleColors, temp);
            node1->numPossibleColors -= 1;
            revised = true;
        }
    }
    return revised;
}

void findRevisedArc(Node** game, Queue** q, Node* node){
    
    QueueNode* tempnode = (*q)->front;
    int x = node->x, y = node->y;

    if(y > 0 && game[y-1][x].numPossibleColors != 1){
        ARC arc = {&game[y-1][x], &game[y][x]};
        Enqueue(q, arc);
    }
    if(y < (Row - 1) && game[y+1][x].numPossibleColors != 1){
        ARC arc = {&game[y+1][x], &game[y][x]};
        Enqueue(q, arc);
    }
    if(x > 0 && game[y][x-1].numPossibleColors != 1){
        ARC arc = {&game[y][x-1], &game[y][x]};
        Enqueue(q, arc);
    }
    if(x < (Col - 1) && game[y][x+1].numPossibleColors != 1){
        ARC arc = {&game[y][x+1], &game[y][x]};
        Enqueue(q, arc);
    }
}

// AC-3演算法
bool AC3(Node** game) {
    Queue *q = InitializeQueue();
    InitializeConstraintQueue(game, &q);
    while (!IsEmptyQueue(q)) {
        ARC arc = Dequeue(q);
        if (IsRevise(game, arc.node1, arc.node2)) {
            // 如果節點的顏色選項被修改，則將該節點的鄰居加入佇列
            //PrintMap(game);
            findRevisedArc(game, &q, arc.node1); 
        }
    }
    printf("\nAC-3 result:\n");
    PrintMap(game);
    free(q);
    if(!IsSolutionFound((const Node**)game)){
        //PrintReducedResult((const Node**)game);
        return false;
    }
    else{
        printf("\nSolution Found!\n");
        return true;
    }
    
}

// AC-3直接找到最終解
bool IsSolutionFound(const Node** game) {
    for (int i = 0; i < Row; i++) {
        for (int j = 0; j < Col; j++) {
            if (game[i][j].numPossibleColors != 1)
                return false;
        }
    }
    return true;
}

// 輸出簡化結果
void PrintReducedResult(const Node** game){
    for (int i = 0; i < Row; i++) {
        for (int j = 0; j < Col; j++){
            if(game[i][j].numPossibleColors != 1)
                printf("Row %d, Column %d possible result:  %s\n", i, j, game[i][j].possibleColors);
        }
    }
}
