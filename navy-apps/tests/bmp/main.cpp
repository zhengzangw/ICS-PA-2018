#include <assert.h>
#include <stdlib.h>
#include <ndl.h>
#include <stdio.h>

int main() {
  NDL_Bitmap *bmp = (NDL_Bitmap*)malloc(sizeof(NDL_Bitmap));
  printf("1\n");
  NDL_LoadBitmap(bmp, "/share/pictures/projectn.bmp");
  assert(bmp->pixels);
  printf("2\n");
  NDL_OpenDisplay(bmp->w, bmp->h);
  printf("3\n");
  NDL_DrawRect(bmp->pixels, 0, 0, bmp->w, bmp->h);
  printf("4\n");
  NDL_Render();
  NDL_CloseDisplay();
  while (1);
  return 0;
}
