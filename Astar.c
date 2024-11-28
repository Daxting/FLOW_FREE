// Astar.c

#include "Astar.h"

// 更新graph中各個格子的狀態
bool UpdateGraph(Node** game, const CL* cl){

    bool isSolved = true;
    for(int i = 0; i< Row; i++){
        for(int j = 0; j< Col; j++){
            for(int k = 0; k< Colors; k++){
                //printf("row: %d, col: %d, colors: %s\n", i, j, cl[k].color);
                if(game[i][j].numPossibleColors != 1 && cl[k].isConnected && IsIncluded(game[i][j].possibleColors, cl[k].color)){
                    RemoveChar(game[i][j].possibleColors, cl[k].color);
                    game[i][j].numPossibleColors -= 1;
                }
                if(game[i][j].numPossibleColors == 1){
                    game[i][j].isDesignated = true;
                    if(strcmp(game[i][j].possibleColors, cl[k].color) == 0 && !(cl[k].start->x == j && cl[k].start->y == i) && !(cl[k].end->x == j && cl[k].end->y == i))
                        game[i][j].visited = true;
                }
            }
        }
    }
    // 在繞線前的最後檢查
    for(int i = 0; i< Row; i++){
        for(int j = 0; j< Col; j++){
            if(game[i][j].numPossibleColors != 1){
                isSolved = false;
                break;
            }
        }
    }
    if(!isSolved){
        //PrintReducedResult((const Node**) game);
        return false;   
    }
    return true;
}

// 複製graph(中間解)
void CopyGraph(Node** test, const Node** game){
    for(int i = 0; i< Row; i++){
        for(int j = 0; j< Col; j++){
            test[i][j].x = game[i][j].x;
            test[i][j].y = game[i][j].y;
            test[i][j].isDesignated = game[i][j].isDesignated;
            test[i][j].visited = game[i][j].visited;
            test[i][j].numPossibleColors = game[i][j].numPossibleColors;
            int len = strlen(game[i][j].possibleColors);
            test[i][j].possibleColors = (char*)malloc((len+1)*sizeof(char));
            strcpy(test[i][j].possibleColors, game[i][j].possibleColors);
        }
    }
}

// 初始化tree
Path* PathsInitialize(){
    Path* paths = (Path*)malloc(sizeof(Path));
    paths->up = NULL;
    paths->down = NULL;
    paths->left = NULL;
    paths->right = NULL;
    paths->parentNode = NULL;
    paths->color = NULL;
    paths->x = -1;
    paths->y = -1;
    paths->fcost = 0;
    paths->gcost = 0;
}

// 啟發式函數
int Heuristic(const Node* currentNode,const Node* parentNode, const Node* targetNode, const Node** test){
    int cx = currentNode->x, cy = currentNode->y, tx = targetNode->x, ty = targetNode->y;
    int minx = cx, miny = cy, maxx = tx, maxy = ty;
    int mahattanDistance = abs(cx - tx) + abs(cy - ty);
    int obstaclePenalty = 0, edgeDiscount = 1, undesignatedPenalty = 0;

    if(cx > tx){
        minx = tx;
        maxx = cx;
    }
    if(cy > ty){
        miny = ty;
        maxy = cy;
    }
    // int tmpx[4] = {0, 0, 1, -1};
    // int tmpy[4] = {1, -1, 0, 0};
    // for(int i = 0; i< 4; i++){
    //     int serX = cx + tmpx[i], serY = cy + tmpy[i];
    //     if((serX) >= 0 && (serX) < Col && (serY) >= 0 && (serY) < Row && test[serY][serX].numPossibleColors != 1)
    //         undesignatedPenalty += 10;
    // }

    for(int x = minx; x <= maxx; x++){
        for(int y = miny; y <= maxy; y++){
            if(test[y][x].isDesignated && !(x == cx && y == cy) && !(x == tx && y == ty)){
                obstaclePenalty += 5;
            }
        }
    }
    if(parentNode != NULL && (cx == 0 || cx == (Col - 1) || cy == 0 || cy == (Row - 1)) && \
    (parentNode->x == 0 || parentNode->x == (Col - 1) || parentNode->y == 0 || parentNode->y == (Row - 1))) // 邊界新增節點cost較小(維持連線於邊界)
        edgeDiscount = 3;
    return  (mahattanDistance + obstaclePenalty + undesignatedPenalty)/edgeDiscount;
}

