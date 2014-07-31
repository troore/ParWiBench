
#include "GeneralFunc.h"
extern int RANDOMSEED;

void ReadInputFromFiles(FIFO<int> *pIn,int Sz[2],const char *name)
{
	FILE *file;

	if((file=fopen(name,"r"))==NULL)
		throw std::runtime_error ("file open error\n");

	int *pReadIn=new int[(Sz[0]*Sz[1])];
	for(int i=0;i<(Sz[0]*Sz[1]);i++)
	{
		if(fscanf(file,"%d",(pReadIn+i)) == EOF)
		{
			if(ferror(file) != 0)
				throw std::runtime_error ("file read error\n");
		}

	}
	fclose(file);

	if(Sz[0]==1)
	{bool flag = (*pIn).Write(pReadIn);}
	else
	{
		int **pReadMatrix=new int*[Sz[0]];
		for(int r=0;r<Sz[0];r++){*(pReadMatrix+r)=new int[Sz[1]];}

		for(int r=0;r<Sz[0];r++)
		{
			for(int c=0;c<Sz[1];c++)
			{
				*(*(pReadMatrix+r)+c)=*(pReadIn+r*Sz[1]+c);
			}
		}
		bool flag = (*pIn).Write(pReadMatrix);
		for(int r=0;r<Sz[0];r++){delete[] *(pReadMatrix+r);}
		delete[] pReadMatrix;
	}

	delete[] pReadIn;
}


void ReadInputFromFiles(int *pIn, int Sz, const char *name)
{
	FILE *file;

	if((file=fopen(name,"r"))==NULL)
		throw std::runtime_error ("file open error\n");

	for(int i = 0;i < Sz; i++)
	{
		if(fscanf(file,"%d",(pIn+i)) == EOF)
		{
			if(ferror(file) != 0)
				throw std::runtime_error ("file read error\n");
		}

	}
	
	fclose(file);
}


void ReadInputFromFiles(FIFO<float> *pIn,int Sz[2],const char *name)
{
	FILE *file;

	if((file=fopen(name,"r"))==NULL)
		throw std::runtime_error ("file open error\n");

	float *pReadIn=new float[(Sz[0]*Sz[1])];
	for(int i=0;i<(Sz[0]*Sz[1]);i++)
	{
		if(fscanf(file,"%f",(pReadIn+i)) == EOF)
		{
			if(ferror(file) != 0)
				throw std::runtime_error ("file read error\n");
		}

	}
	fclose(file);

	if(Sz[0]==1)
	{bool flag = (*pIn).Write(pReadIn);}
	else
	{
		float **pReadMatrix=new float*[Sz[0]];
		for(int r=0;r<Sz[0];r++){*(pReadMatrix+r)=new float[Sz[1]];}

		for(int r=0;r<Sz[0];r++)
		{
			for(int c=0;c<Sz[1];c++)
			{
				*(*(pReadMatrix+r)+c)=*(pReadIn+r*Sz[1]+c);
			}
		}
		bool flag = (*pIn).Write(pReadMatrix);
		for(int r=0;r<Sz[0];r++){delete[] *(pReadMatrix+r);}
		delete[] pReadMatrix;
	}

	delete[] pReadIn;
}


void ReadInputFromFiles(float *pIn,int Sz, const char *name)
{
	FILE *file;

	if((file=fopen(name,"r"))==NULL)
		throw std::runtime_error ("file open error\n");

//	float *pReadIn=new float[(Sz[0]*Sz[1])];
	for(int i=0; i < Sz; i++)
	{
		if(fscanf(file, "%f", (pIn+i)) == EOF)
		{
			if(ferror(file) != 0)
				throw std::runtime_error ("file read error\n");
		}

	}
	fclose(file);

	/*
	if(Sz[0]==1)
	{bool flag = (*pIn).Write(pReadIn);}
	else
	{
		float **pReadMatrix=new float*[Sz[0]];
		for(int r=0;r<Sz[0];r++){*(pReadMatrix+r)=new float[Sz[1]];}

		for(int r=0;r<Sz[0];r++)
		{
			for(int c=0;c<Sz[1];c++)
			{
				*(*(pReadMatrix+r)+c)=*(pReadIn+r*Sz[1]+c);
			}
		}
		bool flag = (*pIn).Write(pReadMatrix);
		for(int r=0;r<Sz[0];r++){delete[] *(pReadMatrix+r);}
		delete[] pReadMatrix;
	}

	delete[] pReadIn;
	*/
}


