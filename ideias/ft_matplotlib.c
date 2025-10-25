#include "ft_matplotlib.h"
#include <stdlib.h>
#include <ft_maki.h>


// cria canvas branco
Canvas *canvas_create(int w, int h) {
    Canvas *c = ft_malloc(sizeof(Canvas));
    c->width = w;
    c->height = h;
    c->pixels = ft_malloc(3 * w * h);
    ft_memset(c->pixels, 255, 3 * w * h); // branco
    return c;
}

void canvas_destroy(Canvas *c) {
    ft_free(c->pixels);
    ft_free(c);
}

// seta pixel
void set_pixel(Canvas *c, int x, int y, Color col) {
    if (x < 0 || y < 0 || x >= c->width || y >= c->height) return;
    int idx = 3 * (y * c->width + x);
    c->pixels[idx] = col.r;
    c->pixels[idx + 1] = col.g;
    c->pixels[idx + 2] = col.b;
}

// desenha linha (Bresenham)
void draw_line(Canvas *c, int x0, int y0, int x1, int y1, Color col) {
    int dx = abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
    int dy = -abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
    int err = dx + dy, e2;

    while (1) {
        set_pixel(c, x0, y0, col);
        if (x0 == x1 && y0 == y1) break;
        e2 = 2 * err;
        if (e2 >= dy) { err += dy; x0 += sx; }
        if (e2 <= dx) { err += dx; y0 += sy; }
    }
}

// salva PNG
void canvas_save(Canvas *c, const char *filename) {
    stbi_write_png(filename, c->width, c->height, 3, c->pixels, c->width * 3);
}

// plot de linha
void plot_line(Canvas *c, double *x, double *y, int n, Color col,
               double xmin, double xmax, double ymin, double ymax) {
    int prev_x = -1, prev_y = -1;
    for (int i = 0; i < n; i++) {
        int px = (int)((x[i] - xmin) / (xmax - xmin) * (c->width - 1));
        int py = (int)((1 - (y[i] - ymin) / (ymax - ymin)) * (c->height - 1));
        if (prev_x >= 0) draw_line(c, prev_x, prev_y, px, py, col);
        prev_x = px;
        prev_y = py;
    }
}

// desenha texto com fonte 8x8
void draw_char(Canvas *c, int x, int y, char ch, Color col) {
    if (ch < 32 || ch > 126) return;
    const unsigned char *bitmap = font8x8_basic[ch - 32];
    for (int row = 0; row < 8; row++) {
        for (int colb = 0; colb < 8; colb++) {
            if (bitmap[row] & (1 << colb)) {
                set_pixel(c, x + colb, y + row, col);
            }
        }
    }
}

void draw_text(Canvas *c, int x, int y, const char *text, Color col) {
    while (*text) {
        draw_char(c, x, y, *text, col);
        x += 8;
        text++;
    }
}

// desenha eixos + grades + labels com decimais
void draw_axes(Canvas *c, double xmin, double xmax, double ymin, double ymax) {
    Color black = {0, 0, 0};
    Color gray = {220, 220, 220};

    // origem em pixel
    int x0 = (int)((0 - xmin) / (xmax - xmin) * (c->width - 1));
    int y0 = (int)((1 - (0 - ymin) / (ymax - ymin)) * (c->height - 1));

    // eixo X
    if (y0 >= 0 && y0 < c->height)
        draw_line(c, 0, y0, c->width - 1, y0, black);

    // eixo Y
    if (x0 >= 0 && x0 < c->width)
        draw_line(c, x0, 0, x0, c->height - 1, black);

    // ticks verticais (X)
    for (double i = ceil(xmin*2)/2; i <= xmax; i += 0.5) {
        int px = (int)((i - xmin) / (xmax - xmin) * (c->width - 1));
        if (px >= 0 && px < c->width) {
            draw_line(c, px, 0, px, c->height - 1, gray);

            char buf[16];
            sprintf(buf, "%.1f", i);
            draw_text(c, px - 10, y0 + 8, buf, black);
        }
    }

    // ticks horizontais (Y)
    for (double j = ceil(ymin*2)/2; j <= ymax; j += 0.5) {
        int py = (int)((1 - (j - ymin) / (ymax - ymin)) * (c->height - 1));
        if (py >= 0 && py < c->height) {
            draw_line(c, 0, py, c->width - 1, py, gray);

            char buf[16];
            sprintf(buf, "%.1f", j);
            draw_text(c, x0 + 6, py - 4, buf, black);
        }
    }
}


