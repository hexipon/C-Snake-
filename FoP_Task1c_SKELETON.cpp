//Group:CS4G1b
//Dan Peverley      (28010687)
//Jared Bruty       (28004627)
//Bailey Saunders   (28022868)
//Ben Simpson       (28014245)

//---------------------------------------------------------------------------
//----- include libraries
//---------------------------------------------------------------------------

//include standard libraries
#include <iostream>	
#include <iomanip> 
#include <conio.h> 
#include <cassert> 
#include <string>
#include <sstream>
#include <vector>
#include <fstream>
using namespace std;

//include our own libraries
#include "RandomUtils.h"    //for seed, random
#include "ConsoleUtils.h"	//for clrscr, gotoxy, etc.
#include "TimeUtils.h"		//for getSystemTime, timeToString, etc.

//---------------------------------------------------------------------------
//----- define constants
//---------------------------------------------------------------------------

//defining the size of the grid
const int  SIZEX(15);    	//horizontal dimension
const int  SIZEY(12);		   //vertical dimension
//defining symbols used for display of the grid and content
const char SPOT('O');    	//snake1 head
const char DOT('Q');    	//snake2 head
const char BODY('o');		   //snake1 body
const char DODY('0');     //snake2 body
const char MOUSE('@');		  //mouse
const char TUNNEL(' ');   //tunnel
const char WALL('#');    	//border
const char PILL('-');     //pill
const char MONGOOSE('M'); //mongoose
//defining the command letters to move the snake on the maze
const int  UP(72);			     //up arrow
const int  DOWN(80); 		   //down arrow
const int  RIGHT(77);		   //right arrow
const int  LEFT(75);		    //left arrow
//defining the other command letters
const int  UP2('W');			     //up arrow
const int  DOWN2('S'); 		   //down arrow
const int  RIGHT2('D');		   //right arrow
const int  LEFT2('A');		    //left arrow
const char QUIT('Q');		   //to end the game

const int numOfMovesForPill(11);

struct Item {
  int x, y;
  char symbol;
  bool visible;
};
struct Items {
  Item mouse,
    pill,
    mongoose;
};
struct Player {
  string name;
  bool isInvincible=false,
    active=false;
  int collectedMice=0,
    score=0,
    invincibleMoves=0,
    bestScore,
    currentdy=0,//direction of player
    currentdx=0;
  vector<Item> snake;
};

struct GameData { 
  string message= "LET'S START...";
  bool gameOver = false,
    hasCheated = false,
    isCheating = false,
    twoPlayer = false,
    timeCheat = false;
  int movesSincePill=0,
    timer=0,
    maxTime=300;
  clock_t startTime = clock();
  float moveTimer = 0.0,
    prevTime = startTime,
    secondCount=0,
    speedIncrease=0.01, //speed increase frame
    gameSpeed=1.0;//setting initial game speed
  Player player1,
    player2;
  vector<Player> leaderboard = {};
};

//---------------------------------------------------------------------------
//----- run game
//---------------------------------------------------------------------------