void ReadInputFromFiles(FIFO<complex<float> > *pIn,int Sz[2],const char *nameReal,const char *nameImag)
{
	FILE *file;

	if((file=fopen(nameReal,"r"))==NULL)
		throw std::runtime_error ("file open error\n");

	float *pReadInReal=new float[(Sz[0]*Sz[1])];
	for(int i=0;i<(Sz[0]*Sz[1]);i++)
	{
		if(fscanf(file,"%f",(pReadInReal+i)) == EOF)
		{
			if(ferror(file) != 0)
				throw std::runtime_error ("file read error\n");
		}

	}
	fclose(file);

	if((file=fopen(nameImag,"r"))==NULL)
		throw std::runtime_error ("file open error\n");

	float *pReadInImag=new float[(Sz[0]*Sz[1])];
	for(int i=0;i<(Sz[0]*Sz[1]);i++)
	{
		if(fscanf(file,"%f",(pReadInImag+i)) == EOF)
		{
			if(ferror(file) != 0)
				throw std::runtime_error ("file read error\n");
		}

	}
	fclose(file);

	if(Sz[0]==1)
	{
		complex<float> *pReadIn=new complex<float>[(Sz[0]*Sz[1])];
		for(int i=0;i<(Sz[0]*Sz[1]);i++)
		{*(pReadIn+i)=complex<float>((*(pReadInReal+i)),(*(pReadInImag+i)));}
		bool flag = (*pIn).Write(pReadIn);
		delete[] pReadIn;
	}
	else
	{
		complex<float> **pReadMatrix=new complex<float>*[Sz[0]];
		for(int r=0;r<Sz[0];r++){*(pReadMatrix+r)=new complex<float>[Sz[1]];}

		for(int r=0;r<Sz[0];r++)
		{
			for(int c=0;c<Sz[1];c++)
			{
				*(*(pReadMatrix+r)+c)=complex<float>((*(pReadInReal+r*Sz[1]+c)),(*(pReadInImag+r*Sz[1]+c)));
			}
		}
		bool flag = (*pIn).Write(pReadMatrix);

		for(int r=0;r<Sz[0];r++){delete[] *(pReadMatrix+r);}
		delete[] pReadMatrix;
	}

	delete[] pReadInReal;
	delete[] pReadInImag;
}

void ReadInputFromFiles(complex<float> *pIn,int Sz,const char *nameReal, const char *nameImag)
{
	FILE *file;

	if((file=fopen(nameReal,"r"))==NULL)
		throw std::runtime_error ("file open error\n");

	float *pReadInReal=new float[Sz];
	
	for(int i=0;i<Sz;i++)
	{
		if(fscanf(file,"%f",(pReadInReal+i)) == EOF)
		{
			if(ferror(file) != 0)
				throw std::runtime_error ("file read error\n");
		}

	}
	fclose(file);

	if((file=fopen(nameImag,"r"))==NULL)
		throw std::runtime_error ("file open error\n");

	float *pReadInImag=new float[Sz];
	for(int i=0;i<Sz;i++)
	{
		if(fscanf(file,"%f",(pReadInImag+i)) == EOF)
		{
			if(ferror(file) != 0)
				throw std::runtime_error ("file read error\n");
		}

	}
	fclose(file);

	/*
	if(Sz[0]==1)
	{
		complex<float> *pReadIn=new complex<float>[(Sz[0]*Sz[1])];
		for(int i=0;i<(Sz[0]*Sz[1]);i++)
		{*(pReadIn+i)=complex<float>((*(pReadInReal+i)),(*(pReadInImag+i)));}
		bool flag = (*pIn).Write(pReadIn);
		delete[] pReadIn;
	}
	else
	{
		complex<float> **pReadMatrix=new complex<float>*[Sz[0]];
		for(int r=0;r<Sz[0];r++){*(pReadMatrix+r)=new complex<float>[Sz[1]];}

		for(int r=0;r<Sz[0];r++)
		{
			for(int c=0;c<Sz[1];c++)
			{
				*(*(pReadMatrix+r)+c)=complex<float>((*(pReadInReal+r*Sz[1]+c)),(*(pReadInImag+r*Sz[1]+c)));
			}
		}
		bool flag = (*pIn).Write(pReadMatrix);

		for(int r=0;r<Sz[0];r++){delete[] *(pReadMatrix+r);}
		delete[] pReadMatrix;
	}
	*/
	for(int i=0;i<Sz;i++)
	{
		pIn[i]=complex<float>(pReadInReal[i], pReadInImag[i]);
	}

	delete[] pReadInReal;
	delete[] pReadInImag;
}



