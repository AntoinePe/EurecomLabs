// CImage.cpp: implementation of the CImage class.
//
//////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <math.h>
#include <new.h>
#include "cimage.h"
#include "imgio.h"
#include "macro.h"


/*-----------------------------------------------------------------------------
|  Routine Name: Constructors
|       Purpose:
|         Input: type, width, height
|        Output:
|       Returns:
|
|    Written By: Christian REY
|          Date: 16/05/2001
| Modifications:
|----------------------------------------------------------------------------*/
CImage::CImage(){
	nb_comp=0;
	width=0;
	height=0;
	C[0] = NULL; 	
	C[1] = NULL; 	
	C[2] = NULL; 
}

CImage::CImage(char t, int w, int h){
	type = t;
	width = w;
	height = h;
	 
	if (type == GRAY_IMAGE) nb_comp = 1;
	else nb_comp = 3; 

	for (int i=0;i<nb_comp;i++) {
		C[i] = new short[width*height];
		memset(C[i],0,sizeof(short)*width*height);
	}
}

CImage::CImage(const CImage &anImage){
	type = anImage.type;
	width = anImage.width;
	height = anImage.height;
	nb_comp = anImage.nb_comp;

	for (int i=0;i<nb_comp;i++) {
		if (C[i]!=NULL) delete(C[i]);
      C[i] = new short[width*height];
		memcpy(C[i], anImage.C[i], sizeof(short)*width*height);
	}
}

int CImage::Create(int t, int w, int h){
	type = t;
	width =w;
	height = h;

	if (type == GRAY_IMAGE) nb_comp = 1;
	else nb_comp = 3; 

	for (int i=0;i<nb_comp;i++) {
		if (C[i]!=NULL) delete(C[i]);
      C[i] = new short[width*height];
		if (C[i]==NULL)
			return ERROR;
	}
    
	return SUCCESS;
}

int CImage::Copy(const CImage &anImage){
	type = anImage.type;
	width = anImage.width;
	height = anImage.height;
	nb_comp = anImage.nb_comp;

	for (int i=0;i<nb_comp;i++) {
		if (C[i]!=NULL) delete(C[i]);
      C[i] = new short[width*height];
		memcpy(C[i], anImage.C[i], sizeof(short)*width*height);
	}
 
	return SUCCESS;
}

/*-----------------------------------------------------------------------------
|  Routine Name: Destructor
|       Purpose:
|         Input:
|        Output:
|       Returns:
|
|    Written By: Christian REY
|          Date: 16/05/2001
| Modifications:
|----------------------------------------------------------------------------*/
CImage::~CImage(){
	for (int i=0;i<nb_comp;i++) 
		if (C[i]!=NULL){
			delete(C[i]);
			C[i] = NULL;
		}
}

/*-----------------------------------------------------------------------------
|  Routine Name: ReadPPMFile
|       Purpose: Read a RGB image (binary PPM format)
|         Input: path
|        Output: CImage (this)
|       Returns: statut
|
|    Written By: Christian REY
|          Date: 16/05/2001
| Modifications:
|----------------------------------------------------------------------------*/
int CImage::ReadPPMFile(char *path){
	int err;
	int nb_levels;

	type = RGB_IMAGE;
	nb_comp = 3;
	for (int c=0;c<nb_comp; c++)
		if (C[c]!=NULL) delete(C[c]);
	
	err = ReadPPM(path, &width, &height, &nb_levels, &C[0], &C[1], &C[2]);
	
	if (err==ERROR)
		return ERR_IO_FILE;
	else
		return SUCCESS;
}

