#include <stdint.h>
#include <SeeedTouchScreen.h>
#include <TFTv2.h>
#include <SPI.h>
#include <Math.h>

// additional color constants
//#define CYAN2		0x07ff

#define SCREEN_X          240
#define SCREEN_Y          320

#define PLANE_X_VEL       2
#define PLANE_ROW_SIZE    7
#define PLANE_SIZE        64

#define MISSILE_Y_VEL     3
#define MISSILE_Y_TAIL    8

int plane_x = 116;
int plane_y = 280;
int missile_y = -1;
int missile_x = 0;
int enemy_plane_x = 320;
int enemy_plane_y = 100;
int chopter_0_x = 100;
int chopter_0_y = 100;
int chopter_0_dir = 1;

int clock = 0;

unsigned int colors[10] = {BLACK, YELLOW, BLUE, GREEN, RED, WHITE, CYAN, BRIGHT_RED, GRAY1, GRAY2};

char plane_straight[PLANE_SIZE] = 
"0001000"
"0001000"
"0011100"
"0111110"
"1111111"
"1101011"
"1001001"
"0011100"
"0101010";                          

char plane_right[PLANE_SIZE] =
"0001000"
"0001000"
"0011000"
"0111100"
"0101110"
"0001010"
"0011000"
"0101100"
"0000010";

char plane_left[PLANE_SIZE] =
"0001000"
"0001000"
"0001100"
"0011110"
"0111010"
"0101000"
"0001100"
"0011010"
"0100000";
                          
char plane_off[PLANE_SIZE] =
"0000000"
"0000000"
"0000000"
"0000000"
"0000000"
"0000000"
"0000000"
"0000000"
"0000000";

char plane_enemy[49] =
"00000006"
"06600066"
"55555555"
"55550055"
"00055500"
"00005500";

char chopter_a[73] =
"00444000"
"44400000"
"00400000"
"01110000"
"11111001"
"22222222"
"01110001"
"00100000"
"01110000";

char chopter_b[73] =
"44400000"
"00444000"
"00400000"
"01110000"
"11111001"
"22222222"
"01110001"
"00100000"
"01110000";

char boat[129] =
"0000001100000000"
"0000001100000000"
"0000111100000000"
"0011111111000000"
"3333333333333333"
"3333333333333300"
"2222222222220000"
"0022222222220000";

char fuel[204] =
"0444440"
"4411144"
"4414444"
"4414444"
"4411144"
"4414444"
"4444444"
"6666666"
"6616166"
"6616166"
"6616166"
"6616166"
"6611166"
"6666666"
"4444444"
"4411144"
"4414444"
"4411444"
"4414444"
"4411144"
"4444444"
"6666666"
"6616666"
"6616666"
"6616666"
"6616666"
"6611166"
"6666666"
"6666666";

TouchScreen ts = TouchScreen(XP, YP, XM, YM); //init TouchScreen port pins

void setup() {

  Tft.TFTinit();  //init TFT library
  Serial.begin(115200);

}

void loop() {
  
  // manage input
  Point p = ts.getPoint();
  
  p.x = map(p.x, TS_MINX, TS_MAXX, 0, 240);
  p.y = map(p.y, TS_MINY, TS_MAXY, 0, 320);  

  if (p.z > __PRESURE)
  {
    if(p.x < 120)
    {
      //draw_sprite(plane_x, plane_y, plane_off, PLANE_SIZE, PLANE_ROW_SIZE);
      clear_sprite(plane_x, plane_y, BLACK, PLANE_SIZE, PLANE_ROW_SIZE);
      plane_x -=PLANE_X_VEL;
      draw_sprite(plane_x, plane_y, plane_left, PLANE_SIZE, PLANE_ROW_SIZE);
    }
    else
    {
      //draw_sprite(plane_x, plane_y, plane_off, PLANE_SIZE, PLANE_ROW_SIZE);
      clear_sprite(plane_x, plane_y, BLACK, PLANE_SIZE, PLANE_ROW_SIZE);
      plane_x +=PLANE_X_VEL;
      draw_sprite(plane_x, plane_y, plane_right, PLANE_SIZE, PLANE_ROW_SIZE);
    }
  }
  else
  {
    //clear_sprite(plane_x, plane_y, BLACK, PLANE_SIZE, PLANE_ROW_SIZE);
    draw_sprite(plane_x, plane_y, plane_straight, PLANE_SIZE, PLANE_ROW_SIZE);
  }


  // missile update
  update_missile();

  // enemies update
  update_enemies();
  
  // clock update
  clock ++;
}

void draw_sprite(int x, int y, char sprite[], int spriteSize, int rowSize)
{
  int colorId;
  unsigned int color;
  
  int column;
  int line;
  
  column = 0;
  line = 0;
  
  for(int i = 0; i < (spriteSize - 1); i ++)
  {
    colorId = (int) sprite[i] - '0';   
    color = colors[colorId];
  
    if(i > 0 && i % rowSize == 0)
    {
      line++;
      column = 0;
    }
    column ++;
  
    if(check_boundaries(column + x, line + y))
    {  
      Tft.setPixel(column + x, line + y, color);
    }
  }
}

bool check_boundaries(int x, int y)
{
  return (x > 0 && x < SCREEN_X) && (y > 0 && y < SCREEN_Y);
}

void clear_sprite(int x, int y, unsigned int color, int spriteSize, int rowSize)
{
  int column;
  int line;
  
  column = 0;
  line = 0;
  
  for(int i = 0; i < (spriteSize - 1); i ++)
  { 
    if(i > 0 && i % rowSize == 0)
    {
      line++;
      column = 0;
    }
    column ++;
    
    Tft.setPixel(column + x, line + y, color);
  }  
}

void draw_missile(int x, int y)
{
  int tail = y;
  unsigned int color;
  
  for(int i = 0; i < MISSILE_Y_TAIL * 2; i ++)
  {
    color = (i < MISSILE_Y_TAIL) ? YELLOW: BLACK;
    if(tail > 0)
    {
      Tft.setPixel(x, tail, color);  
    }
    tail++;
  }
}


void fire_missile()
{
  missile_x = plane_x + 4;
  missile_y = plane_y;
}

void update_missile()
{
  if(missile_y < -MISSILE_Y_TAIL)
  {
    fire_missile();
  }

  missile_y -= MISSILE_Y_VEL;
  draw_missile(missile_x, missile_y);
  
}

void update_enemies()
{
  // plane test
  if(enemy_plane_x < -10)
  {
    enemy_plane_x = SCREEN_X;
  }
  
  if(clock % 30 == 0)
  {
    enemy_plane_y ++;
  }
  
  clear_sprite(enemy_plane_x, enemy_plane_y - 2, BLACK, 60, 10);
  enemy_plane_x -= 1;
  draw_sprite(enemy_plane_x, enemy_plane_y, plane_enemy, 49, 8);
  
  // chopter test
  
  chopter_0_x += chopter_0_dir;
  
  if(chopter_0_x < 10  || chopter_0_x >= SCREEN_X - 10 )
  {
    chopter_0_dir *= -1;
  }
  
  if(clock % 2 == 0)
  {
    clear_sprite(chopter_0_x -2, chopter_0_y, BLACK, 110, 12);
    draw_sprite(chopter_0_x, chopter_0_y, chopter_a, 73, 8);
  }
  else
  {
    clear_sprite(chopter_0_x -2, chopter_0_y, BLACK, 110, 12);
    draw_sprite(chopter_0_x, chopter_0_y, chopter_b, 73, 8);
  }
}
