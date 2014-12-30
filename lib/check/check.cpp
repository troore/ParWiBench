#include "check.h"

int check_float(char *s1,char *s2)
{
	FILE *fp1 = fopen(s1,"r");
	FILE *fp2 = fopen(s2,"r");
	float s_1,s_2;
	while(fscanf(fp1,"%f",&s_1)!=EOF&&fscanf(fp2,"%f",&s_2)!=EOF)
	{
//		printf("x");
		if(fabs(s_1-s_2)> 0.00001) return -1;
	}
	return 0;
}
/*
int main()
{
	char s1[10]="a.txt";
	char s2[10]="b.txt";
	printf("%d",check_float(s1,s2));
	return 0;
}*/
