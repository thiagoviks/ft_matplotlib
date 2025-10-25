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

// função para colocar um pixel
void put_pixel(int x, int y, Color c) {
    if (x >= 0 && x < WIDTH && y >= 0 && y < HEIGHT) {
        image[HEIGHT - y - 1][x][0] = c.r; // inverte y p/ origem baixo-esquerda
        image[HEIGHT - y - 1][x][1] = c.g;
        image[HEIGHT - y - 1][x][2] = c.b;
    }
}

// desenha linha com Bresenham
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

int main() {
    // fundo branco
    for (int y = 0; y < HEIGHT; y++)
        for (int x = 0; x < WIDTH; x++)
            image[y][x][0] = image[y][x][1] = image[y][x][2] = 255;

    // gera dados: seno
    int N = 500;
    double xmin = 0, xmax = 4 * M_PI;
    double ymin = -1, ymax = 1;
    Color red = {255, 0, 0};

    int prev_x = 0, prev_y = HEIGHT/2;
    for (int i = 1; i < N; i++) {
        double x = xmin + (xmax - xmin) * i / (N - 1);
        double y = sin(x);

        int px = (int)((x - xmin) / (xmax - xmin) * (WIDTH - 1));
        int py = (int)((y - ymin) / (ymax - ymin) * (HEIGHT - 1));

        draw_line(prev_x, prev_y, px, py, red);

        prev_x = px;
        prev_y = py;
    }

    // salva imagem
    stbi_write_png("seno.png", WIDTH, HEIGHT, 3, image, WIDTH * 3);
    printf("Imagem salva: seno.png\n");
    return 0;
}
