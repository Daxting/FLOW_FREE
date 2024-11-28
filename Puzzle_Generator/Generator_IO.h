#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>

typedef struct Position {
   // set the left and top as 0
   int row;
   int col;
   struct Position *prev;
   struct Position *next;
} Position;

// get the input from the user
char** user_input(int *rows_num, int *cols_num) {
   //get the number of rows and columns
   printf("Enter the number of rows and columns: ");
   scanf("%d %d", rows_num, cols_num);
   //declare the board
   char **board = (char **)malloc(*rows_num * sizeof(char *));
   for (int i = 0; i < *rows_num; i++) {
      board[i] = (char *)malloc(*cols_num * sizeof(char));
   }
   //initialize the board
   for (int i = 0; i < *rows_num; i++) {
      for (int j = 0; j < *cols_num; j++) {
         board[i][j] = '.';
      }
   }
   return board;
}

// print the board
void print_board(char **board, int rows_num, int cols_num) {
   printf("\n-----print the board-----\n");
   for (int i = 0; i < rows_num; i++) {
      for (int j = 0; j < cols_num; j++) {
         printf("%c ", board[i][j]);
      }
      printf("\n");
   }
   printf("------end of the board------\n\n");
}

// count the length of the path
int count_length_of_path(Position *p) {
   int count = 0;
   while(p != NULL) {
      count++;
      p = p->next;
   }
   return count;
}

// print the length of each path
void print_length_of_each_path(Position **positions, int colors_num) {
   for (int i = 0; i < colors_num; i++) {
      printf("The length of the path %d (color %c) is %d\n", i, 'A'+i, count_length_of_path(positions[i]));
   }
}

// count the empty cells
int count_empty_cells(char **board, int rows_num, int cols_num) {
   int count = 0;
   for (int i = 0; i < rows_num; i++) {
      for (int j = 0; j < cols_num; j++) {
         if (board[i][j] == '.') {
            count++;
         }
      }
   }
   return count;
}

// print the comprehensive information of the board
void print_board_info(char **board, int rows_num, int cols_num, Position **positions, int colors_num, bool isDetail) {
   
   if(isDetail){
      print_board(board, rows_num, cols_num);
      print_length_of_each_path(positions, colors_num);
   }
   printf("The number of total paths is %d\n", colors_num);
   printf("The number of empty cells is %d\n", count_empty_cells(board, rows_num, cols_num));
   if(colors_num != 0)
      printf("Everage length of the paths is %d\n", (rows_num*cols_num-count_empty_cells(board, rows_num, cols_num))/colors_num);
   else
      printf("Everage length of the paths is 0\n");
   printf("----------------------------------------------\n");
}

// print the path to the output file with only the head and tail of each path
void print_game(char* file_name, char **board, int rows_num, int cols_num, Position **positions) {
   FILE *file = fopen(file_name, "w");
   if (file == NULL) {
      printf("Error: cannot open the file while printting game!\n");
      return;
   }
   for(int i = 0; i < rows_num; i++) {
      for(int j = 0; j < cols_num; j++) {
        // if the cell is '.', print '-'
        if(board[i][j] == '.') {
            if (j != cols_num-1)
               fprintf(file, "- ");
            else
               fprintf(file, "-");
            continue;
        }

        int NumColor = board[i][j] - 'A';
        // determine if the cell is the head or tail of the path
        Position *p = positions[NumColor];
        while(p != NULL) {
            if(p->row == i && p->col == j) {
               if(p->prev == NULL || p->next == NULL) {
                  if(j != cols_num-1)
                     fprintf(file, "%c ", board[i][j]);
                  else
                     fprintf(file, "%c", board[i][j]);
               }
               else {
                  if(j != cols_num-1)
                     fprintf(file, ". ");
                  else
                     fprintf(file, ".");
               }
               break;
            }
            p = p->next;
        }
      }
      if(i != rows_num-1)
         fprintf(file, "\n");
   }
   fclose(file);

}

// print the board to the output file with the whole path
void print_solution(char* solution_name, char* info_name, char **board, int rows_num, int cols_num, Position **positions, int colors_num, double time) {
   FILE *file = fopen(info_name, "w");
   if (file == NULL) {
      printf("Error: cannot open the file while printting solution!\n");
      return;
   }
   for(int i = 0; i < rows_num; i++) {
      for(int j = 0; j < cols_num; j++) {
         if(j != cols_num-1)
            fprintf(file, "%c ", board[i][j]);
         else
            fprintf(file, "%c", board[i][j]);
      }
      if(i != rows_num-1)
         fprintf(file, "\n");
   }

   //print the information of each path
   fprintf(file, "\n\ninformation of each path:\n");
   for(int i = 0; i < colors_num; i++) {
      int count = 0;
      Position *p = positions[i];
      fprintf(file, "Color %c (%d): ", 'A'+i, i);
      while(p != NULL) {
         fprintf(file, "(%d, %d) ", p->row, p->col);
         p = p->next;
         count++;
      }
      fprintf(file, ", Length: %d\n", count);
   }
   fprintf(file, "Execution time for making the puzzle: %f seconds\n", time);
   fclose(file);


   file = fopen(solution_name, "w");
   if (file == NULL) {
      printf("Error: cannot open the file while printting solution!\n");
      return;
   }
   for(int i = 0; i < rows_num; i++) {
      for(int j = 0; j < cols_num; j++) {
         if(j != cols_num-1)
            fprintf(file, "%c ", board[i][j]);
         else
            fprintf(file, "%c", board[i][j]);
      }
      if(i != rows_num-1)
         fprintf(file, "\n");
   }
   fclose(file);
}

// print to the output file
void print_to_file(char* folder_name, char **board, int rows_num, int cols_num, Position **positions, int colors_num, double time) {
   //get the output file name
   printf("Enter the output file name(without file extension): ");
   char output_file_name[100];
   //flush the input buffer
   fflush(stdin);
   //use getlin to get the input
   fgets(output_file_name, 100, stdin);
   // if the output file name is empty, set the default name as "row_numxcol_num"
   if(strlen(output_file_name) == 1 && output_file_name[0] == '\n') {
      sprintf(output_file_name, "%dx%d", rows_num, cols_num);
   }
   else if(output_file_name[strlen(output_file_name)-1] == '\n') {
      output_file_name[strlen(output_file_name)-1] = '\0';
   }
   //add the folder name to the front of the output file name directly
   char output_file_name_with_folder[100];
   strcpy(output_file_name_with_folder, folder_name);
   strcat(output_file_name_with_folder, "/");
   strcat(output_file_name_with_folder, output_file_name);

   //set the game name as the output_file_name_game
   char game_name[100];
   strcpy(game_name, output_file_name_with_folder);
   strcat(game_name, "_game.txt");
   //set the solution name as the output_file_name_solution
   char solution_name[100];
   strcpy(solution_name, output_file_name_with_folder);
   strcat(solution_name, "_solution.txt");
   // set the information of the game as output_file_name_info
   char info_name[100];
   strcpy(info_name, output_file_name_with_folder);
   strcat(info_name, "_info.txt");
   //print the game and the solution to the output files
   print_game(game_name, board, rows_num, cols_num, positions);
   print_solution(solution_name, info_name, board, rows_num, cols_num, positions, colors_num, time);
}
