#ifndef FT_MATPLOTLIB_H
#define FT_MATPLOTLIB_H
/*

Observações

As funções plt_plot_ndarray e plt_plot_dataframe já temos, só adaptei o nome.

plt_scatter_ndarray é praticamente igual à sua plt_plot_ndarray, só que desenhando pontos (draw_rect pequeno) em vez de linhas.

plt_bar_ndarray pode usar draw_rect para cada barra.

plt_hist é só contar frequências e chamar plt_bar_ndarray.

plt_imshow pega ndarray 2D e mapeia valores para cores.

plt_title, plt_xlabel, plt_ylabel podem usar draw_text.

plt_grid desenha linhas horizontais/verticais.

plt_legend ainda simples: só lista textos no canto.

plt_savefig e plt_show dependem do backend (PNG, BMP, SDL etc.).

*/

#include <ft_mini_numpy.h>
#include <ft_mini_pandas.h>
#include <ft_maki.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#include "font8x8_basic.h"

// === Core ===
typedef struct {
    int width, height;
    unsigned char *pixels;
} Canvas;

typedef struct {
    unsigned char r, g, b;
} Color;

//Auto scale
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


void np_free(ndarray *arr);
// Funções auxiliares de desenho
void set_pixel(Canvas *c, int x, int y, Color col) ;
void draw_line(Canvas *c, int x0, int y0, int x1, int y1, Color col);
void draw_rect(Canvas *c, int x, int y, int w, int h, Color col);
void draw_text(Canvas *c, int x, int y, const char *text, Color col);
void draw_rect(Canvas *c, int x, int y, int w, int h, Color col) ;
/*
    algoritmo do círculo de Bresenham, mas adaptado para preencher
*/
void draw_circle(Canvas *c, int cx, int cy, int r, Color col);
void draw_axes(Canvas *c, double xmin, double xmax, double ymin, double ymax);

Canvas *create_canvas (int w, int h);
Legend *legend_create(int max_items);
void legend_add(Legend *lg, const char *label, Color col);
void plt_draw_legend(Canvas *c, Legend *lg);

AxisLimits plt_axis_auto(ndarray *x, ndarray *y);
AxisLimits plt_axis_auto_multi(ndarray **xs, ndarray **ys, int nplots);

// === Matplotlib em C (API mínima) ===

// 1. Gráfico de linha
void plt_plot_ndarray(Canvas *c, ndarray *x, ndarray *y, Color col,
                      double xmin, double xmax, double ymin, double ymax);

// 2. Gráfico de linha a partir de dataframe
void plt_plot_dataframe(Canvas *c, dataframe *df, int colx, int coly, Color col,
                        double xmin, double xmax, double ymin, double ymax);

// 3. Gráfico de dispersão
void plt_scatter_ndarray(Canvas *c, ndarray *x, ndarray *y, Color col,
                        double xmin, double xmax, double ymin, double ymax);
void plt_scatter_circle_ndarray(Canvas *c, ndarray *x, ndarray *y, Color col,
                        double xmin, double xmax, double ymin, double ymax,
                        int radius);

// 4. Gráfico de barras
void plt_bar_ndarray(Canvas *c, ndarray *x, ndarray *height, Color col,
                     double xmin, double xmax, double ymin, double ymax);

// 5. Histograma
void plt_hist(Canvas *c, ndarray *data, int bins, Color col,
              double xmin, double xmax, double ymin, double ymax);
void plt_hist2(Canvas *c, ndarray *data, int bins, Color col,
              double xmin, double xmax, AxisLimits lim);

// 6. Imagem (heatmap)
void plt_imshow(Canvas *c, ndarray *matrix);

// 7. Personalização
//void plt_title(Canvas *c, const char *text);
void plt_xlabel(Canvas *c, const char *text, Color col);
void plt_ylabel(Canvas *c, const char *text, Color col);
void plt_grid(Canvas *c, int on);
//void plt_legend(Canvas *c, const char **labels, int nlabels);

// 8. Saída
void plt_savefig(Canvas *c, const char *filename);
void plt_show(Canvas *c);

// títulos, legendas, eixos
void plt_title(Canvas *c, const char *text, Color col);
void plt_legend(Canvas *c, const char *text, int x, int y, Color col);
void plt_axes(Canvas *c, Color col);
void plt_axes2(Canvas *c, AxisLimits lim, Color col, int n_ticks);


#endif