//-------------------------------------------------------------------
//
// SDL 1.x GUI:
//
// Drawing Primitive:
//
// FILE:
//   draw.c
//
// START DATE: 04/11/2018 - 07:00
//
//-------------------------------------------------------------------
//
#include "app.h"

static const unsigned char fixed_font[14][764] = {
  "                                   xx                                                                                                                                                                                                                                                                                                                                                                                                                                                                              xx             xxx                                                                                                                                                                                                                                                      ",
  "                                   xx    xxx                                                                                                                                                                                                                                                                                                                                                                                                                                                                      xxxx             xx                                                                      xx       xx                                                                                                                                                                     ",
  "           xx    xx  xx   xx xx   xxxx  xx xx     xxx      xx       xx    xx                                                 xx    xxxx     xx    xxxx    xxxx    xx     xxxxxx    xxx   xxxxxx   xxxx    xxxx                       xx          xx       xxxx   xxxxxx    xx    xxxxx    xxxx   xxxx    xxxxxx  xxxxxx   xxxx   xx  xx   xxxx       xx  xx  xx  xx      xx   xx xx   xx  xxxx   xxxxx    xxxx   xxxxx    xxxx   xxxxxx  xx  xx  xx  xx  xx   xx xx  xx  xx  xx  xxxxxx   xxxx   xx       xxxx   xx  xx             xx           xx                  xx            xxxx          xx        xx       xx   xx      xxxx                                                             xx                                                        xx     xx     xx     xxx   x   ",
  "          xxxx   xx  xx   xx xx  xx  xx xx xx x  xx xx     xx      xx      xx                                                xx   xx  xx   xxx   xx  xx  xx  xx   xx     xx        xx        xx  xx  xx  xx  xx                     xx            xx     xx  xx xx    xx  xxxx   xx  xx  xx  xx  xx xx   xx      xx      xx  xx  xx  xx    xx        xx  xx  xx  xx      xx   xx xx   xx xx  xx  xx  xx  xx  xx  xx  xx  xx  xx    xx    xx  xx  xx  xx  xx   xx xx  xx  xx  xx      xx   xx     xx         xx                                   xx                  xx           xx             xx                      xx        xx                                                             xx                                                       xx      xx      xx   xx xx xx   ",
  "          xxxx   xx  xx  xxxxxxx xx      xxx xx  xx xx     xx      xx      xx     xx xx    xx                               xx    xx xxx xxxxx   xx  xx  xx  xx   xx xx  xx       xx        xx   xx  xx  xx  xx    xxx     xxx     xx              xx    xx  xx xx    xx xx  xx  xx  xx  xx  xx  xx  xx  xx      xx      xx  xx  xx  xx    xx        xx  xx xx   xx      xxx xxx xxx  xx xx  xx  xx  xx  xx  xx  xx  xx  xx        xx    xx  xx  xx  xx  xx   xx  xx x   xx  xx      xx   xx      xx        xx                            xxxx   xxxxx    xxxx    xxxxx   xxxx    xx      xxxxx  xxxxx   xxxx     xxxx   xx  xx    xx    xxxxxx  xxxxx    xxxx   xxxxx    xxxxx  xx  xx   xxxxx  xxxxxx  xx  xx  xx  xx  xx   xx xx  xx  xx  xx  xxxxxx    xx      xx      xx   x   xxx    ",
  "          xxxx            xx xx   xx        xx    xxx             xx        xx     xxx     xx                               xx    xx xxx    xx       xx      xx   xx xx  xx      xxxxx      xx   xxx xx  xx  xx    xxx     xxx    xx     xxxxxx     xx      xx  xx  xxxx xx  xx  xx  xx  xx      xx  xx  xx      xx      xx      xx  xx    xx        xx  xx xx   xx      xx x xx xxxx xx xx  xx  xx  xx  xx  xx  xx  xx   xx       xx    xx  xx  xx  xx  xx x xx   xx    xx  xx     xx    xx      xx        xx                               xx  xx  xx  xx  xx  xx  xx  xx  xx   xx     xx  xx  xx  xx    xx       xx   xx  xx    xx    xx x xx xx  xx  xx  xx  xx  xx  xx  xx  xx xxx  xx       xx     xx  xx  xx  xx  xx x xx xx  xx  xx  xx      xx    xx      xx      xx              ",
  "           xx             xx xx    xx      xx    xx               xx        xx   xxxxxxx xxxxxx          xxxxxx            xx     xx  xx    xx      xx     xxx    xx xx  xxxxx   xx  xx    xx     xxxx   xx  xx                  xx                  xx    xx   xx xx xx xx  xx  xxxxx   xx      xx  xx  xxxxx   xxxxx   xx      xxxxxx    xx        xx  xxxx    xx      xx x xx xx xxxx xx  xx  xxxxx   xx  xx  xxxxx     xx      xx    xx  xx  xx  xx  xx x xx   xx     xxxx     xx     xx       xx       xx                               xx  xx  xx  xx      xx  xx  xx  xx  xxxxxx  xx  xx  xx  xx    xx       xx   xx xx     xx    xx x xx xx  xx  xx  xx  xx  xx  xx  xx  xxx     xx       xx     xx  xx  xx  xx  xx x xx  xxxx   xx  xx     xx    xx       xx       xx             ",
  "           xx             xx xx     xx    xx     xx xxxx          xx        xx     xxx     xx                              xx     xxx xx    xx     xx        xx  xx  xx      xx  xx  xx    xx    xx xxx   xxxxx                   xx     xxxxxx     xx     xx   xx xx xx xxxxxx  xx  xx  xx      xx  xx  xx      xx      xx xxx  xx  xx    xx        xx  xx xx   xx      xx x xx xx  xxx xx  xx  xx      xx  xx  xx xx      xx     xx    xx  xx  xx  xx  xx x xx  x xx     xx     xx      xx       xx       xx                            xxxxx  xx  xx  xx      xx  xx  xxxxxx   xx     xx  xx  xx  xx    xx       xx   xxxx      xx    xx x xx xx  xx  xx  xx  xx  xx  xx  xx  xx       xxxx    xx     xx  xx  xx  xx  xx x xx   xx    xx  xx    xx    xx        xx        xx            ",
  "                         xxxxxxx     xx  xx xxx  xx  xx           xx        xx    xx xx    xx                             xx      xxx xx    xx    xx     xx  xx  xxxxxxx     xx  xx  xx   xx     xx  xx     xx                     xx              xx           xx  xxxx xx  xx  xx  xx  xx  xx  xx  xx  xx      xx      xx  xx  xx  xx    xx    xx  xx  xx xx   xx      xx   xx xx   xx xx  xx  xx      xx  xx  xx  xx      xx    xx    xx  xx  xx  xx   xx xx  xx  xx    xx    xx       xx        xx      xx                           xx  xx  xx  xx  xx      xx  xx  xx       xx     xx  xx  xx  xx    xx       xx   xx xx     xx    xx x xx xx  xx  xx  xx  xx  xx  xx  xx  xx          xx   xx     xx  xx  xx  xx  xx x xx  xxxx   xx  xx   xx      xx       xx       xx             ",
  "           xx             xx xx  xx  xx  x xx xx xx  xx           xx        xx                     xxx             xxx    xx      xx  xx    xx   xx      xx  xx      xx     xx   xx  xx   xx     xx  xx    xx      xxx     xxx      xx            xx       xx   xx       xx  xx  xx  xx  xx  xx  xx xx   xx      xx      xx  xx  xx  xx    xx    xx  xx  xx  xx  xx      xx   xx xx   xx xx  xx  xx      xx  xx  xx  xx  xx  xx    xx    xx  xx   xxxx    xx xx  xx  xx    xx    xx       xx        xx      xx                           xx  xx  xx  xx  xx  xx  xx  xx  xx       xx     xx  xx  xx  xx    xx       xx   xx  xx    xx    xx x xx xx  xx  xx  xx  xx  xx  xx  xx  xx          xx   xx     xx  xx   xxxx    xx xx  xx  xx  xx  xx  xx        xx      xx      xx              ",
  "           xx             xx xx   xxxx     xx xx  xxx xx           xx      xx                      xxx             xxx   xx        xxxx     xx   xxxxxx   xxxx       xx  xxxx     xxxx    xx      xxxx    xxx      xxx     xxx       xx          xx        xx    xxxxxxx xx  xx  xxxxx    xxxx   xxxx    xxxxxx  xx       xxxxx  xx  xx   xxxx    xxxx   xx  xx  xxxxxx  xx   xx xx   xx  xxxx   xx       xxxx   xx  xx   xxxx     xx     xxxx     xx     xx xx  xx  xx    xx    xxxxxx   xx         xx     xx                            xxxxx  xxxxx    xxxx    xxxxx   xxxx    xx      xxxxx  xx  xx  xxxxxx     xx   xx  xx  xxxxxx  xx   xx xx  xx   xxxx   xxxxx    xxxxx  xx      xxxxx     xxxx   xxxxx    xx     xx xx  xx  xx   xxxx   xxxxxx    xx      xx      xx              ",
  "                                   xx       xxx                    xx      xx                       xx                   xx                                                                                                 xx                                                                                                                                                                              xx                                                                            xx         xx     xx                                                                               xx                     xx                                           xx          xx                                                             xx             xx      xx      xx              ",
  "                                   xx                               xx    xx                       xx                                                                                                                      xx                                                                                                                                                                                xx                                                                           xx                xx                                                                               xx                     xx                                           xx          xx                                                            xx               xx     xx     xx               ",
  "                                                                                                                                                                                                                                                                                                                                                                                                                                                                                          xxxx            xxxx          xxxxxxxx                                                         xxxxx                   xxxx                                            xx          xx                                                         xxxx                       xx                      ",
};
unsigned char pal[24][3] = {
  {197, 40, 0},
  {255, 178, 156},
  {255, 178, 156},
  {255, 157, 131},
  {255, 133, 98},
  {246, 109, 74},
  {246, 85, 41},
  {246, 60, 8},
  {246, 72, 24},
  {246, 85, 32},
  {246, 97, 41},
  {246, 109, 49},
  {246, 121, 57},
  {246, 129, 74},
  {255, 141, 82},
  {255, 153, 90},
  {255, 165, 98},
  {255, 178, 106},
  {255, 190, 123},
  {255, 202, 131},
  {255, 214, 139},
  {255, 222, 148},
  {255, 234, 156},
  {255, 234, 156}
};

