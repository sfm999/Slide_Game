
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

/*
    NIGHT REALM

    Seems that the night realms stones are initiated by negative numbers. I'm assuming that
    the negative numbers work the same way positives do in that if it's a tnt barrel but in the night realm,
    it still explodes in a radius around itself.

    The night realm only shows after a shift and it won't show on the initial printing of the map.
    The night realm is also unrotateable, so there may need to be some sort of state that dictates
    whether night or day. This could theoretically be done from having a variable in the main
    that represents the night realm being on or off, and this variable can be accessed by the
    main functionality loop that grabs all the user inputs. This way we can provide some sort of context
    such as if the night realm is activated, then any rotation commands are ignored until daytime mode,
    and we just print the map. Seems fairly simple.

    The mirror mode seems even simpler in that we have a new type of block, '2', which represents a mirror
    shot. The laser upon hitting the mirror object will stop its horizontal trajectory and the laser
    will shoot out both sides, perpendicular to the mirror block (above and below it) to that of
    half the power of the ORIGINAL shot. The original shot always has a potential of 4 objects
    destroyed (let's forget about tnt for now as it isn't that relevant yet); this means that the two
    perpendicular shots being produced by the mirror block EACH have a potential of 2 objects destroyed, respectively.

    TILL NEXT TIME
*/


// Map functions
int print_map(int map[SIZE][SIZE], int laser_y, int context);
void place_stone(int map[SIZE][SIZE], int row, int column, int value);
int shift_left(int map[SIZE][SIZE], int laser_y);

// Field rotation functions
void rotate_90_clockwise(int map[SIZE][SIZE]);
void rotate_90_anticlockwise(int map[SIZE][SIZE]);
void transpose(int map[SIZE][SIZE]);
void reflect(int map[SIZE][SIZE]);

void fire_laser(int map[SIZE][SIZE], int laser_y);
void euclidean_distance(int map[SIZE][SIZE], int x1, int y1);


int main(int argc, char *argv[])
{

  // The map
  int map[SIZE][SIZE] = {EMPTY};

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
    place_stone(map, row, column, value);
    stone_count++;
  }

  // Print map to show blocks have been added
  print_map(map, laser_y, PLAY_ON);

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
          print_map(map, laser_y, PLAY_ON);
        }
      }else if(cmd2 == UP)
      {
        if(laser_y + 1 >= SIZE){
          continue;
        }else{
          ++laser_y;
          print_map(map, laser_y, PLAY_ON);
        }
      }
    }else if(cmd1 == FIRE_LASER)
    {
      fire_laser(map, laser_y);
      if(print_map(map, laser_y, FIRE_LASER) == WIN)
      {
        printf("You Won!\n");
        break;
      }
    }else if(cmd1 == SHIFT)
    {
      if(shift_left(map, laser_y) == -1)
      {
        disable = 0;
        print_map(map, laser_y, SHIFT);
        printf("Game Lost!\n");
        break;
      }else{
        print_map(map, laser_y, SHIFT);
      }
    }else if(cmd1 == ROTATE)
    {
      scanf("%d", &cmd2);
      if(cmd2 == 1)
      {
        rotate_90_clockwise(map);
      }else if(cmd2 == 2)
      {
        rotate_90_anticlockwise(map);
      }
      print_map(map, laser_y, 4);
    }


    cmd1 = 0;
    cmd2 = 0;
  }


  return 0;
}

// Prints the map using two while loops and will also place the arrow on
//  the same line as the laser to indicate laser line currently.
// Context is used to determine context of the map printing so
//  we can check conditions accordingly.
int print_map(int map[SIZE][SIZE], int laser_y, int context)
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
      if(map[i][j] == 1)
      {
        win = -1;
      }
      printf("%d ", map[i][j]);
      j++;
    }
    printf("\n");
    i++;
  }
  if(win == -1 && context == 2)
  {
    return LOSE;
  }else{
    return WIN;
  }
  return PLAY_ON;
}

// Function to place stone. If row||column >= SIZE, return, else place on map
void place_stone(int map[SIZE][SIZE], int row, int column, int value)
{
  if(row >= SIZE || column >= SIZE)
    return;
  else
    map[row][column] = value;
}


void fire_laser(int map[SIZE][SIZE], int laser_y)
{
  int j = 0;
  int count = 0;
  while(j < SIZE)
  {
    if(count >= 4)
    {
      return;
    }

    if(map[laser_y][j] == 1)
    {
      map[laser_y][j] = EMPTY;
      count++;
    }

    if(map[laser_y][j] >= 4 && map[laser_y][j] <= 9)
    {
      euclidean_distance(map, laser_y, j);
      map[laser_y][j] = EMPTY;
      return;
    }

    j++;
  }
}

void euclidean_distance(int map[SIZE][SIZE], int x1, int y1)
{
  int x2 = 0;
  while(x2 < SIZE)
  {
    int y2 = 0;
    while(y2 < SIZE)
    {
      if((x2 == x1) && (y2 == y1))
      {
        ++y2;
        continue;
      }
      double d = sqrt(pow((x2 - x1), 2.0) + pow((y2 - y1), 2.0));
      if(d < map[x1][y1])
      {
        map[x2][y2] = EMPTY;
      }
      ++y2;
    }
    ++x2;
  }
}

int shift_left(int map[SIZE][SIZE], int laser_y)
{
  int i = 0;
  while(i < SIZE)
  {
    int j = 0;
    while(j < SIZE)
    {
      if(j == 0 && map[i][j] == 1)
        return LOSE;

      if(j == SIZE - 1)
        map[i][j] = EMPTY;
      else
        map[i][j] = map[i][j+1];

      j++;
    }
    i++;
  }
  return PLAY_ON;
}

void rotate_90_clockwise(int map[SIZE][SIZE])
{
  transpose(map);
  reflect(map);
}

void rotate_90_anticlockwise(int map[SIZE][SIZE])
{
  reflect(map);
  transpose(map);
}

void transpose(int map[SIZE][SIZE])
{
  int i = 0;
  while(i < SIZE)
  {
    int j = 0;
    while(j < i)
    {
      int temp = map[i][j];
      map[i][j] = map[j][i];
      map[j][i] = temp;
      ++j;
    }
    ++i;
  }
}

void reflect(int map[SIZE][SIZE])
{
  int i = 0;
  int N = sizeof(map[0]) / sizeof(map[0][0]);
  while(i < N)
  {
    int j = 0;
    while(j < N/2)
    {
      int temp = map[i][j];
      map[i][j] = map[i][N - j - 1];
      map[i][N - j - 1] = temp;
      ++j;
    }
    ++i;
  }
}
