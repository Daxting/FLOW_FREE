// Engine.c

#include "Engine.h"
#include "Interface.h"


// 建立題目的graph
Node** CreateGraph(int r, int c){
    // 記住全域變數row, col
    Row = r;
    Col = c;
    
    // 建立graph
    Node** graph = (Node**)malloc(Row * sizeof(Node*));
    for(int i = 0; i< Row; i++){
        graph[i] = (Node*)malloc(Col * sizeof(Node));
        for(int j = 0; j< Col; j++){
            graph[i][j].x = j; // x座標
            graph[i][j].y = i; // y座標
            graph[i][j].visited = false; // 是否已被訪問
            graph[i][j].isDesignated = false; // 是否為起終點
            graph[i][j].numPossibleColors = 0; // 可填入顏色數量
            graph[i][j].possibleColors = NULL; // 可填入顏色選項
        }
    }

    return graph;
}

// 建立各顏色的起終點資料
CL* CreateColorLines(){
    
    CL* colorLines = (CL*)malloc(Colors * sizeof(CL));
    for(int i = 0; i< Colors; i++){
        colorLines[i].color = NULL;
        colorLines[i].start = NULL;
        colorLines[i].end = NULL;
        colorLines[i].isConnected = false;
    }

    return colorLines;
}

// 檢查連線情況
bool IsConnected(const Node** game, Node* start, Node* end){
    int preX = start->x, preY = start->y;
    int pastX = -1, pastY = -1;
    int goalX = end->x, goalY = end->y;
    bool failed = false;
    while(!failed){
        //printf("%d, %d\n", preX, preY);
        if(preX == 0){ // 目前位置在最左側
            bool con1 = strcmp(game[preY][preX].possibleColors, game[preY][preX+1].possibleColors) == 0 && !(pastX == (preX+1) && pastY == preY);
            if(preY == 0){
                bool con2 = strcmp(game[preY][preX].possibleColors, game[preY+1][preX].possibleColors) == 0 && !(pastX == preX && pastY == (preY+1));
                pastX = preX;
                pastY = preY;
                if(con1)
                    preX += 1;
                else if(con2)
                    preY += 1;
                else
                    failed = true;
            }
            else if(preY == (Row - 1)){
                bool con2 = strcmp(game[preY][preX].possibleColors, game[preY-1][preX].possibleColors) == 0 && !(pastX == preX && pastY == (preY-1));
                pastX = preX;
                pastY = preY;
                if(con1)
                    preX += 1;
                else if(con2)
                    preY -= 1;
                else
                    failed = true;
            }
            else{
                bool con2 = strcmp(game[preY][preX].possibleColors, game[preY+1][preX].possibleColors) == 0 && !(pastX == preX && pastY == (preY+1));
                bool con3 = strcmp(game[preY][preX].possibleColors, game[preY-1][preX].possibleColors) == 0 && !(pastX == preX && pastY == (preY-1));
                pastX = preX;
                pastY = preY;
                if(con1)
                    preX += 1;
                else if(con2)
                    preY += 1;
                else if(con3)
                    preY -= 1;
                else
                    failed = true;
            }
        }
        else if(preX == (Col - 1)){ // 目前位置在最右側
            bool con1 = strcmp(game[preY][preX].possibleColors, game[preY][preX-1].possibleColors) == 0 && !(pastX == (preX-1) && pastY == preY);
            if(preY == 0){
                bool con2 = strcmp(game[preY][preX].possibleColors, game[preY+1][preX].possibleColors) == 0 && !(pastX == preX && pastY == (preY+1));
                pastX = preX;
                pastY = preY;
                if(con1)
                    preX -= 1;
                else if(con2)
                    preY += 1;
                else
                    failed = true;
            }
            else if(preY == (Row - 1)){
                bool con2 = strcmp(game[preY][preX].possibleColors, game[preY-1][preX].possibleColors) == 0 && !(pastX == preX && pastY == (preY-1));
                pastX = preX;
                pastY = preY;
                if(con1)
                    preX -= 1;
                else if(con2)
                    preY -= 1;
                else
                    failed = true;
            }
            else{
                bool con2 = strcmp(game[preY][preX].possibleColors, game[preY+1][preX].possibleColors) == 0 && !(pastX == preX && pastY == (preY+1));
                bool con3 = strcmp(game[preY][preX].possibleColors, game[preY-1][preX].possibleColors) == 0 && !(pastX == preX && pastY == (preY-1));
                pastX = preX;
                pastY = preY;
                if(con1)
                    preX -= 1;
                else if(con2)
                    preY += 1;
                else if(con3)
                    preY -= 1;
                else
                    failed = true;                
            }
        }
        else{ // 目前位置在中間
            bool con1 = strcmp(game[preY][preX].possibleColors, game[preY][preX+1].possibleColors) == 0 && !(pastX == (preX+1) && pastY == preY);
            bool con2 = strcmp(game[preY][preX].possibleColors, game[preY][preX-1].possibleColors) == 0 && !(pastX == (preX-1) && pastY == preY);
            if(preY == 0){
                bool con3 = strcmp(game[preY][preX].possibleColors, game[preY+1][preX].possibleColors) == 0 && !(pastX == preX && pastY == (preY+1));
                pastX = preX;
                pastY = preY;
                if(con1)
                    preX += 1;
                else if(con2)
                    preX -= 1;
                else if(con3)
                    preY += 1;
                else
                    failed = true;
            }
            else if(preY == (Row - 1)){
                bool con3 = strcmp(game[preY][preX].possibleColors, game[preY-1][preX].possibleColors) == 0 && !(pastX == preX && pastY == (preY-1));
                pastX = preX;
                pastY = preY;
                if(con1)
                    preX += 1;
                else if(con2)
                    preX -= 1;
                else if(con3)
                    preY -= 1;
                else
                    failed = true;
            }
            else{
                bool con3 = strcmp(game[preY][preX].possibleColors, game[preY+1][preX].possibleColors) == 0 && !(pastX == preX && pastY == (preY+1));
                bool con4 = strcmp(game[preY][preX].possibleColors, game[preY-1][preX].possibleColors) == 0 && !(pastX == preX && pastY == (preY-1));
                pastX = preX;
                pastY = preY;
                if(con1)
                    preX += 1;
                else if(con2)
                    preX -= 1;
                else if(con3)
                    preY += 1;
                else if(con4)
                    preY -= 1;
                else
                    failed = true;
            }
        }
        if(preX == goalX && preY == goalY)
            return true;
    }
    return false;
}

