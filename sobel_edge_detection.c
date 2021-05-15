#include <stdio.h>
#include <stdlib.h>
#include <math.h>
/* run this program using the console pauser or add your own getch, system("pause") or input loop */

int charToInt(char* charValue){
	int i=0;
	int intValue=0;
	int coef=1;
	while(charValue[i] != '\0' && charValue[i] != ' ' && charValue[i] != '\n'){
		i++;		//eleman sayisi
	}
	for(i;i>0;i--){//sondan basladik birler basamagi
		intValue+=coef*(charValue[i-1] - '0'); //her karakterde bir kere daha 10la çarpiliyor yani basamak degeri ile sayi degeri elde ediliyor
		coef=coef*10;	
	}
	return intValue;
}
void writeImg(unsigned char** image, int rows, int cols, int maxG, char* header, char* filename){	
   FILE* newPgm;
   int i,j;
   newPgm = fopen(filename, "wb"); //binary modda yazmaya acildi
   fprintf(newPgm, header); // header
   fprintf(newPgm, "\n"); //comment
   fprintf(newPgm, "%d %d\n", cols, rows); // sütun ve satir
   fprintf(newPgm, "%d\n", maxG); // max gri seviyesi dosyaya yazildi
   for (i=0; i < rows; i++) {
        fwrite((void*)&(image[i][0]),sizeof(unsigned char),cols, newPgm); //sonuc resmi dosyaya yazildi
    }
	
   fclose(newPgm);
}
unsigned char** padding(unsigned char** image, int row, int col){
	int i=0,j=0;
	while(i<row){
		image[i][0]=0;
		image[i][col-1]=0;
		i++;
	}
	while(j<col){
		image[0][j]=0;
		image[row-1][j]=0;
		j++;
	}							//resmin cercevesine 0 degeri atandi
	return image;
} 
unsigned char** normalization(unsigned char** image, int row, int col,int maxGrayVal){
	int i,j,min=10000,max=-1000;
	float norm;
	 for(i=0;i<row;i++){
		for(j=0;j<col;j++){			   
       		if (image[i][j]<min)
   			    min=image[i][j];
			if(image[i][j]>max)
				max=image[i][j]; //min ve max degerler bulundu
        	}	
    	}                                                                                                                               
	for(i=1;i<row-1;i++){
		for(j=1;j<col-1;j++){
	 		norm=((image[i][j]-min)/(float)(max-min))*255; //0-255 aralýgýna normalize edildi
			image[i][j] = (char)norm;
		}
	}
	return image;
	
}
void detectEdges(unsigned char** pgmImage,unsigned char** newImage,unsigned char** newImageX,unsigned char** newImageY, int row, int col,int maxGrayVal, char* magicNum){
	int sobel_x[3][3]={{-1, 0, 1},{-2, 0, 2},{-1, 0, 1}};
	int sobel_y[3][3]={{1, 2, 1}, {0, 0, 0},{-1,-2,-1}}; //sobel kernellari olusturuldu
	int i,j;
	char x_file[50] = "out_image_gx.pgm";
	char y_file[50] = "out_image_gy.pgm";
	char img_file[50] = "out_image_g.pgm";
	
	for(i=1;i<row-1;i++){
		for(j=1;j<col-1;j++){
			int x = (int)(pgmImage[i-1][j-1])*sobel_x[0][0]+(pgmImage[i-1][j])*sobel_x[0][1]+(pgmImage[i-1][j+1])*sobel_x[0][2]+(pgmImage[i][j-1])*sobel_x[1][0]+(pgmImage[i][j])*sobel_x[1][1]+(pgmImage[i][j+1])*sobel_x[1][2]+(pgmImage[i+1][j-1])*sobel_x[2][0]+(pgmImage[i+1][j])*sobel_x[2][1]+(pgmImage[i+1][j+1])*sobel_x[2][2];
			int y = (int)(pgmImage[i-1][j-1])*sobel_y[0][0]+(pgmImage[i-1][j])*sobel_y[0][1]+(pgmImage[i-1][j+1])*sobel_y[0][2]+(pgmImage[i][j-1])*sobel_y[1][0]+(pgmImage[i][j])*sobel_y[1][1]+(pgmImage[i][j+1])*sobel_y[1][2]+(pgmImage[i+1][j-1])*sobel_y[2][0]+(pgmImage[i+1][j])*sobel_y[2][1]+(pgmImage[i+1][j+1])*sobel_y[2][2];
			if (x<0)		//islemler yapildiktan sonra düzeltmeler yapiliyor
				newImageX[i][j] = 0;
			else if(x>255)
				newImageX[i][j] = 255;
			else
				newImageX[i][j] = (unsigned char) x;
			
			if(y<0)
				newImageY[i][j] = 0;
			else if(y>255)
				newImageY[i][j]=255;
			else
				newImageY[i][j]=(unsigned char) y;
			
			newImage[i][j]=(unsigned char)(sqrt(x * x - y * y)); //deger hesaplanip atandi
		}
	}
	newImage=padding(newImage,row,col);
	newImageX=padding(newImageX,row,col);
	newImageY=padding(newImageY,row,col); //padding yapilidi
	
	newImageY=normalization(newImageY,row,col,maxGrayVal);
	newImageX=normalization(newImageX,row,col,maxGrayVal);
	newImage=normalization(newImage,row,col,maxGrayVal); //min max normalizasyonu yapildi
	
	
	writeImg(newImage,row,col,maxGrayVal,magicNum,img_file); 
	writeImg(newImageX,row,col,maxGrayVal,magicNum,x_file); 
	writeImg(newImageY,row,col,maxGrayVal,magicNum,y_file); //dosyaya yazildi
	
	printf("\nresimler olusturuldu");
}