// 初始化集合
void makeSet(int **parent, int **rank, int rows, int cols) {
    for(int i = 0; i < rows; i++){
        for(int j = 0; j < cols; j++){
            parent[i][j] = i * cols + j;  // 每個節點指向自己，展開為一維
            rank[i][j] = 0;
        }
    }
}

// 找到節點的根
int find(int **parent, int x, int y, int cols) {
    int id = x * cols + y;  // 將二維座標展開為一維索引
    if(parent[x][y] != id){
        int px = parent[x][y] / cols;  // 找到父節點的座標
        int py = parent[x][y] % cols;
        parent[x][y] = find(parent, px, py, cols);  // 路徑壓縮
    }
    return parent[x][y];
}

// 合併兩個集合
void unionSets(int **parent, int **rank, int x1, int y1, int x2, int y2, int cols) {
    int root1 = find(parent, x1, y1, cols);
    int root2 = find(parent, x2, y2, cols);

    if(root1 != root2){
        int r1x = root1 / cols, r1y = root1 % cols;
        int r2x = root2 / cols, r2y = root2 % cols;

        if(rank[r1x][r1y] > rank[r2x][r2y])
            parent[r2x][r2y] = root1;
        else if(rank[r1x][r1y] < rank[r2x][r2y])
            parent[r1x][r1y] = root2;
        else{
            parent[r2x][r2y] = root1;
            rank[r1x][r1y]++;
        }
    }
}


