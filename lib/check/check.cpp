#include <stdlib.h>
#include<stdio.h>
#include<cmath>
//#include "check.h"

int check_float(char *s1, char *s2)
{
	FILE *fp1;
	FILE *fp2;

	float s_1, s_2;
	int cnt = 0, err_n = 0;

	fp1 = fopen(s1, "r");
	fp2 = fopen(s2, "r");

	if (fp1 == NULL)
	{
		printf("Open file %s fail.\n", s1);
		exit(1);
	}
	if (fp2 == NULL)
	{
		printf("Open file %s fail.\n", s2);
		exit(1);
	}

	while(fscanf(fp1,"%f",&s_1)!=EOF&&fscanf(fp2,"%f",&s_2)!=EOF)
	{
//		printf("x");
		if(fabs(s_1-s_2) > 0.01) 
		{
	//		return -1;
			printf("%d\n", cnt);
			err_n++;
		}
		cnt++;
	}
	return err_n;
}
/*
int main()
{
	char s1[10]="a.txt";
	char s2[10]="b.txt";
	printf("%d",check_float(s1,s2));
	return 0;
}*/
