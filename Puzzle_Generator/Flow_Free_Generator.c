#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "Generator_IO.h"
#define MAX_COLORS_BUILDING 150
#define MAX_COLORS_FILLING 62 // out of 62 colors may cause segmentation fault
#define MAX_COLORS_PRINTING 62 // out of 62 colors may cause printing error
#define Max_Lengh_Path_Ratio 1 // the ratio of the maximum Length of the path to the Length of the longer side of the board
int longest_length_path = 0;


// determine if the board is full
bool is_board_full(char **board, int rows_num, int cols_num) {
   for (int i = 0; i < rows_num; i++) {
      for (int j = 0; j < cols_num; j++) {
         if (board[i][j] == '.') {
            return false;
         }
      }
   }
   return true;
}

// delete the empty path
void delete_empty_path(char **board, int rows_num, int cols_num, Position ***positions, int *colors_num) {
   // reallocate the memory to remove the deleted path
   Position **new_positions = NULL;
   int new_colors_num = 0;
   for(int i=0;i<*colors_num;i++){
      if((*positions)[i] != NULL) {
         new_colors_num++;
         new_positions = (Position **)realloc(new_positions, new_colors_num * sizeof(Position *));
         new_positions[new_colors_num-1] = (*positions)[i];
      }
   }

   // clear the board
   for (int i = 0; i < rows_num; i++) {
      for (int j = 0; j < cols_num; j++) {
         board[i][j] = '.';
      }
   }

   // remark the color of the path
   for(int i=0;i<new_colors_num;i++){
      Position *p = new_positions[i];
      char color = 'A' + i;
      while(p != NULL) {
         board[p->row][p->col] = color;
         p = p->next;
      }
   }

   free(*positions);
   *positions = new_positions;
   *colors_num = new_colors_num;
}

// get the random position
void get_random_position(char **board, int rows_num, int cols_num, int *row, int *col) {
   do {
      *row = rand() % rows_num;
      *col = rand() % cols_num;
   } while (board[*row][*col] != '.');
}

// get the random direction
void get_random_direction_pair(int direction[4]) {
   // get the random direction in the range of 0 to 3, and the direction can not be repeated  
   // record the direction that has been used
   bool used[4] = { false, false, false, false };
   int count = 0;
   while(count < 4) {
      int dir = rand() % 4;
      if (used[dir] == false) {
         direction[count++] = dir;
         used[dir] = true;
      }
   }
}

// flip the position sequence of the path
void flip_path(Position **positions, int NumColor) {
   Position *p = positions[NumColor]->next;
   Position *temp = positions[NumColor];
   while(p != NULL) {   
      temp->next = temp->prev;
      temp->prev = p;
      temp = p;
      p = p->next;
   }
   positions[NumColor] = temp;
   positions[NumColor]->next = positions[NumColor]->prev;
   positions[NumColor]->prev = NULL;
}

// extend the tail of the path
void extend_the_tail_of_path(char **board, int rows_num, int cols_num, Position **positions, int NumColor) {
   // get the tail of the path
   Position *p = positions[NumColor];
   // extend the tail of the path as long as possible
   // the tail of the path can try to extend to the randon adjacent cell of the same color,
   // but the tail of the path cannot be extended to the any adjacent cell of the same color or out of the board,
   // if the four adjacent cells are not available, the tail of the path will be terminated.
   bool isTerminated = false;
   while (!isTerminated) {
      isTerminated = true;
      // get the random direction in the range of 0 to 3, and the direction can not be repeated  
      // 0: up, 1: down, 2: left, 3: right
      int directions[4];
      get_random_direction_pair(directions);
      // check the four adjacent cells

      for (int i = 0; i < 4; i++) {
         while(p->next != NULL) {
            p = p->next;
         }
         int dir = directions[i];
         int newRow = p->row, newCol = p->col;
         switch (dir) {
            case 0: 
               newRow--; 
               break;// up
            case 1: 
               newRow++; 
               break;// down
            case 2: 
               newCol--; 
               break;// left
            case 3: 
               newCol++; 
               break;// right
         }
         //printf("dir: %d, row: %d, col: %d\n", dir, newRow, newCol);
         // check if the new position is invalid or the cell is not empty
         if (newRow < 0 || newRow >= rows_num || newCol < 0 || newCol >= cols_num || board[newRow][newCol] != '.') {
            continue;
         }
         // check if the new position is adjacent to the same color
         p = positions[NumColor];
         while(p->next != NULL) {
            if(newRow == p->row+1 && newCol == p->col)// down
               break;
            else if(newRow == p->row-1 && newCol == p->col)// up
               break;
            else if(newRow == p->row && newCol == p->col+1)// right
               break;
            else if(newRow == p->row && newCol == p->col-1)// left
               break;
            else
               p = p->next;
         }
         if(p->next != NULL) { // the new position is adjacent to the same color
            continue;
         }
         else{
            // memory allocation
            Position *position = (Position *)malloc(sizeof(Position));
            position->row = newRow;
            position->col = newCol;
            position->next = NULL;
            position->prev = p;
            p->next = position;
            // set the color
            char color = 'A' + NumColor;
            board[newRow][newCol] = color;
            isTerminated = false;
            if(count_length_of_path(positions[NumColor]) == longest_length_path) {
               isTerminated = true;
            }
            //printf("(%d, %d) ", newRow, newCol);
            break;
         }
      } 
   }
   //printf("\n");
}

