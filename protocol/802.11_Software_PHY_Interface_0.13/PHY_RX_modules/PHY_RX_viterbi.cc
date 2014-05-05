
/*
char source[1000];
char bit_source[8000];
char BCC_source[16000];
char punc[16000];
int len;
int punc_len;
char bcc_route[8000];

char pre[128][2];
char next[128][2];
char recover[16000];
char route[8000][128];
char match[128];
int dis[128];
int now[128];
char result_state[8000];
char result_match[16000];
char result_bit[8000];
char result_byte[1000];
*/


char source[2000];
char bit_source[16000];
char BCC_source[32000];
char punc[32000];
int len;
int punc_len;
char bcc_route[16000];

char pre[128][2];
char next[128][2];
char recover[32000];
char route[16000][256];
char match[256];
int dis[256];
int now[256];
char result_state[16000];
char result_match[32000];
char result_bit[16000];
char result_byte[2000];



void input()
{
	cout << "input: " << endl;
	cin.getline(source, 1000);
}

void inputToBit()
{
	int i,j;
	len = strlen(source);
	int k = 0;
	for(i = 0; i < len; i++)
	{
		for(j = 7; j >= 0; j--)
		{
			char tmp = (source[i] & (1 << j)) >> j;
			bit_source[k++] = (tmp);
		}
	}
	if(k != len * 8)
		cout << "sourceToBit Error!" << endl;
	bit_source[k] = '\0';
}

void BCC_single(char state, char op[2])
{
	if(state & 0x80)
	{
		cout << "ERROR!" << endl;
		return;
	}
	op[0] = (state & 0x1) ^ ((state & 0x4) >> 2) ^ ((state & 0x8) >> 3) ^ ((state & 0x20) >> 5) ^ ((state & 0x40) >> 6);
	op[1] = (state & 0x1) ^ ((state & 0x2) >> 1) ^ ((state & 0x4) >> 2) ^ ((state & 0x8) >> 3) ^ ((state & 0x40) >> 6);
}

void BCC(int mode)
{
	int i,j;
	if(mode != 75 && mode != 66 && mode != 50)
	{
		cout << "BCC mode error!" << endl;
		//exit(1);
	}

	int bit_len = len * 8;
	int bcc_i = 0;
	int punc_i = 0;
	int k = 0;
	char state = 0;
	char output[2];
	char store[6];
	for(i = 0; i < bit_len; i++)
	{
		char tmp = bit_source[i];
		state <<= 1;
		state |= tmp;
		state &= 0x7f;
		bcc_route[i] = state;
		BCC_single(state, output);
		store[k * 2] = output[0];
		store[k * 2 + 1] = output[1];
		k++;
		if(mode == 75 && k == 3)
		{
			for(j = 0; j < 6; j++)
			{
				BCC_source[bcc_i++] = store[j];
				if(j == 3 || j == 4)
					continue;
				else
					punc[punc_i++] = store[j];
			}
			k = 0;
		}
		else if(mode == 66 && k == 2)
		{
			for(j = 0; j < 4; j++)
			{
				BCC_source[bcc_i++] = store[j];
				if(j == 3)
					continue;
				else
					punc[punc_i++] = store[j];
			}
			k = 0;
		}
		else if(k == 3)
		{
			for(j = 0; j < 6; j++)
			{
				BCC_source[bcc_i++] = store[j];
				punc[punc_i++] = store[j];
			}
			k = 0;
		}
	}
	if(k != 0)
	{
		k *= 2;
		for(j = 0; j < k; j++)
		{
			BCC_source[bcc_i++] = store[j];
			punc[punc_i++] = store[j];
		}
	}
	punc_len = punc_i;
}

void errorBySend()
{
	int i;
	int num;
	int er[512];
	cout << "error number is: ";
	cin >> num;
	cout << "\ninput what bits to be error: " << endl;
	for(i = 0; i < num; i++)
	{
		cin >> er[i];
	}
	cout << "ok" << endl;
	for(i = 0; i < num; i++)
	{
		int tmp = er[i];
		punc[tmp] ^= 0x1;
	}
}

void createMatch()
{
	int i;
	for(i = 0; i < 128; i++)
	{
		char op[2];
		char tmp = i;
		BCC_single(tmp, op);
		match[i] = (op[0] << 1) | op[1];
	}
}

void createPrevious()
{
	int i;
	for(i = 0; i < 128; i++)
	{
		char tmp = i;
		char tmp1 = (tmp >> 1);
		char tmp2 = tmp1 | 0x40;
		pre[i][0] = tmp1;
		pre[i][1] = tmp2;
	}
}

void punc_Recover(int mode)
{
	int i;
	if(mode == 50)
	{
		int re_i = 0;
		for(i = 0; i < punc_len; i++)
			recover[re_i++] = punc[i];
		return;
	}
	if(mode != 75 && mode != 66)
	{
		cout << "recover mode error!" << endl;
		//exit(1);
	}
	int k = 0, re_i = 0;
	for(i = 0; i < punc_len; i++)
	{
		recover[re_i++] = punc[i];
		k++;
		if(mode == 75 && k == 3)
		{
			recover[re_i++] = 0;
			recover[re_i++] = 0;
		}
		else if(mode == 75 && k == 4)
			k = 0;
		else if(mode == 66 && k == 3)
		{
			recover[re_i++] = 0;
			k = 0;
		}
	}
}

