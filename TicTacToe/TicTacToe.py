import random

class TicTacToe:
    #game board looks like:
    #          0 1 2
    #          3 4 5
    #          6 7 8
    def __init__(self):
        self.game_board = [0,0,0,0,0,0,0,0,0] #this is the initial game board
        self.player = 1 #this initializes the current player variable
        self.AIplayer = 1 #this is the AI player

    #AI_Player determines whether AI player is player 1 or 2 based on the player's choice
    def AI_Player(self, first):
        if first == True:
            self.AIplayer = 1
            return self.AIplayer
        else:
            self.AIplayer = 2
            return self.AIplayer

    #swapPlayers() allows the programe to swap two players
    def swapPlayers(self):
        if self.player == 1:
            self.player = 2
        else:
            self.player = 1
        return self.player

    #makeMove() marks whether player 1 or 2 made the move on the game board
    def makeMove(self, player, index):
        self.game_board[index] = player
        return self.game_board

    #not_empty() returns a bool var based on whether the row/column has a empty space or not
    def not_empty(self, lst):
        notEmpty = True
        for i in lst:
            if self.game_board[i] == 0:
                notEmpty = False
        return notEmpty

    #onGoingGame() returns the status of the game
    def onGoingGame(self):
        onGoing = True
        if self.gameResult() == -1:
            onGoing = True
        else:
            onGoing = False
        return onGoing
    
    def gameResult(self):
        #condition to win:
        #[0,1,2] [3,4,5] [6,7,8] [0,3,6] [1,4,7] [2,5,8] [0,4,8] [2,4,6]
        #if winPlayer = 0, game is draw
        #if winPlayer = 1, player 1 wins
        #if winPlayer = 2, player 2 wins
        
        winPlayer = -1
        if self.game_board[0] == self.game_board[1] == self.game_board[2]:
            if self.not_empty([0,1,2])==True:
                if self.game_board[0] == 1:
                    winPlayer = 1
                else:
                    winPlayer = 2
            else:
                winPlayer = -1
            
        elif self.game_board[3] == self.game_board[4] == self.game_board[5]:
            if self.not_empty([3,4,5])==True:
                if self.game_board[3] == 1:
                    winPlayer = 1
                else:
                    winPlayer = 2
            else:
                winPlayer = -1
        elif self.game_board[6] == self.game_board[7] == self.game_board[8]:
            if self.not_empty([6,7,8])==True:
                if self.game_board[6] == 1:
                    winPlayer = 1
                else:
                    winPlayer = 2
            else:
                winPlayer = -1
        elif self.game_board[0] == self.game_board[3] == self.game_board[6]:
            if self.not_empty([0,3,6])==True:
                if self.game_board[0] == 1:
                    winPlayer = 1
                else:
                    winPlayer = 2
            else:
                winPlayer = -1
        elif self.game_board[1] == self.game_board[4] == self.game_board[7]:
            if self.not_empty([1,4,7])==True:
                if self.game_board[1] == 1:
                    winPlayer = 1
                else:
                    winPlayer = 2
            else:
                winPlayer = -1
        elif self.game_board[2] == self.game_board[5] == self.game_board[8]:
            if self.not_empty([2,5,8])==True:
                if self.game_board[2] == 1:
                    winPlayer = 1
                else:
                    winPlayer = 2
            else:
                winPlayer = -1
        elif self.game_board[0] == self.game_board[4] == self.game_board[8]:
            if self.not_empty([0,4,8])==True:
                if self.game_board[0] == 1:
                    winPlayer = 1
                else:
                    winPlayer = 2
            else:
                winPlayer = -1
        elif self.game_board[2] == self.game_board[4] == self.game_board[6]:
            if self.not_empty([2,4,6])==True:
                if self.game_board[2] == 1:
                    winPlayer = 1
                else:
                    winPlayer = 2
            else:
                winPlayer = -1
        
        elif 0 not in self.game_board: #draw case
            winPlayer = 0
        else:
            winPlayer = -1 #game state is on going game
        return winPlayer

    #create a copy of the current state of the game
    def newGameCopy(self):
        copy = TicTacToe()
        copy.game_board = self.game_board[:]
        copy.player = self.player
        copy.AIplayer = self.AIplayer
        return copy
    
    def display(self):
        print()
        display_board = ["_","_","_","_","_","_","_","_","_"]
        for i in range(9):
            if self.game_board[i] == 1:
                display_board[i] = 'X'
            if self.game_board[i] == 2:
                display_board[i] = 'O'
            if (i%3 == 2):
                print(display_board[i],"\t")
            else:
                print(display_board[i], end = " ")
        print()
        print("This is the game board:")
        print("0 1 2\n3 4 5\n6 7 8")
        
    #generate a list of allowed moves
    def allowedMoves(self):
        index = []
        for i in range(9):
            if self.game_board[i] == 0:
                index.append(i)
        return index
    def player_input(self):
        playerInput = input("Please choose your next move: ")
        return (int(playerInput))

