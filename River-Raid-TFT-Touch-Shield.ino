#include <stdint.h>
#include <SeeedTouchScreen.h>
#include <TFTv2.h>
#include <SPI.h>
#include <Math.h>

// additional color constants
#define CYAN2		0x08ff

#define PLANE_X_VEL       2
#define PLANE_ROW_SIZE    7
#define PLANE_SIZE        63

#define MISSILE_Y_VEL     3
#define MISSILE_Y_TAIL    8

unsigned int colors[11] = {BLACK, YELLOW, BLUE, GREEN, RED, WHITE, CYAN, CYAN2, BRIGHT_RED, GRAY1, GRAY2};

char plane_straight[PLANE_SIZE + 1] = "0001000"
                          "0001000"
                          "0011100"
                          "0111110"
                          "1111111"
                          "1101011"
                          "1001001"
                          "0011100"
                          "0101010";                          

char plane_right[PLANE_SIZE + 1] =    "0001000"
                          "0001000"
                          "0011000"
                          "0111100"
                          "0101110"
                          "0001010"
                          "0011000"
                          "0101100"
                          "0000010";

char plane_left[PLANE_SIZE + 1] =     "0001000"
                          "0001000"
                          "0001100"
                          "0011110"
                          "0111010"
                          "0101000"
                          "0001100"
                          "0011010"
                          "0100000";
                          
char plane_off[PLANE_SIZE + 1] =      "0000000"
                          "0000000"
                          "0000000"
                          "0000000"
                          "0000000"
                          "0000000"
                          "0000000"
                          "0000000"
                          "0000000";

char plane_enemy[49] =    "00000006"
                          "06600066"
                          "77777777"
                          "77770077"
                          "00077700"
                          "00007700";                          

int plane_x = 116;
int plane_y = 280;
int missile_y = -1;
int missile_x = 0;



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
      draw_sprite(plane_x, plane_y, plane_off, PLANE_SIZE, PLANE_ROW_SIZE);
      plane_x -=PLANE_X_VEL;
      draw_sprite(plane_x, plane_y, plane_left, PLANE_SIZE, PLANE_ROW_SIZE);
    }
    else
    {
      draw_sprite(plane_x, plane_y, plane_off, PLANE_SIZE, PLANE_ROW_SIZE);
      plane_x +=PLANE_X_VEL;
      draw_sprite(plane_x, plane_y, plane_right, PLANE_SIZE, PLANE_ROW_SIZE);
    }
  }
  else
  {
    draw_sprite(plane_x, plane_y, plane_straight, PLANE_SIZE, PLANE_ROW_SIZE);
  }


  if(missile_y < -MISSILE_Y_TAIL)
  {
    fire_missile();
  }
  
  update_missile();

}

void draw_sprite(int x, int y, char sprite[], int spriteSize, int rowSize)
{
  int colorId;
  unsigned int color;
  
  int column;
  int line;
  
  column = 0;
  line = 0;
  
  for(int i = 0; i < spriteSize; i ++)
  {
    colorId = (int) sprite[i] - '0';   
    color = colors[colorId];
  
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
  missile_y -= MISSILE_Y_VEL;
  draw_missile(missile_x, missile_y);
  
  Serial.println(missile_y);
}