// 更新各顏色的連線情況
void LinesConnectedCheck(const Node** game, CL** cl){
    for(int i = 0; i< Colors; i++){
        if(!(*cl)[i].isConnected)
            (*cl)[i].isConnected = IsConnected((const Node**)game, (*cl)[i].start, (*cl)[i].end);
    }
}

// 拓展各顏色連線情況
bool ExpandAdjacent(Node* current, Node** start, Node** game, int* curX, int* curY, int* preX, int* preY){
    // 對於相鄰節點(已確定為同色的相鄰格子)的擴展
    int dx[4] = {0, 0, -1, 1};  // 左, 右, 上, 下
    int dy[4] = {-1, 1, 0, 0};
    for(int i = 0; i < 4; i++) {
        int newX = (*curX) + dx[i];
        int newY = (*curY) + dy[i];
    
        // 邊界檢查
        if(newX >= 0 && newX < Col && newY >= 0 && newY < Row) {
            Node* neighbor = &game[newY][newX];
            // 已確定為同色且有效移動
            if(neighbor->numPossibleColors == 1 && strcmp(current->possibleColors, neighbor->possibleColors) == 0 && !((*preX) == newX && (*preY) == newY)) {
                *start = &game[newY][newX];
                *preX = *curX;
                *preY = *curY;
                *curX = newX;
                *curY = newY;
                return true;
            }
        }
    }
    return false;   
}

// 更新各顏色的連線情況(檢查未連線之顏色)
void LinesReducedCheck(Node** game, CL** cl){
    for(int i = 0; i< Colors; i++){
        if(!(*cl)[i].isConnected){
            // 要先將每一個顏色與起終點相鄰的格子走過
            bool progress = false;
            int curX = (*cl)[i].start->x, curY = (*cl)[i].start->y,*curXptr = &curX, *curYptr = &curY;
            int preX = curX, preY = curY, *preXptr = &preX, *preYptr = &preY;
            do{ 
                progress = false;
                Node* current = &game[curY][curX];
                // 對於相鄰節點(已確定為同色的相鄰格子)的擴展
                progress = ExpandAdjacent(current, &((*cl)[i].start), game, curXptr, curYptr, preXptr, preYptr);                   
            }while(progress);
            
            curX = (*cl)[i].end->x;
            curY = (*cl)[i].end->y;
            preX = curX;
            preY = curY;
            do{ 
                progress = false;
                Node* current = &game[curY][curX];
                // 對於相鄰節點(已確定為同色的相鄰格子)的擴展
                progress = ExpandAdjacent(current, &((*cl)[i].end), game, curXptr, curYptr, preXptr, preYptr);
            }while(progress);
        }        
    }
}

// 釋放graph記憶體
void FreeGraph(Node** graph){
    for(int i = 0; i< Row; i++){
        for(int j = 0; j< Col; j++)
            free(graph[i][j].possibleColors);
        free(graph[i]);
    }
    free(graph);
}

// 釋放CL記憶體
void FreeCL(CL* colorLines){
    for(int i = 0; i< Colors; i++){
        free(colorLines[i].color);
    }
    free(colorLines);
}
