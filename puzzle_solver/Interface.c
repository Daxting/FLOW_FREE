// Interface.c

#include "Interface.h"

// 讀取題目內容
Node** ReadMap(char* fileName, CL** colorLines){

    char buffer[256] = {0};
    char color[30] = {0};
    int row = 0, col = 0, numColors = 0;

    FILE *file = fopen(fileName, "r");
    if(file == NULL){
        printf("Error opening file!\n");
        return NULL;
    }

    // 先讀取題目的行數與列數
    while(fgets(buffer, sizeof(buffer), file) != NULL){
        if(row == 0)
            col = strlen(buffer);
        for(int i = 0; i < col; i += 2){
            bool isfound = false;
            if(buffer[i] != '.'){
                for(int j = 0; j< numColors; j++){
                    if(buffer[i] == color[j]){
                        isfound = true;
                        break;
                    }
                }
                if(!isfound){
                    color[numColors] = buffer[i];
                    numColors++;
                }
            }
        }
        row++;
    }
    Colors = numColors;
    ColorNames = (char*)malloc((numColors + 1)*sizeof(char));
    strcpy(ColorNames, color);
    col /= 2;
    // 將標頭移回文件檔標頭
    rewind(file);

    // 建立2d動態陣列graph
    Node** graph = CreateGraph(row, col);

    // 建立各顏色的起終點資料
    (*colorLines) = CreateColorLines();

    // 逐行讀取題目
    for(int i = 0; i< row; i++){
        fgets(buffer, sizeof(buffer), file);
        for(int j = 0; j < col; j ++){
            int bp = 2*j;
            if(buffer[bp] == '.'){
                graph[i][j].numPossibleColors = numColors;
                graph[i][j].possibleColors = (char*)malloc((numColors + 1) * sizeof(char));
                strcpy(graph[i][j].possibleColors, ColorNames);
            }
            else{
                graph[i][j].numPossibleColors = 1;
                graph[i][j].isDesignated = true;
                graph[i][j].possibleColors = (char*)malloc(2 * sizeof(char));
                graph[i][j].possibleColors[0] = buffer[bp];
                graph[i][j].possibleColors[1] = '\0';
                for(int k = 0; k< Colors; k++){
                    if((*colorLines)[k].start != NULL && (*colorLines)[k].end != NULL)
                        continue;
                    else if((*colorLines)[k].start == NULL){
                        (*colorLines)[k].color = (char*)malloc(2 * sizeof(char));
                        strcpy((*colorLines)[k].color, graph[i][j].possibleColors);
                        (*colorLines)[k].start = &graph[i][j];
                        break;
                    }
                    else if((*colorLines)[k].start != NULL && strcmp((*colorLines)[k].color, graph[i][j].possibleColors) == 0){
                        (*colorLines)[k].end = &graph[i][j];
                        break;
                    }
                }
            }
        }
    }
    fclose(file);
    return graph;
}

// 輸出題目內容
void PrintMap(Node** game){
    
    for(int i = 0; i< Row; i++){
        for(int j = 0; j< Col; j++){
            if(game[i][j].numPossibleColors == 1){
                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), (((int)game[i][j].possibleColors[0] - 65)%16)); // 這行只可以在windows環境中運行
                printf("%s", game[i][j].possibleColors);
            }
            else{
                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7); // 這行只可以在windows環境中運行
                printf(".");
            }
            printf(" ");
        }
        printf("\n");
    }
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
    printf("row: %d\n", Row);
    printf("colomn: %d\n\n", Col);
}

// 輸出各顏色基本資料
void PrintCL(CL* cl){
    
    printf("\nColor Lines information:\n");
    for(int i = 0; i< Colors; i++){
        printf("\nColorName: %s\n", cl[i].color);
        printf("Start point: %d, %d\n", cl[i].start->x, cl[i].start->y);
        printf("End point: %d, %d\n", cl[i].end->x, cl[i].end->y);
        if(cl[i].isConnected)
            printf("Is Connected: Yes\n");
        else
            printf("Is Connected: No\n");
    }
    printf("\n");
}
