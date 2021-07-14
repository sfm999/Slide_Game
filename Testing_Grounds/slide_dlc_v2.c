
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>


#define SIZE 15
#define EMPTY 0
#define STONE 1

#define WIN 1
#define PLAY_ON 0
#define LOSE -1

#define MOVE 1
#define DOWN -1
#define UP 1
#define FIRE_LASER 2
#define SHIFT 3
#define ROTATE 4

#define CLOCKWISE 1
#define ANTI_CLOCKWISE 2

#define NIGHT 1
#define DAY 0

#define NORMAL  "\x1B[0m"
#define RED  "\x1B[31m"
#define GREEN  "\x1B[32m"
#define YELLOW  "\x1B[33m"
#define BLUE  "\x1B[34m"
#define MAGENTA  "\x1B[35m"
#define CYAN  "\x1B[36m"
#define WHITE  "\x1B[37m"


/*
    NIGHT REALM

    Seems that the night realms stones are initiated by negative numbers. I'm assuming that
    the negative numbers work the same way positives do in that if it's a tnt barrel but in the night realm,
    it still explodes in a radius around itself.

    The night realm only shows after a shift and it won't show on the initial printing of the realm.
    The night realm is also unrotateable, so there may need to be some sort of state that dictates
    whether night or day. This could theoretically be done from having a variable in the main
    that represents the night realm being on or off, and this variable can be accessed by the
    main functionality loop that grabs all the user inputs. This way we can provide some sort of context
    such as if the night realm is activated, then any rotation commands are ignored until daytime mode,
    and we just print the realm. Seems fairly simple.

    The mirror mode seems even simpler in that we have a new type of block, '2', which represents a mirror
    shot. The laser upon hitting the mirror object will stop its horizontal trajectory and the laser
    will shoot out both sides, perpendicular to the mirror block (above and below it) to that of
    half the power of the ORIGINAL shot. The original shot always has a potential of 4 objects
    destroyed (let's forget about tnt for now as it isn't that relevant yet); this means that the two
    perpendicular shots being produced by the mirror block EACH have a potential of 2 objects destroyed, respectively.

    TILL NEXT TIME
*/


// realm functions
int print_realm(int realm[SIZE][SIZE], int laser_y, int context);
void place_stone(int realm[SIZE][SIZE], int row, int column, int value);
int shift_left(int realm[SIZE][SIZE], int laser_y);

// Field rotation functions
void rotate_90_clockwise(int realm[SIZE][SIZE]);
void rotate_90_anticlockwise(int realm[SIZE][SIZE]);
void transpose(int realm[SIZE][SIZE]);
void reflect(int realm[SIZE][SIZE]);

void fire_laser(int realm[SIZE][SIZE], int laser_y);
void euclidean_distance(int realm[SIZE][SIZE], int x1, int y1);


int main(int argc, char *argv[])
{

  // The realm
  int day_realm[SIZE][SIZE] = {EMPTY};
  int night_realm[SIZE][SIZE] = {EMPTY};

  // If night mode is set to 0, then night mode is inactive.
  // Conversely, if night mode is set to 1, then night mode is active.
  // Shifting the board switching night mode on and off per shift.
  int night_mode = DAY;

  // Rounds down to 7 thereby placing it in the middle of the screen
  int laser_y = SIZE / 2;

  printf("How many blocks? ");
  int block_amount;
  scanf("%d", &block_amount);

  // Scan in the blocks
  printf("Enter blocks:\n");
  int row, column, value, stone_count = 0;

  while(stone_count < block_amount)
  {
    scanf("%d%d%d", &row, &column, &value);
    if(value < 0)
      place_stone(night_realm, row, column, value);
    else
      place_stone(day_realm, row, column, value);
    stone_count++;
  }
  // Print realm to show blocks have been added
  (night_mode == 0) ? print_realm(day_realm, laser_y, SHIFT) : print_realm(night_realm, laser_y, SHIFT);

  int cmd1, cmd2, disable = 1;

  // Continue grabbing values till disable flag raised
  while(scanf("%d", &cmd1) == 1 && disable == 1)
  {
    if(cmd1 == MOVE)
    {
      scanf("%d", &cmd2);
      if(cmd2 == DOWN){
        if(laser_y - 1 < 0)
        {
          continue;
        }else{
          --laser_y;
          if(night_mode == DAY)
            print_realm(day_realm, laser_y, PLAY_ON);
          else if(night_mode == NIGHT)
            print_realm(night_realm, laser_y, PLAY_ON);
        }
      }else if(cmd2 == UP)
      {
        if(laser_y + 1 >= SIZE){
          continue;
        }else{
          ++laser_y;
          if(night_mode == DAY)
            print_realm(day_realm, laser_y, PLAY_ON);
          else if(night_mode == NIGHT)
            print_realm(night_realm, laser_y, PLAY_ON);
        }
      }
    }else if(cmd1 == FIRE_LASER)
    {
      if(night_mode == DAY)
      {
        fire_laser(day_realm, laser_y);
        if(print_realm(day_realm, laser_y, FIRE_LASER) == WIN)
        {
          printf("You Won!\n");
          break;
        }
      }else if(night_mode == NIGHT)
      {
        fire_laser(night_realm, laser_y);
        if(print_realm(night_realm, laser_y, FIRE_LASER) == WIN)
        {
          printf("You Won!\n");
          break;
        }
      }

    }else if(cmd1 == SHIFT)
    {
      if(night_mode == DAY){
        night_mode = NIGHT;
      }else if(night_mode == NIGHT){
        night_mode = DAY;
      }
      int game_one = shift_left(day_realm, laser_y);
      int game_two = shift_left(night_realm, laser_y);
      if(game_one == LOSE || game_two == LOSE)
      {
        disable = 0;
        print_realm(day_realm, laser_y, SHIFT);
        printf("Game Lost!\n");
        break;
      }else{
        if(night_mode == DAY){
          print_realm(day_realm, laser_y, SHIFT);
          night_mode = NIGHT;
        }else if(night_mode == NIGHT){
          print_realm(night_realm, laser_y, SHIFT);
          night_mode = DAY;
        }
      }
    }else if(cmd1 == ROTATE)
    {
      scanf("%d", &cmd2);
      if(cmd2 == 1)
      {
        if(night_mode == DAY){
          rotate_90_clockwise(day_realm);
        }else if(night_mode == NIGHT){
          rotate_90_clockwise(night_realm);
        }
      }else if(cmd2 == 2)
      {
        if(night_mode == DAY){
          rotate_90_anticlockwise(day_realm);
        }else if(night_mode == NIGHT){
          rotate_90_anticlockwise(night_realm);
        }
      }
      if(night_mode == DAY){
        print_realm(day_realm, laser_y, ROTATE);
      }else if(night_mode == NIGHT){
        print_realm(night_realm, laser_y, ROTATE);
      }
    }
    cmd1 = 0;
    cmd2 = 0;
  }
  return 0;
}

