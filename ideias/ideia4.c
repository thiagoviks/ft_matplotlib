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

typedef struct {
    int margin;
    double xmin, xmax;
    double ymin, ymax;
    int x0, y0, x1, y1;
} Plot;

// ==== Desenho básico ====
void put_pixel(int x, int y, Color c) {
    if (x>=0 && x<WIDTH && y>=0 && y<HEIGHT) {
        image[HEIGHT-y-1][x][0] = c.r;
        image[HEIGHT-y-1][x][1] = c.g;
        image[HEIGHT-y-1][x][2] = c.b;
    }
}

void draw_line(int x0, int y0, int x1, int y1, Color c) {
    int dx = abs(x1-x0), sx = x0<x1?1:-1;
    int dy = -abs(y1-y0), sy = y0<y1?1:-1;
    int err = dx+dy, e2;
    while (1) {
        put_pixel(x0,y0,c);
        if (x0==x1 && y0==y1) break;
        e2 = 2*err;
        if (e2>=dy){err+=dy; x0+=sx;}
        if (e2<=dx){err+=dx; y0+=sy;}
    }
}

void draw_rect(int x, int y, int w, int h, Color c) {
    for (int i=0;i<w;i++)
        for (int j=0;j<h;j++)
            put_pixel(x+i,y+j,c);
}

void draw_circle(int cx,int cy,int r,Color c) {
    for (int y=-r;y<=r;y++)
        for (int x=-r;x<=r;x++)
            if (x*x+y*y <= r*r)
                put_pixel(cx+x,cy+y,c);
}

// ==== Conversão coordenadas -> pixels ====
int to_px(Plot *p,double x){
    return p->x0 + (int)((x-p->xmin)/(p->xmax-p->xmin)*(p->x1-p->x0));
}
int to_py(Plot *p,double y){
    return p->y0 + (int)((y-p->ymin)/(p->ymax-p->ymin)*(p->y1-p->y0));
}

// ==== Funções de plot ====
void ft_plot_line(Plot *p,double *x,double *y,int n,Color col){
    int prev_x=-1,prev_y=-1;
    for(int i=0;i<n;i++){
        int px=to_px(p,x[i]);
        int py=to_py(p,y[i]);
        if(prev_x!=-1) draw_line(prev_x,prev_y,px,py,col);
        prev_x=px; prev_y=py;
    }
}

void ft_plot_scatter(Plot *p,double *x,double *y,int n,Color col){
    for(int i=0;i<n;i++){
        int px=to_px(p,x[i]);
        int py=to_py(p,y[i]);
        draw_circle(px,py,3,col);
    }
}

void ft_plot_bar(Plot *p,double *x,double *h,int n,Color col,double width){
    for(int i=0;i<n;i++){
        int px=to_px(p,x[i]-width/2);
        int pw=to_px(p,x[i]+width/2)-px;
        int py=to_py(p,h[i]);
        int base=to_py(p,0);
        draw_rect(px,py,pw,base-py,col);
    }
}

// ==== Inicialização do plot ====
void ft_plot_init(Plot *p,double xmin,double xmax,double ymin,double ymax,int margin){
    p->xmin=xmin; p->xmax=xmax;
    p->ymin=ymin; p->ymax=ymax;
    p->margin=margin;
    p->x0=margin; p->y0=margin;
    p->x1=WIDTH-margin; p->y1=HEIGHT-margin;
    // fundo branco
    for(int y=0;y<HEIGHT;y++)
        for(int x=0;x<WIDTH;x++)
            image[y][x][0]=image[y][x][1]=image[y][x][2]=255;
    // eixos
    draw_line(p->x0,p->y0,p->x1,p->y0,(Color){0,0,0});
    draw_line(p->x0,p->y0,p->x0,p->y1,(Color){0,0,0});
}

// ==== main demo ====
int main(){
    Plot plot;
    ft_plot_init(&plot,0,10,-1.5,1.5,60);

    // linha: seno
    int N=200;
    double xs[N],ys[N];
    for(int i=0;i<N;i++){
        xs[i]=10.0*i/(N-1);
        ys[i]=sin(xs[i]);
    }
    ft_plot_line(&plot,xs,ys,N,(Color){255,0,0});

    // scatter: cos
    for(int i=0;i<N;i++) ys[i]=cos(xs[i]);
    ft_plot_scatter(&plot,xs,ys,N,(Color){0,0,255});

    // barras
    double xb[5]={1,2,3,4,5};
    double hb[5]={0.5,1.0,-0.7,1.2,0.8};
    ft_plot_bar(&plot,xb,hb,5,(Color){0,200,0},0.4);

    // salvar
    stbi_write_png("plot_demo.png",WIDTH,HEIGHT,3,image,WIDTH*3);
    printf("Imagem salva: plot_demo.png\n");
    return 0;
}
