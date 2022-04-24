#include<stdlib.h>
#include<string.h>
#include<stdio.h>

int key_len;
int text_len;


//Function to determine ith byte of the key
double Check(char cand,int idx,double* asciiFre,char* buffer){
	double alpha[256]={0,};
	double tt=0;
	for(int i=idx;i<text_len;i+=key_len){
		unsigned char now=buffer[i]^cand;
		alpha[now]+=1;
		tt+=1;
	}
	for(int i=0;i<256;i++){
		alpha[i]=alpha[i]/tt;
	}
	double tmp=0;
	for(int j=0;j<256;j++){
		tmp+=alpha[j]*asciiFre[j];
	}
	return tmp;
}


int main(){
	FILE *fp=NULL;
	unsigned char *buffer=NULL;
	off_t ps=0;
	long long size;
	if((fp=fopen("./hw1_input.txt","r"))==NULL){
		perror("Failed to open file");	
	}
	fseek(fp,0,SEEK_END);
	size=ftell(fp);
	buffer=malloc(size+1);
	memset(buffer,0,size+1);
	fseek(fp,0,SEEK_SET);
	long long i=0;
	fread(buffer,size,1,fp);


	key_len=1;
	text_len=(int)size;
	
	//Finding key length
	double poss_key[11]={0,};
	while(key_len<=10){
		double alpha[256]={0,};
		double tt=0;
		for(int i=0;i<text_len;i+=key_len){
			alpha[buffer[i]]+=1;
			tt+=1;
		}
		for(int i=0;i<256;i++){
			alpha[i]=alpha[i]/tt;

		}
		double tmp=0;
		for(int j=0;j<256;j++){
			tmp+=alpha[j]*alpha[j];
		}
		poss_key[key_len]=tmp;
		key_len+=1;
	}

	//Select the key length 
	double bb=0;
	int tmpidx=0;
	for(int i=0;i<11;i++){
		if(poss_key[i]>bb){
			bb=poss_key[i];
			tmpidx=i;
		}
	}
	bb-=0.005;
	for(int i=0;i<11;i++){
		if(poss_key[i]>bb &&tmpidx%i==0){
			key_len=i;
			break;
		}
	}


	//Find the biggest frequent byte at each index
	int *biggestlist=(int*)malloc(sizeof(int)*key_len);
	for(int i=0;i<key_len;i++){
		int alpha[256]={0,};
		for(int j=i;j<text_len;j+=key_len){
			alpha[buffer[j]]+=1;
		}
		int tmpbig=0;
		int tmpbigidx=0;
		for(int j=0;j<256;j++){
			if(alpha[j]>tmpbig){
				tmpbig=alpha[j];
				tmpbigidx=j;
			}
		}
		biggestlist[i]=tmpbigidx;
	}

	//import the imformation about ascii code Frequency
	double asciiFre[256]={0,};

	int frequency[256]={0,};
	FILE *nf=fopen("./asciiFrequency.txt","r");

	char line[1024];
	char *pLine;
	int ttt=0;
	while(!feof(nf)){
		pLine=fgets(line,1024,nf);
		char* tkn=strtok(pLine,":");
		int a=atoi(tkn);
		frequency[ttt++]=a;
		tkn=strtok(NULL,":");
		double b=atof(tkn);
		asciiFre[a]=b;
	}
	

	

	unsigned char* cand=(unsigned char*)malloc(sizeof(unsigned char)*key_len);


	double **remem=(double**)malloc(sizeof(double*)*key_len);
	for(int i=0;i<key_len;i++){
		cand[i]=0;
		remem[i]=(double*)malloc(sizeof(double)*256);
	}

	//make each byte candidate from most frequent to least frequent
	for(int i=0;i<key_len;i++){
		int check=0;
		for(int j=0;j<256;j++){
			cand[i]=frequency[j]^biggestlist[i];
			double cur_poss=Check(cand[i],i,asciiFre,buffer);
			remem[i][j]=cur_poss;
			if(cur_poss>0.06){
				check=1;
				break;
			}
		}
		if(check==0){
			double frebig=0;
			int freidx=0;
			for(int j=0;j<256;j++){
				if(remem[i][j]>frebig){
					frebig=remem[i][j];
					freidx=j;
				}
			}
			cand[i]=frequency[freidx]^biggestlist[i];
		}
	}
	


	//print to txt file
	FILE *fp2= fopen("./hw1_output.txt","w");
	for(int i=0;i<key_len;i++){
		fprintf(fp2,"0x%02x ",cand[i]);	
	}

	fprintf(fp2,"\n");

	for(int i=0;i<text_len;i++){
		unsigned char tmp=buffer[i]^cand[i%key_len];
		fprintf(fp2,"%c",tmp);
	}



	//file close ans allocation free
	fclose(fp);
	fclose(fp2);
	fclose(nf);
	for(int i=0;i<key_len;i++)
		free(remem[i]);
	free(remem);
	free(cand);
	free(biggestlist);
	free(buffer);
	}

	
	