int main()
{

  //function declarations (prototypes)
  void WriteLeaderboard(const vector<Player> leaderboard); 
  void ReadLeaderboard(vector<Player>& leaderboard);
  void MaintainLeaderboard(vector<Player>& leaderboard, const Player player);
  void initialiseGame(char g[][SIZEX], char m[][SIZEX], GameData& player, Items& items);
  void renderGame(const char g[][SIZEX], const GameData player);
  void updateGame(char g[][SIZEX], const char m[][SIZEX], const int kc, GameData& gameData, Items& items);
  void ToggleCheats(GameData& player);
  void WriteBestScore(const string playerName, const int score);
  bool isArrowKey(const int k);
  int  getKeyPress();
  void endProgram(const string message);
  void DisplayLeaderboard(const vector<Player> leaderboard);
  void timer(GameData& gameData);

  GameData playerSetup();
  Items itemSetup();
  
  char grid[SIZEY][SIZEX], maze[SIZEY][SIZEX]; //grid for display, structure of the maze
  GameData gameData = playerSetup();
  Items items = itemSetup();
  ReadLeaderboard(gameData.leaderboard);

  seed();								//seed the random number generator
  SetConsoleTitle("FoP 2018-19 - Task 1c - Game Skeleton");
  initialiseGame(grid, maze, gameData, items);	//initialise grid (incl. walls and snake)
  int key(0), prevKey(0);							//current key selected by player
  do {
    timer(gameData);
    renderGame(grid, gameData);			//display game info, modified grid and messages
    prevKey = key;
    if (_kbhit())
    {
      key = toupper(getKeyPress()); 	//read in  selected key: arrow or letter command
    }
    else
      key = prevKey;
    if (isArrowKey(key))
    {
      if (gameData.moveTimer >= (gameData.timeCheat ? 1 : gameData.gameSpeed)) //if time cheat is enabled for testing, speed equals 1
      {
        updateGame(grid, maze, key, gameData, items);
        gameData.moveTimer = 0;
      }
    }
    else
    {
      switch (key)
      {
      case 'B'://toggle leaderboard
        DisplayLeaderboard(gameData.leaderboard);
        break;
      case 'C'://toggle game cheats
        ToggleCheats(gameData);
        gameData.message = "Toggling cheats";
        break;
      case 'Q':
        gameData.message = "Player Quit!";
        gameData.gameOver = true;
        break;
      case 'Z'://toggle speed cheat for testing
        gameData.gameSpeed = 1;
        gameData.timeCheat = !gameData.timeCheat;
        break;
      default:
        gameData.message = "INVALID KEY!";  //set 'Invalid key' message
        break;
      }
      key = prevKey; //resetting key to valid movement key
    }
  } while (!gameData.gameOver);		//while user does not want to quit


  if (gameData.player1.collectedMice == 10) {
    gameData.message = "Player 1 wins!";
      if (gameData.player1.score < gameData.player1.bestScore && !gameData.hasCheated)
      {
          gameData.player1.bestScore = gameData.player1.score;
          WriteBestScore(gameData.player1.name, gameData.player1.score);
          MaintainLeaderboard(gameData.leaderboard, gameData.player1);
          WriteLeaderboard(gameData.leaderboard);
      }
  }
  if (gameData.twoPlayer)
  {
      if (gameData.player2.collectedMice == 10) {
          gameData.message = "Player 2 wins!";
          if (gameData.player2.score < gameData.player2.bestScore && !gameData.hasCheated)
          {
              gameData.player2.bestScore = gameData.player2.score;
              WriteBestScore(gameData.player2.name, gameData.player2.score);
              MaintainLeaderboard(gameData.leaderboard, gameData.player2);
              WriteLeaderboard(gameData.leaderboard);
          }
      }
  }
  renderGame(grid, gameData);			//display game info, modified grid and messages
  endProgram(gameData.message);						//display final message
  return 0;
}

Items itemSetup()
{
  Items items;
  items.mouse = { 0, 0, MOUSE, true };
  items.pill = { 0, 0, PILL, false };
  items.mongoose = { 0, 0, MONGOOSE, false };

  return items;
}

GameData playerSetup()
{
  int ReadBestScore(const string playerName);
  void getPlayerNames(string& player1Name, string& player2Name, const bool playerNum);
  bool GetPlayerNumber();
  GameData gameData;

  //set up game data
  gameData.twoPlayer = GetPlayerNumber();
  getPlayerNames(gameData.player1.name, gameData.player2.name, gameData.twoPlayer);

  //set up player1
  gameData.player1.active = true;
  gameData.player1.bestScore = ReadBestScore(gameData.player1.name);
  gameData.player1.snake = { { 0, 0, SPOT, true }, { 0, 0, BODY, false }, { 0, 0, BODY, false },{ 0, 0, BODY, false } }; 		//snake's position and symbol

  //set up player 2
  if (gameData.twoPlayer)
  {
    gameData.player2.active = true;
    gameData.player2.bestScore = ReadBestScore(gameData.player1.name);
    gameData.player2.snake = { { 0, 0, DOT, true },{ 0, 0, DODY, false },{ 0, 0, DODY, false },{ 0, 0, DODY, false } }; 		//snake's position and symbol

  }

  return gameData;
}

void getPlayerNames(string& player1Name, string& player2Name, const bool playerNum)
{
  const int maxNameChars(20);
  cout << "Enter Player 1's Name:";
  cin >> player1Name;
  if (player1Name.length() >= maxNameChars)
    player1Name.erase(player1Name.begin() + maxNameChars, player1Name.end());
  if (playerNum)
  {
    cout << "Enter Player 2's Name:";
    cin >> player2Name;
    if (player2Name.length() >= maxNameChars)
      player2Name.erase(player2Name.begin() + maxNameChars, player2Name.end());
  }

}

bool GetPlayerNumber() {
  char answer;
  cout << "Two Players? y/n";
  cin >> answer;
  return(tolower(answer) == 'y');
}

//---------------------------------------------------------------------------
//----- initialise game state
//---------------------------------------------------------------------------