//////////////////// Random Generate Input and Write to Files ////////////////////////

void GeneRandomInput(FIFO<int> *pIn,int Sz[2],const char *name)
{
	FILE *fptr=NULL;
	int sd = -111;
	if(Sz[0]==1)
	{
		int *pRandom=new int[Sz[1]];

		fptr = fopen(name,"w+");
		for(int i=0;i<Sz[1];i++)
		{
			sd-=i;
			float v=(float)gauss1(&sd);
			if(v>0){*(pRandom+i)=1;}
			else{*(pRandom+i)=0;}
			fprintf(fptr,"%d\t",*(pRandom+i));
		}
		fclose(fptr);
		bool flag = (*pIn).Write(pRandom);

		delete[] pRandom;
	}
	else
	{
		int **pRandom = new int*[Sz[0]];
		for(int i=0;i<Sz[0];i++){*(pRandom+i)=new int[Sz[1]];}

		fptr = fopen(name,"w+");
		for(int i=0;i<Sz[0];i++)
		{
			for(int j=0;j<Sz[1];j++)
			{
				sd-=(i+j);
				float v=(float)gauss1(&sd);
				if(v>0){*(*(pRandom+i)+j)=1;}
				else{*(*(pRandom+i)+j)=0;}
				fprintf(fptr,"%d\t",*(*(pRandom+i)+j));
			}
			fprintf(fptr,"\n");
		}
		fclose(fptr);
		bool flag = (*pIn).Write(pRandom);

		for(int i=0;i<Sz[0];i++){delete[] *(pRandom+i);}
		delete[] pRandom;
	}
}

void GeneRandomInput(int *pIn,int Sz,const char *name)
{
	FILE *fptr=NULL;
	int sd = -111;

	int *pRandom=new int[Sz];

	fptr = fopen(name,"w+");
	for(int i = 0; i < Sz; i++)
	{
		sd-=i;
		float v=(float)gauss1(&sd);
		if(v>0){*(pRandom+i)=1;}
		else{*(pRandom+i)=0;}
		fprintf(fptr,"%d\n",*(pRandom+i));
	}
	fclose(fptr);

	for (int i = 0; i < Sz; i++)
	{
		pIn[i] = pRandom[i];
	}

	delete[] pRandom;
}