void DrawPixel (BMP *bmp, int x, int y, int color) {
    int bpp;
    Uint8 *p;

    // Get clip
    if(
        x < bmp->clip_rect.x ||
        x > bmp->clip_rect.x + bmp->clip_rect.w ||
        y < bmp->clip_rect.y ||
        y >= bmp->clip_rect.y + bmp->clip_rect.h
    )
  return;

    bpp = bmp->format->BytesPerPixel;

    // Here p is the address to the pixel we want to set
    p = (Uint8 *)bmp->pixels + y * bmp->pitch + x * bpp;

    if (bpp==2) { *(Uint16 *)p = color; return; }
    if (bpp==4) *(Uint32 *)p = color;

}// DrawPixel()

void DrawHline (BMP *bmp, int x1, int y, int x2, int color) {
    int i; // counter
    Uint8 *p; // pixel

    if ( y < bmp->clip_rect.y || y > bmp->clip_rect.y + bmp->clip_rect.h-1)
        return;

    // Set cliping
    if (x1 < bmp->clip_rect.x) x1 = bmp->clip_rect.x;
    if (x2 > bmp->clip_rect.x + bmp->clip_rect.w-1) x2 = bmp->clip_rect.x + bmp->clip_rect.w-1;

    int bpp = bmp->format->BytesPerPixel;

    //Here p is the address to the pixel we want to set
    p = (Uint8 *)bmp->pixels + y * bmp->pitch + x1 * bpp;

    switch (bpp) {
    case 2:
        for (i = x1; i <= x2; i++) {
            *(Uint16 *)p = color; // Set color
            p += bpp;             // Increment
        }
        break;
    case 4:
        for (i = x1; i <= x2; i++) {
            *(Uint32 *)p = color; // Set color
            p += bpp;             // Increment
        }
        break;
    }
}// hline ();

