// compile: gcc plot.c -lm -o plot
// run: ./plot
// result: seno.png

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>

#define WIDTH 800
#define HEIGHT 600

typedef struct {
    unsigned char r, g, b;
} Color;

typedef struct {
    int width;
    int height;
    unsigned char *pixels; // RGB
} Canvas;

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

// seta pixel no canvas
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

// função de plot
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

// desenha eixos + grades
void draw_axes(Canvas *c, double xmin, double xmax, double ymin, double ymax) {
    Color black = {0, 0, 0};
    Color gray = {200, 200, 200};

    // converter origem para pixel
    int x0 = (int)((0 - xmin) / (xmax - xmin) * (c->width - 1));
    int y0 = (int)((1 - (0 - ymin) / (ymax - ymin)) * (c->height - 1));

    // eixo X
    if (y0 >= 0 && y0 < c->height)
        draw_line(c, 0, y0, c->width - 1, y0, black);

    // eixo Y
    if (x0 >= 0 && x0 < c->width)
        draw_line(c, x0, 0, x0, c->height - 1, black);

    // grades verticais
    for (int i = (int)ceil(xmin); i <= (int)floor(xmax); i++) {
        int px = (int)((i - xmin) / (xmax - xmin) * (c->width - 1));
        if (px >= 0 && px < c->width)
            draw_line(c, px, 0, px, c->height - 1, gray);
    }

    // grades horizontais
    for (int j = (int)ceil(ymin); j <= (int)floor(ymax); j++) {
        int py = (int)((1 - (j - ymin) / (ymax - ymin)) * (c->height - 1));
        if (py >= 0 && py < c->height)
            draw_line(c, 0, py, c->width - 1, py, gray);
    }
}

int main() {
    Canvas *c = canvas_create(WIDTH, HEIGHT);

    int N = 500;
    double x[N], y[N];
    for (int i = 0; i < N; i++) {
        x[i] = i * 0.02;
        y[i] = sin(x[i]);
    }

    double xmin = 0, xmax = 10, ymin = -1.2, ymax = 1.2;

    // desenha eixos + grades
    draw_axes(c, xmin, xmax, ymin, ymax);

    // plota curva
    Color red = {255, 0, 0};
    plot_line(c, x, y, N, red, xmin, xmax, ymin, ymax);

    canvas_save(c, "seno.png");
    canvas_destroy(c);

    return 0;
}