// 確定新增節點為合法
bool IsPathLegal(Node** test, int x, int y, char* color, int endX, int endY, int preX, int preY, const CL* cl, int curColor){
    int lx = x-1, rx = x+1, uy = y-1, dy = y+1;
    // 1.如果新增節點的((左,左上,上), (左,左下,下), (右,右上,上), (右,右下,下))與其顏色相同則不新增節點
    if(lx >= 0 && uy >= 0 && test[y][lx].numPossibleColors == 1 && test[uy][lx].numPossibleColors == 1 && test[uy][x].numPossibleColors == 1 && \
    strcmp(test[uy][lx].possibleColors, test[y][lx].possibleColors) == 0 && strcmp(test[uy][lx].possibleColors, test[uy][x].possibleColors) == 0 && strcmp(test[uy][lx].possibleColors, color) == 0)
        return false;
    else if(lx >= 0 && dy < Row && test[y][lx].numPossibleColors == 1 && test[dy][lx].numPossibleColors == 1 && test[dy][x].numPossibleColors == 1 &&\
    strcmp(test[dy][lx].possibleColors, test[y][lx].possibleColors) == 0 && strcmp(test[dy][lx].possibleColors, test[dy][x].possibleColors) == 0 && strcmp(test[dy][lx].possibleColors, color) == 0)
        return false;
    else if(rx < Col && uy >= 0 && test[y][rx].numPossibleColors == 1 && test[uy][rx].numPossibleColors == 1 && test[uy][x].numPossibleColors == 1 &&\
    strcmp(test[uy][rx].possibleColors, test[y][rx].possibleColors) == 0 && strcmp(test[uy][rx].possibleColors, test[uy][x].possibleColors) == 0 && strcmp(test[uy][rx].possibleColors, color) == 0)
        return false;
    else if(rx < Col && dy < Row && test[y][rx].numPossibleColors == 1 && test[dy][rx].numPossibleColors == 1 && test[dy][x].numPossibleColors == 1 &&\
    strcmp(test[dy][rx].possibleColors, test[y][rx].possibleColors) == 0 && strcmp(test[dy][rx].possibleColors, test[dy][x].possibleColors) == 0 && strcmp(test[dy][rx].possibleColors, color) == 0)
        return false;
    // 2.如果新增節點的四周有兩個以上的同色節點，且其中之一並非終點，則不新增節點(不包括邊界)
    if(lx >= 0 && uy >= 0 && rx < Col && dy < Row){ // 非邊界
        int tmpx[4] = {0, 0, -1, 1};
        int tmpy[4] = {-1, 1, 0, 0};
        int num = 0;
        for(int i = 0; i< 4; i++){
            int serX = x+tmpx[i], serY = y+tmpy[i];
            if(strcmp(test[serY][serX].possibleColors, color) == 0 && (serY != endY || serX != endX))
                num++;
        }
        if(num >= 2)
            return false;
    }
    // 3.如果新增節點導致該節點周圍的未確定節點被孤立(周圍節點顏色確定且相鄰節點沒有兩點同色)，則不新增節點
    int tmpx[4] = {0, 0, -1, 1}; // 上, 下, 左, 右
    int tmpy[4] = {-1, 1, 0, 0};
    for(int i = 0; i< 4; i++){
        int serX = x+tmpx[i], serY = y+tmpy[i];
        if(serX >= 0 && serX < Col && serY >= 0 && serY < Row && test[serY][serX].numPossibleColors != 1){
            bool islegal = false; // 該節點合法
            char c[5] = "0000\0";
            int cnum = 0;
            for(int j = 0; j< 4; j++){
                int serX2 = serX+tmpx[j], serY2 = serY+tmpy[j];
                if(serX2 >= 0 && serX2 < Col && serY2 >= 0 && serY2 < Row){
                    if(serX2 == x && serY2 == y){ // 目前節點
                        for(int k = 0; k< cnum; k++){
                            if(c[k] == color[0]){
                                islegal = true;
                                break;
                            }
                        }
                        if(cnum == 0 || !islegal){
                            c[cnum] = color[0];
                            cnum++;
                        }
                    }
                    else if(test[serY2][serX2].numPossibleColors != 1){ // 該節點四周仍有未確定點
                        islegal = true;
                        break;
                    }
                    else{
                        for(int k = 0; k< cnum; k++){
                            if(test[serY2][serX2].possibleColors[0] == color[0] && !(serX2 == endX && serY2 == endY)) // 若與目前連線者同色且非目前節點，則必須為終點，否則不新增節點
                                break;
                            if(c[k] == test[serY2][serX2].possibleColors[0]){
                                islegal = true;
                                break;
                            }
                        }
                        if((cnum == 0 || !islegal) && !(test[serY2][serX2].possibleColors[0] == color[0] && !(serX2 == endX && serY2 == endY))){
                            c[cnum] = test[serY2][serX2].possibleColors[0];
                            cnum++;
                        }
                    }
                }
                if(islegal)
                    break;
            }
            if(!islegal)
                return false;
        }
    }
    // 4.如果新增節點與其路徑中產生非法未確認節點，則不新增節點
    for(int i = 0; i< 4; i++){
        int serX = x+tmpx[i], serY = y+tmpy[i], serX2 = x+tmpx[i]*2, serY2 = y+tmpy[i]*2, serX3 = x+tmpx[i]*3, serY3 = y+tmpy[i]*3;
        if(serX3 >= 0 && serX3 < Col && serY3 >= 0 && serY3 < Row &&\
        test[serY3][serX3].numPossibleColors == 1 && test[serY3][serX3].possibleColors[0] == color[0]){ // 距離3格
            if((((serX2+tmpy[i]) >= 0 && (serX2+tmpy[i]) < Col && (serY2+tmpx[i]) >= 0 && (serY2+tmpx[i]) < Row && test[serY2+tmpx[i]][serX2+tmpy[i]].numPossibleColors == 1 && test[serY2+tmpx[i]][serX2+tmpy[i]].possibleColors[0] == color[0] && test[serY+tmpx[i]][serX+tmpy[i]].numPossibleColors == 1 && test[serY+tmpx[i]][serX+tmpy[i]].possibleColors[0] == color[0]) ||\
            ((serX2-tmpy[i]) >= 0 && (serX2-tmpy[i]) < Col && (serY2-tmpx[i]) >= 0 && (serY2-tmpx[i]) < Row && test[serY2-tmpx[i]][serX2-tmpy[i]].numPossibleColors == 1 && test[serY2-tmpx[i]][serX2-tmpy[i]].possibleColors[0] == color[0] && test[serY-tmpx[i]][serX-tmpy[i]].numPossibleColors == 1 && test[serY-tmpx[i]][serX-tmpy[i]].possibleColors[0] == color[0])) &&\
            (test[serY2][serX2].numPossibleColors != 1 && test[serY][serX].numPossibleColors != 1))
                return false;
        }
        else if(serX2 >= 0 && serX2 < Col && serY2 >= 0 && serY2 < Row &&\
        test[serY2][serX2].numPossibleColors == 1 && test[serY2][serX2].possibleColors[0] == color[0]){ // 距離2格
            if((((serX+tmpy[i]) >= 0 && (serX+tmpy[i]) < Col && (serY+tmpx[i]) >= 0 && (serY+tmpx[i]) < Row && test[serY+tmpx[i]][serX+tmpy[i]].numPossibleColors == 1 && test[serY+tmpx[i]][serX+tmpy[i]].possibleColors[0] == color[0]) ||\
            ((serX-tmpy[i]) >= 0 && (serX-tmpy[i]) < Col && (serY-tmpx[i]) >= 0 && (serY-tmpx[i]) < Row && test[serY-tmpx[i]][serX-tmpy[i]].numPossibleColors == 1 && test[serY-tmpx[i]][serX-tmpy[i]].possibleColors[0] == color[0])) &&\
            test[serY][serX].numPossibleColors != 1)
                return false;
        }
    }
    // 5.若新增節點後該節點周圍的節點無法合法拓展，則不新增節點
    for(int i = 0; i< 4; i++){
        int serX = x+tmpx[i], serY = y+tmpy[i];
        if(serX >= 0 && serX < Col && serY >= 0 && serY < Row && test[serY][serX].numPossibleColors != 1){
            int undesignated = 0, same = 0;
            for(int j = 0; j< 4; j++){
                int serX2 = serX+tmpx[j], serY2 = serY+tmpy[j];
                if(serX2 >= 0 && serX2 < Col && serY2 >= 0 && serY2 < Row){
                    if(((serX2 == x && serY2 == y) || strcmp(test[serY2][serX2].possibleColors, cl[curColor].color) == 0) && !(serX2 == endX && serY2 == endY)){
                        same++;
                    }
                    else if(test[serY2][serX2].numPossibleColors != 1)
                        undesignated++;
                    else{
                        for(int k = (curColor+1); k< Colors; k++){
                            if(!cl[k].isConnected){
                                if((serX2 == cl[k].start->x && serY2 == cl[k].start->y) || (serX2 == cl[k].end->x && serY2 == cl[k].end->y)){
                                    undesignated = 2;
                                    break;
                                }
                            }                
                        }
                    }
                }
                if(undesignated == 2)
                    break;
                if(same == 2 && j == 3)
                    return false;
            }
        }
    }
    // 6.如果新增節點使其他顏色無法連線，則不新增節點
    //前提，如果新增節點為終點旁節點，則直接新增
    for(int i = 0; i< 4; i++){
        if(endX == (x+tmpx[i]) && endY == (y+tmpy[i]))
            return true;
    }
    // 動態分配記憶體給 parent 和 rank
    int **parent = (int **)malloc(Row * sizeof(int *));
    int **rank = (int **)malloc(Row * sizeof(int *));
    for(int i = 0; i < Row; i++){
        parent[i] = (int *)malloc(Col * sizeof(int));
        rank[i] = (int *)malloc(Col * sizeof(int));
    }
    makeSet(parent, rank, Row, Col);
    // 遍歷格子，合併相連的有效節點
    for(int i = 0; i < Row; i++){
        for(int j = 0; j < Col; j++){
            if(test[i][j].numPossibleColors != 1 && !(i == y && j == x)){
                for(int k = 0; k < 4; k++){
                    int nx = i + tmpx[k];
                    int ny = j + tmpy[k];
                    if(nx >= 0 && nx < Row && ny >= 0 && ny < Col && test[nx][ny].numPossibleColors != 1 && !(i == y && j == x))
                        unionSets(parent, rank, i, j, nx, ny, Col);
                }
            }
        }
    }
    // 分配集合編號
    int **regionMap = (int **)malloc(Row * sizeof(int *));
    for(int i = 0; i < Row; i++){
        regionMap[i] = (int *)malloc(Col * sizeof(int));
        memset(regionMap[i], -1, Col * sizeof(int));
    }
    int regionCount = 0;
    int *regionId = (int *)calloc(Row * Col, sizeof(int));  // 用於記錄集合代表的編號
    for(int i = 0; i < Row; i++){
        for(int j = 0; j < Col; j++){
            if(test[i][j].numPossibleColors != 1 && !(i == y && j == x)){
                int root = find(parent, i, j, Col);
                if(regionId[root] == 0){
                    regionCount++;
                    regionId[root] = regionCount;  // 分配新編號
                }
                regionMap[i][j] = regionId[root];
            }
        }
    }
    // 輸出結果
    // printf("Region map:\n");
    // for(int i = 0; i < Row; i++){
    //     for(int j = 0; j < Col; j++){
    //         if(test[i][j].numPossibleColors != 1 && !(i == y && j == x))
    //             printf("%d ", regionMap[i][j]);
    //         else
    //             printf("0 ");
    //     }
    //     printf("\n");
    // }
    for(int i = curColor; i< Colors; i++){
        if(!cl[i].isConnected){
            bool islegal = false;
            if(i == curColor){
                for(int j = 0; j< 4; j++){
                    int sX = x+tmpx[j], sY = y+tmpy[j];
                    if(sX >= 0 && sX < Col && sY >= 0 && sY < Row && test[sY][sX].numPossibleColors != 1){
                        for(int k = 0; k< 4; k++){
                            int eX = endX+tmpx[k], eY = endY+tmpy[k];
                            if(eX >= 0 && eX < Col && eY >= 0 && eY < Row && test[eY][eX].numPossibleColors != 1){
                                if(regionMap[sY][sX] == regionMap[eY][eX]){
                                    islegal = true;
                                    break;
                                }
                            }
                        }
                        if(islegal)
                            break;
                    }
                }
            }
            else{
                for(int j = 0; j< 4; j++){
                    int sX = cl[i].start->x+tmpx[j], sY = cl[i].start->y+tmpy[j];
                    if(sX >= 0 && sX < Col && sY >= 0 && sY < Row && test[sY][sX].numPossibleColors != 1){
                        for(int k = 0; k< 4; k++){
                            int eX = cl[i].end->x+tmpx[k], eY = cl[i].end->y+tmpy[k];
                            if(eX >= 0 && eX < Col && eY >= 0 && eY < Row && test[eY][eX].numPossibleColors != 1){
                                if(regionMap[sY][sX] == regionMap[eY][eX]){
                                    islegal = true;
                                    break;
                                }
                            }
                        }
                        if(islegal)
                            break;
                    }
                }
            }
            if(!islegal){
                // 釋放記憶體
                for (int i = 0; i < Row; i++){
                    free(parent[i]);
                    free(rank[i]);
                    free(regionMap[i]);
                }
                free(parent);
                free(rank);
                free(regionMap);
                free(regionId);
                return false;
            }
        }
    }

    // 釋放記憶體
    for (int i = 0; i < Row; i++){
        free(parent[i]);
        free(rank[i]);
        free(regionMap[i]);
    }
    free(parent);
    free(rank);
    free(regionMap);
    free(regionId);
    return true;
}

