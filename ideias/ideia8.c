// compile: gcc plot.c -lm -o plot
// run: ./plot
// result: seno.png

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

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

// ===== Mini fonte 5x7 =====
// caracteres: 0–9, -, .
const unsigned char font5x7[12][5] = {
    {0x3E,0x51,0x49,0x45,0x3E}, // 0
    {0x00,0x42,0x7F,0x40,0x00}, // 1
    {0x42,0x61,0x51,0x49,0x46}, // 2
    {0x21,0x41,0x45,0x4B,0x31}, // 3
    {0x18,0x14,0x12,0x7F,0x10}, // 4
    {0x27,0x45,0x45,0x45,0x39}, // 5
    {0x3C,0x4A,0x49,0x49,0x30}, // 6
    {0x01,0x71,0x09,0x05,0x03}, // 7
    {0x36,0x49,0x49,0x49,0x36}, // 8
    {0x06,0x49,0x49,0x29,0x1E}, // 9
    {0x00,0x00,0x7F,0x00,0x00}, // -
    {0x00,0x60,0x60,0x00,0x00}  // .
};

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

// desenha caractere 5x7
void draw_char(Canvas *c, int x, int y, char ch, Color col) {
    int idx = -1;
    if (ch >= '0' && ch <= '9') idx = ch - '0';
    else if (ch == '-') idx = 10;
    else if (ch == '.') idx = 11;

    if (idx < 0) return;

    for (int colx = 0; colx < 5; colx++) {
        unsigned char bits = font5x7[idx][colx];
        for (int row = 0; row < 7; row++) {
            if (bits & (1 << row)) {
                set_pixel(c, x + colx, y + row, col);
            }
        }
    }
}

// desenha string
void draw_text(Canvas *c, int x, int y, const char *text, Color col) {
    int offset = 0;
    for (const char *p = text; *p; p++) {
        draw_char(c, x + offset, y, *p, col);
        offset += 6; // 5 px char + 1 px espaço
    }
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

int main() {
    Canvas *c = canvas_create(WIDTH, HEIGHT);

    int N = 500;
    double x[N], y[N];
    for (int i = 0; i < N; i++) {
        x[i] = i * 0.02;
        y[i] = sin(x[i]);
    }

    double xmin = 0, xmax = 10, ymin = -1.2, ymax = 1.2;

    // eixos + grades + labels decimais
    draw_axes(c, xmin, xmax, ymin, ymax);

    // curva seno em vermelho
    Color red = {255, 0, 0};
    plot_line(c, x, y, N, red, xmin, xmax, ymin, ymax);

    canvas_save(c, "seno8.png");
    canvas_destroy(c);

    return 0;
}
