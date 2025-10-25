#ifndef FT_MATPLOTLIB_H
#define FT_MATPLOTLIB_H
/*

Notes

We already have the plt_plot_ndarray and plt_plot_dataframe functions; I just
adapted the name.

plt_scatter_ndarray is practically the same as plt_plot_ndarray, but
it draws points (small draw_rect) instead of lines.

plt_bar_ndarray can use draw_rect for each bar.

plt_hist simply counts frequencies and calls plt_bar_ndarray.

plt_imshow takes a 2D ndarray and maps values ​​to colors.

plt_title, plt_xlabel, and plt_ylabel can use draw_text.

plt_grid draws horizontal/vertical lines.

plt_legend is still simple: it just displays text in the corner.

plt_savefig and plt_show depend on the backend (PNG, BMP, SDL, etc.).

*/

#include <ft_maki.h>
#include <ft_ndarray.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "font8x8_basic.h"
#include "stb_image_write.h"

// Core
typedef struct {
  int width, height;
  unsigned char *pixels;
} Canvas;

typedef struct {
  unsigned char r, g, b;
} Color;

// Auto scale
typedef struct {
  double xmin, xmax;
  double ymin, ymax;
} AxisLimits;

typedef struct {
  const char *label;
  Color col;
} LegendItem;

typedef struct {
  LegendItem *items;
  int nitems;
  int max_items;
} Legend;

// Auxiliary drawing functions
void set_pixel(Canvas *c, int x, int y, Color col);
void draw_line(Canvas *c, int x0, int y0, int x1, int y1, Color col);
void draw_rect(Canvas *c, int x, int y, int w, int h, Color col);
void draw_text(Canvas *c, int x, int y, const char *text, Color col);
void draw_rect(Canvas *c, int x, int y, int w, int h, Color col);
/*
    Bresenham circle algorithm, but adapted to fill
*/
void draw_circle(Canvas *c, int cx, int cy, int r, Color col);
void draw_axes(Canvas *c, double xmin, double xmax, double ymin, double ymax);

Canvas *create_canvas(int w, int h);
Legend *legend_create(int max_items);
void legend_add(Legend *lg, const char *label, Color col);
void plt_draw_legend(Canvas *c, Legend *lg);

AxisLimits plt_axis_auto(ndarray *x, ndarray *y);
AxisLimits plt_axis_auto_multi(ndarray **xs, ndarray **ys, int nplots);

// 1. Line graph
void plt_plot_ndarray(Canvas *c, ndarray *x, ndarray *y, Color col, double xmin,
                      double xmax, double ymin, double ymax);

// 2. Line chart from dataframe
void plt_plot_dataframe(Canvas *c, dataframe *df, int colx, int coly, Color col,
                        double xmin, double xmax, double ymin, double ymax);

// 3. Scatterplot
void plt_scatter_ndarray(Canvas *c, ndarray *x, ndarray *y, Color col,
                         double xmin, double xmax, double ymin, double ymax);
void plt_scatter_circle_ndarray(Canvas *c, ndarray *x, ndarray *y, Color col,
                                double xmin, double xmax, double ymin,
                                double ymax, int radius);

// 4. Bar chart
void plt_bar_ndarray(Canvas *c, ndarray *x, ndarray *height, Color col,
                     double xmin, double xmax, double ymin, double ymax);
void plt_bar_ndarray2(Canvas *c, ndarray *x, ndarray *y, Color col, double xmin,
                      double xmax, double ymin, double ymax, int bar_width);

// 5. Histogram
void plt_hist(Canvas *c, ndarray *data, int bins, Color col, double xmin,
              double xmax);
void plt_hist2(Canvas *c, ndarray *data, int bins, Color col, double xmin,
               double xmax);

// void plt_hist(Canvas *c, ndarray *data, int bins, Color col,
//               double xmin, double xmax, double ymin, double ymax);

// void plt_hist2(Canvas *c, ndarray *data, int bins, Color col,
//               double xmin, double xmax, AxisLimits lim) ;

void plt_hist_full(Canvas *c, ndarray *data, int bins, Color col,
                   const char *title, const char *xlabel, const char *ylabel);

// 6. Image (heatmap)
void plt_imshow(Canvas *c, ndarray *matrix);

// 7. Personalization
// void plt_title(Canvas *c, const char *text);
void plt_xlabel(Canvas *c, const char *text, Color col);
void plt_ylabel(Canvas *c, const char *text, Color col);
void plt_grid(Canvas *c, int on);
// void plt_legend(Canvas *c, const char **labels, int nlabels);

// 8. Save and Output
void plt_savefig(Canvas *c, const char *filename);
void plt_show(Canvas *c);

// title, legends, axis
void plt_title(Canvas *c, const char *text, Color col);
void plt_legend(Canvas *c, const char *text, int x, int y, Color col);
void plt_axes(Canvas *c, Color col);
void plt_destroy(Canvas *c);
void plt_axes2(Canvas *c, AxisLimits lim, Color col, int n_ticks);

#endif