// Prints the realm using two while loops and will also place the arrow on
//  the same line as the laser to indicate laser line currently.
// Context is used to determine context of the realm printing so
//  we can check conditions accordingly.
int print_realm(int realm[SIZE][SIZE], int laser_y, int context)
{
  int i = 0;
  int win = 0;
  while(i < SIZE)
  {

    // If i lands on the row that laser_y is currently on
    if(i == laser_y){
      printf("> ");
    }else{
      printf("  ");
    }
    int j = 0;
    while(j < SIZE){
      int val = realm[i][j];
      if(realm[0][j] == 1)
      {
        win = -1;
      }
      if(i == 0 || j == 0){
        printf("%s%d%s ", RED, val, NORMAL);
      }else if(i == 14 || j == 14){
        printf("%s%d%s ", RED, val, NORMAL);
      }else{
        if(val == 1){
          printf("%s%d%s ", BLUE, val, NORMAL);
        }else if(val >= 4 && val <= 9){
          printf("%s%d%s ", GREEN, val, NORMAL);
        }else{
          printf("%s%d%s ", MAGENTA, val, NORMAL);
        }
      }
      ++j;
    }
    printf("\n");
    ++i;
  }
  if(win == -1 && context == 2)
  {
    return LOSE;
  }
  return PLAY_ON;
}

// Function to place stone. If row||column >= SIZE, return, else place on realm
void place_stone(int realm[SIZE][SIZE], int row, int column, int value)
{
  if(row >= SIZE || column >= SIZE){
    return;
  }else{
    if(value < 0){
      realm[row][column] = value * -2 / 2;
    }else{
      realm[row][column] = value;
    }
  }
}

void fire_laser(int realm[SIZE][SIZE], int laser_y)
{
  int j = 0;
  int count = 0;
  while(j < SIZE){
    if(count >= 4){
      break;
    }
    if(realm[laser_y][j] == 1){
      realm[laser_y][j] = EMPTY;
      ++count;
      continue;
    }

    if(realm[laser_y][j] >= 4 && realm[laser_y][j] <= 9){
      euclidean_distance(realm, laser_y, j);
      realm[laser_y][j] = EMPTY;
      return;
    }
    ++j;
  }
  count = 0;
  return;
}

void euclidean_distance(int realm[SIZE][SIZE], int x1, int y1)
{
  int x2 = 0;
  while(x2 < SIZE){
    int y2 = 0;
    while(y2 < SIZE)
    {
      if((x2 == x1) && (y2 == y1))
      {
        ++y2;
        continue;
      }
      double d = sqrt(pow((x2 - x1), 2.0) + pow((y2 - y1), 2.0));
      if(d < realm[x1][y1])
      {
        realm[x2][y2] = EMPTY;
      }
      ++y2;
    }
    ++x2;
  }
}

int shift_left(int realm[SIZE][SIZE], int laser_y)
{
  int i = 0;
  while(i < SIZE){
    int j = 0;
    while(j < SIZE)
    {
      if(j == 0 && realm[i][j] == 1)
        return LOSE;

      if(j == SIZE - 1)
        realm[i][j] = EMPTY;
      else
        realm[i][j] = realm[i][j+1];

      j++;
    }
    i++;
  }
  return PLAY_ON;
}

void rotate_90_clockwise(int realm[SIZE][SIZE])
{
  transpose(realm);
  reflect(realm);
}

void rotate_90_anticlockwise(int realm[SIZE][SIZE])
{
  reflect(realm);
  transpose(realm);
}

void transpose(int realm[SIZE][SIZE])
{
  int i = 0;
  while(i < SIZE){
    int j = 0;
    while(j < i)
    {
      int temp = realm[i][j];
      realm[i][j] = realm[j][i];
      realm[j][i] = temp;
      ++j;
    }
    ++i;
  }
}

void reflect(int realm[SIZE][SIZE])
{
  int i = 0;
  int N = sizeof(realm[0]) / sizeof(realm[0][0]);
  while(i < N){
    int j = 0;
    while(j < N/2)
    {
      int temp = realm[i][j];
      realm[i][j] = realm[i][N - j - 1];
      realm[i][N - j - 1] = temp;
      ++j;
    }
    ++i;
  }
}
