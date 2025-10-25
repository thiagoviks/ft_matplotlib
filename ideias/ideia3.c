#include <stdio.h>
#include <math.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#define WIDTH 800
#define HEIGHT 600

typedef struct {
    unsigned char r, g, b;
} Color;

static unsigned char image[HEIGHT][WIDTH][3];

// ===== Fonte bitmap 5x7 =====
// Cada caractere é 5x7 bits
static const unsigned char font5x7[96][7] = {
    // caracteres ASCII 32..127
    // espaço (32)
    {0,0,0,0,0,0,0},
    // !
    {0x04,0x04,0x04,0x04,0x00,0x04,0x00},
    // "
    {0x0A,0x0A,0x0A,0x00,0x00,0x00,0x00},
    // #
    {0x0A,0x1F,0x0A,0x1F,0x0A,0x00,0x00},
    // $
    {0x04,0x0F,0x14,0x0E,0x05,0x1E,0x04},
    // %
    {0x19,0x19,0x02,0x04,0x08,0x13,0x13},
    // &
    {0x0C,0x12,0x14,0x08,0x15,0x12,0x0D},
    // '
    {0x06,0x04,0x08,0x00,0x00,0x00,0x00},
    // (
    {0x02,0x04,0x08,0x08,0x08,0x04,0x02},
    // )
    {0x08,0x04,0x02,0x02,0x02,0x04,0x08},
    // *
    {0x00,0x04,0x15,0x0E,0x15,0x04,0x00},
    // +
    {0x00,0x04,0x04,0x1F,0x04,0x04,0x00},
    // ,
    {0x00,0x00,0x00,0x00,0x06,0x04,0x08},
    // -
    {0x00,0x00,0x00,0x1F,0x00,0x00,0x00},
    // .
    {0x00,0x00,0x00,0x00,0x00,0x06,0x06},
    // /
    {0x01,0x01,0x02,0x04,0x08,0x10,0x10},
    // 0
    {0x0E,0x11,0x13,0x15,0x19,0x11,0x0E},
    // 1
    {0x04,0x0C,0x04,0x04,0x04,0x04,0x0E},
    // 2
    {0x0E,0x11,0x01,0x0E,0x10,0x10,0x1F},
    // 3
    {0x0E,0x11,0x01,0x06,0x01,0x11,0x0E},
    // 4
    {0x02,0x06,0x0A,0x12,0x1F,0x02,0x02},
    // 5
    {0x1F,0x10,0x1E,0x01,0x01,0x11,0x0E},
    // 6
    {0x06,0x08,0x10,0x1E,0x11,0x11,0x0E},
    // 7
    {0x1F,0x01,0x02,0x04,0x08,0x08,0x08},
    // 8
    {0x0E,0x11,0x11,0x0E,0x11,0x11,0x0E},
    // 9
    {0x0E,0x11,0x11,0x0F,0x01,0x02,0x0C},
    // :
    {0x00,0x06,0x06,0x00,0x06,0x06,0x00}
    // (continua… mas já cobre dígitos e alguns símbolos)
};

// ===== funções gráficas =====
void put_pixel(int x, int y, Color c) {
    if (x >= 0 && x < WIDTH && y >= 0 && y < HEIGHT) {
        image[HEIGHT - y - 1][x][0] = c.r;
        image[HEIGHT - y - 1][x][1] = c.g;
        image[HEIGHT - y - 1][x][2] = c.b;
    }
}

void draw_line(int x0, int y0, int x1, int y1, Color c) {
    int dx = abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
    int dy = -abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
    int err = dx + dy, e2;

    while (1) {
        put_pixel(x0, y0, c);
        if (x0 == x1 && y0 == y1) break;
        e2 = 2 * err;
        if (e2 >= dy) { err += dy; x0 += sx; }
        if (e2 <= dx) { err += dx; y0 += sy; }
    }
}

// desenha caractere 5x7
void draw_char(int x, int y, char ch, Color col) {
    if (ch < 32 || ch > 127) return;
    const unsigned char *bitmap = font5x7[ch - 32];
    for (int row = 0; row < 7; row++) {
        for (int colb = 0; colb < 5; colb++) {
            if (bitmap[row] & (1 << (4 - colb)))
                put_pixel(x + colb, y + row, col);
        }
    }
}

void draw_text(int x, int y, const char *str, Color col) {
    int offset = 0;
    while (*str) {
        draw_char(x + offset, y, *str, col);
        offset += 6;
        str++;
    }
}

// ===== main =====
int main() {
    // fundo branco
    for (int y = 0; y < HEIGHT; y++)
        for (int x = 0; x < WIDTH; x++)
            image[y][x][0] = image[y][x][1] = image[y][x][2] = 255;

    // parâmetros
    int margin = 60;
    double xmin = 0, xmax = 4 * M_PI;
    double ymin = -1.2, ymax = 1.2;

    Color black = {0,0,0};
    Color red   = {255,0,0};
    Color blue  = {0,0,255};

    int x0 = margin, y0 = margin;
    int x1 = WIDTH - margin, y1 = HEIGHT - margin;

    // eixos
    draw_line(x0, y0, x1, y0, black);
    draw_line(x0, y0, x0, y1, black);

    // ticks X
    int nticks = 8;
    for (int i = 0; i <= nticks; i++) {
        double t = xmin + (xmax - xmin) * i / nticks;
        int px = x0 + (int)((t - xmin)/(xmax-xmin) * (x1-x0));
        draw_line(px, y0-5, px, y0+5, black);

        char buf[32];
        sprintf(buf, "%.1f", t);
        draw_text(px-10, y0-20, buf, black);
    }

    // ticks Y
    nticks = 6;
    for (int i = 0; i <= nticks; i++) {
        double t = ymin + (ymax - ymin) * i / nticks;
        int py = y0 + (int)((t - ymin)/(ymax-ymin) * (y1-y0));
        draw_line(x0-5, py, x0+5, py, black);

        char buf[32];
        sprintf(buf, "%.1f", t);
        draw_text(x0-45, py-3, buf, black);
    }

    // curva seno
    int N = 1000;
    int prev_x=-1, prev_y=-1;
    for (int i=0; i<N; i++) {
        double x = xmin + (xmax - xmin) * i / (N-1);
        double y = sin(x);

        int px = x0 + (int)((x - xmin)/(xmax-xmin) * (x1-x0));
        int py = y0 + (int)((y - ymin)/(ymax-ymin) * (y1-y0));

        if (prev_x!=-1) draw_line(prev_x, prev_y, px, py, red);
        prev_x=px; prev_y=py;
    }

    // título
    draw_text(WIDTH/2-40, HEIGHT-margin+30, "Grafico do Seno", blue);

    // salvar
    stbi_write_png("seno_labels.png", WIDTH, HEIGHT, 3, image, WIDTH*3);
    printf("Imagem salva: seno_labels.png\n");
    return 0;
}
