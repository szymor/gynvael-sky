// .S.k.y by Gynvael Coldwind of Vexillium
// http://gynvael.coldwind.pl
// http://vexillium.org
// mailto: gynvael@vexillium.org
//
// Port to Miyoo/Bittboy by vamastah
// mailto: szymorawski@gmail.com
//
// Note:
//  This version of .S.k.y. has yet to be optimised.
//  This version uses SDL (I've used 1.2.12), and has been tested on
//    Windows Vista SP1 (g++ Sky.cpp Vector3D.cpp -lSDL)
//    MacOSX 10.5.5     (g++ `sdl-config --cflags` Sky.cpp Vector3D.cpp `sdl-config --libs`)
//    Ubuntu 8.10       (g++ `sdl-config --cflags` Sky.cpp Vector3D.cpp `sdl-config --libs`)
//
//  Additional -O3 won't hurt, since it's pretty slow.
//
// License (BSD):
//  Copyright (c) 2008, Gynvael Coldwind of Vexillium
//  All rights reserved.
//
//  Redistribution and use in source and binary forms, with or without
//  modification, are permitted provided that the following conditions are met:
//      * Redistributions of source code must retain the above copyright
//        notice, this list of conditions and the following disclaimer.
//      * Redistributions in binary form must reproduce the above copyright
//        notice, this list of conditions and the following disclaimer in the
//        documentation and/or other materials provided with the distribution.
//      * Neither the name of the Gynvael Coldwind nor Vexillium nor the
//        names of its contributors may be used to endorse or promote products
//        derived from this software without specific prior written permission.
//
//  THIS SOFTWARE IS PROVIDED BY Gynvael Coldwind ''AS IS'' AND ANY
//  EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
//  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
//  DISCLAIMED. IN NO EVENT SHALL Gynvael Coldwind BE LIABLE FOR ANY
//  DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
//  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
//  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
//  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
//  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
//  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

//
// Includes
//
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <SDL/SDL.h>
#include <SDL/SDL_gfxPrimitives.h>
#include <math.h>
#include "Vector3D.h"

//
// Constants
//
#define WIDTH  320
#define HEIGHT 240

#define HMAP_WIDTH 1281
#define HMAP_DEPTH 641

//
// Function declarations
//


//
// Global variables
// (not to pretty, but still it works)
//

SDL_Surface *Screen;
double xPos, yPos;         // Mouse coordinates
unsigned char *picture; // RGB buffer
int *RenderCache;       // Buffer for indexes of the heightmap elements vs the picture buffer
float *hmap;            // Buffer containing the heightmap
Vector3D *NormalMap;    // Buffer containing the normalmap
int total_min_y;        // Variable containing the start y of the terrain (the terrain does not start from y=0)

Vector3D sun_pos    = Vector3D( -10000, 100.0f, -100); // Light position 
Vector3D sun_color  = Vector3D( 1.0f, 1.5f, 2.0f );    // Diffusive light color
Vector3D sun_color2 = Vector3D( 2.0f, 2.0f, 2.0f );    // Specular light color

int TH_from[4], TH_to[4];  // Threads
bool TH_done[4];           // Threads

int fps = 0;
bool fps_on = false;

//
// Structures
//
//#pragma pack(push, 1)
struct myRGB { 
  unsigned char r,g,b,a;
};
//#pragma pack(pop)

void fps_counter(double dt)
{
	static double total = 0;
	static int count = 0;
	total += dt;
	++count;
	if (total > 1.0)
	{
		fps = count;
		total -= 1.0;
		count = 0;
	}
}

void fps_draw(void)
{
	char string[8] = "";
	sprintf(string, "%d", fps);
	stringRGBA(Screen, 0, HEIGHT - 10, string, 255, 255, 255, 255);
}

//
// Function: calc_normal
//   Calculated a normal vector for a given triangle
// 
// In:
//   3 points that create a triangle
//
// Out:
//   Normal vector
//
Vector3D calc_normal(Vector3D v[3])
{
  Vector3D out = Vector3D(0,0,0);
  Vector3D v1, v2;

  // Do some math
  v1 = v[0] - v[1];
  v2 = v[1] - v[2];

  out.x = v1.y*v2.z - v1.z*v2.y;
  out.y = v1.z*v2.x - v1.x*v2.z;
  out.z = v1.x*v2.y - v1.y*v2.x;

  // Normalize
  out.Norm();

  // Done
  return out;
}