int countHam(char a, char b, char mode)
{
	int count = 0;
	if(mode & 0x1)
	{
		if((a & 0x1) ^ (b & 0x1))
			count++;
	}
	if(mode & 0x2)
	{
		if((a & 0x2) ^ (b & 0x2))
			count++;
	}
	return count;
}

void viterbi()
{
	int i,j;
	int k = 0;
	int min_dis = 99999;
	int min_i = -1;
	int b_len = len * 8;
        memset(dis, 0, sizeof(dis));
	for(i = 0; i < b_len; i++)
	{
		for(j = 0; j < 128; j++)
		{
			char state1 = pre[j][0];
			char state2 = pre[j][1];
			char tmp = (recover[i * 2] << 1) | recover[i * 2 + 1];
			char tmp1 = match[state1];
			char tmp2 = match[state2];
			int d1 = dis[state1] + countHam(tmp1, tmp, 3);
			int d2 = dis[state2] + countHam(tmp2, tmp, 3);
			if(d1 < d2)
			{
				now[j] = d1;
				route[i][j] = state1;
			}
			else
			{
				now[j] = d2;
				route[i][j] = state2;
			}
			k++;
		}
		for(j = 0; j < 128; j++)
			dis[j] = now[j];
	}
	for(j = 0; j < 128; j++)
	{
		if(dis[j] < min_dis)
		{
			min_dis = dis[j];
			min_i = j;
		}
	}

	char tmp = min_i;
	for(i = b_len - 1; i >= 0; i--)
	{
		tmp = route[i][tmp];
		result_state[i] = tmp;
		result_match[i * 2] = match[tmp] >> 1;
		result_match[i * 2 + 1] = match[tmp] & 0x1;
		result_bit[i] = tmp & 0x1;
	}
	for(i = 0; i < len; i++)
	{
		result_byte[i] = (result_bit[i * 8] << 7) |
						 (result_bit[i * 8 + 1] << 6) |
						 (result_bit[i * 8 + 2] << 5) |
						 (result_bit[i * 8 + 3] << 4) |
						 (result_bit[i * 8 + 4] << 3) |
						 (result_bit[i * 8 + 5] << 2) |
						 (result_bit[i * 8 + 6] << 1) |
						 (result_bit[i * 8 + 7]);
	}
}

void viterbi_ignore(int mode)
{

	int i,j;
	if(mode != 75 && mode != 66)
	{
		cout << "Viterbi mode error!" << endl;
		//exit(1);
	}
	int k = 0;
	int min_dis = 99999;
	int min_i = -1;
	int b_len = len * 8;
        memset(dis, 0, sizeof(dis));
	for(i = 0; i < b_len; i++)
	{
		bool f = false;
		k++;
		for(j = 0; j < 128; j++)
		{
			char state1 = pre[j][0];
			char state2 = pre[j][1];
			char tmp = (recover[i * 2] << 1) | recover[i * 2 + 1];
			char tmp1 = match[state1];
			char tmp2 = match[state2];
			int d1, d2;
			if(k == 1)
			{
				d1 = dis[state1] + countHam(tmp1, tmp, 3);
				d2 = dis[state2] + countHam(tmp2, tmp, 3);
			}
			else if(k == 2)
			{
				d1 = dis[state1] + countHam(tmp1, tmp, 2);
				d2 = dis[state2] + countHam(tmp2, tmp, 2);
				if(mode == 66)
					f = true;
			}
			else if(k == 3)
			{
				d1 = dis[state1] + countHam(tmp1, tmp, 1);
				d2 = dis[state2] + countHam(tmp2, tmp, 1);
				f = true;
			}
			
			if(d1 < d2)
			{
				now[j] = d1;
				route[i][j] = state1;
			}
			else
			{
				now[j] = d2;
				route[i][j] = state2;
			}
		}
		if(f)
			k = 0;
		for(j = 0; j < 128; j++)
			dis[j] = now[j];
	}
	for(j = 0; j < 128; j++)
	{
		if(dis[j] < min_dis)
		{
			min_dis = dis[j];
			min_i = j;
		}
	}
	char tmp = min_i;
	for(i = b_len - 1; i >= 0; i--)
	{
		tmp = route[i][tmp];
		result_state[i] = tmp;
		result_match[i * 2] = match[tmp] >> 1;
		result_match[i * 2 + 1] = match[tmp] & 0x1;
		result_bit[i] = tmp & 0x1;
	}
	for(i = 0; i < len; i++)
	{
		result_byte[i] = (result_bit[i * 8] << 7) |
						 (result_bit[i * 8 + 1] << 6) |
						 (result_bit[i * 8 + 2] << 5) |
						 (result_bit[i * 8 + 3] << 4) |
						 (result_bit[i * 8 + 4] << 3) |
						 (result_bit[i * 8 + 5] << 2) |
						 (result_bit[i * 8 + 6] << 1) |
						 (result_bit[i * 8 + 7]);
	}
}