// build the path
void build_path(char **board, int rows_num, int cols_num, Position **positions, int NumColor) {
   // get the random position   
   int row, col;
   get_random_position(board, rows_num, cols_num, &row, &col);
   // memory allocation
   Position *position = (Position *)malloc(sizeof(Position));
   position->row = row;
   position->col = col;
   position->prev = NULL;
   position->next = NULL;
   positions[NumColor] = position;
   // set the color
   char color = 'A' + NumColor;
   board[row][col] = color;
   // extend the tail of the path as long as possible
   extend_the_tail_of_path(board, rows_num, cols_num, positions, NumColor);
}

// delete the path that is not long enough to reach three cells
void delete_short_path(int NumLimit, char **board, int rows_num, int cols_num, Position ***positions, int *colors_num) {
   for(int i=0;i<*colors_num;i++){
      if(count_length_of_path((*positions)[i]) < NumLimit) {
         Position *p = (*positions)[i];
         while(p != NULL) {
            Position *temp = p;
            board[p->row][p->col] = '.';
            p = p->next;
            free(temp);
         }
         (*positions)[i] = NULL;
      }
   }
   // reallocate the memory to remove the deleted path
   delete_empty_path(board, rows_num, cols_num, positions, colors_num);
}

// fill the empty cells with random colors and extend it by occupying other adjacent cells
void fill_empty_cells(int NumLimit, char **board, int rows_num, int cols_num, Position ***positions, int *colors_num, int NumColor) {
   // get the random position   
   int row, col;
   get_random_position(board, rows_num, cols_num, &row, &col);
   // memory allocation
   Position *position = (Position *)malloc(sizeof(Position));
   position->row = row;
   position->col = col;
   position->prev = NULL;
   position->next = NULL;
   (*positions)[NumColor] = position;
   // set the color
   char color = 'A' + NumColor;
   board[row][col] = color;
   // extend the double end of the path as long as possible
   extend_the_tail_of_path(board, rows_num, cols_num, *positions, NumColor);
   flip_path(*positions, NumColor);
   extend_the_tail_of_path(board, rows_num, cols_num, *positions, NumColor);
   // if the path is not long enough to reach three cells, occupy the adjacent cells
   // choose the shortest path adjacent to the cell to delete
   if(count_length_of_path((*positions)[NumColor]) > NumLimit) {
      return;
   }
   // get the shortest path
   // collect the valid adjacent cells position of the path
   int row_delete = -1, col_delete = -1;
   int min_length = 1000000;
   Position *p = (*positions)[NumColor];
   while(p!=NULL){
      int row = p->row;
      int col = p->col;
      // check the four adjacent cells
      for(int i=0;i<4;i++){
         // get the direction
         int directions[4];
         get_random_direction_pair(directions);
         int newRow = row, newCol = col;
         switch(directions[i]){
            case 0: newRow--; break;// up
            case 1: newRow++; break;// down
            case 2: newCol--; break;// left
            case 3: newCol++; break;// right
         }
         // check if the new position is invalid or the cell is not empty
         if(newRow < 0 || newRow >= rows_num || newCol < 0 || newCol >= cols_num || board[newRow][newCol] == '.' || board[newRow][newCol] == color) {
            continue;
         }
         if(count_length_of_path((*positions)[board[newRow][newCol]-'A']) < min_length) {
            min_length = count_length_of_path((*positions)[board[newRow][newCol]-'A']);
            row_delete = newRow;
            col_delete = newCol;
            if(rand()%2) { // add the randomness to jump out of the infinite loop
               break;
            }
         }
      }
      p = p->next;
   }
   // delete the shortest path adjacent to the cell
   if(row_delete != -1 && col_delete != -1) {
      p = (*positions)[board[row_delete][col_delete]-'A'];
      (*positions)[board[row_delete][col_delete]-'A'] = NULL;
      while(p != NULL) {
         Position *temp = p;
         board[p->row][p->col] = '.';
         p = p->next;
         free(temp);
      }
      // delete the empty path
      delete_empty_path(board, rows_num, cols_num, positions, colors_num);
      NumColor--;
      // extend the double end of the path as long as possible
      extend_the_tail_of_path(board, rows_num, cols_num, *positions, NumColor);
      flip_path(*positions, NumColor);
      extend_the_tail_of_path(board, rows_num, cols_num, *positions, NumColor);
   }
}