void DrawVline (BMP *bmp, int x, int y1, int y2, int color) {
    const int bpp = bmp->format->BytesPerPixel;
    int i;
    Uint8 *p; // pixel

    if ( x < bmp->clip_rect.x || x > bmp->clip_rect.x + bmp->clip_rect.w-1 )
        return;

    // Set cliping
    if ( y1 < bmp->clip_rect.y ) y1 = bmp->clip_rect.y;
    if ( y2 > bmp->clip_rect.y + bmp->clip_rect.h-1 ) y2 = bmp->clip_rect.y + bmp->clip_rect.h-1;

    //Here p is the address to the pixel we want to set
    p = (Uint8 *)bmp->pixels + y1 * bmp->pitch + x * bpp;

    switch (bpp) {
    case 2:
        for (i = y1; i <= y2; i++) {
            *(Uint16 *)p = color; // Set color
            p += bmp->pitch;      // Increment
        }
        break;


    case 4:
        for (i = y1; i <= y2; i++) {
            *(Uint32 *)p = color; // Set color
            p += bmp->pitch;      // Increment
        }
        break;
    }
}// vline ();

void DrawRect (BMP *bmp, int x, int y, int w, int h, int color) {
    DrawHline(bmp, x, y, x+w, color);
    DrawHline(bmp, x, y+h, x+w, color);
    DrawVline(bmp, x, y, y+h, color);
    DrawVline(bmp, x+w, y, y+h, color);
}
void DrawRectR (BMP *bmp, int x, int y, int w, int h, int color) {
    DrawHline(bmp, x+2, y, x+w-3, color);
    DrawHline(bmp, x+2, y+h-1, x+w-3, color);
    DrawVline(bmp, x, y+2, y+h-3, color);
    DrawVline(bmp, x+w-1, y+2, y+h-3, color);

    DrawPixel(bmp, x+1, y+1, color);
    DrawPixel(bmp, x+w-2, y+1, color);
    DrawPixel(bmp, x+1, y+h-2, color);
    DrawPixel(bmp, x+w-2, y+h-2, color);
}
/*
void draw_rectr (BMP *bmp, int x1, int y1, int x2, int y2, int color) {
    short ponto[] = {
      x1+1, y1+1,
      x2-1, y1+1,
      x2-1, y2-1,
      x1+1, y2-1
    };
    draw_hline( bmp, x1+2, y1, x2-2, color ); // top
    draw_vline( bmp, x2, y1+2, y2-2, color ); // right >
    draw_hline( bmp, x1+2, y2, x2-2, color ); // button
    draw_vline( bmp, x1, y1+2, y2-2, color ); // left <

    draw_point (bmp, 8, ponto, color);
}
*/