//
// Function: FileSetContent
//   Writes a given array to a given file
//
// In:
//   A file name, and an array
//
// Out:
//   true on success
//
bool FileSetContent(const char *filename, char *data, int sz)
{
  FILE *f;

  // Open the file
  f = fopen(filename, "wb");
  if(f)
  {
    // Write data
    fwrite(data, 1, sz, f);
    fclose(f);
  }
  else
  {
    // Return false
    return false;
  }

  // Return success
  return true;
}

//
// Function: FileGetContent
//   Read the content of a file to an array
//
// In:
//   A file name, and a variable for storing array/file size
//
// Out:
//   The array, NULL on failure
//
char *FileGetContent(const char *filename, int *sz)
{
  char *data;
  FILE *f;
  int size;

  // Open the file
  f = fopen(filename, "rb");
  if(!f)
    return NULL;

  // Get file size
  fseek(f, 0, SEEK_END);
  size = (int)ftell(f);
  fseek(f, 0, SEEK_SET);

  // Allocate
  // TODO Some check here?
  data = new char[size];

  // Read data
  fread(data, 1, size, f);
  fclose(f);

  // Write size
  *sz = size;

  // Return
  return data;
}

//
// Function: LoadHmap
//   Load the heightmap from the given file to the given array
//
// In:
//   The target array and a file name
//
// Out:
//   true on success
//
bool LoadHmap(float *hmap, const char *file)
{
  int size;
  unsigned char *data;

  // Get file content
  data = (unsigned char*)FileGetContent(file, &size);
  if(!data)
    return false; // Fail!

  // Copy with conversion to float
  int i;
  for(i = 0; i < HMAP_WIDTH * HMAP_DEPTH; i++)
    hmap[i] = (float)data[i];

  // Delete the unneded array
  delete data;
 
  // Return
  return true;
}