/*-----------------------------------------------------------------------------
|  Routine Name: WritePPMFile
|       Purpose: Write a RGB image (binary PPM format)
|         Input: path
|        Output: none
|       Returns: statut
|
|    Written By: Christian REY
|          Date: 16/05/2001
| Modifications: 
|----------------------------------------------------------------------------*/
int CImage::WritePPMFile(char *path){
	int err;

	switch (type) {
		case RGB_IMAGE :
			err = WritePPM(path, width, height, 255, C[0], C[1], C[2]);
			break;
		case GRAY_IMAGE :
			err = WritePPM(path, width, height, 255, C[0], C[0], C[0]);
			break;
		default :
			return ERR_UNKNOWN_IMAGE_TYPE;
	}
	
	if (!err)
		return ERR_IO_FILE;
	else
		return SUCCESS;
}

/*-----------------------------------------------------------------------------
|  Routine Name: ReadPGMFile
|       Purpose: Read a RGB or a Gray image (binary PGM format)
|         Input: path
|        Output: CImage (this)
|       Returns: statut
|
|    Written By: Christian REY
|          Date: 16/05/2001
| Modifications:
|----------------------------------------------------------------------------*/
int CImage::ReadPGMFile(char *path){
	type = GRAY_IMAGE;
	nb_comp = 1;
	
	if (C[0]!=NULL) delete(C[0]);

	ReadPGM(path, &C[0], &width, &height);
	
	if (C[0]==NULL)
		return ERR_IO_FILE;
	
	return SUCCESS;
}

/*-----------------------------------------------------------------------------
|  Routine Name: WritePGMFile
|       Purpose: Write a RGB or a Gray image (binary PGM format)
|         Input: path
|        Output: none
|       Returns: statut
|
|    Written By: Christian REY
|          Date: 16/05/2001
| Modifications: 
|----------------------------------------------------------------------------*/
int CImage::WritePGMFile(char *path){
	if (type!=GRAY_IMAGE)
		return ERROR;
	
	WritePGM(path, C[0], width, height);
	
	return SUCCESS;
}


/*-----------------------------------------------------------------------------
|  Routine Name: BlockMean
|       Purpose: Calcule la valeur moyenne du bloc
|         Input: c, posx, posy, bloc_width, block_height, min, max
|        Output: *value
|       Returns: status
|
|    Written By: Christian REY
|          Date: 29/06/2001
| Modifications:
|----------------------------------------------------------------------------*/
int CImage::BlockMean(short *value, int c, int posx, int posy,
							 int block_width, int block_height){

	if (C[c]==NULL)
		return ERROR;
	if (posx<0 || posy<0 || posx+block_width-1>width
		 ||posy+block_height-1>height)
		return ERROR;
	
	/* A COMPLETER */
	long int sum=0;
	int x,y;
	for (x=posx;x<posx+block_width;x++){
		for (y=posy;y<posy+block_height;y++){
			sum+=C[c][x+width*y];
		}
	}

	(*value) = (short)(sum/(block_width*block_height));

	return SUCCESS;
}


/*-----------------------------------------------------------------------------
|  Routine Name: DrawBadBlock
|       Purpose: Dessine un carre
|         Input: posx, posy, bloc_width, block_height
|        Output: CImage (this)
|       Returns: status
|
|    Written By: Christian REY
|          Date: 29/06/2001
| Modifications:
|----------------------------------------------------------------------------*/
int CImage::DrawBadBlock(int posx, int posy, int block_width, int block_height){
	if (posx<0 || posy<0 || posx+block_width-1>width
		 ||posy+block_height-1>height)
		return ERROR;

	for (int c=0; c<nb_comp; c++) {
		if (C[c]==NULL)
			return ERROR;
		for (int j=0;j<block_height;j++)
			for (int i=0;i<block_width;i++){
				if (i==0 || j==0)
					C[c][i+posx+(j+posy)*width] = 255;
				else if (i==block_width-1 || j==block_height-1)
					C[c][i+posx+(j+posy)*width] = 0;
				else if (i==j || (block_width-1-i)==j) {
					if (c==0)
						C[c][i+posx+(j+posy)*width] = 255;
					else
						C[c][i+posx+(j+posy)*width] = 0;	
				}
				else
					C[c][i+posx+(j+posy)*width] = 240;
			}
	}
	
	return SUCCESS;
}

