
#include <stdio.h>
#include <stdlib.h>

  void displayBoard(char gameBoard[][8]); //display the game board
  int validSteps(char gameBoard[][8],int moves[][8],char player);//check whether player has valid moves
  void playStep(char gameBoard[][8],int row,int col,char player);//play the move
  int ranomPlayouts(char gameBoard[][8], int legalMoves[][8], char player, int skip, int checkers, int newMoveRow, int newMoveCol);//find move with Monte Carlo Tree Search
  void nextStep(char gameBoard[][8], int legalMoves[][8], int player, int skip, int playedCheckers);

  int main(){
     char gameBoard[8][8];//game board
     int legalMoves[8][8] = {0};//2D array of legal moves, if return 1 then playable, 0 is unplayable
     int row,col,x,y;
     int playedCheckers = 0;//number of played checkers on board
     int player = 0;//current player
     int skipPlay = 0;//if the number of skipped plays is 2, then game over (both players unable to play a move)
     int Score[2];

     char select,ch;

     printf("------Welcome to Reversi (Pure MCTS)------\n\n");
     printf("PRESS ENTER TO PLAY\n");


     scanf("%c",&select);

     do{

         if(player==0){ //decide player
             player = 1;
         }else{
             player = 0;
         }

         //clears game board
         for(row=0;row<8;row++){
             for(col=0;col<8;col++){
                 gameBoard[row][col]=0;
             }
         }

         playedCheckers = 4;//4 initial pieces on board
         gameBoard[3][3] = gameBoard[4][4] = 1;
         gameBoard[3][4] = gameBoard[4][3] = -1;

         printf("\ninitial board:\n");
         displayBoard(gameBoard);
         printf("You are 'x' \n");

         do{
             if(player==1){//human player is x
                 player = 0;

                 if(validSteps(gameBoard,legalMoves,2)){
                     while(1){
                         fflush(stdin); //clear buffer
                         printf("make your move(row,col) e.g. 1a:");
                         scanf("%d%c",&x,&ch);
                         x--; //since game board start at 0,0 input values = input values-1
                         //convert input letters to numbers through ASCII
                         if(ch>='a'){
                             y = ch - 'a' + 1;
                         } else{
                             y = ch - 'A' + 1;
                         }
                         y--;
                         //check if move is out of bounds or if move is allowed
                         if(x>=0&&x<8&&y>=0&&y<8&&legalMoves[x][y]){
                             playStep(gameBoard,x,y,2);
                             playedCheckers++;
                             break;
                         }else{
                             printf("invalid coordinate, please re-enter\n\n");
                         }
                     }

                     printf("\nnew game board after your move:\n");
                     displayBoard(gameBoard);

             }else if(++skipPlay < 2){//if number of skipped plays < 2
                     fflush(stdin); //clears buffer
                     printf("there is no move for you, press ENTER for computer play");
                     scanf("%c",&select);
                 } else{
                     printf("GAME OVER!\n");
                 }
             }else{//computer turn, computer is 'o'

                 player = 1;
                 if(validSteps(gameBoard,legalMoves,1)) {
                     skipPlay = 0;//clear skipped plays
                     nextStep(gameBoard,legalMoves,1,skipPlay, playedCheckers); //play move
                     playedCheckers++;
                     printf("\nnew game board after computer move:\n");
                     displayBoard(gameBoard);
                 }else if(++skipPlay < 2){
                     fflush(stdin);
                     printf("no move for me, your turn!\n");
                     scanf("%c",&select);
                 } else{
                     printf("GAME OVER\n");
                 }



             }


         }while(playedCheckers<64&&skipPlay<2); //while game is not over

         displayBoard(gameBoard);
         Score[0] = Score[1] = 0;//clear score

         for(row=0;row<8;row++){
             for(col=0;col<8;col++){
                 if(gameBoard[row][col]==-1){
                     Score[0]++;//count 'x' on board
                 }
                 if(gameBoard[row][col]==1){
                     Score[1]++;//count 'o' on board
                 }
             }
         }
         printf("Final score:\n");
         printf("YOU:%d\nAI:%d\n",Score[0],Score[1]);
         if (Score[0] > Score[1]){
            printf("CONGRATULATIONS! YOU WIN!\n");
         }
         else if(Score[0] == Score[1]){
            printf("IT'S A DRAW!\n");
         }
         else{
            printf("COMPUTER WINS! BETTER LUCK NEXT TIME!\n");
         }
         fflush(stdin);
         printf("Would you like to play another game?(y/n)\n");
         scanf("%c",&select);

     }while(select=='y'||select=='Y');
     printf("GAME OVER");
     system("pause");
     return 0;

 }


 //display board
 void displayBoard(char gameBoard[][8]){
     int row, col;
   printf("\n  ");
   for (col = 0; col < 8; col++)        //label column
         printf("  %c", 'A' + col);
   printf("\n");
   printf("  -");    //line on top
   for (col = 0; col < 7; col++)        //separate lines
         printf("---");
   printf("---\n");
   for (row = 0; row < 8; row++)
   {
         printf("%2d|", row + 1);       //label rows
         for (col = 0; col < 8; col++)  //label moves
         {
         if (gameBoard[row][col] == 1)  //o
             printf("o |");
         else if (gameBoard[row][col] == -1)    //x
             printf("x |");
         else   //empty cells
             printf("  |");
         }
         printf("\n");
         if (row < 8-1)
         {
         printf("  |--"); //cross lines
         for (col = 0; col < 8-1; col++)
             printf("---");
         printf("-\n");
         }
   }
   printf("  -------------------------");
   printf("\n");
 }