void GeneRandomInput(FIFO<float> *pIn,int Sz[2],const char *name)
{
    FILE *fptr=NULL;
    int sd = -111;
    if(Sz[0]==1)
    {
		float *pRandom=new float[Sz[1]];
 
		fptr = fopen(name,"w+");
		for(int i=0;i<Sz[1];i++)
		{
			sd-=i;
			*(pRandom+i)=(float)gauss1(&sd);
			fprintf(fptr,"%f\t",*(pRandom+i));
		}
		fclose(fptr);
		bool flag = (*pIn).Write(pRandom);
 
		delete[] pRandom;
    }
    else
    {
		float **pRandom = new float*[Sz[0]];
		for(int i=0;i<Sz[0];i++){*(pRandom+i)=new float[Sz[1]];}
 
		fptr = fopen(name,"w+");
		for(int i=0;i<Sz[0];i++)
		{
			for(int j=0;j<Sz[1];j++)
			{
				sd-=(i+j);
				*(*(pRandom+i)+j)=(float)gauss1(&sd);
				fprintf(fptr,"%f\t",*(*(pRandom+i)+j));
			}
			fprintf(fptr,"\n");
		}
		fclose(fptr);
		bool flag = (*pIn).Write(pRandom);
 
		for(int i=0;i<Sz[0];i++){delete[] *(pRandom+i);}
		delete[] pRandom;
    }
}

void GeneRandomInput(complex<float> *pIn, int Sz, const char *nameReal, const char *nameImag)
{
    FILE *fptr_real=NULL;
    FILE *fptr_imag=NULL;
    int sd = -111;

	complex<float> *pRandom=new complex<float>[Sz];

	fptr_real = fopen(nameReal,"w+");
	fptr_imag = fopen(nameImag,"w+");
	for(int i=0;i<Sz;i++)
	{
		sd-=i;
		float vr = (float)gauss1(&sd);
		sd-=222;
		float vi = (float)gauss1(&sd);
		*(pRandom+i)=complex<float>(vr,vi);
		fprintf(fptr_real,"%f\n",(*(pRandom+i)).real());
		fprintf(fptr_imag,"%f\n",(*(pRandom+i)).imag());
	}
	fclose(fptr_real);
	fclose(fptr_imag);

	delete[] pRandom;
}



void GeneRandomInput(FIFO<complex<float> > *pIn,int Sz[2],const char *nameReal,const char *nameImag)
{
    FILE *fptr_real=NULL;
    FILE *fptr_imag=NULL;
    int sd = -111;
    if(Sz[0]==1)
    {
		complex<float> *pRandom=new complex<float>[Sz[1]];

		fptr_real = fopen(nameReal,"w+");
		fptr_imag = fopen(nameImag,"w+");
		for(int i=0;i<Sz[1];i++)
		{
			sd-=i;
			float vr = (float)gauss1(&sd);
			sd-=222;
			float vi = (float)gauss1(&sd);
			*(pRandom+i)=complex<float>(vr,vi);
			fprintf(fptr_real,"%f\t",(*(pRandom+i)).real());
			fprintf(fptr_imag,"%f\t",(*(pRandom+i)).imag());
		}
		fclose(fptr_real);
		fclose(fptr_imag);
		bool flag = (*pIn).Write(pRandom);

		delete[] pRandom;
    }
    else
    {
		complex<float> **pRandom = new complex<float>*[Sz[0]];
		for(int i=0;i<Sz[0];i++){*(pRandom+i)=new complex<float>[Sz[1]];}

		fptr_real = fopen(nameReal,"w+");
		fptr_imag = fopen(nameImag,"w+");
		for(int i=0;i<Sz[0];i++)
		{
			for(int j=0;j<Sz[1];j++)
			{
				sd-=(i+j);
				float vr = (float)gauss1(&sd);
				sd-=333;
				float vi = (float)gauss1(&sd);
				*(*(pRandom+i)+j)=complex<float>(vr,vi);
				fprintf(fptr_real,"%f\t",(*(*(pRandom+i)+j)).real());
				fprintf(fptr_imag,"%f\t",(*(*(pRandom+i)+j)).imag());
			}
			fprintf(fptr_real,"\n");
			fprintf(fptr_imag,"\n");
		}
		fclose(fptr_real);
		fclose(fptr_imag);
		bool flag = (*pIn).Write(pRandom);
      
		for(int i=0;i<Sz[0];i++){delete[] *(pRandom+i);}
		delete[] pRandom;
    }
}


////////////////////END Random Generate Input and Write to Files ////////////////////////