// 8 x 13
void DrawChar (BMP *bmp, char ch, int x, int y, int color) {

  if (ch > 32) {
    register unsigned char count;
    register int xx;

      xx = (ch - SDLK_SPACE) * 8;

      // insert color
      for (count=0; count < 8; count++) {
          if ( fixed_font[ 0][xx] == 'x' ) { DrawPixel(bmp, x, y+0,  color); }
          if ( fixed_font[ 1][xx] == 'x' ) { DrawPixel(bmp, x, y+1,  color); }
          if ( fixed_font[ 2][xx] == 'x' ) { DrawPixel(bmp, x, y+2,  color); }
          if ( fixed_font[ 3][xx] == 'x' ) { DrawPixel(bmp, x, y+3,  color); }
          if ( fixed_font[ 4][xx] == 'x' ) { DrawPixel(bmp, x, y+4,  color); }
          if ( fixed_font[ 5][xx] == 'x' ) { DrawPixel(bmp, x, y+5,  color); }
          if ( fixed_font[ 6][xx] == 'x' ) { DrawPixel(bmp, x, y+6,  color); }
          if ( fixed_font[ 7][xx] == 'x' ) { DrawPixel(bmp, x, y+7,  color); }
          if ( fixed_font[ 8][xx] == 'x' ) { DrawPixel(bmp, x, y+8,  color); }
          if ( fixed_font[ 9][xx] == 'x' ) { DrawPixel(bmp, x, y+9,  color); }
          if ( fixed_font[10][xx] == 'x' ) { DrawPixel(bmp, x, y+10, color); }
          if ( fixed_font[11][xx] == 'x' ) { DrawPixel(bmp, x, y+11, color); }
          if ( fixed_font[12][xx] == 'x' ) { DrawPixel(bmp, x, y+12, color); }
          if ( fixed_font[13][xx] == 'x' ) { DrawPixel(bmp, x, y+13, color); }
          xx++; x++;
      }
  }
}

void DrawText (BMP *bmp, char *text, int x, int y, int color) {
//    short orig_x = x;

    while (*text) {
        if(*text > 32)
            DrawChar (bmp, *text, x, y, color);
        text++;
        x += 8;
/*
        if (*text == '\n') {
            x = orig_x;
            y += 15;
        }
*/
    }
}

void DrawWindow (SDL_Rect *r) {
    int border = MRGB(pal[0][0], pal[0][1], pal[0][2]);
    int i;
    for (i = 1; i < 24; i++)
        DrawHline (screen, r->x+1, r->y+2+i, r->x+r->w-2, MRGB(pal[i][0], pal[i][1], pal[i][2]));
    DrawHline (screen, r->x+1, r->y+2+i, r->x+r->w-2, border);
    DrawRectR (screen, r->x, r->y, r->w, r->h, border);
//SDL_MapRGB ( ASbuf->format, r, g, b )
}

