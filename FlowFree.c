// main program FlowFree.c

#include "Interface.h"
#include "Engine.h"
#include "AC3.h"
#include "Astar.h"
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include <time.h>

int main(){

    // 記錄開始時間
    clock_t start = clock();

    char* fileName = "./game/10x10_1_game.txt\0";
    bool solution = false;
    // 讀取題目內容
    CL* colorLines = NULL;
    Node** game = ReadMap(fileName, &colorLines);
    // 輸出題目內容
    PrintMap(game);
    //printf("All colors: %s\n", ColorNames);
    //PrintCL(colorLines);

    // AC-3演算法
    solution = AC3(game);
    
    // AC-3直接找到解
    if(solution){
        free(ColorNames);
        FreeCL(colorLines);
        FreeGraph(game);
        return 0;
    }
    // 檢查
    LinesConnectedCheck((const Node**)game, &colorLines);
    LinesReducedCheck(game, &colorLines);
    PrintCL(colorLines);
    // 更新graph中各個格子的狀態
    solution = UpdateGraph(game, (const CL*)colorLines);
    
    // 化減後找到解
    if(solution){
        printf("\nPuzzle has been solved!\n");
        printf("\nAnswer: \n");
        PrintMap(game);
        free(ColorNames);
        FreeCL(colorLines);
        FreeGraph(game);
        return 0;
    }

    // A*演算法
    solution = Astar(game, &colorLines);
    if(!solution){
        PrintReducedResult((const Node**) game);
        printf("\nIt has no solution!\n");
    }
    else{
        printf("\nPuzzle has been solved!\n");
        printf("\nAnswer: \n");
        PrintMap(game);

        // 記錄結束時間
        clock_t end = clock();
        // 計算花費的時間（秒）
        double time_spent = (double)(end - start) / CLOCKS_PER_SEC;
        printf("\nExecution time: %.6f seconds\n", time_spent);

        fileName = "./output/solution.txt";
        FILE *file = fopen(fileName, "w");
        for(int i = 0; i< Row; i++){
            for(int j = 0; j< Col; j++){
                fprintf(file, "%s ", game[i][j].possibleColors);
            }
            fprintf(file, "\n");
        }
    }

    free(ColorNames);
    FreeCL(colorLines);
    FreeGraph(game);

    return 0;
}