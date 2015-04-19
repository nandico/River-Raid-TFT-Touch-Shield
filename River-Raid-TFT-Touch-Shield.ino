#include <stdint.h>
#include <SeeedTouchScreen.h>
#include <TFTv2.h>
#include <SPI.h>
#include <Math.h>

/*
#define RED		0xf800
#define GREEN	0x07e0
#define BLUE	0x001f
#define BLACK	0x0000
#define YELLOW	0xffe0
#define WHITE	0xffff

//Other Colors
#define CYAN		0x07ff	
#define BRIGHT_RED	0xf810	
#define GRAY1		0x8410  
#define GRAY2		0x4208 
*/

#define CYAN2		0x08ff

unsigned int colors[11] = {BLACK, YELLOW, BLUE, GREEN, RED, WHITE, CYAN, CYAN2, BRIGHT_RED, GRAY1, GRAY2};

char plane_straight[64] = "0001000"
                          "0001000"
                          "0011100"
                          "0111110"
                          "1111111"
                          "1101011"
                          "1001001"
                          "0011100"
                          "0101010";                          

char plane_right[64] =    "0001000"
                          "0001000"
                          "0011000"
                          "0111100"
                          "0101110"
                          "0001010"
                          "0011000"
                          "0101100"
                          "0000010";

char plane_left[64] =     "0001000"
                          "0001000"
                          "0001100"
                          "0011110"
                          "0111010"
                          "0101000"
                          "0001100"
                          "0011010"
                          "0100000";
                          
char plane_off[64] =      "0000000"
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

#define PLANE_X_VEL       2
#define MISSILE_Y_VEL     3
#define MISSILE_Y_TAIL    8

TouchScreen ts = TouchScreen(XP, YP, XM, YM); //init TouchScreen port pins

void setup() {

  Tft.TFTinit();  //init TFT library
  Serial.begin(115200);
  


}

void loop() {
  // put your main code here, to run repeatedly:
  
  // test plane
  //draw_plane(random(0,240), random(0,320), plane_straight);

  // manage input
  Point p = ts.getPoint();
  
  p.x = map(p.x, TS_MINX, TS_MAXX, 0, 240);
  p.y = map(p.y, TS_MINY, TS_MAXY, 0, 320);  

  if (p.z > __PRESURE)
  {
    if(p.x < 120)
    {
      draw_plane(plane_x, plane_y, plane_off);
      plane_x -=PLANE_X_VEL;
      draw_plane(plane_x, plane_y, plane_left);
    }
    else
    {
      draw_plane(plane_x, plane_y, plane_off);
      plane_x +=PLANE_X_VEL;
      draw_plane(plane_x, plane_y, plane_right);
    }
  }
  else
  {
    draw_plane(plane_x, plane_y, plane_straight);
  }


  if(missile_y < -MISSILE_Y_TAIL)
  {
    fire_missile();
  }
  
  update_missile();

}

void draw_plane(int x, int y, char plane[])
{
  int colorId;
  unsigned int color;
  
  int column;
  int line;
  
  column = 0;
  line = 0;
  
  for(int i = 0; i < 63; i ++)
  {
    colorId = (int) plane[i] - '0';   
    color = colors[colorId];
  
    if(i > 0 && i % 7 == 0)
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
  // missile_x = plane_x; // follow missile
  draw_missile(missile_x, missile_y);
  
  Serial.println(missile_y);
}