void initialiseGame(char g[][SIZEX], char m[][SIZEX], GameData& gameData, Items& items)
{ //initialise grid and place snake in middle
  void setInitialMazeStructure(char maze[][SIZEX]);
  void setCoordinates(char g[][SIZEX], Item& item);
  void updateGrid(char g[][SIZEX], const char m[][SIZEX], GameData&player, const Items& items);

  setInitialMazeStructure(m);		//initialise maze
  updateGrid(g, m, gameData, items);
  setCoordinates(g, gameData.player1.snake[0]);
  updateGrid(g, m, gameData, items);
  if (gameData.twoPlayer == true)
  {
    setCoordinates(g, gameData.player2.snake[0]);
    updateGrid(g, m, gameData, items);
  }
  setCoordinates(g, items.mouse);
  updateGrid(g, m, gameData, items);		//prepare grid
  setCoordinates(g, items.mongoose);
  updateGrid(g, m, gameData, items);
}

void setCoordinates(char g[][SIZEX], Item& item)
{ //set coordinates inside the grid at random at beginning of game
  int tempX(0), tempY(0);
  do
  {
    tempX = random(SIZEX - 2);      //vertical coordinate in range [1..(SIZEY - 2)]
    tempY = random(SIZEY - 2);      //horizontal coordinate in range [1..(SIZEX - 2)]
  } while (g[tempY][tempX] != TUNNEL);
  item.y = tempY;
  item.x = tempX;
}

void setInitialMazeStructure(char maze[][SIZEX])
{ //set the position of the walls in the maze
  //initialise maze configuration
  char initialMaze[SIZEY][SIZEX] 	//local array to store the maze structure
    = {
  { '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#' },
    { '#', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '#' },
    { '#', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '#', ' ', ' ', ' ', '#' },
    { '#', ' ', ' ', '#', ' ', ' ', ' ', '#', '#', ' ', ' ', ' ', ' ', ' ', '#' },
    { '#', ' ', ' ', '#', ' ', ' ', ' ', ' ', '#', ' ', ' ', ' ', ' ', ' ', '#' },
    { '#', ' ', ' ', '#', '#', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '#' },
    { '#', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '#' },
    { '#', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '#' },
    { '#', ' ', ' ', ' ', '#', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '#' },
  { '#', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '#', ' ', ' ', ' ', ' ', '#' },
  { '#', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '#' },
  { '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#' }
  };
  //with '#' for wall, ' ' for tunnel, etc. 
  //copy into maze structure with appropriate symbols
  for (int row(0); row < SIZEY; ++row)
    for (int col(0); col < SIZEX; ++col)
      switch (initialMaze[row][col])
      {
        //not a direct copy, in case the symbols used are changed
      case '#': maze[row][col] = WALL; break;
      case ' ': maze[row][col] = TUNNEL; break;
      }
}

//---------------------------------------------------------------------------
//----- Update Game
//---------------------------------------------------------------------------