bool is_adjacent_to_space(char **board, int rows_num, int cols_num, int row, int col) {
   if(row-1 >= 0 && board[row-1][col] == '.')
      return true;
   if(row+1 < rows_num && board[row+1][col] == '.')
      return true;
   if(col-1 >= 0 && board[row][col-1] == '.')
      return true;
   if(col+1 < cols_num && board[row][col+1] == '.')
      return true;
   return false;
}

// seperate the path into two paths if the path is long enough, and any space is near to the cut point
void seperate_long_path(int NumLimit, char **board, int rows_num, int cols_num, Position ***positions, int *colors_num){
   //randomly choose a path
   int NumColor = rand()%*colors_num;
   const int fix_num=NumColor;
   bool isCut = false;
   do{
      int current_total_length = count_length_of_path((*positions)[NumColor]);
      while(isCut || current_total_length <= NumLimit) {
         isCut = false;
         NumColor++;
         if(NumColor == *colors_num)
            NumColor = 0;
         if(NumColor == fix_num)
            return;
         current_total_length = count_length_of_path((*positions)[NumColor]);
      }
      // search from middle to two ends
      int pass_length = current_total_length/2;
      // first minus, then plus
      int distance_to_middle = 0;
      Position *observation = (*positions)[NumColor];
      for(int i=0;i<pass_length-distance_to_middle;i++){
         observation = observation->next;
      }
      isCut = true;
      while(observation!=NULL && (pass_length < current_total_length-NumLimit || pass_length > NumLimit)) {
         if(is_adjacent_to_space(board, rows_num, cols_num, observation->row, observation->col)) {
            Position* p = observation;
            // count the length of two ends to the point p
            int front_length = 1, back_length = 1;
            Position *temp = (*positions)[NumColor];
            while(temp != p) {
               front_length++;
               temp = temp->next;
            }
            while(p->next != NULL) {
               p = p->next;
               back_length++;
            }
            // Position reallocation+1
            (*colors_num)++;
            *positions = (Position **)realloc(*positions, (*colors_num) * sizeof(Position *));

            if(front_length > back_length) {
               (*positions)[(*colors_num)-1] = observation;
               if(observation->prev!=NULL)
                  observation->prev->next = NULL;
               observation->prev = NULL;
               flip_path(*positions, *colors_num-1);
            }
            else {
               (*positions)[*colors_num-1] = (*positions)[NumColor];
               (*positions)[NumColor] = observation->next;
               if (observation->next != NULL)
                  observation->next->prev = NULL;
               observation->next = NULL;
            }
            // set the color
            char color = 'A' + *colors_num-1;
            p = (*positions)[*colors_num-1];
            while(p != NULL) {
               board[p->row][p->col] = color;
               p = p->next;
            }
            // extend the tail of the path as long as possible
            extend_the_tail_of_path(board, rows_num, cols_num, *positions, (*colors_num)-1);
            delete_short_path(NumLimit, board, rows_num, cols_num, positions, colors_num);
            isCut = false;
            break;
         }
         // if distance_to_middle is odd, then pass_length = pass_length - distance_to_middle
         // if distance_to_middle is even, then pass_length = pass_length + distance_to_middle
         distance_to_middle++;
         if(distance_to_middle%2 == 0){
            pass_length = pass_length + distance_to_middle;
            for(int i=0;i<distance_to_middle;i++){
               observation = observation->next;
            }
         }
         else{
            pass_length = pass_length - distance_to_middle;
            for(int i=0;i<distance_to_middle;i++){
               observation = observation->prev;
            }
         }
      }
   }while(*colors_num < MAX_COLORS_FILLING);


}