class MCTS:
    def __init__(self, game):
        self.game = game
        self.board = self.game.game_board
        self.state = self.game.onGoingGame()
        self.AI = self.game.AIplayer
        self.player = self.game.player

    #simulate playout through the copy game   
    def ranPlayout(self, move):
        """
        Computer plays against itself by generating random moves with in the range
        of allowed moves. If the result is computer win, the winning moves are
        labelled a higher value than the losing moves, and drawing moves.
        Draw moves worth more than losing moves. 
        """
        
        copyGame = self.game.newGameCopy()
        copyGame.makeMove(copyGame.player, move) #make the move
        copyGame.swapPlayers()
        while copyGame.onGoingGame() != False:
            ranNextMove = random.randint(0,8) #generate random move
            while (ranNextMove not in copyGame.allowedMoves()):
                ranNextMove = random.randint(0,8) #generate new random move that is in the range of allowed moves
            copyGame.makeMove(copyGame.player, ranNextMove) #make the randomly generated move
            copyGame.swapPlayers() #switch players and computer plays against itself
            copyGame.state = copyGame.onGoingGame()
        if copyGame.AIplayer == 1: #if AI player is player 1
            if copyGame.gameResult() == 1: #playout where AI wins
                return 100 #return the positive value for winning case
            elif copyGame.gameResult() == 2: #playout where AI loses
                return -1000 #return the negative value for losing case
            else:
                return 50 #draw
        else: #case where AI player is player 2
            if copyGame.gameResult() == 2: #playout where AI wins
                return 100
            elif copyGame.gameResult() == 1: #playout where AI loses
                return -1000
            else:
                return 50 #draw
    
    def nextMove(self):
        """
        Using the values returned from the ranPlayout(), each allowed
        moves on the game board are assigned a value based on its
        winning, losing and drawing outcomes. Select the move
        with the highest value to produce a never losing outcome
        """
        numWin = {} #a dictionary to keep track of values of each allowed moves
        for i in self.game.allowedMoves(): #all allowed moves have an initial value of 0
            numWin[i] = 0
        for i in self.game.allowedMoves():
            for j in range(15000): #15000 random playouts
                numWin[i] += self.ranPlayout(i) #assign values to keys(moves) when they finished random playout
        
        move = max(numWin, key = numWin.get)
        self.game.makeMove(self.game.player, move)
        
def play_a_new_game():
    """
    initialize the game tictactoe class
    initialize the AI player with Monte Carlo Tree Search class
    asks whether player would like to go first
    """
    tictactoe = TicTacToe()
    computer = MCTS(tictactoe)
    state = tictactoe.onGoingGame()
    next_Move = 0
    print("Welcome to a game of Tic Tac Toe!")
    player1 = input("Would you like to go first? Please enter YES or NO: ")
    #if player wants to go first
    if player1 == "YES" or player1 == "yes":
        tictactoe.AI_Player(False) #AI player is assigned value 2
        tictactoe.display()
        next_Move = tictactoe.player_input()
    #AI goes first
    elif player1 == "NO" or player1 == "no":
        computer.nextMove() #computer generate next move
        tictactoe.swapPlayers()
        tictactoe.display()
        next_Move = tictactoe.player_input()
        if next_Move not in tictactoe.allowedMoves():
            print("This is an invalid move, game will be restarted")
            play_a_new_game()
    else:
        print("invalid response detected, please retry")
        play_a_new_game()
    #while game is on going
    #this is after first move is made
    while state == True:
        tictactoe.makeMove(tictactoe.player, next_Move) #player entered move is printed on the gameboard
        tictactoe.display()
        tictactoe.swapPlayers()
        computer.nextMove() #computer generate next move
        state = tictactoe.onGoingGame() #check whether game is ended or not
        if state == True: #if game has not ended
            tictactoe.swapPlayers()
            tictactoe.display()
            next_Move = tictactoe.player_input() #player make next move
            if next_Move not in tictactoe.allowedMoves():
                print("This is an invalid move, please retry")
                next_Move = tictactoe.player_input()
            status = tictactoe.newGameCopy()
            status.makeMove(status.player, next_Move)
            state = status.onGoingGame()
    tictactoe.makeMove(tictactoe.player, next_Move)
    print()
    tictactoe.display()
    print("Game Over!")
    if tictactoe.gameResult() == 0:
        print("It is a draw!")
    else:
        print("Computer wins! Better luck next time!")
    again = input("Would you like to play another game? Please enter YES or NO: ")
    if again == "YES"or again == "yes":
        play_a_new_game()
    else:
        exit()
if __name__ == '__main__':
  play_a_new_game()
 