ndarray *np_linspace(double start, double end, int n) {
    ndarray *arr = ft_malloc(sizeof(ndarray));
    arr->size = n;
    arr->data = ft_malloc(sizeof(double) * n);
    double step = (end - start) / (n - 1);
    for (int i = 0; i < n; i++) {
        arr->data[i] = start + i * step;
    }
    return arr;
}

ndarray *np_sin(ndarray *x) {
    ndarray *arr = ft_malloc(sizeof(ndarray));
    arr->size = x->size;
    arr->data = ft_malloc(sizeof(double) * arr->size);
    for (int i = 0; i < arr->size; i++) {
        arr->data[i] = sin(x->data[i]);
    }
    return arr;
}

void np_free(ndarray *arr) {
    ft_free(arr->data);
    ft_free(arr);
}


// --- mesmas funções internas que já mostrei ---
// canvas, set_pixel, draw_line, draw_text (com font8x8_basic.h)...

Canvas *plt_create(int w, int h) {
    Canvas *c = ft_malloc(sizeof(Canvas));
    c->width = w;
    c->height = h;
    c->pixels = ft_malloc(3 * w * h);
    ft_memset(c->pixels, 255, 3 * w * h); // fundo branco
    return c;
}

void plt_destroy(Canvas *c) {
    ft_free(c->pixels);
    ft_free(c);
}

void plt_save(Canvas *c, const char *filename) {
    stbi_write_png(filename, c->width, c->height, 3, c->pixels, c->width * 3);
}

void plt_plot_ndarray(Canvas *c, ndarray *x, ndarray *y, Color col,
                      double xmin, double xmax, double ymin, double ymax) {
    int prev_x = -1, prev_y = -1;
    for (int i = 0; i < x->size; i++) {
        int px = (int)((x->data[i] - xmin) / (xmax - xmin) * (c->width - 1));
        int py = (int)((1 - (y->data[i] - ymin) / (ymax - ymin)) * (c->height - 1));
        if (prev_x >= 0) draw_line(c, prev_x, prev_y, px, py, col);
        prev_x = px;
        prev_y = py;
    }
}

void plt_plot_dataframe(Canvas *c, dataframe *df, int colx, int coly, Color col,
                        double xmin, double xmax, double ymin, double ymax) {
    int n = df->rows;
    int prev_x = -1, prev_y = -1;
    for (int i = 0; i < n; i++) {
        int px = (int)((df->data[colx][i] - xmin) / (xmax - xmin) * (c->width - 1));
        int py = (int)((1 - (df->data[coly][i] - ymin) / (ymax - ymin)) * (c->height - 1));
        if (prev_x >= 0) draw_line(c, prev_x, prev_y, px, py, col);
        prev_x = px;
        prev_y = py;
    }
}

// título
void plt_title(Canvas *c, const char *text, Color col) {
    draw_text(c, c->width/2 - ft_strlen(text)*4, 20, text, col);
}

// legenda
void plt_legend(Canvas *c, const char *text, int x, int y, Color col) {
    draw_text(c, x+20, y-5, text, col);
    draw_line(c, x, y, x+15, y, col);
}

// eixos
void plt_axes(Canvas *c, Color col) {
    draw_line(c, 50, c->height/2, c->width-50, c->height/2, col);
    draw_line(c, c->width/2, 50, c->width/2, c->height-50, col);
}

int main() {
    Canvas *c = plt_create(WIDTH, HEIGHT);

    ndarray *X = np_linspace(0, 10, 500);
    ndarray *Y = np_sin(X);

    Color orange = {255, 172, 28};
    Color black = {0, 0, 0};
    Color green = {81, 225, 90};

    int N = 500;
    double x[N], y[N];
    for (int i = 0; i < N; i++) {
        x[i] = i * 0.02;
        y[i] = sin(x[i]);
    }

    double xmin = 0, xmax = 10, ymin = -1.2, ymax = 1.2;
    draw_axes(c, xmin, xmax, ymin, ymax);


    plt_axes(c, black);
    plt_plot_ndarray(c, X, Y, orange, 0, 10, -1.2, 1.2);
    plt_title(c, "grafico da funcao feno", green);
    plt_legend(c, "y = sin(x)", 600, 100, orange);

    plt_save(c, "seno_ft_matplotlib1_1.png");
    plt_destroy(c);

}