// 新增TNode
void AddTNode(Path** current, int direction, int hcost){

    int curX = (*current)->x, curY = (*current)->y;
    Path* newPath = (Path*)malloc(sizeof(Path));
    newPath->up = NULL;
    newPath->down = NULL;
    newPath->left = NULL;
    newPath->right = NULL;
    newPath->color = (char*)malloc(2*sizeof(char));
    strcpy(newPath->color, (*current)->color);
    newPath->gcost = (*current)->gcost + 1;
    newPath->fcost =  newPath->gcost + hcost;
    
    newPath->parentNode = (*current);
    switch(direction){
        case 0: // 左
            newPath->x = (curX - 1);
            newPath->y = curY;
            (*current)->left = newPath;
            break;
        case 1: // 右
            newPath->x = (curX + 1);
            newPath->y = curY;
            (*current)->right = newPath;
            break;
        case 2: // 上
            newPath->x = curX;
            newPath->y = (curY - 1);
            (*current)->up = newPath;
            break;
        case 3: // 下
            newPath->x = curX;
            newPath->y = (curY + 1);
            (*current)->down = newPath;
            break;
        default:
            break;
    }
}

// 更新test
void UpdateTest(Node** test, const Path* current, int x, int y){
    test[y][x].visited = true;
    test[y][x].numPossibleColors = 1;
    strcpy(test[y][x].possibleColors, current->color);
}