//check if player has moves
 int validSteps(char gameBoard[][8], int legalMoves[][8], char player)
 { int rowdelta, coldelta, row, col, x, y = 0;
   int allowedMoves = 0;//allowed moves on the board
   char opponent = (player == 1) ? -1 : 1;
   char myplayer = -1 * opponent;
   for (row = 0; row < 8; row++) //clear array of legal moves
        for (col = 0; col < 8; col++)
            legalMoves[row][col] = 0;
   for (row = 0; row < 8; row++) //use for loop to determine if there are empty cells
   {
         for (col = 0; col < 8; col++)
         {
             if (gameBoard[row][col] != 0)
                 continue;
             for (rowdelta = -1; rowdelta <= 1; rowdelta++)  //check rows
             {
                 for (coldelta = -1; coldelta <= 1; coldelta++)   //check cols
                 {
                   if (row + rowdelta < 0 || row + rowdelta >= 8 || col + coldelta < 0 || col + coldelta >= 8|| (rowdelta == 0 && coldelta == 0))//if move is out of bounds
                                           continue;
                   if (gameBoard[row + rowdelta][col + coldelta] == opponent)//if there are opponent pieces around (row,col)
                   {
                           x = row + rowdelta;
                           y = col + coldelta;
                           while(1)//start from opponent piece, find friendly pieces
                           {
                             x += rowdelta; //coordinate of opponent pieces
                             y += coldelta;
                             if (x < 0 || x >= 8 || y < 0 || y >= 8) //out of bounds
                                 break;
                               if (gameBoard[x][y] == 0)          //if coordinate is empty, break
                                 break;
                               if (gameBoard[x][y] == myplayer) //if coordinate is current player's
                             {
                               legalMoves[row][col] = 1;
                                  allowedMoves++;  //increment on allowed moves
                                 break;
                             }
                          }
                   }
                 }
             }
         }
   }
   return allowedMoves;
 }


 void playStep(char gameBoard[][8], int row, int col, char player)
 {
   int rowdelta = 0;
   int coldelta = 0;
   int x = 0;
   int y = 0;
   char opponent = (player == 1) ? -1 : 1;
   char myplayer = -1 * opponent;
   gameBoard[row][col] = myplayer;
   for (rowdelta = -1; rowdelta <= 1; rowdelta++)//check surrounding pieces
   {
         for (coldelta = -1; coldelta <= 1; coldelta++)
         {
             if (row + rowdelta < 0 || row + rowdelta >= 8 || col + coldelta < 0 || col + coldelta >= 8 || (rowdelta == 0 && coldelta == 0))
                         //out of boundsvoid nextStep(char gameBoard[][8], int legalMoves[][8], int player, int skip, int playedCheckers)
                         continue;
                     //if it's enemy piece
                     if (gameBoard[row + rowdelta][col + coldelta] == opponent)
                     {
                         x = row + rowdelta;
                         y = col + coldelta;
                         while(1) //find friendly pieces around enemy pieces
                         {
                           x += rowdelta;
                           y += coldelta;
                           if (x < 0 || x >= 8 || y < 0 || y >= 8)   //out of bounds
                                   break;
                           if (gameBoard[x][y] == 0)//if empty cell
                                   break;
                           if (gameBoard[x][y] == myplayer) //if coordinate is friendly piece
                           {
                               while (gameBoard[x -= rowdelta][y -= coldelta] == opponent)
                               gameBoard[x][y] = myplayer;   //flip
                               break;
                           }
                         }
                     }
         }
   }

 }


 int ranomPlayouts(char gameBoard[][8], int legalMoves[][8], char player, int skip, int checkers, int newMoveRow, int newMoveCol)
 {
   int row, col, row1, col1, i, j;
   int Score = 0, MinScore = 100;//opponent score (lower the better)
   char copyBoard[8][8];   //save current board into a copy game board
   int copyLegalMoves[8][8];    //save current legal moves into a copy legal moves
   char opponent = (player == 1) ? -1 : 1;
   int validRow[8], validCol[8];
   int playerScore[2];

   for (i = 0; i < 8; i++){//copy current gameboard state
             for (j = 0; j < 8; j++){
                copyBoard[i][j] = gameBoard[i][j];
             }
         }
   playStep(copyBoard, newMoveRow, newMoveCol,player);
   while (skip <2 && checkers < 64){
    for (int x = 0; x<8; x++){
    validRow[x] = -1;
    validCol[x] = -1;
   }
   for (row = 0; row < 8; row++)
   {
         for (col = 0; col < 8; col++)
         {
         if (legalMoves[row][col] == 0)  //illegal moves
             continue;
         validRow[row]=row;
         validCol[row]=col;
         }
   }

         //for (i = 0; i < 8; i++){//copy current gameboard state
             //for (j = 0; j < 8; j++){
                //copyBoard[i][j] = gameBoard[i][j];
             //}
         //}
         int rand_step = rand() % 8;
         while (validRow[rand_step] == -1){
            rand_step = rand() % 8;
         }

         playStep(copyBoard, validRow[rand_step], validCol[rand_step], opponent);
                                 //playStep in copyBoard
         validSteps(copyBoard, copyLegalMoves, player);  //check if opponent has legal moves
         checkers++;
   }
   for (row=0; row<8; row++){
    for (col = 0; col<8; col++){
        if (copyBoard[row][col] == -1){
            playerScore[0]++; //score of x
        }
        if (copyBoard[row][col] == 1){
            playerScore[1]++; //score of o
        }
    }
   }
   if (playerScore[1] > playerScore[0]){
    return 100;
   }
   else{
    return -100;
   }

 }
 void nextStep(char gameBoard[][8], int legalMoves[][8], int player, int skip, int playedCheckers){
   int row, col, i, j;
   int validRow[8];
   int validCol[8];
   int allowedMoves=0;
   int maxValue;
   int winMove=0;
   for (int x = 0; x<8; x++){
    validRow[x] = -1;
    validCol[x] = -1;
   }
   for (row = 0; row < 8; row++){
    for (col = 0; col < 8; col++){
        if (legalMoves[row][col] == 0)  //illegal moves
             continue;
         validRow[row]=row;
         validCol[row]=col;
         }
    }
    /*
    for (i=0;i<8;i++){
        printf("validRow %d: %d\nvalidCol %d: %d\n", i, validRow[i], i, validCol[i]);

    }
    */
    for (int x=0; x<8; x++){
        if (validRow[x] != -1){
            allowedMoves ++;
        }
    }
    int index[allowedMoves];
    int numWin[allowedMoves];
    for (i= 0; i<8; i++){
        for (j=0; j<allowedMoves; j++){

        if (validRow[i]!= -1){
            index[j]=i;
        }
        }
    }

    for (int y=0; y<allowedMoves; y++){
        for (int p = 0; p<20000; p++){
            numWin[y] += ranomPlayouts(gameBoard, legalMoves, player, skip, playedCheckers, validRow[index[y]], validCol[index[y]]);
        }
    }
    for (i=0; i<allowedMoves; i++){

        if (numWin[i] > maxValue){
            maxValue = numWin[i];
            winMove = index[i];
            //printf("max value is: %d\nwinMove is: %d\n", maxValue, winMove);
        }
    }
   //printf("\nwinMove is: %d\nchosen move is: row: %d, col: %d\n",winMove,validRow[winMove],validCol[winMove] );
   //printf("\nsince game board starts at 0,0; 5,5 is translated to 6,f\n");
   printf("Computer play:%d%c", validRow[winMove]+1, validCol[winMove]+'a');
   playStep(gameBoard, validRow[winMove], validCol[winMove], player);
 }