//
// Function: MakePicture
//   Prepares the rendering. This is mostly loading, calculating normals, and caching the rendering.
//
// In:
//   Nothing
//
// Out:
//   Nothing
//
void MakePicture()
{
  // Allocate an array for the hmap
  hmap = new float[HMAP_WIDTH * HMAP_DEPTH];

  // Some vars
  int x, y, z, i;
  //myRGB *p;

  // Gen seed
  /*  for(i = 0; i < HMAP_WIDTH * HMAP_DEPTH; i++)
    hmap[i] = ((float)(rand() % 25600)) / 100.0f;

  // Gen noise
  MakePerlinNoise(hmap);*/

  // Load the heightmap
  if(!LoadHmap(hmap, "himap.raw"))
  {
    // Error
    puts("hmap not found!");
    return;
  }

  // Perspective fix
  for(z = 0; z < HMAP_DEPTH; z++)
  {
    float mody = powf((float)(1 + HMAP_DEPTH - z), 0.1); // Play with this to get nice effects
    printf("                                                 \rPerspective fix: %i\r", z);
    for(x = 0; x < HMAP_WIDTH; x++)
      hmap[x + z * HMAP_WIDTH] /= mody;
  }

  // Precalc normals (or load)
  int size;

  // Try to load first
  NormalMap = (Vector3D*)FileGetContent("normal.cache", &size);

  if(!NormalMap)
  {
    // No cache, then recalculate (kinda slow)
    // First some allocations
    NormalMap = new Vector3D[HMAP_WIDTH * HMAP_DEPTH];
    Vector3D *PreNormalMap = new Vector3D[HMAP_WIDTH * HMAP_DEPTH];
  
    // And now, for each point, calculate the normals
    for(z = 1; z < HMAP_DEPTH; z++)
    {
      printf("                                                 \rPrecalulating normals: %i\r", z);
      for(x = 1; x < HMAP_WIDTH; x++)
      {
        // Make a triangle
        float max_y_prev[3] = {
          hmap[(x - 0) + (z - 0) * HMAP_WIDTH],
          hmap[(x - 1) + (z - 0) * HMAP_WIDTH],
          hmap[(x - 0) + (z - 1) * HMAP_WIDTH]
        };
        
        Vector3D vp_all[3] = {
          Vector3D( (float)x, (float)max_y_prev[0], (float)z ),
          Vector3D( (float)(x-1), (float)max_y_prev[1], (float)z ),
          Vector3D( (float)x, (float)max_y_prev[2], (float)(z-1) )
        };
  
        // Calc normal for that triangle
        // XXX the correct way should have two triangles with some avg. normal, but who cares ;p
        PreNormalMap[x + z * HMAP_WIDTH] = calc_normal(vp_all);
      }
    }
  
    // Smooth normal map
    // If we don't do it, the normalmap is noisy and all (it's to precise)
    for(z = 1; z < HMAP_DEPTH - 1; z++)
    {
      printf("                                                 \rBluring normals: %i\r", z);
      for(x = 1; x < HMAP_WIDTH - 1; x++)
      {
        // This is a simple blur and nothing more
        //NormalMap[x + z * HMAP_WIDTH] = PreNormalMap[x + z * HMAP_WIDTH];
        NormalMap[x + z * HMAP_WIDTH] = (
          PreNormalMap[x - 1 + z * HMAP_WIDTH] +
          PreNormalMap[x     + z * HMAP_WIDTH] +
          PreNormalMap[x + 1 + z * HMAP_WIDTH] +
          PreNormalMap[x - 1 + (z + 1) * HMAP_WIDTH] +
          PreNormalMap[x     + (z + 1) * HMAP_WIDTH] +
          PreNormalMap[x + 1 + (z + 1) * HMAP_WIDTH] +
          PreNormalMap[x - 1 + (z - 1) * HMAP_WIDTH] +
          PreNormalMap[x     + (z - 1) * HMAP_WIDTH] +
          PreNormalMap[x + 1 + (z - 1) * HMAP_WIDTH]) * (1.0f / 9.0f);
  
      }
    }
  
    // Delete the PreNormalMap, the NormalMap is enough
    delete PreNormalMap;
  
    // Save the normals in a cache file to use at the next run
    // Don't check for success ;>
    FileSetContent("normal.cache", (char*)NormalMap, sizeof(*NormalMap)*HMAP_WIDTH*HMAP_DEPTH);
  }

  // Create render cache
  // This is basically the whole terain getting rendered
  // Later just the colors are assigned

  // Allocate and initialize
  RenderCache = new int[WIDTH * HEIGHT];
  for(i = 0; i < WIDTH * HEIGHT; i++)
    RenderCache[i] = -1;

  // Set the highest y on the screen to the screen bottom
  // It will get lifted in the loop
  // total_min_y is used for opt. btw
  total_min_y = HEIGHT;

  float base_y = 0.0f;
  for(z = HMAP_DEPTH - 15; z >= 1; z--, base_y = z / 2.0f)
  {
    printf("                                                 \rCaching: %i\r", z);
    for(x = 0; x < WIDTH; x++)
    {
      int id = x + z * HMAP_WIDTH;
      // Get the heightmap data
      float max_y = hmap[id] * 0.5;

      // Bar top and bottom
      int y_from = (HEIGHT - 1) - (int)base_y + 200;
      int y_to   = (HEIGHT - 1) - (int)base_y - (int)max_y + 200;

      // Does reach out of the screen ? Fir it then
      if(y_from < 0)          y_from = 0;
      if(y_to   < 0)          y_to   = 0;
      if(y_from > HEIGHT - 1) y_from = HEIGHT - 1;
      if(y_to   > HEIGHT - 1) y_to   = HEIGHT - 1;

      // Lift the minimal y ?
      if(y_to < total_min_y)
        total_min_y = y_to; // Yep

      // "Draw" the bar 
      for(y = y_from; y > y_to; y--)
      {
        RenderCache[x + y * WIDTH] = id; 
      }
    }
  }  
}