/*-----------------------------------------------------------------------------
|  Routine Name: DrawFlatBlock
|       Purpose:
|         Input: posx, posy, bloc_width, block_height, gray level
|        Output: CImage (this)
|       Returns: status
|
|    Written By: Christian REY
|          Date: 29/06/2001
| Modifications:
|----------------------------------------------------------------------------*/
int CImage::DrawFlatBlock(int c, int posx, int posy, int block_width,
								  int block_height, short g){
	if (posx<0 || posy<0 || posx+block_width-1>width
		 || posy+block_height-1>height)
		return ERROR;

	if (C[c]==NULL)
		return ERROR;
  
	for (int j=0;j<block_height;j++)
		for (int i=0;i<block_width;i++)
			C[c][i+posx+(j+posy)*width] = g;
	  
	return SUCCESS;
}


/*-----------------------------------------------------------------------------
|  Routine Name: CRCBlock
|       Purpose: Calculate CRC value of a block
|         Input: c, posx, posy, block_width, block_height, *crcTable
|        Output: none
|       Returns: crc
|
|    Written By: Christian REY
|          Date: 02/05/2002
| Modifications:
|----------------------------------------------------------------------------*/
unsigned long CImage::CRCBlock(int c, int posx, int posy, int block_width,
										 int block_height, const unsigned long *crcTable){
	register unsigned long crc;
	int val;

	crc = 0xFFFFFFFF;

	for (int k=0; k<block_width; k++)
		for (int l=0; l<block_height; l++) {
			val = (int)C[c][posx+k + (posy+l)*width];
			SETBIT(val,0,0);
			crc = ((crc>>8) & 0x00FFFFFF) ^ (crcTable[ (crc^val) & 0xFF ]);
		}

	return (crc^0xFFFFFFFF);
}


/*-----------------------------------------------------------------------------
|  Routine Name: InsertNoiseLSB	
|       Purpose: Met les LSB des pixels de l'image a zero 
|         Input: CImage (this)
|        Output: CImage (this)
|       Returns: status
|
|    Written By: Christian REY
|          Date: 25/04/2002
| Modifications:
|----------------------------------------------------------------------------*/
int CImage::InsertNoiseLSB(){
	int noise;
	srand(0);
	noise=rand()%2;
	for (int c=0; c<nb_comp;c++) {
		if (C[c]==NULL)
			return ERROR;
			
		/* A COMPLETER */
		else
			for(int i=0;i<width;i++){
				for(int j=0;j<height;j++){
					SETBIT(C[c][i+j*width],0,noise); /*Set the pixels LSB value to randomly 0 or 1*/
					noise=rand()%2; /*Change the random value*/
				}
			}
	}

	return SUCCESS;
}

/*-----------------------------------------------------------------------------
|  Routine Name: ExtractNoiseLSB	
|       Purpose: 
|         Input: CImage (this)
|        Output: CImage (this)             
|       Returns: status
|
|    Written By: Christian REY
|          Date: 25/04/2002
| Modifications:
|----------------------------------------------------------------------------*/
int CImage::ExtractNoiseLSB(){
	int noise;
	srand(0);
	int LSBValue;

	for (int c=0; c<nb_comp;c++) {
		if (C[c]==NULL)
			return ERROR;	
     
		/* A COMPLETER */
		else
			for(int i=0;i<width;i++){
				for(int j=0;j<height;j++){
					noise=rand()%2;
					LSBValue=GETBIT(C[c][i+j*width],0);
					if(LSBValue==noise){
						C[c][i+j*width]=0;
					}
					else{
						C[c][i+j*width]=255;
					}
				}
			}
	}      
  
	return SUCCESS;
}

