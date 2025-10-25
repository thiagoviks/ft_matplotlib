// compile: gcc plot.c -lm -o plot
// run: ./plot
// result: seno.png

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "ft_matplotlib.h"
#include "stb_image_write.h"
#include "font8x8_basic.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>


// cria canvas branco
Canvas *canvas_create(int w, int h) {
    Canvas *c = malloc(sizeof(Canvas));
    c->width = w;
    c->height = h;
    c->pixels = malloc(3 * w * h);
    memset(c->pixels, 255, 3 * w * h); // branco
    return c;
}

void canvas_destroy(Canvas *c) {
    free(c->pixels);
    free(c);
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

int main() {
    Canvas *c = canvas_create(WIDTH, HEIGHT);

    // dados
    int N = 500;
    double x[N], y[N];
    for (int i = 0; i < N; i++) {
        x[i] = i * 0.02;
        y[i] = sin(x[i]);
    }

    // cores
    Color black = {0, 0, 0};
    Color red = {255, 0, 0};

    // eixos
    draw_line(c, 50, HEIGHT/2, WIDTH-50, HEIGHT/2, black); // eixo X
    draw_line(c, WIDTH/2, 50, WIDTH/2, HEIGHT-50, black); // eixo Y

    // plot
    plot_line(c, x, y, N, red, 0, 10, -1.2, 1.2);

    // título
    draw_text(c, WIDTH/2 - 80, 20, "grafico da funcao seno", black);

    // legenda
    draw_line(c, WIDTH-200, 100, WIDTH-150, 100, red);
    draw_text(c, WIDTH-140, 95, "y = sin(x)", black);

    // salvar
    canvas_save(c, "seno9.png");
    canvas_destroy(c);

    return 0;
}
