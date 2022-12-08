#include <GL/gl.h>
#include <stdio.h> 
#include <stdlib.h>
#include <assert.h>


struct Image {
    unsigned long sizeX;
    unsigned long sizeY;
    GLubyte *data;
};
typedef struct Image Image;
typedef struct Arbre{
    int r;
    int g;
    int b;
    struct Arbre *un;
    struct Arbre *deux;
    struct Arbre *trois;
    struct Arbre *quatre;
}Arbre;

typedef unsigned short utab [3][3][3];

int ImageLoad_PPM(char *filename, Image *image);
void upsidedown(Image *);
void image_quadtree(Arbre *,Image **,int deb1,int deb2,int fin1,int fin2);
void Quadtree_c(Image *,Arbre **,char *,int );
void Dequad(Image **,Arbre **,FILE *);
void printarbre(Arbre *);