//
// Function: Render
//   Render part of an image.
//
// In:
//   Output RGB buffer, and the range of y which should be rendered.
//
// Out:
//   Nothing (basically the RGB buffer is the output)
//
void Render(myRGB *img, int from, int to)
{
  int x, y, z;

  // This is the foreground color
  // Play with it to get nice effects
  // It's a gradient to simulate the fog
  static float fg_start[3] = { 23.0f, 38.0f, 61.0f };
  static float fg_end[3]   = { 200.0f, 237.0f, 243.0f };
  static float fg_diff[3]  = { (fg_end[0] - fg_start[0]) / (float)(HMAP_DEPTH),
                        (fg_end[1] - fg_start[1]) / (float)(HMAP_DEPTH),
                        (fg_end[2] - fg_start[2]) / (float)(HMAP_DEPTH) };

  // Render
  for(y = from; y < to; y++)
  {
    for(x = 0; x < WIDTH; x++)
    {
      // Is there a pixel here ?
      int idx = RenderCache[x + y * WIDTH];
      if(idx == -1)
        continue;

      // Get the height
      float max_y = hmap[idx];
      z = idx / HMAP_WIDTH;

      // Calc color
      Vector3D color = Vector3D(
        (fg_start[0] + fg_diff[0] * (float)z) / 255.0,
        (fg_start[1] + fg_diff[1] * (float)z) / 255.0,
        (fg_start[2] + fg_diff[2] * (float)z) / 255.0
        );

      //Vector3D color = Vector3D(0.7, 0.7, 0.7);

      Vector3D vp_color = color;

      // Sun light

      // Setup vectors
      Vector3D vp = Vector3D( (float)x, (float)max_y, (float)z );

      Vector3D L = sun_pos - vp;

      L.Norm();
      Vector3D N = NormalMap[idx];


      float per = 0;

//      if(z < 2*HMAP_WIDTH/4)
//      {
        per = ((float)(z/* - 2*HMAP_WIDTH/4*/) / (float)HMAP_DEPTH);
        per *= per;
        per *= per;
        per *= per;
        per = 1.0f - per;
//      }

      // Diffusive
      float dot;
      dot = L.Dot(NormalMap[idx]);
      if (dot < 0.0)
      {
        float diff = -dot * 0.1f * per; // Play with this to get nice effects
        color += (sun_color * vp_color) * diff;
      }

      // Diffusive 2
      if (dot < -0.2)
      {
        float diff = dot*dot*dot*dot*dot*dot * 0.3f * per; // Play with this to get nice effects
        color += sun_color * diff;
      }

      // Specular
      Vector3D R = L -  N * L.Dot(N) * 2.0l;
      Vector3D Cs = Vector3D(0, max_y, max_y);
      Cs.Norm();
      dot = Cs.Dot(R);
      if (dot < 0.0)
      {
        dot = pow(dot, 40.0);
        float spec = dot * (0.2f * per); // Play with this to get nice effects
        color += sun_color2 * spec;
      }

      // Check
      if(color.arr[0] > 1.0f) color.arr[0] = 1.0f;
      if(color.arr[1] > 1.0f) color.arr[1] = 1.0f;
      if(color.arr[2] > 1.0f) color.arr[2] = 1.0f;

      // Write pixel color
      // Uncomment other shaders to see the normals or the dot product

      // Shader - full
      img[x + y * WIDTH].b = (unsigned char)(color.arr[0] * 255.0f);
      img[x + y * WIDTH].g = (unsigned char)(color.arr[1] * 255.0f);
      img[x + y * WIDTH].r = (unsigned char)(color.arr[2] * 255.0f);

      // Normals - shader
      //img[x + y * WIDTH].b = 127 + (unsigned char)(NormalMap[idx].arr[0] * 127.0f);
      //img[x + y * WIDTH].g = 127 + (unsigned char)(NormalMap[idx].arr[1] * 127.0f);
      //img[x + y * WIDTH].r = 127 + (unsigned char)(NormalMap[idx].arr[2] * 127.0f);
      
      // Shader - dot
      //dot = L.Dot(NormalMap[idx]);
      //img[x + y * WIDTH].b = 127 + (unsigned char)(-dot * 127.0f);
      //img[x + y * WIDTH].g = 127 + (unsigned char)(-dot * 127.0f);
      //img[x + y * WIDTH].r = 127 + (unsigned char)(-dot * 127.0f);

    }
  }
}

// Get rid of SDL main on windows
#ifdef _WIN32
#  undef main
#endif