///////////////////// Random Generate Input but NOT write to Files ///////////////////////
void GeneRandomInput(FIFO<int> *pIn,int Sz[2])
{
	int sd = -111;
	if(Sz[0]==1)
	{
		int *pRandom=new int[Sz[1]];

		for(int i=0;i<Sz[1];i++)
		{
			sd-=i;
			float v=(float)gauss1(&sd);
			if(v>0){*(pRandom+i)=1;}
			else{*(pRandom+i)=0;}
		}
		bool flag = (*pIn).Write(pRandom);

		delete[] pRandom;
	}
	else
	{
		int **pRandom = new int*[Sz[0]];
		for(int i=0;i<Sz[0];i++){*(pRandom+i)=new int[Sz[1]];}

		for(int i=0;i<Sz[0];i++)
		{
			for(int j=0;j<Sz[1];j++)
			{
				sd-=(i+j);
				float v=(float)gauss1(&sd);
				if(v>0){*(*(pRandom+i)+j)=1;}
				else{*(*(pRandom+i)+j)=0;}
			}
		}
     
		bool flag = (*pIn).Write(pRandom);

		for(int i=0;i<Sz[0];i++){delete[] *(pRandom+i);}
		delete[] pRandom;
	}
}

void GeneRandomInput(FIFO<float> *pIn,int Sz[2])
{
    int sd = -111;
    if(Sz[0]==1)
    {
		float *pRandom=new float[Sz[1]];
		for(int i=0;i<Sz[1];i++)
		{
			sd-=i;
			*(pRandom+i)=(float)gauss1(&sd);
		}
		bool flag = (*pIn).Write(pRandom);

		delete[] pRandom;
    }
    else
    {
		float **pRandom = new float*[Sz[0]];
		for(int i=0;i<Sz[0];i++){*(pRandom+i)=new float[Sz[1]];}

		for(int i=0;i<Sz[0];i++)
		{
			for(int j=0;j<Sz[1];j++)
			{
				sd-=(i+j);
				*(*(pRandom+i)+j)=(float)gauss1(&sd);
			}
		}
		bool flag = (*pIn).Write(pRandom);

		for(int i=0;i<Sz[0];i++){delete[] *(pRandom+i);}
		delete[] pRandom;
    }
}


void GeneRandomInput(FIFO<complex<float> > *pIn,int Sz[2])
{
    int sd = -111;
    if(Sz[0]==1)
    {
		complex<float> *pRandom=new complex<float>[Sz[1]];

		for(int i=0;i<Sz[1];i++)
		{
			sd-=i;
			float vr = (float)gauss1(&sd);
			sd-=222;
			float vi = (float)gauss1(&sd);
			*(pRandom+i)=complex<float>(vr,vi);
		}
		bool flag = (*pIn).Write(pRandom);

		delete[] pRandom;
    }
    else
    {
		complex<float> **pRandom = new complex<float>*[Sz[0]];
		for(int i=0;i<Sz[0];i++){*(pRandom+i)=new complex<float>[Sz[1]];}

		for(int i=0;i<Sz[0];i++)
		{
			for(int j=0;j<Sz[1];j++)
			{
				sd-=(i+j);
				float vr = (float)gauss1(&sd);
				sd-=333;
				float vi = (float)gauss1(&sd);
				*(*(pRandom+i)+j)=complex<float>(vr,vi);
			}
		}
		bool flag = (*pIn).Write(pRandom);

		for(int i=0;i<Sz[0];i++){delete[] *(pRandom+i);}
		delete[] pRandom;
    }

}
/////////////////////END Random Generate Input but NOT write to Files ///////////////////////


////////////////////// Read Output and Write Output to Files ////////////////////////