void updateGame(char g[][SIZEX], const char m[][SIZEX], const int key, GameData& gameData, Items& items)
{ //update game
  void updateGameData(char g[][SIZEX], const int key, GameData& gameData, Items& items);
  void updateGrid(char g[][SIZEX], const char maze[][SIZEX], GameData& gameData, const Items& items);
  updateGameData(g, key, gameData, items);		//move snake in required direction
  updateGrid(g, m, gameData, items);					//update grid information
}
void updateGameData(char g[][SIZEX], const int key, GameData& gameData, Items& items)
{ //move snake in required direction
  bool isArrowKey(const int k);
  void setKeyDirection(int k, int& dx, int& dy, int& dx2, int& dy2);
  void setCoordinates(char g[][SIZEX], Item& mouse);
  void moveSnake(Player& player, const int dy, const int dx,const bool wall);
  void resetSnake(vector<Item>& snake);
  void moveMongoose(Item& mongoose, char g[][SIZEX]);
  bool wall = false;
  vector<Item> chosenSnake;
  //reset message to blank
  gameData.message = "";

  //calculate direction of movement for given key
  int dx(0), dy(0), dx2(0), dy2(0);
  setKeyDirection(key, dx, dy, dx2, dy2);
  if ((dx == 0) && (dy == 0))
  {
    dx = gameData.player1.currentdx;
    dy = gameData.player1.currentdy;
  }
  if ((dx2 == 0) && (dy2 == 0))
  {
    dx2 = gameData.player2.currentdx;
    dy2 = gameData.player2.currentdy;
  }
  gameData.player1.currentdx = dx;
  gameData.player1.currentdy = dy;
  gameData.player2.currentdx = dx2;
  gameData.player2.currentdy = dy2;
  //check new target position in grid and update game data (incl. snake coordinates) if move is possible

  switch (g[gameData.player1.snake[0].y + dy][gameData.player1.snake[0].x + dx]) // checking player 1 next position
  {			//...depending on what's on the target position in grid...
  case MOUSE:
    setCoordinates(g, items.mouse);
    gameData.message = "You have eaten a mouse you monster";
    if (!gameData.isCheating)
    {
      gameData.player1.snake.push_back({ gameData.player1.snake[0].x, gameData.player1.snake[0].y, BODY, false });//adding 2 to snake 2 body
      gameData.player1.snake.push_back({ gameData.player1.snake[0].x, gameData.player1.snake[0].y, BODY, false });
        gameData.player1.collectedMice++;
      if ((gameData.player1.collectedMice % 2 == 0) && !items.pill.visible)//pill becomes visible every 2 mice eaten
      {
        setCoordinates(g, items.pill);
        items.pill.visible = true;
      }
    }
    if (gameData.player1.collectedMice == 3)//mongoose appears if 3 mice eaten
      items.mongoose.visible = true;
    if (gameData.player1.collectedMice >= 10)
    {
      gameData.gameOver = true;
      if (gameData.twoPlayer)
        gameData.message = "Player 1 wins";
      else
        gameData.message = "You win!";
    }
  case TUNNEL:		//can move
    moveSnake(gameData.player1, dy, dx, wall);
    break;
  case PILL:
    resetSnake(gameData.player1.snake);
    items.pill.visible = false;
    moveSnake(gameData.player1, dy, dx, wall);
    gameData.player1.invincibleMoves = 0;
    gameData.player1.isInvincible = true;
    break;
  case MONGOOSE:
    if (!gameData.isCheating && !gameData.player1.isInvincible && items.mongoose.visible)
    {
      gameData.gameOver = true;
      if (gameData.twoPlayer)
        gameData.message = "Player 2 wins";
      else
      gameData.message = "Death by mongoose";
    }
    break;
  case BODY:
  case DODY:
    if (!gameData.isCheating && !gameData.player1.isInvincible)
    {
      gameData.gameOver = true;
      if (gameData.twoPlayer)
        gameData.message = "Player 2 wins";
      else
      gameData.message = "Death by snake";
    }
    break;
  case WALL:

    if (!gameData.isCheating && !gameData.player1.isInvincible)
    {
      if (gameData.twoPlayer)
        gameData.message = "Player 2 wins";
      else
      gameData.message = "Death by wall";
      gameData.gameOver = true;
    }
    else if (gameData.player1.isInvincible) //loop to other side of map if hit wall and invisible
    {
      wall = true;
      switch (gameData.player1.snake[0].y + dy)
      {
      case 0:
      case (SIZEY - 1):
        moveSnake(gameData.player1, dy, dx, wall);
        break;
      }

      switch (gameData.player1.snake[0].x + dx)
      {
      case 0:
      case (SIZEX - 1):
        moveSnake(gameData.player1, dy, dx, wall);
        break;
      }
    }
    break;
  case DOT:
    if (!gameData.isCheating && !gameData.player1.isInvincible)
    {
      gameData.gameOver = true;
      if (gameData.twoPlayer)
        gameData.message = "Both players loose.";
      else
      gameData.message = "Death by snake";
    }
    break;
  }

if (gameData.twoPlayer) //checking player2 next position
  {
  switch (g[gameData.player2.snake[0].y + dy2][gameData.player2.snake[0].x + dx2])
  {			//...depending on what's on the target position in grid...
  case MOUSE:
    setCoordinates(g, items.mouse);
    gameData.message = "You have eaten a mouse you monster";
    if (!gameData.isCheating)
    {
      gameData.player2.snake.push_back({ gameData.player2.snake[0].x, gameData.player2.snake[0].y, DODY, false }); //adding 2 to snake 2 body
      gameData.player2.snake.push_back({ gameData.player2.snake[0].x, gameData.player2.snake[0].y, DODY, false });
      gameData.player2.collectedMice++;
      if ((gameData.player2.collectedMice % 2 == 0) && !items.pill.visible) //pill becomes visible every 2 mice eaten
      {
        setCoordinates(g, items.pill);
        items.pill.visible = true;
      }
    }
    if (gameData.player2.collectedMice == 3) //mongoose appears if 3 mice eaten
      items.mongoose.visible = true;
    if (gameData.player2.collectedMice >= 10)
    {
      gameData.gameOver = true;
        gameData.message = "Player 1 wins";
    }
  case TUNNEL:		//can move
    moveSnake(gameData.player2, dy2, dx2, wall);
    break;
  case PILL:
    resetSnake(gameData.player2.snake);
    items.pill.visible = false;
    moveSnake(gameData.player2, dy2, dx2, wall);
    gameData.player1.invincibleMoves = 0;
    gameData.player2.isInvincible = true;
    break;
  case MONGOOSE:
    if (!gameData.isCheating && !gameData.player2.isInvincible && items.mongoose.visible)
    {
      gameData.gameOver = true;
        gameData.message = "Player 1 wins";
    }
    break;
  case BODY:
  case DODY:
    if (!gameData.isCheating && !gameData.player2.isInvincible)
    {
      gameData.gameOver = true;
        gameData.message = "Player 1 wins";
    }
    break;
  case WALL:  		

    if (!gameData.isCheating && !gameData.player2.isInvincible)
    {
        gameData.message = "Player 1 wins";
      gameData.gameOver = true;
    }
    else if (gameData.player2.isInvincible) //loop to other side of map if hit wall and invincible
    {
      wall = true;
      switch (gameData.player2.snake[0].y + dy2)
      {
      case 0:
      case (SIZEY - 1):
        moveSnake(gameData.player2, dy2, dx2, wall);
        break;
      }

      switch (gameData.player2.snake[0].x + dx2)
      {
      case 0:
      case (SIZEX - 1):
        moveSnake(gameData.player2, dy2, dx2, wall);
        break;
      }
    }
    break;
  case SPOT:

    if (!gameData.isCheating && !gameData.player2.isInvincible)
    {
      gameData.gameOver = true;
        gameData.message = "Both players loos";
    }
    break;
  }

  gameData.player2.score++;
  }
  gameData.player1.score++;

  if (items.pill.visible)
    gameData.movesSincePill++;
  if (gameData.movesSincePill == numOfMovesForPill)
  {
    items.pill.visible = false;
    gameData.movesSincePill = 0;
  }

  if ((items.mongoose.visible) && (!gameData.gameOver))
    moveMongoose(items.mongoose, g);
  if (items.mongoose.visible)
  {
    if ((gameData.player1.snake[0].x == items.mongoose.x) && (gameData.player1.snake[0].y == items.mongoose.y) && (!gameData.isCheating)) //player1 dies if spot and mongoose are on the same grid coordinates
    {
      gameData.gameOver = true;
      if (gameData.twoPlayer)
        gameData.message = "Player 2 wins";
      else
        gameData.message = "Death by mongoose";
    }
    if (gameData.twoPlayer)
    {
      if ((gameData.player2.snake[0].x == items.mongoose.x) && (gameData.player2.snake[0].y == items.mongoose.y) && (!gameData.isCheating)) //player2 dies if spot and mongoose are on the same grid coordinates
      {
        gameData.gameOver = true;
        gameData.message = "Player 1 wins!";
      }
    }
  }

  if ((gameData.timer == 0) && (!gameData.gameOver) && (!gameData.isCheating))
  {
    gameData.gameOver = true;
    gameData.message = "Timer up, you loose";
  }
}


