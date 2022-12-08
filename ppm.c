#include "ima.h"

#define INSPIREPAR "RPFELGUEIRAS"
#define CREATOR "JJ"
#define RGB_COMPONENT_COLOR 255

int ImageLoad_PPM(char *filename, Image *img){
	char d, buff[16];
	FILE *fp;
	int c, rgb_comp_color, size;
	fp = fopen(filename, "rb");
	if (!fp) {
		fprintf(stderr, "Unable to open file '%s'\n", filename);
		exit(1);
	}

	//read image format
	if (!fgets(buff, sizeof(buff), fp)) {
		perror(filename);
		exit(1);
	}

	//check the image format
	if (buff[0] != 'P' || buff[1] != '6') {
		fprintf(stderr, "Invalid image format (must be 'P6')\n");
		exit(1);
	}

	//check for comments
	c = getc(fp);
	while (c == '#') {
		while (getc(fp) != '\n')
			;
		c = getc(fp);
	}
	ungetc(c, fp);
	//read image size information
	if (fscanf(fp, "%lu %lu", &img->sizeX, &img->sizeY) != 2) {
		fprintf(stderr, "Invalid image size (error loading '%s')\n", filename);
		exit(1);
	}

	//read rgb component
	if (fscanf(fp, "%d", &rgb_comp_color) != 1) {
		fprintf(stderr, "Invalid rgb component (error loading '%s')\n", filename);
		exit(1);
	}
	fscanf(fp, "%c ", &d);
	//check rgb component depth
	if (rgb_comp_color!= RGB_COMPONENT_COLOR) {
		fprintf(stderr, "'%s' does not have 8-bits components\n", filename);
		exit(1);
	}
	/* allocation memoire */
	size = img->sizeX * img->sizeY * 3;
	img->data = (GLubyte *) malloc ((size_t) size * sizeof (GLubyte));
	assert(img->data);


	//read pixel data from file
	if (fread(img->data, (size_t) 1, (size_t) size, fp) == 0) {
		fprintf(stderr, "Error loading image '%s'\n", filename);
	}

	upsidedown(img);

	fclose(fp);
	return 1;
}

void upsidedown(Image *img) {
	/* remettre l image dans le bon sens */
	int b, c, size, sizex;
	GLubyte tmp, * ptrdeb, *ptrfin, *lastline;
	size = img->sizeX * img->sizeY * 3;
	sizex = img->sizeX;
	lastline = img->data + size - sizex * 3;
	for (b = 0; b < img->sizeY / 2; b++) {
		ptrdeb = img->data + b * sizex * 3;
		ptrfin = lastline - (b * sizex * 3);
		for (c = 0; c < 3 * sizex; c++) {
			tmp = *ptrdeb;
			*ptrdeb = *ptrfin;
			*ptrfin = tmp;
			ptrfin++;
			ptrdeb++;
		}		
	}
}

int verif(Image *img,int deb1,int deb2,int fin1,int fin2){
	int r=img->data[(((deb2)*img->sizeX)+deb1)*3];
	int g=img->data[(((deb2)*img->sizeX)+deb1)*3+1];
	int b=img->data[(((deb2)*img->sizeX)+deb1)*3+2];
	for(int i=0;i<fin2-deb2;i++){
		for(int y=0;y<fin1-deb1;y++){
			if(r!=img->data[(((deb2+i)*img->sizeX)+deb1+y)*3] || g!=img->data[(((deb2+i)*img->sizeX)+deb1+y)*3+1] || b!=img->data[(((deb2+i)*img->sizeX)+deb1+y)*3+2]){
				return 1;
			}
		}
	}
	return 0;
}

void construct(Arbre **quad,Image *img,int deb1,int deb2,int fin1,int fin2){
	if(fin1-deb1==0 || fin2-deb2==0){
		*quad=NULL;
		return;
	}
	else{
	Arbre *t=malloc(sizeof(Arbre));
	int a=verif(img,deb1,deb2,fin1,fin2);
	if(a==0){
	t->r=img->data[(((deb2)*img->sizeX)+deb1)*3];
	t->g=img->data[(((deb2)*img->sizeX)+deb1)*3+1];
	t->b=img->data[(((deb2)*img->sizeX)+deb1)*3+2];
	t->un=NULL;
	t->deux=NULL;
	t->trois=NULL;
	t->quatre=NULL;
	*quad=t;
	return;
	}
	else{
		t->r=-1;
		t->g=-1;
		t->b=-1;

		int u=(fin1-deb1)/2;
		int ui=(fin2-deb2)/2;
		construct(&t->un,img,deb1,deb2+ui,deb1+u,fin2);
		construct(&t->deux,img,deb1+u,deb2+ui,fin1,fin2);
		construct(&t->trois,img,deb1+u,deb2,fin1,deb2+ui);
		construct(&t->quatre,img,deb1,deb2,deb1+u,deb2+ui);
		*quad=t;
	}}
}