void WriteOutputToFiles(FIFO<int> *pOut,int Sz[2],const char *name)
{
	FILE *fptr=NULL;
	if(Sz[0]==1)
	{
		int *pW=new int[Sz[1]];
		bool flag = (*pOut).Read(pW);
		fptr = fopen(name,"w+");
		for(int i=0;i<Sz[1];i++)
		{
			fprintf(fptr,"%d\t",*(pW+i));
		}
		fclose(fptr);
		delete[] pW;
	}
	else
	{
		int **pW=new int*[Sz[0]];
		for(int i=0;i<Sz[0];i++){*(pW+i)=new int[Sz[1]];}

		bool flag = (*pOut).Read(pW);
		fptr = fopen(name,"w+");
		for(int i=0;i<Sz[0];i++)
		{
			for(int j=0;j<Sz[1];j++)
			{
				fprintf(fptr,"%d\t",*(*(pW+i)+j));
			}
			fprintf(fptr,"\n");
		}
		fclose(fptr);

		for(int i=0;i<Sz[0];i++){delete[] *(pW+i);}
		delete[] pW;
	}
}


void WriteOutputToFiles(int *pOut, int Sz, const char *name)
{
	FILE *fptr=NULL;

	fptr = fopen(name,"w+");
	for(int i=0;i<Sz;i++)
	{
		fprintf(fptr,"%d\t",*(pOut+i));
	}
	fclose(fptr);
}

void WriteOutputToFiles(FIFO<float> *pOut,int Sz[2],const char *name)
{
	FILE *fptr=NULL;
	if(Sz[0]==1)
	{
		float *pW=new float[Sz[1]];
		bool flag = (*pOut).Read(pW);
		fptr = fopen(name,"w+");
		for(int i=0;i<Sz[1];i++)
		{
			fprintf(fptr,"%f\t",*(pW+i));
		}
		fclose(fptr);
		delete[] pW;
	}
	else
	{
		float **pW=new float*[Sz[0]];
		for(int i=0;i<Sz[0];i++){*(pW+i)=new float[Sz[1]];}

		bool flag = (*pOut).Read(pW);
		fptr = fopen(name,"w+");
		for(int i=0;i<Sz[0];i++)
		{
			for(int j=0;j<Sz[1];j++)
			{
				fprintf(fptr,"%f\t",*(*(pW+i)+j));
			}
			fprintf(fptr,"\n");
		}
		fclose(fptr);

		for(int i=0;i<Sz[0];i++){delete[] *(pW+i);}
		delete[] pW;
	}
}


void WriteOutputToFiles(float *pOut, int Sz, const char *name)
{
	FILE *fptr=NULL;
	
	fptr = fopen(name,"w+");
	for(int i=0;i<Sz;i++)
	{
		fprintf(fptr,"%f\n",*(pOut+i));
	}
	fclose(fptr);
}

void WriteOutputToFiles(FIFO<complex<float> > *pOut,int Sz[2],const char *nameReal,const char *nameImag)
{
	FILE *fptr_real=NULL;
	FILE *fptr_imag=NULL;
	if(Sz[0]==1)
	{
		complex<float> *pW=new complex<float>[Sz[1]];
		bool flag = (*pOut).Read(pW);
		fptr_real = fopen(nameReal,"w+");
		fptr_imag = fopen(nameImag,"w+");
		for(int i=0;i<Sz[1];i++)
		{
			fprintf(fptr_real,"%f\t",(*(pW+i)).real());
			fprintf(fptr_imag,"%f\t",(*(pW+i)).imag());
		}
		fclose(fptr_real);
		fclose(fptr_imag);
		delete[] pW;
	}
	else
	{
		complex<float> **pW=new complex<float>*[Sz[0]];
		for(int i=0;i<Sz[0];i++){*(pW+i)=new complex<float>[Sz[1]];}

		bool flag = (*pOut).Read(pW);
		fptr_real = fopen(nameReal,"w+");
		fptr_imag = fopen(nameImag,"w+");
		for(int i=0;i<Sz[0];i++)
		{
			for(int j=0;j<Sz[1];j++)
			{
				fprintf(fptr_real,"%f\t",(*(*(pW+i)+j)).real());
				fprintf(fptr_imag,"%f\t",(*(*(pW+i)+j)).imag());
			}
			fprintf(fptr_real,"\n");
			fprintf(fptr_imag,"\n");
		}
		fclose(fptr_real);
		fclose(fptr_imag);

		for(int i=0;i<Sz[0];i++){delete[] *(pW+i);}
		delete[] pW;
	}
}