void timer(GameData& gameData)
{
  clock_t moveEnd = clock();                                 // get clock at the moment the the function si called 

  float deltaTime = 1.0/(moveEnd - gameData.prevTime);  // set play time to duartion since the clock started and this function was called
  int playTime = (moveEnd - gameData.startTime) / CLOCKS_PER_SEC;
  gameData.timer = gameData.maxTime - playTime;				// subtract duaration from timer

  if (gameData.timer < 0)										// if timer falls below zero reset it to zero so it doesnt show negative time
  {
    gameData.timer = 0;
  }
  gameData.moveTimer += deltaTime;
  gameData.prevTime = moveEnd;
  gameData.secondCount += deltaTime;
  if (gameData.secondCount >= 1) //increase speed every second 
  {
    gameData.gameSpeed -= gameData.speedIncrease;
    gameData.secondCount = 0;
  }
  //return gameData.timer, gameData.deltaTime;
}

void moveSnake(Player& player, const int dy, const int dx,const bool wall)
{
  for (int i = player.snake.size() - 1; i > 0; --i) //Changing position of the snakes body 
  {
    player.snake[i].x = player.snake[i - 1].x;//set new positions of body parts
    player.snake[i].y = player.snake[i - 1].y;
    if ((player.snake[i].x != 0) && (player.snake[i].y != 0))//make body visible if on map
      player.snake[i].visible = true;
  }
  if (player.isInvincible)
  {
    player.invincibleMoves++;
    if (player.invincibleMoves >= 20) {
      player.isInvincible = false;
    }
    if (wall)//loop round if invincible and hit side wall
      {
        switch (dx)
        {
        case 1:
          player.snake[0].x = 0;
          break;
        case -1:
          player.snake[0].x = SIZEX - 1;
        }
        switch (dy)
        {
        case 1:
          player.snake[0].y = 0;
          break;
        case -1:
          player.snake[0].y = SIZEY - 1;
        }
      }
  }

  player.snake[0].y += dy;	//go in that Y direction
  player.snake[0].x += dx;	//go in that X direction

}