/*-----------------------------------------------------------------------------
|  Routine Name: InsertCRCLSB	
|       Purpose:
|         Input:
|        Output:
|       Returns: status
|
|    Written By: Christian REY
|          Date: 25/04/2002
| Modifications:
|----------------------------------------------------------------------------*/
int CImage::InsertCRCLSB(){
	int nb_xblocks = width/8;
	int nb_yblocks = height/8;
	unsigned long crc;
	unsigned long *crcTable;
	short bit;
	
	CRCTable(&crcTable);

	for (int c=0; c<nb_comp;c++) {
		if (C[c]==NULL)
			return ERROR;	

		for (int i=0; i<nb_xblocks; i++)
			for (int j=0; j<nb_yblocks; j++) {
				/* A COMPLETER */
				crc = CRCBlock(c,8*i,8*j,8,8,crcTable);//Compute the checksum of the block (i,j)
					for (int k=0; k<32; k++){
					bit=GETBIT(crc,k);//Get the k-th bit of crc
					SETBIT(									//Set
						C[c][8*i+k%8+width*(8*j+k/8)],  //the pixel's at posx=(8*i+k%8) and posy=(8*j+k/8)
						0,									//LSB
						bit									//to bit
					);	
					}
			}
	}

	free(crcTable);
  
	return SUCCESS;
}

/*-----------------------------------------------------------------------------
|  Routine Name: ExtractCRCLSB	
|       Purpose:  
|         Input: 
|        Output:              
|       Returns: status
|
|    Written By: Christian REY
|          Date: 25/04/2002
| Modifications:
|----------------------------------------------------------------------------*/
int CImage::ExtractCRCLSB(){
	int nb_xblocks = width/8;
	int nb_yblocks = height/8;
	unsigned long crc, xcrc;
	unsigned long *crcTable;
	short bit;

	CRCTable(&crcTable);
	
	
	for (int c=0; c<nb_comp;c++) {
		if (C[c]==NULL)
			return ERROR;	

		for (int i=0; i<nb_xblocks; i++)
			for (int j=0; j<nb_yblocks; j++) {	

				/* A COMPLETER */
				crc = CRCBlock(c,8*i,8*j,8,8,crcTable);//Compute the checksum of the block (i,j)
				for (int k=0; k<32; k++){
					bit=GETBIT(C[c][8*i+k%8+width*(8*j+k/8)],0);
					SETBIT(xcrc,k,bit);
				}
				if(crc!=xcrc){
					DrawBadBlock(8*i,8*j,8,8);
				}
			}
	}

	free(crcTable);
  
	return SUCCESS;
}

/*-----------------------------------------------------------------------------
|  Routine Name: InsertSelfEmbeddingLSB	
|       Purpose: 
|         Input: 
|        Output:              
|       Returns: status
|
|    Written By: Christian REY
|          Date: 25/04/2002
| Modifications:
|----------------------------------------------------------------------------*/
int CImage::InsertSelfEmbeddingLSB(){
	int nb_xblocks = width/8;
	int nb_yblocks = height/8;
	unsigned long crc;
	unsigned long *crcTable;
	short bit;
	short m[4];
	int ti,tj;

	
	CRCTable(&crcTable);
	ti = 4;
	tj = 4;

	for (int c=0; c<nb_comp;c++) {
		if (C[c]==NULL)
			return ERROR;	

		for (int i=0; i<nb_xblocks; i++){
			for (int j=0; j<nb_yblocks; j++) {

				/* A COMPLETER */
				crc = CRCBlock(c,8*i,8*j,8,8,crcTable);//Compute the checksum of the block (i,j)

				for (int k=0; k<32; k++){
					bit=GETBIT(crc,k);//Get the k-th bit of crc
					SETBIT(									//Set
						C[c][8*i+k%8+width*(8*j+k/8)],  //the pixel's at posx=(8*i+k%8) and posy=(8*j+k/8)
						0,									//LSB
						bit									//to bit
					);	
				}
				//Compute the mean of the 4 blocks

				BlockMean(&m[0],c,i*8  ,j*8  ,4,4);
				BlockMean(&m[1],c,i*8+4,j*8  ,4,4);
				BlockMean(&m[2],c,i*8  ,j*8+4,4,4);
				BlockMean(&m[3],c,i*8+4,j*8+4,4,4);

				//Set the last 32 LSB according to mean
				for(int k=0;k<4;k++){//for each sub-block
					for(int l=0;l<8;l++){//for each bit
						bit=GETBIT(m[k],l);
						SETBIT(C[c][ ((8*(i+ti)+l)%width) +width* ((8*(j+tj)+k+4)%height) ],0,bit);
					}
				}
			}
		}
	}

	free(crcTable);

	return SUCCESS;
}