void printarbre(Arbre *q){
	if(q==NULL){return;}
	if(q->r!=-1){
	printf("%d|%d|%d\n",q->r,q->g,q->b);}
	printarbre(q->un);
	printarbre(q->deux);
	printarbre(q->trois);
	printarbre(q->quatre);
}
void write(Arbre *q,FILE *fp){
	if(q==NULL){fputs("\n", fp);
	return;}
	if(q->r!=-1){
	char colr[100];
	sprintf(colr, "%d", q->r);
	fputs(colr, fp);
	fputc(' ',fp);
	char colg[100];
	sprintf(colg, "%d", q->g);
	fputs(colg, fp);
	fputc(' ',fp);
	char colb[100];
	sprintf(colb, "%d", q->b);
	fputs(colb, fp);
	fputc(' ',fp);}
	if(q->r==-1){
		fputs("\t", fp);
	}
	write(q->un,fp);
	write(q->deux,fp);
	write(q->trois,fp);
	write(q->quatre,fp);
}

void savetofile(char* nom,int taille,Arbre *q,int x,int y){
	int new_taille=0;
	int test=0;
	for(int i=0;i<taille;i++){
		if(nom[i]=='.'){
			test=1;
		}
		if(test==0){
			new_taille++;
		}
	}
	char new_nom[new_taille+4];
	test=0;
	for(int i=0;i<taille;i++){
		if(nom[i]=='.'){
			test=1;
		}
		if(test==0){
			new_nom[i]=nom[i];
		}
	}
	new_nom[new_taille]='.';
	new_nom[new_taille+1]='q';
	new_nom[new_taille+2]='d';
	new_nom[new_taille+3]='t';
	new_nom[new_taille+4]='\0';
	FILE *fp;
	fp  = fopen (new_nom, "w+");
	fputs(nom, fp);
	fputc('\n',fp);
	char xnum[100];
	sprintf(xnum, "%d", x);
	fputs(xnum, fp);
	fputc('\n',fp);
	char ynum[100];
	sprintf(ynum, "%d", y);
	fputs(ynum, fp);
	fputc('\n',fp);
	write(q,fp);
	fclose (fp);
}

void Quadtree_c(Image *img,Arbre **q,char *nom,int taille){
	construct(q,img,0,0,img->sizeX,img->sizeY);
	savetofile(nom,taille,*q,img->sizeX,img->sizeY);
}

void image_quadtree(Arbre *q,Image **img,int deb1,int deb2,int fin1,int fin2){
	if(q==NULL){
	return;}
	if(q->r!=-1){
	int col=fin1-deb1;
	int lin=fin2-deb2;
	for(int i=0;i<lin;i++){
		for(int y=0;y<col;y++){
			(*img)->data[(((deb2+i)*(*img)->sizeX)+deb1+y)*3]=q->r;
			(*img)->data[(((deb2+i)*(*img)->sizeX)+deb1+y)*3+1]=q->g;
			(*img)->data[(((deb2+i)*(*img)->sizeX)+deb1+y)*3+2]=q->b;

			}
		}

	}
	int u=(fin1-deb1)/2;
	int ui=(fin2-deb2)/2;
	image_quadtree(q->un,img,deb1,deb2+ui,deb1+u,fin2);
	image_quadtree(q->deux,img,deb1+u,deb2+ui,fin1,fin2);
	image_quadtree(q->trois,img,deb1+u,deb2,fin1,deb2+ui);
	image_quadtree(q->quatre,img,deb1,deb2,deb1+u,deb2+ui);
}

void constrquad(Arbre **q,FILE *fp){
	char a=fgetc(fp);
	if(a=='\n'){
		q=NULL;
		return;
	}
	else{ 
	Arbre *t=malloc(sizeof(Arbre));
	if(a=='\t'){
	t->r=-1;
	t->g=-1;
	t->b=-1;

	constrquad(&t->un,fp);
	constrquad(&t->deux,fp);
	constrquad(&t->trois,fp);
	constrquad(&t->quatre,fp);
	*q=t;
	}
	else{
		Arbre *t=malloc(sizeof(Arbre));
		char col[3];
		col[0]=a;
		int i=1;
		while((a=fgetc(fp)) !=' '){
			col[i]=a;
			i++;
		}
		col[i]='\0';
		t->r=atoi(col);
		i=0;
		while((a=fgetc(fp)) !=' '){
			col[i]=a;
			i++;
		}
		col[i]='\0';
		t->g=atoi(col);
		i=0;
		while((a=fgetc(fp)) !=' '){
			col[i]=a;
			i++;
		}
		col[i]='\0';
		t->b=atoi(col);

	t->un=NULL;
	t->deux=NULL;
	t->trois=NULL;
	t->quatre=NULL;

	constrquad(&t->un,fp);
	constrquad(&t->deux,fp);
	constrquad(&t->trois,fp);
	constrquad(&t->quatre,fp);
	*q=t;
	}}
}
void Dequad(Image **img,Arbre **q,FILE *fp){
	constrquad(q,fp);
	image_quadtree(*q,img,0,0,(*img)->sizeX,(*img)->sizeY);
	}