void moveMongoose(Item& mongoose, char g[][SIZEX])
{
  int dy;
  int dx;
  do
  {
    switch (random(4))	//...depending on the selected key...
    {
    case 1:  	//when LEFT arrow pressed...
      dx = -1;	   //decrease the X coordinate
      dy = 0;
      break;
    case 2: 	//when RIGHT arrow pressed...
      dx = +1;	   //increase the X coordinate
      dy = 0;
      break;
    case 3: 	 //when down arrow pressed...
      dx = 0;
      dy = +1;    //increase the y coordinate
      break;
    case 4: 	   //when up arrow pressed...
      dx = 0;
      dy = -1;    //decrease the y coordinate
      break;
    }
  } while ((g[mongoose.y + dy][mongoose.x + dx] != TUNNEL));
  mongoose.y += dy;
  mongoose.x += dx;
}

void updateGrid(char g[][SIZEX], const char maze[][SIZEX], GameData& gameData, const Items& items)
{ //update grid configuration after each move
  void placeMaze(char g[][SIZEX], const char b[][SIZEX]);
  void placeItem(char g[][SIZEX], const Item& snake);
  placeMaze(g, maze);	//reset the empty maze configuration into grid

  //place items
  if (items.mongoose.visible)
  {
    placeItem(g, items.mongoose); //set mongoose in grid
  }
  placeItem(g, items.mouse); //set mouse in grid
  if (items.pill.visible)
    placeItem(g, items.pill); //set pill in grid

  //place player1
  placeItem(g, gameData.player1.snake[0]);	//set snake in grid
  for (int i = 0; i < gameData.player1.snake.size(); i++)
  {
    if (gameData.player1.snake[i].visible)
      placeItem(g, gameData.player1.snake.at(i));
  }

  //place player2
  if (gameData.twoPlayer)
  {
    placeItem(g, gameData.player2.snake[0]);
    for (int i = 0; i < gameData.player2.snake.size(); i++)
    {
      if (gameData.player2.snake[i].visible)
        placeItem(g, gameData.player2.snake.at(i));
    }
  }

}

void placeMaze(char grid[][SIZEX], const char maze[][SIZEX])
{ //reset the empty/fixed maze configuration into grid
  for (int row(0); row < SIZEY; ++row)
    for (int col(0); col < SIZEX; ++col)
      grid[row][col] = maze[row][col];
}

void placeItem(char g[][SIZEX], const Item& item)
{ //place item at its new position in grid
  g[item.y][item.x] = item.symbol;
}
//---------------------------------------------------------------------------
//----- process key
//---------------------------------------------------------------------------
void setKeyDirection(const int key, int& dx, int& dy, int& dx2, int& dy2)
{ //calculate direction indicated by key
  bool isArrowKey(const int k);
  switch (key)	//...depending on the selected key...
  {
  case LEFT:  	//when LEFT arrow pressed...
    dx = -1;	   //decrease player1 X coordinate
    dy = 0;
    break;
  case RIGHT: 	//when RIGHT arrow pressed...
    dx = +1;	   //increase player1 X coordinate
    dy = 0;
    break;
  case DOWN: 	 //when down arrow pressed...
    dx = 0;
    dy = +1;    //increase player1 y coordinate
    break;
  case UP: 	   //when up arrow pressed...
    dx = 0;
    dy = -1;    //decrease player1 y coordinate
    break;


  case LEFT2:  	//when LEFT arrow pressed...
    dx2 = -1;	   //decrease player2 X coordinate
    dy2 = 0;
    break;
  case RIGHT2: 	//when RIGHT arrow pressed...
    dx2 = +1;	   //increase player2 X coordinate
    dy2 = 0;
    break;
  case DOWN2: 	 //when down arrow pressed...
    dx2 = 0;
    dy2 = +1;    //increase player2 y coordinate
    break;
  case UP2: 	   //when up arrow pressed...
    dx2 = 0;
    dy2 = -1;    //decrease player2 y coordinate
    break;
  }
}

int getKeyPress()
{ //get key or command selected by user
  //KEEP THIS FUNCTION AS GIVEN
  int keyPressed;
  keyPressed = _getch();			//read in the selected arrow key or command letter
  while (keyPressed == 224) 		//ignore symbol following cursor key
    keyPressed = _getch();
  return keyPressed;
}

bool isArrowKey(const int key)
{	//check if the key pressed is an arrow key (also accept 'K', 'M', 'H' and 'P')
  return (key == LEFT) || (key == RIGHT) || (key == DOWN) || (key == UP) || (key == LEFT2) || (key == RIGHT2) || (key == DOWN2) || (key == UP2);
}


//---------------------------------------------------------------------------
//----- display info on screen
//---------------------------------------------------------------------------

