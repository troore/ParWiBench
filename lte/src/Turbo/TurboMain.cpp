
#include "TurboMain.h"

int RANDOMSEED;

#define TurboEnc

void test_turbo_encoding(int *piSeq, int in_buf_sz, int *pcSeq, int out_buf_sz)
{
	std::cout << "Turbo Encoder starts" << std::endl;
	
	ReadInputFromFiles(piSeq, in_buf_sz, "TurboEncoderInput");

	turbo_encoding(piSeq, in_buf_sz, pcSeq);

	WriteOutputToFiles(pcSeq, out_buf_sz, "testTurboEncoderOutput");

	cout<<"Turbo Encoder ends"<<endl;
}

void test_turbo_decoding(float *pInpData, int in_buf_sz, int *pOutBits, int out_buf_sz)
{
	cout<<"Turbo Decoder starts"<<endl;
	
	ReadInputFromFiles(pInpData, in_buf_sz, "testTurboEncoderOutput");

	
	for (int i = 0; i < in_buf_sz; i++)
		pInpData[i] = (1 - 2 * pInpData[i]);
   
	turbo_decoding(pInpData, pOutBits, out_buf_sz);

	
	for (int i = 0; i < out_buf_sz; i++)
		pOutBits[i] = (1 - pOutBits[i]) / 2;
	
	WriteOutputToFiles(pOutBits, out_buf_sz, "testTurboDecoderOutput");
	
	cout<<"Turbo Decoder ends"<<endl;
}

int main(int argc, char *argv[])
{
	int data_length;
	int in_buf_sz;
	int out_buf_sz;
	int n_block;
	int last_block_len;

	data_length = 2368;
	
	turbo_init(data_length);

	n_block = (data_length + BLOCK_SIZE - 1) / BLOCK_SIZE;
	if (data_length % BLOCK_SIZE)
	{
		last_block_len = data_length % BLOCK_SIZE;
	}
	else
	{
		last_block_len = BLOCK_SIZE;
	}
	
#ifdef TurboEnc

	int piSeq[N_TURBO_IN_MAX];
	int pcSeq[N_TURBO_OUT_MAX];
	
	in_buf_sz = data_length;
	out_buf_sz = RATE * ((n_block - 1) * (BLOCK_SIZE + 4) + 1 * (last_block_len + 4));
	
	test_turbo_encoding(piSeq, in_buf_sz, pcSeq, out_buf_sz);
	
	#else

	float pInpData[N_TURBO_OUT_MAX];
	int pOutBits[N_TURBO_IN_MAX];

	in_buf_sz = RATE * ((n_block - 1) * (BLOCK_SIZE + 4) + 1 * (last_block_len + 4));
	out_buf_sz = data_length;
	
	test_turbo_decoding(pInpData, in_buf_sz, pOutBits, out_buf_sz);

	#endif

	return 0;
}