void readImage(char* filename){
	char ch;
	char magicNum[2],comment[70];
	char r[5],c[5],maxG[5];
	unsigned char** pgmImage, **newImage, **newImageX, **newImageY;	
	int i,j,count,row,col,maxGrayVal;
	FILE* imgFile;
	imgFile = fopen(filename, "rb"); //dosya binary modda okumaya acildi
	if(imgFile) 
		printf("file opened\n");
	else
		printf("file cannot be opened\n");
	
	fscanf(imgFile, "%s\n", &magicNum); //header
	if(getc(imgFile)=='#')
		fscanf(imgFile,"%[^\n]\n",comment); //comment
	else
		fseek(imgFile, -1, SEEK_CUR);
	fscanf(imgFile, "%s %s\n", &c, &r); //sütun satir
	fscanf(imgFile, "%s\n",&maxG);		//max gri seviyesi
	row = charToInt(r);
	col = charToInt(c);
	maxGrayVal = charToInt(maxG);
	0
	pgmImage = (unsigned char **)malloc(row * sizeof(unsigned char*));
	for(i = 0; i < row; i++) pgmImage[i] = (unsigned char *)malloc(col * sizeof(unsigned char));
	newImage = (unsigned char **)malloc(row * sizeof(unsigned char*));
	for(i = 0; i < row; i++) newImage[i] = (unsigned char *)malloc(col * sizeof(unsigned char));
	newImageX = (unsigned char **)malloc(row * sizeof(unsigned char*));
	for(i = 0; i < row; i++) newImageX[i] = (unsigned char *)malloc(col * sizeof(unsigned char));
	newImageY = (unsigned char **)malloc(row * sizeof(unsigned char*));
	for(i = 0; i < row; i++) newImageY[i] = (unsigned char *)malloc(col * sizeof(unsigned char)); //matrisler
	i=0;
	j=0;
	for (i = 0; i <row; i++) {
	    fread((void *)&(pgmImage[i][0]),sizeof(unsigned char),col,imgFile); //satir satir pgmImage matrisine yazýlýyor
		if (feof(imgFile)) break;
	}
	detectEdges(pgmImage,newImage,newImageX,newImageY,row,col,maxGrayVal,magicNum); //fonksiyona gönderiliyor
	free(pgmImage);
	free(newImageX);
	free(newImageY);
	free(newImage);
	fclose(imgFile);
}



int main(int argc, char *argv[]) {
	printf("PGM resmin yolunu giriniz\n");
	char filename[500];
	scanf("%s",&filename);
	
	readImage(filename);
	
   
return 0;
}
