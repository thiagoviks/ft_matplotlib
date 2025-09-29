#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#include "font8x8_basic.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>

typedef struct {} Point;
typedef struct {} Font;

typedef struct {
    unsigned char r, g, b;
} Color;

typedef struct {
    int width;
    int height;
    unsigned char *pixels; // RGB
} Canvas;

#define WIDTH 800
#define HEIGHT 600

Canvas *canvas_create(int w, int h);
void canvas_destroy(Canvas *c);
void set_pixel(Canvas *c, int x, int y, Color col);
void draw_line(Canvas *c, int x0, int y0, int x1, int y1, Color col);
void canvas_save(Canvas *c, const char *filename);
void plot_line(Canvas *c, double *x, double *y, int n, Color col,
               double xmin, double xmax, double ymin, double ymax);
void draw_char(Canvas *c, int x, int y, char ch, Color col);
void draw_text(Canvas *c, int x, int y, const char *text, Color col);
void draw_axes(Canvas *c, double xmin, double xmax, double ymin, double ymax);


Canvas *plt_create(int w, int h);
void plt_destroy(Canvas *c);

void plt_clear(Canvas *c, Color bg);
void plt_save(Canvas *c, const char *filename);


typedef struct {
    int size;
    double *data;
} ndarray;

typedef struct {
    int rows;
    int cols;
    double **data; // matriz
    char **columns;
} dataframe;

ndarray *np_linspace(double start, double end, int n);
ndarray *np_sin(ndarray *x);


// integração com numpy
void plt_plot_ndarray(Canvas *c, ndarray *x, ndarray *y, Color col,
                      double xmin, double xmax, double ymin, double ymax);

// integração com pandas
void plt_plot_dataframe(Canvas *c, dataframe *df, int colx, int coly, Color col,
                        double xmin, double xmax, double ymin, double ymax);

// títulos, legendas, eixos
void plt_title(Canvas *c, const char *text, Color col);
void plt_legend(Canvas *c, const char *text, int x, int y, Color col);
void plt_axes(Canvas *c, Color col);