//
// Function: WinMain
//   The main function.
//
// In:
//   Nothing interesting
//
// Out:
//   Nothing interesting
//
int
main(int argc, char **argv)
{
  // Unused
  (void)argc;
  (void)argv;

  // Allocate the array for the picture
  picture = NULL;

  // Load and cache stuff...
  MakePicture();

  // Init SDL
  SDL_Init(SDL_INIT_VIDEO | SDL_INIT_NOPARACHUTE);

  // Create the window
  Screen = SDL_SetVideoMode(WIDTH, HEIGHT, 32, SDL_HWSURFACE);
  if(!Screen)
  {
    fprintf(stderr, "SDL_SetVideoMode() Failed!");
    return 1;
  }

  SDL_WM_SetCaption(".S.k.y. by gynvael.coldwind//vx", NULL);
  SDL_ShowCursor(SDL_DISABLE);

  // Finally set the picture
  picture = (unsigned char*)Screen->pixels;

  // >> rendering setup
  int x, y;
  //myRGB *img = (myRGB*)picture;
  myRGB *img = (myRGB*)Screen->pixels;

  // Generate background
  //  float bg_start[3] = { 32.0f, 69.0f, 113.0f };
  float bg_start[3] = { 96.0f, 128.0f, 190.0f };
  //float bg_end[3]   = { 168.0f, 228.0f, 236.0f };
  float bg_end[3]   = { 255.0f, 255.0f, 255.0f };
  float bg_diff[3]  = { (bg_end[0] - bg_start[0]) / (float)(HEIGHT*22/40),
                        (bg_end[1] - bg_start[1]) / (float)(HEIGHT*22/40),
                        (bg_end[2] - bg_start[2]) / (float)(HEIGHT*22/40) };

  // Render the background (it has to be done only once)
  for(y = 0; y < HEIGHT; y++, bg_start[0] += bg_diff[0], bg_start[1] += bg_diff[1], bg_start[2] += bg_diff[2] )
  {
    for(x = 0; x < WIDTH; x++)
    {
      img[x + y * WIDTH].b = (unsigned char)bg_start[0];
      img[x + y * WIDTH].g = (unsigned char)bg_start[1];
      img[x + y * WIDTH].r = (unsigned char)bg_start[2];
    }
  }
  // >> end of rendering setup

  double vx = 0;
  double vy = 0;
  xPos = WIDTH / 2;
  yPos = HEIGHT / 2;
  bool automove = true;

  // Message loop
  Uint32 curr = SDL_GetTicks();
  Uint32 prev = curr;
  bool Done = false;
  while(!Done)
  {
    SDL_Event Ev;

    // Check on the events
    memset(&Ev, 0, sizeof(Ev));
    while(SDL_PollEvent(&Ev))
    {
      switch(Ev.type)
      {
        case SDL_KEYUP:
          switch (Ev.key.keysym.sym)
          {
              case SDLK_LEFT:
              case SDLK_RIGHT:
                vx = 0;
                break;
              case SDLK_UP:
              case SDLK_DOWN:
                vy = 0;
                break;
          }
          break;
        case SDL_KEYDOWN:
          switch (Ev.key.keysym.sym)
          {
              case SDLK_RETURN:
                fps_on = !fps_on;
                break;
              case SDLK_LEFT:
                vx = -100;
                automove = false;
                break;
              case SDLK_RIGHT:
                vx = 100;
                automove = false;
                break;
              case SDLK_UP:
                vy = -100;
                automove = false;
                break;
              case SDLK_DOWN:
                vy = 100;
                automove = false;
                break;
              case SDLK_ESCAPE:
                  Done = true;
                  break;
          }
          break;
        // Done ?
        case SDL_QUIT:
          Done = true;
          break;
      }
    }

    curr = SDL_GetTicks();
    Uint32 delta = curr - prev;
    prev = curr;
    double dt = delta / 1000.0;
    fps_counter(dt);

    if (automove)
    {
        double ct = curr / 3000.0;
        double phase = ct - (int)(ct / (2 * M_PI)) * (2 * M_PI);
        xPos = WIDTH / 2 + (WIDTH / 2) * sin(2 * phase);
        yPos = 2 * HEIGHT / 3 + (HEIGHT / 2) * cos(3 * phase);
    }
    else
    {
        xPos += vx * dt;
        yPos += vy * dt;
    }

    // Recalc the suns position
    sun_pos.x = xPos;//sinf(r) * 1000;
    sun_pos.z = HMAP_DEPTH - yPos;//cosf(r) * 1000;

    Render((myRGB*)picture, total_min_y, HEIGHT);

    if (fps_on)
        fps_draw();
    // Update the window
    SDL_Flip(Screen);
  }

  // Done
  return 0;
}

