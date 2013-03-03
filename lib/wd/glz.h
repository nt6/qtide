#ifndef GLZ_H
#define GLZ_H

extern "C" {
  Dllexport int glzarc (const int *p);
  Dllexport int glzbrush ();
  Dllexport int glzbrushnull ();
  Dllexport int glzclear ();
  Dllexport int glzclip (const int *);
  Dllexport int glzclipreset ();
  Dllexport int glzcmds (const int *, const int);
  Dllexport int glzellipse (const int *p);
  Dllexport int glzfont (char *face);
  Dllexport int glzfont2 (const int *p, int len);
  Dllexport int glzfontangle (int a);
  Dllexport int glzlines (const int *p, int len);
  Dllexport int glznodblbuf (int a);
  Dllexport int glzpen (const int *p);
  Dllexport int glzpie (const int *p);
  Dllexport int glzpixel (const int *p);
  Dllexport int glzpixels (const int *, int);
  Dllexport int glzpixelsx (const int *p);
  Dllexport int glzpolygon (const int *p, int len);
  Dllexport int glzqextent (char *s,int *wh);
  Dllexport int glzqextentw (char *s,int *w);
  Dllexport int glzqtextmetrics (int *tm);
  Dllexport int glzqwh (float *wh, int unit);
  Dllexport int glzrect (const int *p);
  Dllexport int glzrgb (const int *p);
  Dllexport int glztext (char *ys);
  Dllexport int glztextcolor ();
  Dllexport int glztextxy (const int *p);
  Dllexport int glzwindoworg (const int *p);

  Dllexport int glzscale (float *jobname);

  Dllexport int glzqresolution ();
  Dllexport int glzqcolormode ();
  Dllexport int glzqduplexmode ();
  Dllexport int glzqorientation ();
  Dllexport int glzqoutputformat ();
  Dllexport int glzqpageorder ();
  Dllexport int glzqpapersize ();
  Dllexport int glzqpapersource ();

  Dllexport int glzresolution (int n);
  Dllexport int colormode (int n);
  Dllexport int duplexmode (int n);
  Dllexport int orientation (int n);
  Dllexport int outputformat (int n);
  Dllexport int pageorder (int n);
  Dllexport int papersize (int n);
  Dllexport int papersource (int n);

  Dllexport int glzabortdoc ();
  Dllexport int glzenddoc ();
  Dllexport int glznewpage ();
  Dllexport int glzprinter (char *printername);
  Dllexport int glzstartdoc (char *jobname, char *filename);

  int glzclear2 (void *);
}

#endif