// 復原test
void RestoreTest(const Node** game, Node** test, int x, int y){
    test[y][x].visited = game[y][x].visited;
    test[y][x].numPossibleColors = game[y][x].numPossibleColors;
    int len = strlen(game[y][x].possibleColors);
    free(test[y][x].possibleColors);
    test[y][x].possibleColors = (char*)malloc((len+1)*sizeof(char));
    strcpy(test[y][x].possibleColors, game[y][x].possibleColors);
}

// 儲存最終解
void StoreFinalAns(Node** game, const Node** test){
    for(int i = 0; i < Row; i++){
        for(int j = 0; j < Col; j++){
            if(game[i][j].numPossibleColors != 1){
                game[i][j].numPossibleColors = test[i][j].numPossibleColors;
                strcpy(game[i][j].possibleColors, test[i][j].possibleColors);
            }
        }
    }
}

// 刪除非法路徑
void FreePath(Path* p){
    free(p->color);
    free(p);
}

// 尋找最底層節點
Path* FindPathsEnd(Path* p){
    Path* endPath = p;
    while(endPath->left != NULL || endPath->right != NULL || endPath->up != NULL || endPath->down != NULL){
        if(endPath->left != NULL)
            endPath = endPath->left;
        else if(endPath->right != NULL)
            endPath = endPath->right;
        else if(endPath->up != NULL)
            endPath = endPath->up;
        else if(endPath->down != NULL)
            endPath = endPath->down;
    }
    return endPath;
}

