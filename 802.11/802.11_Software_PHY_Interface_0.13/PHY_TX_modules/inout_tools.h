
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


void Read_input(char *Input_buffer, char *str)
{
	FILE *fin;
	fin = fopen(str, "rb");
	
	int state, count, i;
	char x, temp;
	
	count = 7;
	state = 1;
	temp = 0x00;
	i = 0;
	while(fscanf(fin, "%c", &x) == 1)
	{
		if(x == '#')
		{
			state = 0;
		}
		if(state == 0 && x == 0x0a)
		{
			state = 1;
		}
		
		if(state == 1 && (x == '1' || x == '0'))
		{
			temp = temp | ((x - '0') << count);
			
			if(count == 0)
			{
				count = 7;
				Input_buffer[i++] = temp;
				temp = 0x00;
			}
			else
			{
				count = count - 1;
			}
		}
	}
	
	fclose(fin);
}






