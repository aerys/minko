// Almost identical to Allegro's ex15.c

#include <IL/il.h>
#include <IL/ilu.h>
#include <IL/ilut.h>
#include <allegro.h>
#include <stdlib.h>
#include <stdio.h>


int main(int argc, char *argv[])
{
  BITMAP *Image;
  PALETTE Pal;
  ILuint Id;

  if (argc != 2) {
     printf("Please specify a filename.\n");
     return 1;
  }

  ilInit();
  ilGenImages(1, &Id);
  ilBindImage(Id);
  ilLoadImage(argv[1]);

//  if (ilGetInteger(IL_IMAGE_FORMAT) == GL_BGR ||
//      ilGetInteger(IL_IMAGE_FORMAT) == GL_BGRA)
        ilSwapColours();

  Image = (BITMAP*)ilutConvertToAlleg(Pal);

  allegro_init();
  install_keyboard();
  if (ilGetInteger(IL_IMAGE_BYTES_PER_PIXEL) == 8) {
     set_color_depth(8);
     set_palette(Pal);
  }
  else
     set_color_depth(32);
  set_gfx_mode(GFX_AUTODETECT, 640, 480, 0, 0);

  blit(Image, screen, 0, 0, (SCREEN_W - Image->w) / 2,
        (SCREEN_H - Image->h) / 2, Image->w, Image->h);

  destroy_bitmap(Image);

  readkey();

  return 0;
}