// 釋放tree記憶體
void FreePaths(Path* p){
    Path* cur = FindPathsEnd(p);
    while(cur->parentNode != NULL || cur->down != NULL){
        Path* temp = cur;
        cur = cur->parentNode;
        if(cur->left == temp)
            cur->left = NULL;
        else if(cur->right == temp)
            cur->right = NULL;
        else if(cur->up == temp)
            cur->up = NULL;
        else if(cur->down == temp)
            cur->down = NULL;
        else
            printf("Error in delete Path!\n");

        FreePath(temp);
        cur = FindPathsEnd(cur);
    }
}

// Astar演算法
bool Astar(Node** game, CL** cl){
    // 先複製flowfree中間解
    Node** test = CreateGraph(Row, Col);
    CopyGraph(test, (const Node**) game);
    
    // 尋找初始Path(起始搜尋點)------可以改善
    int curColor = 0;
    Path* paths = PathsInitialize();
    for(int i = 0; i< Colors; i++){
        if(!(*cl)[i].isConnected){
            paths->x = (*cl)[i].start->x;
            paths->y = (*cl)[i].start->y;
            paths->color = (char*)malloc(2*sizeof(char));
            strcpy(paths->color, (*cl)[i].color);
            paths->fcost  = Heuristic((const Node*) (*cl)[i].start, NULL, (const Node*) (*cl)[i].end, (const Node**) test); // 此時沒有gcost
            test[paths->y][paths->x].visited = true;
            curColor = i;
            break;
        }
    }
    Path* current = NULL;
    // 開始尋找解
    int count = 0;
    while(!IsSolutionFound((const Node**) test)){
        // 初始化current
        if(current == NULL)
            current = paths;
        
        bool isfinish = false;
        int endX = (*cl)[curColor].end->x, endY = (*cl)[curColor].end->y, curX = current->x, curY = current->y, preX = curX, preY = curY;
        
        // 確認是否到達該顏色的終點, 若該顏色到達終點, 要尋找新的顏色進行搜尋
        if (curX == endX && curY == endY)
            isfinish = true;
        // 若該顏色到達終點, 要尋找新的顏色進行搜尋
        bool hasSol = false;
        if(isfinish && curColor < (Colors-1)){
            for(int i = (curColor+1); i< Colors; i++){
                if(!(*cl)[i].isConnected){
                    Path* newPath = (Path*)malloc(sizeof(Path));
                    newPath->up = NULL;
                    newPath->down = NULL;
                    newPath->left = NULL;
                    newPath->right = NULL;
                    newPath->color = (char*)malloc(2*sizeof(char));
                    strcpy(newPath->color, (*cl)[i].color);
                    newPath->fcost = Heuristic((const Node*) (*cl)[i].start, NULL, (const Node*) (*cl)[i].end, (const Node**)test); // 此時沒有gcost
                    newPath->gcost = 0;
                    newPath->x = (*cl)[i].start->x;
                    newPath->y = (*cl)[i].start->y;
                    newPath->parentNode = current;
                    current->left = newPath;
                    current = current->left;
                    test[current->y][current->x].visited = true;
                    endX = (*cl)[i].end->x;
                    endY = (*cl)[i].end->y;
                    
                    curX = current->x;
                    curY = current->y;
                    preX = curX;
                    preY = curY;
                    curColor = i;
                    isfinish = false;
                    break;
                }
                else if(i == (Colors - 1))
                    hasSol = true;
            }
        }
        if((isfinish && curColor == (Colors-1)) || hasSol) // 最後一個點
            continue;
        // 是否有路徑來源
        if(current->parentNode != NULL){
            preX = current->parentNode->x;
            preY = current->parentNode->y;
        }
        // 對於相鄰節點的擴展, 優先級: 1. 已確定為同色的相鄰格子 2. 未被訪問的格子 && 此移動為有效
        bool isUnique = false;
        for(int search = 0; search< 2; search++){
            int dx[4] = {-1, 1, 0, 0};  // 左, 右, 上, 下
            int dy[4] = {0, 0, -1, 1};
            for(int i = 0; i < 4; i++) {
                int newX = curX + dx[i];
                int newY = curY + dy[i];
                
                // 邊界檢查
                if(newX >= 0 && newX < Col && newY >= 0 && newY < Row) {
                    Node* neighbor = &test[newY][newX];
                    // 已確定為同色且有效移動
                    if(search == 0 && neighbor->numPossibleColors == 1 && !neighbor->visited && strcmp(current->color, neighbor->possibleColors) == 0 && !(preX == newX && preY == newY)){
                        AddTNode(&current, i, 0);
                        isUnique = true;
                        break;
                    }
                    else if(search == 1){
                        // 處理鄰居節點
                        if(!(neighbor->numPossibleColors == 1) && !neighbor->visited && IsIncluded((const char*) neighbor->possibleColors, (const char* const) current->color) && \
                        !(preX == newX && preY == newY) && IsPathLegal(test, newX, newY, current->color, endX, endY, preX, preY, (*cl), curColor)) {    //
                            int newHcost = Heuristic((const Node*) neighbor,(const Node*) &test[curY][curX], (const Node*) &test[endY][endX], (const Node**) test);
                            AddTNode(&current, i, newHcost);    
                        }
                    }
                }   
            }
            if(isUnique)
                break;
        }
        // 尋找下一個搜索點  p.s.要修改test中節點 // 期望能更改成透過fcost搜尋
        bool isFound = false;
        while(!isFound){
            if(current->left != NULL || current->right != NULL || current->up != NULL || current->down != NULL){
                int newfscore = -1, nextpath = 0;
                if(current->left != NULL){
                    newfscore = current->left->fcost;
                    nextpath = 1;
                }
                if(current->right != NULL &&(newfscore == -1 || newfscore > current->right->fcost)){
                    newfscore = current->right->fcost;
                    nextpath = 2;
                }
                if(current->up != NULL &&(newfscore == -1 || newfscore > current->up->fcost)){
                    newfscore = current->up->fcost;
                    nextpath = 3;
                }
                if(current->down != NULL &&(newfscore == -1 || newfscore > current->down->fcost)){
                    newfscore = current->down->fcost;
                    nextpath = 4;
                }
                switch(nextpath){
                    case 1:
                        UpdateTest(test, (const Path*) current, (curX-1), curY);
                        current = current->left;
                        isFound = true;
                        break;
                    case 2:
                        UpdateTest(test, (const Path*) current, (curX+1), curY);
                        current = current->right;
                        isFound = true;
                        break;
                    case 3:
                        UpdateTest(test, (const Path*) current, curX, (curY-1));
                        current = current->up;
                        isFound = true;
                        break;
                    case 4:
                        UpdateTest(test, (const Path*) current, curX, (curY+1));
                        current = current->down;
                        isFound = true;
                        break;
                    default:
                        printf("\nNew Path not found!\n");
                }
            }
            if(!isFound || (current->y == (*cl)[Colors - 1].end->y && current->x == (*cl)[Colors - 1].end->x && !IsSolutionFound((const Node**) test))){
                // 死路->無解->回推路徑(刪除錯誤路徑節點, 使current換到其他direction) //要透過game將test中節點復原
                isFound = false;
                Path* failPath = current;
                // 復原test狀態
                RestoreTest((const Node**) game, test, current->x, current->y);
                // 將目前節點回推至父節點, 並將該節點刪除
                if(current->color[0] != current->parentNode->color[0])
                    curColor -= 1;
                current = current->parentNode;
                curX = current->x;
                curY = current->y;
                if(current->left == failPath)
                    current->left = NULL;
                else if(current->right == failPath)
                    current->right = NULL;
                else if(current->up == failPath)
                    current->up = NULL;
                else if(current->down == failPath)
                    current->down = NULL;
                else{
                    printf("Error in delete Path!\n");
                }
                FreePath(failPath);
            }
        }
        //count++;
        //if(count %100000 == 1)
        //PrintMap(test);
    }
    // 紀錄最終解 // 透過test修改game
    StoreFinalAns(game, (const Node**)test);

    // 釋放paths記憶體
    FreePaths(paths);
    free(paths);

    // 釋放test記憶體
    FreeGraph(test);
}