// determine if the cell is adjacent to any end of the paths
bool is_adjacent_to_end_of_path(Position **positions, int colors_num, int NumColor, int row, int col, int *rows_adj, int *cols_adj) {
   for(int i=0;i<colors_num;i++){
      //printf("is_adjacent_to_end_of_path?: %d\n", i);

      if(i == NumColor) {
         continue;
      }
      // check the head of the other paths
      Position *p = positions[i];
      if(p == NULL) {
         continue;
      }
      if(row == p->row+1 && col == p->col) {// down
         *rows_adj = p->row;
         *cols_adj = p->col;
         return true;
      }
      else if(row == p->row-1 && col == p->col) {// up
         *rows_adj = p->row;
         *cols_adj = p->col;
         return true;
      }
      else if(row == p->row && col == p->col+1) {// right
         *rows_adj = p->row;
         *cols_adj = p->col;
         return true;
      }
      else if(row == p->row && col == p->col-1) {// left
         *rows_adj = p->row;
         *cols_adj = p->col;
         return true;
      }
      // check the tail of the other paths
      while(p->next != NULL) {
         p = p->next;
      }
      if(row == p->row+1 && col == p->col) {// down
         *rows_adj = p->row;
         *cols_adj = p->col;
         return true;
      }
      else if(row == p->row-1 && col == p->col) {// up
         *rows_adj = p->row;
         *cols_adj = p->col;
         return true;
      }
      else if(row == p->row && col == p->col+1) {// right
         *rows_adj = p->row;
         *cols_adj = p->col;
         return true;
      }
      else if(row == p->row && col == p->col-1) {// left
         *rows_adj = p->row;
         *cols_adj = p->col;
         return true;
      }
   }
   return false;
}

// determine if the path are adjacent to any point of the given path except of the tail
bool is_adjacent_to_path(Position **positions, int NumColor, int row, int col, int isHead) {
   // while isHead is true, the tail of the given numbered path and the position of the given row and col are not taken into account
   Position *p = positions[NumColor];
   while(p != NULL) {
      if(isHead && p->next == NULL) {
         p = p->next;
         continue;
      }
      if(row == p->row+1 && col == p->col) {// down
         return true;
      }
      else if(row == p->row-1 && col == p->col) {// up
         return true;
      }
      else if(row == p->row && col == p->col+1) {// right
         return true;
      }
      else if(row == p->row && col == p->col-1) {// left
         return true;
      }
      p = p->next;
   }
   return false;
} 