void compare()
{
	int i;
	int count = 0;
	int c_len = len * 16;
	for(i = 0; i < c_len; i++)
	{
		if(BCC_source[i] & 0xfe || result_match[i] & 0xfe)
			cout << "SSS" << endl;
		if(BCC_source[i] ^ result_match[i])
			count++;
	}
	cout << "BCC_source ^ result_match: error_number = " << count << ", per = " << (double)count / (double)c_len << endl;

	count = 0;
	int b_len = len * 8;
	for(i = 0; i < b_len; i++)
	{
		if(bit_source[i] & 0xfe || result_bit[i] & 0xfe)
			cout << "TTT" << endl;
		if(bit_source[i] ^ result_bit[i])
			count++;
	}
	cout << "bit_source ^ result_bit: error_number = " << count << ", per = " << (double)count / (double)b_len << endl;
}

void printBitSource()
{
	int i;
	int b_len = len * 8;
	cout << "bit_source = " << endl;
	for(i = 0; i < b_len; i++)
	{
		bit_source[i] += '0';
		cout << bit_source[i];
		bit_source[i] -= '0';
	}
	cout << endl;
}

void printBccSource()
{
	int i;
	int c_len = len * 16;
	cout << "BCC_source = " << endl;
	for(i = 0; i < c_len; i++)
	{
		BCC_source[i] += '0';
		cout << BCC_source[i];
		BCC_source[i] -= '0';
	}
	cout << endl;
}

void printPunc()
{
	int i;
	int p_len = punc_len;
	cout << "punc = " << endl;
	for(i = 0; i < p_len; i++)
	{
		punc[i] += '0';
		cout << punc[i];
		punc[i] -= '0';
	}
	cout << endl;
}

void printRecover()
{
	int i;
	int r_len = len * 16;
	cout << "recover = " << endl;
	for(i = 0; i < r_len; i++)
	{
		recover[i] += '0';
		cout << recover[i];
		recover[i] -= '0';
	}
	cout << endl;
}

void printResultMatch()
{
	int i;
	int r_len = len * 16;
	cout << "result_match = " << endl;
	for(i = 0; i < r_len; i++)
	{
		result_match[i] += '0';
		cout << result_match[i];
		result_match[i] -= '0';
	}
	cout << endl;
}

void printResultBit()
{
	int i;
	int r_len = len * 8;
	cout << "result_bit = " << endl;
	for(i = 0; i < r_len; i++)
	{
		result_bit[i] += '0';
		cout << result_bit[i];
		result_bit[i] -= '0';
	}
	cout << endl;
}

void printResultByte()
{
	int i;
	cout << "result_byte = " << endl;
	for(i = 0; i < len; i++)
		cout << result_byte[i];
	cout << endl;
}


int Viterbi_s(char *Input, char *Output)
{
	int i, j, mode;
	char out_temp;
	int n_in, n_out;
	
	n_in = 0;
	n_out = 0;
	
	mode = 50;
	punc_len = 48;
	len = 3;
	
	for(i=0; i<6; i++)
	{
		for(j=0; j<8; j++)
		{
			punc[i*8+j] = (Input[n_in]>>j) & 0x01;
		}
		n_in++;
	}
	
	createMatch();
	createPrevious();
	punc_Recover(mode);
	viterbi();
	
	for(i=0; i<3; i++)
	{
		out_temp = 0;
		for(j=0; j<8; j++)
		{
			out_temp = out_temp | (result_bit[i*8+j]<<j);
		}
		Output[n_out++] = out_temp;
	}
	
	
	return 1;
}


int Viterbi_d(char *Input, char *Output, int bcc_rate, int len_afterbcc, int len_prebcc)
{
	int i, j, mode;
	char out_temp;
	int n_in, n_out;
	
	n_in = 0;
	n_out = 0;
	
	
	if(bcc_rate == R_1_2)
		mode = 50;
	else if(bcc_rate == R_2_3)
		mode = 66;
	else if(bcc_rate == R_3_4)
		mode = 75;
		
	punc_len = len_afterbcc*8;
	len = len_prebcc;
	
	for(i=0; i<len_afterbcc; i++)
	{
		for(j=0; j<8; j++)
		{
			punc[i*8+j] = (Input[n_in]>>j) & 0x01;
		}
		n_in++;
	}
	
	createMatch();
	createPrevious();
	punc_Recover(mode);
	
	if(bcc_rate == R_1_2)
		viterbi();
	else
		viterbi_ignore(mode);
		
	for(i=0; i<len_prebcc; i++)
	{
		out_temp = 0;
		for(j=0; j<8; j++)
		{
			out_temp = out_temp | (result_bit[i*8+j]<<j);
		}
		Output[n_out++] = out_temp;
	}		
	
	return 1;
		
		
}