string tostring(const int x)
{	//convert an integer to a string
  std::ostringstream os;
  os << x;
  return os.str();
}
string tostring(const char x)
{	//convert a char to a string
  std::ostringstream os;
  os << x;
  return os.str();
}
void showMessage(const WORD backColour, const WORD textColour, int x, int y, const string& message)
{	//display a string using specified colour at a given position 
  gotoxy(x, y);
  selectBackColour(backColour);
  selectTextColour(textColour);
  cout << message + string(40 - message.length(), ' ');
}
void renderGame(const char g[][SIZEX], const GameData gameData)
{ //display game title, messages, maze, snake and other items on screen
  string tostring(const char x);
  string tostring(const int x);
  void showMessage(const WORD backColour, const WORD textColour, int x, int y, const string& message);
  void paintGrid(const char g[][SIZEX], const GameData player);
  //display game title
  showMessage(clRed, clWhite, 0, 0, "___GAME___");
  showMessage(clWhite, clRed, 40, 0, "FoP Task 1c - February 2019");


  if (gameData.twoPlayer)
  {
    showMessage(clRed, clYellow, 40, 1, "Players   :" + gameData.player1.name + "|" + gameData.player2.name);
    showMessage(clRed, clYellow, 40, 2, "Score     :" + tostring(gameData.player1.score) + "|" + tostring(gameData.player2.score));
    showMessage(clRed, clYellow, 40, 3, "Mice eaten:" + tostring(gameData.player1.collectedMice) + "|" + tostring(gameData.player2.collectedMice));
    showMessage(clRed, clYellow, 40, 4, "Best score:" + ((gameData.player1.bestScore == 0) ? "500" : tostring(gameData.player1.bestScore)) + "|" + ((gameData.player1.bestScore == 0) ? "500" : tostring(gameData.player2.bestScore)));
    showMessage(clRed, clYellow, 40, 13, (gameData.player2.isInvincible) ? "Player 2 invincible!" : "Player 2 not invincible!");
  }
  else
  {
    showMessage(clRed, clYellow, 40, 1, "Players     :" + gameData.player1.name);
    showMessage(clRed, clYellow, 40, 2, "Score       :" + tostring(gameData.player1.score));
    showMessage(clRed, clYellow, 40, 3, "Mice eaten  :" + tostring(gameData.player1.collectedMice));
    showMessage(clRed, clYellow, 40, 4, "Best score  :" + ((gameData.player1.bestScore == 0) ? "500" : tostring(gameData.player1.bestScore)));
  }

  showMessage(clRed, clYellow, 40, 5, "Timer:" + tostring(gameData.timer));

  showMessage(clRed, clYellow, 40, 6, "TO MOVE - P1: ARROW KEYS  P2: WASD ");
  showMessage(clRed, clYellow, 40, 7, "TO QUIT - ENTER 'Q'");
  showMessage(clRed, clYellow, 40, 8, "TO CHEAT - ENTER 'C'");
  showMessage(clRed, clYellow, 40, 9, "TO DISPLAY LEADERBOARDS - ENTER 'B'");
  showMessage(clRed, clYellow, 40, 10, "Date:" + getDate() + "|Time:" + getTime());
  showMessage(clBlack, clWhite, 40, 11, gameData.message);
  showMessage(clRed, clYellow, 40, 12, ((gameData.isCheating) ? "Cheating: On!" : "Cheating: Off!"));
  showMessage(clRed, clYellow, 40, 13, (gameData.player1.isInvincible) ? "Player 1 invincible!" : "Player 1 not invincible!");
  showMessage(clRed, clYellow, 40, 16, "Group:CS4G1b");
  showMessage(clRed, clYellow, 40, 17, "Dan Peverley      (28010687)");
  showMessage(clRed, clYellow, 40, 18, "Jared Bruty       (28004627)");
  showMessage(clRed, clYellow, 40, 19, "Bailey Saunders   (28022868)");
  showMessage(clRed, clYellow, 40, 20, "Ben Simpson       (28014245)");


  //display grid contents
  paintGrid(g, gameData);
}

void paintGrid(const char g[][SIZEX], const GameData gameData)
{ //display grid content on screen
  void setColourOfItem(const char g, const GameData gameData);
  gotoxy(0, 2);

  for (int row(0); row < SIZEY; ++row)
  {
    for (int col(0); col < SIZEX; ++col)
    {
      setColourOfItem(g[row][col], gameData);
      cout << g[row][col];	//output cell content
    }
    cout << endl;
  }
}