// if the head and tail of the paths are adjacent and their path are not adjacent at any point, connect them
void connect_path(char **board, int rows_num, int cols_num, Position ***positions, int *colors_num) {
   // check from the highest color to the lowest color
   // if the end of the path is adjacent to the end of the other path,
   // and the path are not adjacent at any point, connect the two paths
   for(int i = 0; i < *colors_num; i++) {
      // check the head of the path
      Position *p = (*positions)[i];
      int row_adj = -1, col_adj = -1;
      bool is_adj = false;
      if(is_adjacent_to_end_of_path(*positions, *colors_num, i, p->row, p->col, &row_adj, &col_adj)) {
         // check if the path are not adjacent at any point
         // get the color num of the adjacent path
         int NumAdjColor = board[row_adj][col_adj] - 'A';

         // printf("AdjColor: %c\n", board[row_adj][col_adj]);
         // printf("NumAdjColor: %d\n", NumAdjColor);
         // printf("row_adj: %d, col_adj: %d\n", row_adj, col_adj);

         if(((*positions)[NumAdjColor])->row == row_adj && ((*positions)[NumAdjColor])->col == col_adj) {
            flip_path(*positions, NumAdjColor);
         }
         while(p != NULL) {
            if(is_adjacent_to_path(*positions, NumAdjColor, p->row, p->col, p->prev == NULL)) {
               is_adj = true;
               break;
            }
            p = p->next;
         }
         if(is_adj == false) {
            // connect the two paths
            p = (*positions)[i];
            Position *q = (*positions)[NumAdjColor];
            while(q->next != NULL) {
               q = q->next;
            }
            q->next = p;
            p->prev = q;
            (*positions)[i] = NULL;
            // reallocate the memory to remove the deleted path
            delete_empty_path(board, rows_num, cols_num, positions, colors_num);
            i--;
            continue;
         }

      }
      if((*positions)[i] == NULL || (*positions)[i]->next == NULL) {
         continue;
      }
      // check the tail of the path
      flip_path(*positions, i);
      p = (*positions)[i];
      is_adj = false;
      if(is_adjacent_to_end_of_path(*positions, *colors_num, i, p->row, p->col, &row_adj, &col_adj)) {
         // check if the path are not adjacent at any point
         // get the color num of the adjacent path
         int NumAdjColor = board[row_adj][col_adj] - 'A';
         
         // printf("AdjColor: %c\n", board[row_adj][col_adj]);
         // printf("NumAdjColor: %d\n", NumAdjColor);
         // printf("row_adj: %d, col_adj: %d\n", row_adj, col_adj);


         if(((*positions)[NumAdjColor])->row == row_adj && ((*positions)[NumAdjColor])->col == col_adj) {
            flip_path(*positions, NumAdjColor);
         }
         while(p != NULL) {
            if(is_adjacent_to_path(*positions, NumAdjColor, p->row, p->col, p->prev == NULL)) {
               is_adj = true;
               break;
            }
            p = p->next;
         }
         if(is_adj == false) {
            // connect the two paths
            p = (*positions)[i];
            Position *q = (*positions)[NumAdjColor];
            while(q->next != NULL) {
               q = q->next;
            }
            q->next = p;
            p->prev = q;
            (*positions)[i] = NULL;
            // reallocate the memory to remove the deleted path
            delete_empty_path(board, rows_num, cols_num, positions, colors_num);
            i--;
         }
      }
   }
}

// free the memory
void free_memory(char **board, int rows_num, Position **positions, int colors_num) {
   for (int i = 0; i < rows_num; i++) {
      free(board[i]);
   }
   free(board);
   for (int i = 0; i < colors_num; i++) {
      Position *p = positions[i];
      while(p != NULL) {
         Position *temp = p;
         p = p->next;
         free(temp);
      }
   }
   free(positions);
}