/*-----------------------------------------------------------------------------
|  Routine Name: ExtractSelfEmbeddingLSB	
|       Purpose: 
|         Input: 
|        Output:              
|       Returns: status
|
|    Written By: Christian REY
|          Date: 25/04/2002
| Modifications:
|----------------------------------------------------------------------------*/
int CImage::ExtractSelfEmbeddingLSB(){
	int nb_xblocks = width/8;
	int nb_yblocks = height/8;
	unsigned long crc, xcrc=0;
	unsigned long *crcTable;
	short bit;
	short m[4];
	int ti,tj;


	CRCTable(&crcTable);
	ti = 4;
	tj = 4;
	
	for (int c=0; c<nb_comp;c++) {
		if (C[c]==NULL)
			return ERROR;	

		for (int i=0; i<nb_xblocks; i++)
			for (int j=0; j<nb_yblocks; j++) {

				/* A COMPLETER */
				crc = CRCBlock(c,8*i,8*j,8,8,crcTable);//Compute the checksum of the block (i,j)
				for (int k=0; k<32; k++){
					bit=(short)GETBIT(C[c][8*i+k%8+width*(8*j+k/8)],0);
					SETBIT(xcrc,k,bit);
				}//Get the checksum embeded in the block

				if(crc!=xcrc){//if this is not a match,
					//extract the restoration bits

					m[0]=0;
					m[1]=0;
					m[2]=0;
					m[3]=0;

					/*
					for(int k=0;k<4;k++){//for each sub-block
						for(int l=0;l<8;l++){//for each bit
							bit=(short)GETBIT(C[c][ (8*(i+ti)%nb_xblocks+l+4) +width* (8*(j+tj)%nb_yblocks+k) ],0);
							SETBIT(m[k],l,bit);
						}
					}*/

					m[0]=0;
					m[1]=0;
					m[2]=0;
					m[3]=0;
					
					for(int kk=0; kk<8; kk++){
						for(int ll=4; ll<8; ll++){
							bit=(short)GETBIT(C[c][(8*(j+tj%nb_yblocks)+ll)*width + 8*(i+ti%nb_xblocks)+kk],0);
							SETBIT(m[ll-4],kk,bit);
						}
					}
					//Restoration of the tampered blocks
					DrawFlatBlock(c,8*i  ,8*j  ,4,4,m[0]);
					DrawFlatBlock(c,8*i+4,8*j  ,4,4,m[1]);
					DrawFlatBlock(c,8*i  ,8*j+4,4,4,m[2]);
					DrawFlatBlock(c,8*i+4,8*j+4,4,4,m[3]);
				}
			}
		}

	free(crcTable);
	return SUCCESS;
}


/**************************************************************/
/*			     FONCTIONS A AMIES                               */
/**************************************************************/
void CRCTable(unsigned long **crcTable){
	unsigned long	crc, poly;
	int i, j;
  
	(*crcTable) = (unsigned long *)malloc(sizeof(unsigned long)*256);
	poly = 0xEDB88320L;
	
	for (i=0; i<256; i++) {
		crc = i;
		for (j=8; j>0; j--) {
			if (crc&1) 
				crc = (crc >> 1) ^ poly;
			else 
				crc >>= 1;
		}
		(*crcTable)[i] = crc;
	}
}