void setColourOfItem(const char g, const GameData gameData)
{
  switch (g)
  {
  case BODY:
  case SPOT:
    if (gameData.player1.isInvincible)
      selectTextColour(clBlue); //snake colour = blue when invincible
    else
      selectTextColour(clGreen); //snake colour = green
    break;
  case MONGOOSE:
    selectTextColour(random(15)); //random mongoose colour
    break;
  case DOT:
  case DODY:
    if (gameData.player2.isInvincible)
      selectTextColour(clBlue); //snake colour = blue when invincible
    else
      selectTextColour(clRed); //snake colour = red
    break;
  default:
    selectBackColour(clBlack);
    selectTextColour(clWhite);
    break;
  }
}

int ReadBestScore(const string playerName) {
  int bestScore = 500;
  fstream fio("scores/" + playerName + ".txt", ios::in);
  fio >> bestScore;
  fio.close();
  return bestScore;
}
void WriteBestScore(const string playerName, const int score) {
  fstream fio("scores/" + playerName + ".txt", ios::out);
  fio << score;
  fio.close();
}


void ReadLeaderboard(vector<Player>& leaderboard) { // From the file
  void InitLeaderboard(vector<Player>& leaderboard);
  ifstream fin("scores/bestScores.txt", ios::in);

  while (fin)
  {
    string leaderboardEntry;
    Player tempPlayer;
    fin >> leaderboardEntry;
    int delimiter = leaderboardEntry.find(":");//Value of delimiter

    tempPlayer.name = leaderboardEntry.substr(0, delimiter);
    tempPlayer.bestScore = atoi((leaderboardEntry.substr(delimiter + 1, leaderboardEntry.size() - delimiter).c_str()));
    if (tempPlayer.name != "")
    {
      leaderboard.push_back(tempPlayer);
    }
  }
  if (leaderboard.size() < 6)
  {
    InitLeaderboard(leaderboard);
  }
  fin.close();
}

void WriteLeaderboard(const vector<Player> leaderboard) {// To the file
  fstream fio("scores/bestScores.txt", ios::out);
  for (int i = 0; i < 6; i++)
  {
    fio << leaderboard.at(i).name << ':' << leaderboard.at(i).bestScore << endl;
  }
  fio.close();
}

void InitLeaderboard(vector<Player>& leaderboard) {
  for (int i = leaderboard.size(); i < 6; i++)
  {
    Player player;
    player.name = "Anonymous";
    player.bestScore = -1;
    leaderboard.push_back(player);
  }
}

void MaintainLeaderboard(vector<Player>& leaderboard, const Player player) {
  int insertIndex(0);
  for (int j = leaderboard.size() - 1; j >= 0; j--)//Reverse loop as it is changing size during runtime, also runs from best to worst
  {
    if (leaderboard.at(j).bestScore < player.bestScore && leaderboard.at(j).bestScore != -1) {//If my score is lower(better) or the score is a placeholder
      insertIndex++;
      if (leaderboard.at(j).name == player.name)
      {
        insertIndex = 7;//Force the player score on the leaderboard outside the scoreboard
      }
    } //Insert in the correct place. 
    else if (leaderboard.at(j).name == player.name) {
      //If the name is the same and the list has a lower or equal score
      leaderboard.erase(leaderboard.begin() + j);//Remove the duplicate
    }
  }
  if (insertIndex < 7)
  {
    leaderboard.insert(leaderboard.begin() + insertIndex, player);//All of the same person is now gone and the position is known
  }
}

void DisplayLeaderboard(const vector<Player> leaderboard) {
  int getKeyPress();
  do //display leaderboard until B key i pressed again
  {
    for (int i = 0; i < leaderboard.size(); i++)
    {
      showMessage(clRed, clYellow, 10, 10 + i, leaderboard.at(i).name + " " + tostring(leaderboard.at(i).bestScore));
    }
    showMessage(clRed, clYellow, 10, 10 + leaderboard.size(), "press B to resume");
  } while (toupper(getKeyPress()) != 'B');
  //Clear Text
  for (int i = 0; i < leaderboard.size(); i++)
  {
    showMessage(clBlack, clYellow, 10, 10 + i, "");
  }
  showMessage(clBlack, clYellow, 10, 10 + leaderboard.size(), "");
}

void ToggleCheats(GameData& gameData) {
  void resetSnake(vector<Item>& snake);
  cout << '\a';
  gameData.isCheating = !gameData.isCheating;
  if (!gameData.hasCheated)
    gameData.hasCheated = true;
  if (gameData.isCheating)
  {
    resetSnake(gameData.player1.snake);
    if (gameData.twoPlayer)
      resetSnake(gameData.player2.snake);
  }

}

void resetSnake(vector<Item>& snake)
{
  snake.resize(4);
}

void endProgram(const string message)
{
  showMessage(clRed, clYellow, 0, 18, message);
  showMessage(clRed, clYellow, 0, 19, "Have fun in the next snek game!");
  cout << "\n";
  system("pause");	//hold output screen until a keyboard key is hit
}