int main(int argc, char *argv[]) {
   char* folder_name;
   // check the number of arguments
   if(argc != 2) {
      printf("The folder name is not provided!\n");
      folder_name = "output";
      //make the output folder which is named "output" if it does not exist
   }
   else{
      folder_name = argv[1]; 
      //make the output folder which is named folder_name if it does not exist
   }
   
   struct stat st = {0};
   if (stat(folder_name, &st) == -1) {
      if (mkdir(folder_name) == 0) {
         printf("Folder '%s' created successfully.\n", folder_name);
      } else {
         perror("Failed to create folder");
         return 1;
      }
   }


   // declare the variables
   int rows_num, cols_num;
   //get the input from the user
   char **board = user_input(&rows_num, &cols_num);
   int NumLimit;
   //get the minimum length of the path
   printf("Enter the minimum length of the color: ");   
   scanf("%d", &NumLimit);
   //start the clock
   clock_t start_time = clock();
   //declare the variables
   int colors_num=0;
   Position **positions = NULL;
   // set the longest length of the path parameter in the function extend_the_tail_of_path
   longest_length_path = rows_num > cols_num ? rows_num*Max_Lengh_Path_Ratio : cols_num*Max_Lengh_Path_Ratio;
   srand(time(NULL)); // seed the random number generator
   //build the initial paths
   while(is_board_full(board, rows_num, cols_num) == false) {
      colors_num++;
      positions = (Position **)realloc(positions, colors_num * sizeof(Position *));
      build_path(board, rows_num, cols_num, positions, colors_num-1);
      if(colors_num >MAX_COLORS_BUILDING)
         break;
   }

   //print the board
   printf("The initial board is:\n");
   print_board_info(board, rows_num, cols_num, positions, colors_num, false);
   print_solution("build_path_game.txt", "build_path_info.txt", board, rows_num, cols_num, positions, colors_num, 0);

   // delete the path that is not long enough to reach three cells
   delete_short_path(NumLimit, board, rows_num, cols_num, &positions, &colors_num);
   //print the board
   printf("The board after deleting the short path is:\n");
   print_board_info(board, rows_num, cols_num, positions, colors_num, false);
   print_solution("delete_short_path_game.txt", "delete_short_path_info.txt", board, rows_num, cols_num, positions, colors_num, 0);


   // extend the head of the paths if possible
   for(int i=0;i<colors_num;i++){
      flip_path(positions, i);
      extend_the_tail_of_path(board, rows_num, cols_num, positions, i);
   }
   //print the board
   printf("The board after extending the head of the paths is:\n");
   print_board_info(board, rows_num, cols_num, positions, colors_num, false);
   print_solution("extend_the_tail_of_path_game.txt", "extend_the_tail_of_path_info.txt", board, rows_num, cols_num, positions, colors_num, 0);


   // seperate the path into two paths if the path is long enough, and any space is near to the cut point
   seperate_long_path(NumLimit, board, rows_num, cols_num, &positions, &colors_num);
   //print the board
   printf("The board after seperating the path into two paths is:\n");
   print_board_info(board, rows_num, cols_num, positions, colors_num, false);
   print_solution("seperate_long_path_game.txt", "seperate_long_path_info.txt", board, rows_num, cols_num, positions, colors_num, 0);


   // extend the head of the paths if possible
   for(int i=0;i<colors_num;i++){
      extend_the_tail_of_path(board, rows_num, cols_num, positions, i);
      flip_path(positions, i);
      extend_the_tail_of_path(board, rows_num, cols_num, positions, i);
   }
   //print the board
   printf("The board after extending the two ends of the paths again is:\n");
   print_board_info(board, rows_num, cols_num, positions, colors_num, false);
   print_solution("extend_the_tail_of_path_again_game.txt", "extend_the_tail_of_path_again_info.txt", board, rows_num, cols_num, positions, colors_num, 0);


   // if the head and tail of the paths are adjacent and their path are not adjacent at any point, connect them
   connect_path(board, rows_num, cols_num, &positions, &colors_num);
   //print the board
   printf("The board after connecting the head and tail of the paths is:\n");
   print_board_info(board, rows_num, cols_num, positions, colors_num, false);
   print_solution("connect_path_game.txt", "connect_path_info.txt", board, rows_num, cols_num, positions, colors_num, 0);


   //fill the empty cells with random colors and extend it by occupying other adjacent cells
   printf("Filling empty cells, please wait...\n");
   while(is_board_full(board, rows_num, cols_num) == false) {
      //printf("filling empty cells: colors_num= %d\n", colors_num);
      if(colors_num > MAX_COLORS_FILLING){
         printf("The number of colors is more than MAX_COLORS_FILLING, the program is terminated!\n");
         free_memory(board, rows_num, positions, colors_num);
         return 0;
      }
      colors_num++;
      positions = (Position **)realloc(positions, colors_num * sizeof(Position *));
      fill_empty_cells(NumLimit, board, rows_num, cols_num, &positions, &colors_num, colors_num-1);
      delete_short_path(NumLimit, board, rows_num, cols_num, &positions, &colors_num);
   }
   //print the board
   printf("The board after filling empty cells by occupying other adjacent cells is:\n");
   print_board_info(board, rows_num, cols_num, positions, colors_num, false);
   print_solution("fill_empty_cells_game.txt", "fill_empty_cells_info.txt", board, rows_num, cols_num, positions, colors_num, 0);



   //print the puzzle to the output file
   printf("\nGenerate the Flow Free Game successfully!\n");
   printf("(NumRows, NumCols= %d, %d)\n", rows_num, cols_num);
   printf("(Limit of the length of the path= %d)\n", NumLimit);
   printf("The number of colors= %d\n", colors_num);
   double execution_time = (double)(clock() - start_time) / CLOCKS_PER_SEC;
   printf("Execution time:  %f secs\n", execution_time);
   //print the board to the output file
   print_to_file(folder_name, board, rows_num, cols_num, positions, colors_num, execution_time);
   //free the memory
   free_memory(board, rows_num, positions, colors_num);
   // finish
   printf("Finished\n");
   return 0;
}
