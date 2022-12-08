#include <GL/glut.h>
#include <unistd.h>     
#include <math.h>
#include "ima.h"
#include <string.h>
#include <time.h>  

Image *image;
char **nom;
Arbre *q;
char *name;
#define ESCAPE 27
int op=0;

void Keyboard(unsigned char key, int x, int y)  {
  switch(key){
  case ESCAPE :
    exit(0);                   
    break;
  default:
    fprintf(stderr, "Unused key\n");
  }
}

void Mouse(int button, int state, int x, int y) {

  switch(button){
  case GLUT_LEFT_BUTTON:
    break;
  case GLUT_MIDDLE_BUTTON:
    break;
  case GLUT_RIGHT_BUTTON:
    break;    
  }
  glutPostRedisplay();
}

int Init(char *s) {

  image = (Image *) malloc(sizeof(Image));
  if (image == NULL) {
    fprintf(stderr, "Out of memory\n");
    return(-1);
  }
  if (ImageLoad_PPM(s, image)==-1) 
	return(-1);
  printf("tailles %d %d\n",(int) image->sizeX, (int) image->sizeY);

  glClearColor(0.0, 0.0, 0.0, 0.0);
  glShadeModel(GL_FLAT);
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  glutReshapeWindow(image->sizeX, image->sizeY);
  
  return (0);
}
int ReInit() {
  /*
  if (ImageLoad_PPM(s, image)==-1) 
	return(-1);
  */
  glClearColor(0.0, 0.0, 0.0, 0.0);
  glShadeModel(GL_FLAT);
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  glutReshapeWindow(image->sizeX, image->sizeY);
  
  return (0);
}

void Display(void) {

  glClear(GL_COLOR_BUFFER_BIT);
  glDrawPixels(image->sizeX, image->sizeY, GL_RGB, GL_UNSIGNED_BYTE, 
	       image->data);

  glFlush();
}

void Reshape(int w, int h) {
  glViewport(0, 0, (GLsizei)w, (GLsizei) h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluOrtho2D(0.0, (GLdouble) w, 0.0, (GLdouble)h);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
}

void menuFunc(int item) {
  switch(item){
  case 0:
    free(image);
    exit(0);
    break;
  case 1:
    double time_spent = 0.0;
    clock_t begin = clock();
    Quadtree_c(image,&q,name,strlen(name));
    clock_t end = clock();
    time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
    printf("Image Compresser\n");
    printf("temps de compression d'une image de %d x %d = %f second\n", (int) image->sizeX, (int) image->sizeY, time_spent);
	for(int i=0;i<image->sizeX*image->sizeY*3;i++){
		image->data[i]=0;
	}
  //printarbre(q);
    Display();
    break;
  case 2:
    printf("Revenir image départ\n");
    Init(nom[1]);
    Display();
    break;
  case 3:
    printf("Nom de l image : %s\n", name);
    printf("Taille de l image : %d %d\n", (int) image->sizeX, (int) image->sizeY);
    break;
  default:
    break;
  }
}

int Init2(char *s) {

  image = (Image *) malloc(sizeof(Image));
  FILE *fp;
  fp=fopen(s,"r");
  if(fp == NULL){
      printf("Error!");   
      exit(1);             
   }
   char line[100];
  fgets(line,100,fp);
  char new_line[100];
  int vef=0;
  for(int i=0;i<100;i++){
    if(line[i]=='\n'){
      vef=1;
      new_line[i]='\0';
    }
    if(vef==0){
      new_line[i]=line[i];
    }
  }
  printf("%s\n",new_line);
  fgets(line,100,fp);
  vef=0;
  for(int i=0;i<100;i++){
    if(line[i]=='\n'){
      vef=1;
      new_line[i]='\0';
    }
    if(vef==0){
      new_line[i]=line[i];
    }
  }
  image->sizeX= atoi(new_line);
  fgets(line,100,fp);
  vef=0;
  for(int i=0;i<100;i++){
    if(line[i]=='\n'){
      vef=1;
      new_line[i]='\0';
    }
    if(vef==0){
      new_line[i]=line[i];
    }
  }
  image->sizeY= atoi(new_line);
  printf("tailles %d %d\n",(int) image->sizeX, (int) image->sizeY);

  if (image == NULL) {
    fprintf(stderr, "Out of memory\n");
    return(-1);
  }
  int size = image->sizeX * image->sizeY * 3;
	image->data = (GLubyte *) malloc ((size_t) size * sizeof (GLubyte));
	assert(image->data);
  double time_spent = 0.0;
  clock_t begin = clock();
  Dequad(&image,&q,fp);
  clock_t end = clock();
  time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
  printf("Image Decompresser\n");
  printf("temps de decompression d'une image de %d x %d = %f second\n", (int) image->sizeX, (int) image->sizeY, time_spent);
  fclose (fp);
  glClearColor(0.0, 0.0, 0.0, 0.0);
  glShadeModel(GL_FLAT);
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  glutReshapeWindow(image->sizeX , image->sizeY);
  
  return (0);
}

int main(int argc, char **argv) {
  if (argc<2) {
    fprintf(stderr, "Usage : palette nom_de_fichier\n");
    exit(0);
  }
  if(argc==2){
    op=1;
    glutInit(&argc, argv);}
  else{
    int d=strcmp(argv[1],"-d"); 
    if(d==0){
      glutInit(&argc, argv);
    }
    else{
      exit(1);
    }
  }
  glutInitDisplayMode(GLUT_RGB | GLUT_SINGLE);
  glutInitWindowSize(640,480);  
  glutInitWindowPosition(100, 100);  
  glutCreateWindow("Quadtree");  
  nom=argv;
  if(op==1){
    name=argv[1];  
  Init(argv[1]);}
  else{
    name=argv[2];  
  Init2(argv[2]);
  }

  glutCreateMenu(menuFunc);
  glutAddMenuEntry("Quit", 0);
  glutAddMenuEntry("Compresser", 1);
  glutAddMenuEntry("default image", 2);
  glutAddMenuEntry("Informations", 3);
  glutAttachMenu(GLUT_RIGHT_BUTTON);

  glutDisplayFunc(Display);  
  glutReshapeFunc(Reshape);
  glutKeyboardFunc(Keyboard);
  
  glutMouseFunc(Mouse);

  glutMainLoop();  

  return 1;
}
