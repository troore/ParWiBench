
void Print_char_UsrHex(char *String, int length)
{
	int i;
	for(i=0; i<length ;i++)
	{
		printf("%02x  ", String[i]&0x0ff);
	}
	printf("\n\n");
}

void Print_char_StrHex(char *String, int length)
{
	int i;
	char temp;
	for(i=0; i<length ;i++)
	{
		temp = 0x00;
		temp |= (String[i] << 7) & 0x80;
		temp |= (String[i] << 5) & 0x40;
		temp |= (String[i] << 3) & 0x20;
		temp |= (String[i] << 1) & 0x10;
		temp |= (String[i] >> 1) & 0x08;
		temp |= (String[i] >> 3) & 0x04;
		temp |= (String[i] >> 5) & 0x02;
		temp |= (String[i] >> 7) & 0x01;
		printf("%02x  ", temp & 0x0ff);
	}
	printf("\n\n");
}

void Print_complex_Str(Complex *String, int length)
{
	int i;
	for(i=0; i<length; i++)
	{
		printf("%-8d", i);
		Print_complex_6(String[i]);
	}
}


void Read_input(Complex *Input_buffer, char *str)
{
	FILE *fin;
	fin = fopen(str, "rb");
	
	float datar, datai;
	int n;
	n=0;

	while(fscanf(fin, "%f", &datar) == 1)
	{
		fscanf(fin, "%f", &datai);
		Input_buffer[n++] = Complex_init(datar, datai);
	}
	
	fclose(fin);
}