void WriteOutputToFiles(complex<float> *pOut, int Sz, const char *nameReal, const char *nameImag)
{
	FILE *fptr_real=NULL;
	FILE *fptr_imag=NULL;
	
	fptr_real = fopen(nameReal, "w+");
	fptr_imag = fopen(nameImag, "w+");
	
	for(int i=0;i<Sz;i++)
	{
		fprintf(fptr_real,"%f\t",(pOut[i]).real());
		fprintf(fptr_imag,"%f\t",(pOut[i]).imag());
	}
	
	fclose(fptr_real);
	fclose(fptr_imag);
}

//////////////////////END Read Output and Write Output to Files ////////////////////////

///////////////////////// Read Output /////////////////////////////
void ReadOutput(FIFO<int> *pOut,int Sz[])
{
	if(Sz[0]==1)
	{
		int *pW=new int[Sz[1]];
		bool flag = (*pOut).Read(pW);
		delete[] pW;
	}
	else
	{
		int **pW=new int*[Sz[0]];
		for(int i=0;i<Sz[0];i++){*(pW+i)=new int[Sz[1]];}

		bool flag = (*pOut).Read(pW);
    
		for(int i=0;i<Sz[0];i++){delete[] *(pW+i);}
		delete[] pW;
	}
}


void ReadOutput(FIFO<float> *pOut,int Sz[])
{
	if(Sz[0]==1)
	{
		float *pW=new float[Sz[1]];
		bool flag = (*pOut).Read(pW);
		delete[] pW;
	}
	else
	{
		float **pW=new float*[Sz[0]];
		for(int i=0;i<Sz[0];i++){*(pW+i)=new float[Sz[1]];}

		bool flag = (*pOut).Read(pW);
    
		for(int i=0;i<Sz[0];i++){delete[] *(pW+i);}
		delete[] pW;
	}
}


void ReadOutput(FIFO<complex<float> > *pOut,int Sz[])
{
	if(Sz[0]==1)
	{
		complex<float> *pW=new complex<float>[Sz[1]];
		bool flag = (*pOut).Read(pW);
		delete[] pW;
	}
	else
	{
		complex<float> **pW=new complex<float>*[Sz[0]];
		for(int i=0;i<Sz[0];i++){*(pW+i)=new complex<float>[Sz[1]];}

		bool flag = (*pOut).Read(pW);

		for(int i=0;i<Sz[0];i++){delete[] *(pW+i);}
		delete[] pW;
	}
}
//////////////////////////END Read Output//////////////////////////

void GenerateLTEChainInput(FIFO<int> *pDataSource, int DataK, int *pTxDS, int RANDOMSEED)
{
	int sd = -RANDOMSEED;

	for(int i=0;i<DataK;i++)
	{
		sd-=i;
		float v=(float)gauss1(&sd);
		if(v>0){*(pTxDS+i)=1;}
		else{*(pTxDS+i)=0;}
	}

	(*pDataSource).Write(pTxDS);
}

void GenerateLTEChainInput(int *pDataSource, int DataK, int *pTxDS, int RANDOMSEED)
{
	int sd = -RANDOMSEED;
	float v;

	for(int i = 0; i < DataK; i++)
	{
		sd -= i;
		v = (float)gauss1(&sd);
		
		pTxDS[i] = (v > 0) ? 1 : 0;
	}

//	(*pDataSource).Write(pTxDS);
	for (int i = 0; i < DataK; i++)
	{
		pDataSource[i] = pTxDS[i];
	}
}

void ReadLTEChainOutput(FIFO<int> *pFileSink, int *pRxFS)
{
	(*pFileSink).Read(pRxFS);
}

void ReadLTEChainOutput(int *pFileSink, int *pRxFS, int DataK)
{
//	(*pFileSink).Read(pRxFS);
	for (int i = 0; i < DataK; i++)
	{
		pRxFS[i] = pFileSink[i];
	}
}


