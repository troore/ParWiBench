In "dft.cpp":
**************************************************************************************************
	dft_loop1:for (k = 0; k < n; k++)
        {
                p[0] = 0.0;
                p[1] = 0.0;
                ang = pre_ang * ((float)k);
                dft_loop2:for (j = 0; j < n; j++)
                {
                        p[0] += (a[2 * j] * cosf(j * ang) - a[2 * j + 1] * sinf(j * ang));
                        p[1] += (a[2 * j] * sinf(j * ang) - a[2 * j + 1] * cosf(j * ang));
                }
                y[2 * k + 0] = p[0];
                y[2 * k + 1] = p[1];
        }
**************************************************************************************************
Apply "PIPELINE" to "dft_loop2" so that "p[0]" and "p[1]" can be calculated simultaneously. In the meanwhile, variable index access to "a[]" (a[2 * j] and a[2 * j + 1]) causes false dependence between loop iterations. Apply "DEPENDENCE" to cancel this dependence. "dft_loop1" is a nested loop, reform it into a perfect loop so that it can be "FLATTEN" to reduce overhead of entering and leaving nested loop. Modified code:
**************************************************************************************************
	dft_loop1:for (k = 0; k < n; k++)
        {
                dft_loop2:for (j = 0; j < n; j++)
                {
#pragma HLS PIPELINE
#pragma HLS DEPENDENCE array inter false
#pragma HLS loop_flatten
                        if(j == 0){
                                p[0] = 0.0;
                                p[1] = 0.0;
                                ang = pre_ang * ((float)k);
                        }
                                p[0] += (a[2 * j] * cosf(j * ang) - a[2 * j + 1] * sinf(j * ang));		//Both load and store on p[0]
                                p[1] += (a[2 * j] * sinf(j * ang) - a[2 * j + 1] * cosf(j * ang));		//Both load and store on p[1]
                        if(j == n-1){
                                y[2 * k + 0] = p[0];
                                y[2 * k + 1] = p[1];
                        }
                }
        }
**************************************************************************************************
In this situation, II = 5, so the code needs further modification. It is because there are both load and store operation on "p[0]" and "p[1]" in the loop body. We can avoid this by creating arrays to do the accumulation, modified code:
**************************************************************************************************
	dft_loop1:for (k = 0; k < n; k++)
        {
                dft_loop1_1:for (j = 0; j < n; j++)
                {
#pragma HLS PIPELINE
#pragma HLS DEPENDENCE array inter false
#pragma HLS loop_flatten
		    if(j == 0){
		        ang = pre_ang * ((float)k);
		    }
		    if(j == n - 1){
		        y[2 * k + 0] = p0[j] + (a[2 * j] * cosf(j * ang) - a[2 * j + 1] * sinf(j * ang));
		        y[2 * k + 1] = p1[j] + (a[2 * j] * sinf(j * ang) - a[2 * j + 1] * cosf(j * ang));
		    }
		    else{
		        p0[j + 1] = p0[j] + (a[2 * j] * cosf(j * ang) - a[2 * j + 1] * sinf(j * ang));
		        p1[j + 1] = p1[j] + (a[2 * j] * sinf(j * ang) - a[2 * j + 1] * cosf(j * ang));
		    }
		}
        }
**************************************************************************************************
We do the add operation on the next element of arrays to accomplish the accumulation, thus to avoid the dependence of both load and store on the same element. The length of array p0 and p1 is 1200, which is the maximum number of dft size. In this situation II = 1.





















In "TransformPrecoder.cpp":
**************************************************************************************************
        TransformPrecoding_loop1:for(int nlayer = 0; nlayer < NumLayer; nlayer++)
        {
                TransformPrecoding_loop2:for(int nsym = 0; nsym < NumULSymbSF - 2; nsym++)
                {

                        int idx = nlayer * (MDFT * (NumULSymbSF - 2)) + nsym * MDFT;
                        float norm = (float)sqrt((float)MDFT);

                        dft(MDFT, pInpSeq + idx * 2, pDataMatrix + idx * 2, 1);

                        TransformPrecoding_loop3:for(int m = 0; m < MDFT; m++)
                        {
                                pDataMatrix[2 * (idx + m) + 0] = pDataMatrix[2 * (idx + m) + 0] / norm;
                                pDataMatrix[2 * (idx + m) + 1] = pDataMatrix[2 * (idx + m) + 1] / norm;
                        }
                }
        }
**************************************************************************************************
Apply "FLATTEN" in the "TransformPrecoding_loop2" because "TransformPrecoding_loop1" and "TransformPrecoding_loop2" can be flattened into one layer. Apply "PIPELINE" to "TransformPrecoding_loop3" to increase latency. Again "DEPENDENCY" is applied in "TransformPrecoding_loop3" to avoid false dependency caused by variable index access. In "TransformPrecoding_loop3", pDataMatrix is accessed four times, including two times read and two times write. The access pattern is interleaved so "PARTITION cyclic" is applied to array "pDataMatrix" with factor = 2. Modified code:
**************************************************************************************************
#pragma HLS ARRAY_PARTITION variable=pDataMatrix cyclic factor=2 dim=1
        int NumULSymbSF = LTE_PHY_N_SYMB_PER_SUBFR;
        TransformPrecoding_loop1:for(int nlayer = 0; nlayer < NumLayer; nlayer++)
        {
                TransformPrecoding_loop2:for(int nsym = 0; nsym < NumULSymbSF - 2; nsym++)
                {
#pragma HLS LOOP_FLATTEN
                        int idx = nlayer * (MDFT * (NumULSymbSF - 2)) + nsym * MDFT;
                        float norm = (float)sqrt((float)MDFT);

                        dft(MDFT, pInpSeq + idx * 2, pDataMatrix + idx * 2, 1);

                        TransformPrecoding_loop3:for(int m = 0;m < MDFT; m++)
                        {
#pragma HLS PIPELINE
#pragma HLS DEPENDENCE array inter false
                                pDataMatrix[2 * (idx + m) + 0] = pDataMatrix[2 * (idx + m) + 0] / norm;
                                pDataMatrix[2 * (idx + m) + 1] = pDataMatrix[2 * (idx + m) + 1] / norm;
                        }
                }
        }
**************************************************************************************************
The result is II for "TransformPrecoding_loop3" is 1. "TransformPrecoding_loop1" and "TransformPrecoding_loop2" is flattened. For the other function "TransformDecoding" in "TransformPrecoder.cpp", similar strategy is applied because the structure is exactly the same.




















In "dmrs.cpp":
**************************************************************************************************
        while (zc_flag)
        {
                if (pPrimeTable[idx][0] == N_dft)
                {
                        Nzc = pPrimeTable[idx][1];
                        zc_flag = 0;
                }
                else
                {
                        idx++;
                }
        }
**************************************************************************************************
This loop has latency of 2. So Apply "PIPELINE" to it. Achieve II = 1. Modified code:
**************************************************************************************************
        look_up_prime_table_loop:while (zc_flag)
        {
#pragma HLS PIPELINE
                if (pPrimeTable[idx][0] == N_dft)
                {
                        Nzc = pPrimeTable[idx][1];
                        zc_flag = 0;
                }
                else
                {
                        idx++;
                }
        }
**************************************************************************************************
In the following loop, double-type operation to calculate the "theta" takes long time, so move some operations not related to loop variable out of loop body to do the pre-calculattion. And then apply "PIPELINE" to it to execute "cos" and "sin" simultaneously. 
**************************************************************************************************
        for (int m = 0;m < Nzc; m++)
        {
                double theta = -PI * q * m * (m + 1.0)/((double)Nzc);
                //px[m] = std::complex<double>(cos(theta), sin(theta));
                px[m][0] = cos(theta);
                px[m][1] = sin(theta);
        }
**************************************************************************************************
Modified code, II = 1:
**************************************************************************************************
        double theta_pre = -PI * q / (double)Nzc;
        calculate_px_array_loop:for (int m = 0;m < Nzc; m++)
        {
#pragma HLS PIPELINE
                double theta = theta_pre *  m * (m + 1.0);
                //px[m] = std::complex<double>(cos(theta), sin(theta));
                px[m][0] = cos(theta);
                px[m][1] = sin(theta);
        }
**************************************************************************************************
In the next loop:
**************************************************************************************************
        for (int slot = 0; slot < 2; slot++)
        {
                for (int layer = 0; layer < N_layer; layer++)
                {
                        int cs = ncs[slot] + 6 * layer;
                        double alpha = 2.0 * PI * cs / 12.0;

                        for (int n = 0; n < N_dft; n++)
                        {
                                int idx = n % Nzc;
                                double a = cos(alpha * n);
                                double b = sin(alpha * n);
                                //	std::complex<double> c = std::complex<double>((cos(alpha*n)),(sin(alpha*n)));
                                //	std::complex<double> c = std::complex<double>(a, b);
                                double c[2] = {a, b};
                                //	*(*(*(pDMRS+slot)+layer)+n)=(complex<float>)(c * px[idx]);
                                //	pDMRS[(slot * N_layer + layer) * N_dft + n] = (std::complex<float>)(c * px[idx]);
                                pDMRS[((slot * N_layer + layer) * N_dft + n) * 2 + 0] = c[0] * px[idx][0] - c[1] * px[idx][1];
                                pDMRS[((slot * N_layer + layer) * N_dft + n) * 2 + 1] = c[0] * px[idx][1] + c[1] * px[idx][0];
                        }
                }
        }
**************************************************************************************************
Move the assignment of "cs" and "alpha" into the layer-3 loop to change the whole loop to perfect nested loop and then apply "FLATTEN" to layer-3 loop to reduce overhead of entering and exiting loop. Calculate the index to access "pDMRS" in advance so that it doesn't need to calculated twice. Apply "PIPELINE" to execute calculation of two elements simultaneously and "DEPENDENCE" to remove false inter-iteration dependency. Modified code, II = 1:
**************************************************************************************************
        calculate_pDMRS_from_px_loop:for (int slot = 0; slot < 2; slot++)
        {
                for (int layer = 0; layer < N_layer; layer++)
                {
                        for (int n = 0; n < N_dft; n++)
                        {
#pragma HLS LOOP_FLATTEN
#pragma HLS PIPELINE
#pragma HLS DEPENDENCE array inter false
				int cs;
				double alpha;
                                if(n == 0){
                                        cs = ncs[slot] + 6 * layer;
                                        alpha = 2.0 * PI * cs / 12.0;
                                }
                                int index = ((slot * N_layer + layer) * N_dft + n) * 2 ;
                                int idx = n % Nzc;
                                double a = cos(alpha * n);
                                double b = sin(alpha * n);
                                //      std::complex<double> c = std::complex<double>((cos(alpha*n)),(sin(alpha*n)));
                                //      std::complex<double> c = std::complex<double>(a, b);
                                double c[2] = {a, b};
                                //      *(*(*(pDMRS+slot)+layer)+n)=(complex<float>)(c * px[idx]);
                                //      pDMRS[(slot * N_layer + layer) * N_dft + n] = (std::complex<float>)(c * px[idx]);
                                pDMRS[index] = c[0] * px[idx][0] - c[1] * px[idx][1];
                                pDMRS[index + 1] = c[0] * px[idx][1] + c[1] * px[idx][0];
                        }
                }
        }
**************************************************************************************************
In the following code, calculation of index-offset can be calculated before the loop. And the calculation of two elements of pDMRS can be done simultaneously by applying "PIPELINE" and "DEPENDENCE".
**************************************************************************************************
        if (N_layer == 2)
        {
                for (int n = 0; n < N_dft; n++)
                {
                        //      (*(*(*(pDMRS+1)+1)+n))*=(-1.0);
                        //      pDMRS[(1 * N_layer + 1) * N_dft + n] *= (-1.0);
                        pDMRS[2 * ((1 * N_layer + 1) * N_dft + n) + 0] = -1.0 * pDMRS[2 * ((1 * N_layer + 1) * N_dft + n) + 0];
                        pDMRS[2 * ((1 * N_layer + 1) * N_dft + n) + 1] = -1.0 * pDMRS[2 * ((1 * N_layer + 1) * N_dft + n) + 1];
                }
        }
        else
        {}
**************************************************************************************************
Modified code, II = 1:
**************************************************************************************************
        if (N_layer == 2)
        {
                int index_offset = 2 * (1 * N_layer + 1) * N_dft;
                negate_pDMRS_loop:for (int n = 0; n < N_dft; n ++)
                {
#pragma HLS PIPELINE
#pragma HLS DEPENDENCE array inter false
                        //      (*(*(*(pDMRS+1)+1)+n))*=(-1.0);
                        //      pDMRS[(1 * N_layer + 1) * N_dft + n] *= (-1.0);
                        pDMRS[index_offset + 2 * n] = -1.0 * pDMRS[index_offset + 2 * n];
                        pDMRS[index_offset + 2 * n + 1] = -1.0 * pDMRS[index_offset + 2 * n + 1];
                }
        }
**************************************************************************************************




















In "Equalizer.cpp":
**************************************************************************************************
        for (int m = 0; m < MDFT; m++)
        {
                float pXt[2 * LTE_PHY_N_ANT_MAX * 2];
                float pXtDagger[LTE_PHY_N_ANT_MAX * 2 * 2];

                for (int slot = 0; slot < 2; slot++)
                {
                        for (int layer = 0; layer < NumLayer; layer++)
                        {
#pragma HLS PIPELINE
#pragma HLS DEPENDENCE array inter false
                                //      pXt[slot * NumLayer + layer] = pDMRS[(slot * NumLayer + layer) * MDFT + m];
                                pXt[2 * (slot * NumLayer + layer) + 0] = pDMRS[2 * ((slot * NumLayer + layer) * MDFT + m) + 0];
                                pXt[2 * (slot * NumLayer + layer) + 1] = pDMRS[2 * ((slot * NumLayer + layer) * MDFT + m) + 1];
                                //      pXtDagger[layer * 2 + slot] = conj(pDMRS[(slot * NumLayer + layer) * MDFT + m]);
                                pXtDagger[2 * (layer * 2 + slot) + 0] = pDMRS[2 * ((slot * NumLayer + layer) * MDFT + m) + 0];
                                pXtDagger[2 * (layer * 2 + slot) + 1] = (-1.0) * pDMRS[2 * ((slot * NumLayer + layer) * MDFT + m) + 1];
                        }
                }

                float pYt[2 * LTE_PHY_N_ANT_MAX * 2];

                for (int slot = 0; slot < 2; slot++)
                {
                        for (int nrx = 0; nrx < NumRxAntenna; nrx++)
                        {
#pragma HLS PIPELINE
#pragma HLS DEPENDENCE array inter false
                                //      pYt[slot * NumRxAntenna + nrx] = pInpData[(nrx * 2 + slot) * MDFT + m];
                                pYt[2 * (slot * NumRxAntenna + nrx) + 0] = pInpData[2 * ((nrx * 2 + slot) * MDFT + m) + 0];
                                pYt[2 * (slot * NumRxAntenna + nrx) + 1] = pInpData[2 * ((nrx * 2 + slot) * MDFT + m) + 1];
                        }
                }

                float pHTranspose[LTE_PHY_N_ANT_MAX * LTE_PHY_N_ANT_MAX * 2];

                FDLSEstimation(pXt, pXtDagger, pYt, pHTranspose, NumLayer, NumRxAntenna);
                FDLSEqualization(pInpData, pHTranspose, m, NumLayer, NumRxAntenna, MDFT, pOutData);
        }
**************************************************************************************************
In the first and the second inner loop, "PIPELINE" and "DEPENDENCE" are applied to increase latency. II = 1. 
**************************************************************************************************
For the sub-function "FDLSEqualization":
**************************************************************************************************
        for (int nrx = 0; nrx < NumRxAntenna; nrx++)
        {
                for (int layer = 0; layer < NumLayer; layer++)
                {
#pragma HLS DEPENDENCE array inter false
#pragma HLS PIPELINE
                        //      pH[nrx * NumLayer + layer] = pHTranspose[layer * NumRxAntenna + nrx];
                        pH[2 * (nrx * NumLayer + layer) + 0] = pHTranspose[2 * (layer * NumRxAntenna + nrx) + 0];
                        pH[2 * (nrx * NumLayer + layer) + 1] = pHTranspose[2 * (layer * NumRxAntenna + nrx) + 1];
                        //      pHDagger[layer * NumRxAntenna + nrx] = conj(pHTranspose[layer * NumRxAntenna + nrx]);
                        pHDagger[2 * (layer * NumRxAntenna + nrx) + 0] = pHTranspose[2 * (layer * NumRxAntenna + nrx) + 0];
                        pHDagger[2 * (layer * NumRxAntenna + nrx) + 1] = (-1.0) * pHTranspose[2 * (layer * NumRxAntenna + nrx) + 1];
                }
        }

        MatrixProd(NumLayer, NumRxAntenna, NumLayer, pHDagger, pH, pHDH);
        MatrixInv(NumLayer, pHDH, pInvHDH);

        //Equalizing Data//
        for (int nSymb = 0; nSymb < NumULSymbSF - 2; nSymb++)
        {
                float pYData[LTE_PHY_N_ANT_MAX * 2];
                for (int nrx = 0; nrx < NumRxAntenna; nrx++)
                {
#pragma HLS PIPELINE
                        int IDX = (NumULSymbSF - 2) * nrx + nSymb + 2 * NumRxAntenna;
                        //      *(pYData+nrx)=*(*(pInpData+IDX)+m);
                        //      pYData[nrx] = pInpData[IDX * MDFTPerUser + m];
                        pYData[2 * nrx + 0] = pInpData[2 * (IDX * MDFTPerUser + m) + 0];
                        pYData[2 * nrx + 1] = pInpData[2 * (IDX * MDFTPerUser + m) + 1];
                }
                MatrixProd(NumLayer, NumRxAntenna, 1, pHDagger, pYData, pHDY);

                float pXData[LTE_PHY_N_ANT_MAX];
                MatrixProd(NumLayer, NumLayer, 1, pInvHDH, pHDY, pXData);
                for (int layer = 0; layer < NumLayer; layer++)
                {
#pragma HLS PIPELINE
#pragma HLS DEPENDENCE array inter false
                        int IDX = (NumULSymbSF - 2) * layer + nSymb;

                        //      *(*(pOutData+IDX)+m)=*(pXData+layer);
                        //      pOutData[IDX * MDFTPerUser + m] = pXData[layer];
                        pOutData[2 * (IDX * MDFTPerUser + m) + 0] = pXData[2 * layer + 0];
                        pOutData[2 * (IDX * MDFTPerUser + m) + 1] = pXData[2 * layer + 1];
                }
        }
**************************************************************************************************
Just apply "PIPELINE" and "DEPENDENCE" would make it enough to achieve II = 1.
**************************************************************************************************
For the sub-function "MatriProd":
**************************************************************************************************
        for (r = 0; r < d1; r++)
        {
                for (c = 0; c < d3; c++)
                {
                        float tmp[2] = {0.0, 0.0};

                        for (i = 0; i < d2; i++)
                        {
                                //      tmp += M1[r * d2 + i] * M2[i * d3 + c];
                                tmp[0] += (M1[2 * (r * d2 + i) + 0] * M2[2 * (i * d3 + c) + 0] - M1[2 * (r * d2 + i) + 1] * M2[2 * (i * d3 + c) + 1]);
                                tmp[1] += (M1[2 * (r * d2 + i) + 0] * M2[2 * (i * d3 + c) + 1] + M1[2 * (r * d2 + i) + 1] * M2[2 * (i * d3 + c) + 0]);
                        }

                        oM[2 * (r * d3 + c) + 0] = tmp[0];
                        oM[2 * (r * d3 + c) + 1] = tmp[1];
                }
        }
**************************************************************************************************
It can be modified to a perfect nested loop. And apply "FLATTEN" to reduce loop overhead, "PIPELINE" and "DEPENDENCE" to increase latency. Again, here two arrays (tmp0[5] and tmp1[5]) are created to execute the accumulation to avoid "load" and "store" to the same element of array in the loop body. Modified code:
**************************************************************************************************
        prod_1:for (r = 0; r < d1; r++)
        {
                prod_2:for (c = 0; c < d3; c++)
                {
                        prod_3:for (i = 0; i < d2; i++)
                        {
#pragma HLS loop_flatten
#pragma HLS PIPELINE
#pragma HLS DEPENDENCE array inter false
                                if(i == d2 - 1){
                                        oM[2 * (r * d3 + c) + 0] = tmp0[i] + (M1[2 * (r * d2 + i) + 0] * M2[2 * (i * d3 + c) + 0] - M1[2 * (r * d2 + i) + 1] * M2[2 * (i * d3 + c) + 1]);
                                        oM[2 * (r * d3 + c) + 1] = tmp1[i] + (M1[2 * (r * d2 + i) + 0] * M2[2 * (i * d3 + c) + 1] + M1[2 * (r * d2 + i) + 1] * M2[2 * (i * d3 + c) + 0]);
                                }
                                else{
                                        tmp0[i + 1] = tmp0[i] + (M1[2 * (r * d2 + i) + 0] * M2[2 * (i * d3 + c) + 0] - M1[2 * (r * d2 + i) + 1] * M2[2 * (i * d3 + c) + 1]);
                                        tmp1[i + 1] = tmp1[i] + (M1[2 * (r * d2 + i) + 0] * M2[2 * (i * d3 + c) + 1] + M1[2 * (r * d2 + i) + 1] * M2[2 * (i * d3 + c) + 0]);
                                }
                        }
                }
        }
**************************************************************************************************
For the sub-function "MatrixInv":
**************************************************************************************************
        inv1:for (int r = 0; r < sz; r++)
        {
                for (int c = 0; c < sz; c++)
                {
#pragma HLS PIPELINE
#pragma HLS DEPENDENCE array inter false
                        //      pX[r * (2 * sz) + c] = pM[r * sz + c];
                        pX[2 * (r * (2 * sz) + c) + 0] = pM[2 * (r * sz + c) + 0];
                        pX[2 * (r * (2 * sz) + c) + 1] = pM[2 * (r * sz + c) + 1];
                }
                for (int c = sz; c < 2 * sz; c++)
                {
#pragma HLS PIPELINE
#pragma HLS DEPENDENCE array inter false
                        if (c == (r + sz))
                        {
                                //      pX[r * (2 * sz) + c] = (std::complex<float>)1.0;
                                pX[2 * (r * (2 * sz) + c) + 0] = 1.0;
                                pX[2 * (r * (2 * sz) + c) + 1] = 0.0;
                        }
                        else
                        {
                                //      pX[r * (2 * sz) + c] = (std::complex<float>)0.0;
                                pX[2 * (r * (2 * sz) + c) + 0] = 0.0;
                                pX[2 * (r * (2 * sz) + c) + 1] = 0.0;
                        }
                }
        }
**************************************************************************************************
This loop just needs to apply "PIPELINE" and "DEPENDENCE" to achieve II = 1.
**************************************************************************************************
        inv2:for (int r = 0; r < sz; r++)
        {
                float B[2];
                B[0] = pX[2 * (r * (2 * sz) + r) + 0];
                B[1] = pX[2 * (r * (2 * sz) + r) + 1];

                for (int c = 0; c < (2 * sz); c++)
                {
#pragma HLS PIPELINE
#pragma HLS DEPENDENCE array inter false
                        float A[2];
                        //      pCurRow[c] = pX[r * (2 * sz) + c] / pX[r * (2 * sz) + r];
                        A[0] = pX[2 * (r * (2 * sz) + c) + 0];
                        A[1] = pX[2 * (r * (2 * sz) + c) + 1];
                        pCurRow[2 * c + 0] = (A[0] * B[0] + A[1] * B[1]) / (B[0] * B[0] + B[1] * B[1]);
                        pCurRow[2 * c + 1] = (A[1] * B[0] - A[0] * B[1]) / (B[0] * B[0] + B[1] * B[1]);
                }

                for (int c = 0; c < (2 * sz); c++)
                {
#pragma HLS PIPELINE
#pragma HLS DEPENDENCE array inter false
                        //      pX[r * (2 * sz) + c] = pCurRow[c];
                        pX[2 * (r * (2 * sz) + c) + 0] = pCurRow[2 * c + 0];
                        pX[2 * (r * (2 * sz) + c) + 1] = pCurRow[2 * c + 1];
                }
**************************************************************************************************
These two loops just need to apply "PIPELINE" and "DEPENDENCE" to achieve II = 1.
**************************************************************************************************
For the following loop, it is a little bit complicated:
**************************************************************************************************
                for (int er = r + 1; er < sz; er++)
                {
                        float curC[2] = {pX[2 * (er * (2 * sz) + r) + 0], pX[2 * (er * (2 * sz) + r) + 1]};

                        for (int c = 0; c < (2 * sz); c++)
                        {
                                //      pX[er * (2 * sz) + c] -= curC * pCurRow[c];
                                pX[2 * (er * (2 * sz) + c) + 0] -= (curC[0] * pCurRow[2 * c + 0] - curC[1] * pCurRow[2 * c + 1]);
                                pX[2 * (er * (2 * sz) + c) + 1] -= (curC[0] * pCurRow[2 * c + 1] + curC[1] * pCurRow[2 * c + 0]);
                        }
                }
**************************************************************************************************
HLS automatically flattens this loop, this cause dependency because of limited memory ports. And because of nonlinear and non-interleaved access, "PARTITION" does not work for the arrays. So apply "FLATTEN off" to cancel the flatten. And create a temporary array "px_tmp" to do the subtraction. Then apply "PIPELINE" to achieve II = 1. See the modified code:
**************************************************************************************************
                for (int er = r + 1; er < sz; er++)
                {
                        float curC[2] = {pX[2 * (er * (2 * sz) + r) + 0], pX[2 * (er * (2 * sz) + r) + 1]};

                        for (int c = 0; c < (2 * sz); c++)
                        {
#pragma HLS PIPELINE
#pragma HLS loop_flatten off
#pragma HLS DEPENDENCE array inter false
                                //      pX[er * (2 * sz) + c] -= curC * pCurRow[c];
                                pX_tmp[2 * (er * (2 * sz) + c) + 0] = pX[2 * (er * (2 * sz) + c) + 0] - (curC[0] * pCurRow[2 * c + 0] - curC[1] * pCurRow[2 * c + 1]);
                                pX_tmp[2 * (er * (2 * sz) + c) + 1] = pX[2 * (er * (2 * sz) + c) + 1] - (curC[0] * pCurRow[2 * c + 1] + curC[1] * pCurRow[2 * c + 0]);
                        }
                }
**************************************************************************************************
For the following loop, the situation is similar with the last loop:
**************************************************************************************************
        inv3:for (int r = sz - 1; r >= 0; r--)
        {
                for (int c = 0; c < (2 * sz); c++)
                {
#pragma HLS PIPELINE
                        //      pCurRow[c] = pX[r * (2 * sz) + c];
                        pCurRow[2 * c + 0] = pX_tmp[2 * (r * (2 * sz) + c) + 0];
                        pCurRow[2 * c + 1] = pX_tmp[2 * (r * (2 * sz) + c) + 1];
                }

                for (int er = r - 1; er >= 0; er--)
                {
                        float curC[2] = {pX_tmp[2 * (er * (2 * sz) + r) + 0], pX_tmp[2 * (er * (2 * sz) + r) + 1]};

                        for (int c = 0; c < (2 * sz); c++)
                        {
#pragma HLS PIPELINE
#pragma HLS loop_flatten off
#pragma HLS DEPENDENCE array inter false
                                //      pX[er * (2 * sz) + c] -= curC * pCurRow[c];
                                pX_tmp2[2 * (er * (2 * sz) + c) + 0] = pX_tmp[2 * (er * (2 * sz) + c) + 0] - (curC[0] * pCurRow[2 * c + 0] - curC[1] * pCurRow[2 * c + 1]);
                                pX_tmp2[2 * (er * (2 * sz) + c) + 1] = pX_tmp[2 * (er * (2 * sz) + c) + 1] - (curC[0] * pCurRow[2 * c + 1] + curC[1] * pCurRow[2 * c + 0]);
                        }
                }
        }
**************************************************************************************************
The last loop of "MatrixInv" is easy to achieve II = 1:
**************************************************************************************************
        inv4:for (int r = 0; r < sz; r++)
        {
                for (int c = 0; c < sz; c++)
                {
#pragma HLS PIPELINE
#pragma HLS DEPENDENCE array inter false
                        int col = c + sz;

                        //      pInvM[r * sz + c] = pX[r * (2 * sz) + col];
                        pInvM[2 * (r * sz + c) + 0] = pX_tmp2[2 * (r * (2 * sz) + col) + 0];
                        pInvM[2 * (r * sz + c) + 1] = pX_tmp2[2 * (r * (2 * sz) + col) + 1];
                }

        }
**************************************************************************************************




















In "fft.cpp":
**************************************************************************************************
        for (p = 1; p <= (n / 2); p <<= 1)
        {
                //      omega_m[0] = cos((2 * PI) / m);
                //      omega_m[1] = ((float)direction) * sin((2 * PI) / m);
                for (i = 0; i < (n >> 1); i++)
                {
#pragma HLS PIPELINE
#pragma HLS DEPENDENCE array inter false
                        int o_idx = i + (n >> 1);

                        k = i & (p - 1); // i % p
                        ang = ((2 * PI * k) / (2 * p));
                        omega[0] = cos(ang);
                        omega[1] = ((float)direction) * sin(ang);
                        t[0] = omega[0] * a[o_idx] - omega[1] * a[o_idx + n];
                        t[1] = omega[0] * a[o_idx + n] + omega[1] * a[o_idx];
                        u[0] = a[i];
                        u[1] = a[i + n];

                        y[2 * i - k] = u[0] + t[0];
                        y[2 * i - k + n] = u[1] + t[1];
                        y[2 * i - k + p] = u[0] - t[0];
                        y[2 * i - k + p + n] = u[1] - t[1];
                }
                for (i = 0; i < n; i++)
                {
#pragma HLS PIPELINE
#pragma HLS DEPENDENCE array inter false
                        a[i] = y[i];
                        a[i + n] = y[i + n];
                }
        }
**************************************************************************************************
Apply "PIPELINE" and "DEPENDENCE" to the first and the second loop. For the second loop II = 1. For the first loop, II = 2 because array y is stored four times in the loop body and there are limited memory ports. Access to array y is not linear nor interleaved so "PARTITION" is not a effective strategy. As a comparison, we separate the array to solve the memory port problem to achieve II = 1. This is discussed later.
**************************************************************************************************
The following code implement the FFT with separated arrays storing real parts and imaginary parts. It achieves II = 1. Synthesis report shows that this data structure also demostrates less latency, similar resources and similar power consumption.
**************************************************************************************************
        for (p = 1; p <= (n / 2); p <<= 1)
        {
                //      omega_m[0] = cos((2 * PI) / m);
                //      omega_m[1] = ((float)direction) * sin((2 * PI) / m);
                for (i = 0; i < (n >> 1); i++)
                {
#pragma HLS DEPENDENCE array inter false
#pragma HLS PIPELINE
                        int o_idx = i + (n >> 1);

                        k = i & (p - 1); // i % p
                        ang = ((2 * PI * k) / (2 * p));
                        omega[0] = cos(ang);
                        omega[1] = ((float)direction) * sin(ang);
                        // t = omega * a[i + n / 2];
                        t[0] = omega[0] * a_real[o_idx] - omega[1] * a_imag[o_idx];
                        t[1] = omega[0] * a_imag[o_idx] + omega[1] * a_real[o_idx];
                        // u = a[i];
                        u[0] = a_real[i];
                        u[1] = a_imag[i];

                        //      y[2 * i - k] = u + t;
                        y_real[2 * i - k] = u[0] + t[0];
                        y_imag[2 * i - k] = u[1] + t[1];
                        //      y[2 * i - k + p] = u - t;
                        y_real[2 * i - k + p] = u[0] - t[0];
                        y_imag[2 * i - k + p] = u[1] - t[1];
                }

                for (i = 0; i < n; i++)
                {
#pragma HLS PIPELINE
                        a_real[i] = y_real[i];
                        a_imag[i] = y_imag[i];
                }
        }
**************************************************************************************************
The following code implement the FFT with iteration method and store real data parts and imaginary data parts in separated arrays. Both arrays "y_real" and "y_imag" are read twice and written twice. They are not accessed in a linear way nor interleaved way. So two temporary arrays are created to avoid the memory ports limitation problem.
**************************************************************************************************
        for (j = 1; j <= s; j++)
        {
                d  = pow2(j);
                t = (2 * PI) / d;
                wdr = cos(t);
                wdi = ((float)direction) * sin(t);

                wr = 1; wi = 0;
                for (k = 0; k < (d / 2); k++)
                {
                        for (m = k; m < n; m += d)
                        {
                                tr = wr * y_real[(m + d / 2)] - wi * y_imag[(m + d / 2)];
                                ti = wr * y_imag[(m + d / 2)] + wi * y_real[(m + d / 2)];
                                xr = y_real[m];
                                xi = y_imag[m];

                                y_real[m] = xr + tr;
                                y_imag[m] = xi + ti;

                                y_real[m + d / 2] = xr - tr;
                                y_imag[m + d / 2] = xi - ti;
                        }
                        wtr = wr * wdr - wi * wdi;
                        wti = wr * wdi + wi * wdr;
                        wr = wtr;
                        wi = wti;
                }
        }
**************************************************************************************************
The modified code is below, apply "PIPELINE" and "DEPENDENCE" and it achieve II = 1:
**************************************************************************************************
                for(k = 0; k < n; k++)
                {
#pragma HLS PIPELINE
                        y_real_tmp[k] = y_real[k];
                        y_imag_tmp[k] = y_imag[k];
                }

                wr = 1; wi = 0;
                for (k = 0; k < (d / 2); k++)
                {

                        for (m = k; m < n; m += d)
                        {
#pragma HLS PIPELINE
#pragma HLS DEPENDENCE array inter false

                                tr = wr * y_real_tmp[(m + d / 2)] - wi * y_imag_tmp[(m + d / 2)];
                                ti = wr * y_imag_tmp[(m + d / 2)] + wi * y_real_tmp[(m + d / 2)];
                                xr = y_real_tmp[m];
                                xi = y_imag_tmp[m];

                                y_real[m] = xr + tr;
                                y_imag[m] = xi + ti;
                                y_real[m + d / 2] = xr - tr;
                                y_imag[m + d / 2] = xi - ti;
                        }
                        wtr = wr * wdr - wi * wdi;
                        wti = wr * wdi + wi * wdr;
                        wr = wtr;
                        wi = wti;
                }
**************************************************************************************************




















In "OFDM.cpp":
**************************************************************************************************
In the sub-function "ofmodulating_two_arrays", in the following code, apply "PIPELINE" and "DEPENDECE" to the two most inner loops make they II = 1. And then apply "FLATTEN" to the outer loop the flatten the nested loop.
**************************************************************************************************
        for (nlayer = 0; nlayer < NumLayer; nlayer++)
        {
                for (nsym = 0; nsym < NumULSymbSF; nsym++)
                {
#pragma HLS loop_flatten
                        int symb_idx = nlayer * NumULSymbSF + nsym;
                        float norm = (float)sqrt((float)NIFFT);

                        fft_iter(NIFFT, pInpDataReal + symb_idx * NIFFT, pInpDataImag + symb_idx * NIFFT,
                                         pOutDataReal + symb_idx * (CPLen + NIFFT) + CPLen, pOutDataImag + symb_idx * (CPLen + NIFFT) + CPLen,
                                         -1);

                        for (i = 0; i < NIFFT; i++)
                        {
#pragma HLS PIPELINE
#pragma HLS DEPENDENCE array inter false
                                pOutDataReal[symb_idx * (NIFFT + CPLen) + CPLen + i] /= norm;
                                pOutDataImag[symb_idx * (NIFFT + CPLen) + CPLen + i] /= norm;
                        }

                        for (i = 0; i < CPLen; i++)
                        {
#pragma HLS PIPELINE
#pragma HLS DEPENDENCE array inter false
                                pOutDataReal[symb_idx * (NIFFT + CPLen) + i] = pOutDataReal[symb_idx * (NIFFT + CPLen) + NIFFT + i];
                                pOutDataImag[symb_idx * (NIFFT + CPLen) + i] = pOutDataImag[symb_idx * (NIFFT + CPLen) + NIFFT + i];
                        }
                }
        }
**************************************************************************************************




















In "RateMatcher.cpp":
**************************************************************************************************
        out_block_offset = 0;
        for (i = 0; i < n_blocks; i++)
        {
                cur_blk_len = (i != (n_blocks - 1)) ? rm_blk_sz : rm_last_blk_len;

                for (j = 0; j < cur_blk_len; j++)
                {
                        for (r = 0; r < RATE; r++)
                        {
#pragma HLS PIPELINE
                                pInMatrix[r * cur_blk_len + j] = pLLRin[out_block_offset + RATE * j + r];
                        }
                }

                SubblockDeInterleaving(cur_blk_len, pInMatrix, pOutMatrix);

                for (j = 0; j < cur_blk_len; j++)
                {
                        for (r = 0; r < RATE; r++)
                        {
#pragma HLS PIPELINE
                                pLLRin[out_block_offset + RATE * j + r] = pOutMatrix[r * cur_blk_len + j];
                        }
                }

                out_block_offset += RATE * cur_blk_len;
        }

        for (i = 0; i < out_buf_sz; i++)
        {
#pragma HLS PIPELINE
                if (pLLRin[i] < 0)
                {
                        pHD[i] = 0;
                }
                else
                {
                        pHD[i] = 1;
                }
        }
        for (i = 0; i < out_buf_sz; i++)
        {
#pragma HLS PIPELINE
                pLLRout[i] = pLLRin[i];
        }
**************************************************************************************************
For the previous sub-function "RxRateMatching", applying "PIPELINE" to its loops is sufficient to achieve II = 1.




















In "SubblockInterleaver_lte.cpp"
**************************************************************************************************
//////////////////// DeInterleaving for i=0,1 ///////////////////////
        float pInterMatrix[((BLOCK_SIZE + 31) / 32) * 32];
#pragma HLS DEPENDENCE variable=pInterMatrix inter false

        for (int StrIdx = 0; StrIdx < (Rate - 1); StrIdx++)
        {
                InIdx=0;
                inter_matrix_assign:for (int r = 0;r < R_sb; r++)
                {
                        for (int c = 0; c < C_sb; c++)
                        {
#pragma HLS PIPELINE
                                int k = r * C_sb + c;

                                if (k < NumDummy)
                                {
                                        pInterMatrix[r * C_sb + c] = DummyValue;
                                }
                                else
                                {
                                        pInterMatrix[r * C_sb + c] = (float)0;
                                }
                        }
                }

                from_input_to_inter:for(int c = 0; c < C_sb; c++)
                {
                        for (int r = 0; r < R_sb; r++)
                        {
#pragma HLS PIPELINE
                                int col = InterColumnPattern[c];
                                int k = col * R_sb + r;
                                //float v = pInterMatrix[r * C_sb + col];

                                if (pInterMatrix[r * C_sb + col] != DummyValue)
                                {
                                        pInterMatrix[r * C_sb + col] = pInpMtr[StrIdx * D + InIdx];
                                        InIdx++;
                                }
                        }
                }

                OutIdx=0;
                from_inter_to_output:for (int r = 0; r < R_sb; r++)
                {
                        for(int c = 0; c < C_sb; c++)
                        {
#pragma HLS PIPELINE
                                float v = pInterMatrix[r * C_sb + c];

                                if (v == DummyValue)
                                {}
                                else
                                {
                                        pOutMtr[StrIdx * D + OutIdx] = pInterMatrix[r * C_sb + c];
                                        OutIdx++;
                                }
                        }
                }
        }

//////////////////// DeInterleaving for i=2 ///////////////////////
        int Pi[((BLOCK_SIZE + 31) / 32) * 32];
        float pInterSeq[((BLOCK_SIZE + 31) / 32) * 32];
#pragma HLS DEPENDENCE variable=pInterSeq inter false

        rate3_dummyvalue_assign:for (int k = 0; k < NumDummy; k++)
                pInterSeq[k] = DummyValue;
//////////////// Pi & DePi//////////////////
        calculate_index:for(int k=0;k<K_pi;k++)
        {
#pragma HLS PIPELINE
                int idxP = k / R_sb;
                int idx = (InterColumnPattern[idxP] + (C_sb * (k % R_sb)) + 1) % K_pi;

                Pi[k]=idx;
        }
/////////////// DeInterleaving ////////////////////
        InIdx=0;
        rate3_from_input_to_inter:for(int k=0;k<K_pi;k++)
        {
#pragma HLS PIPELINE
                //float v = pInterSeq[Pi[k]];
                if (pInterSeq[Pi[k]] != DummyValue)
                {
                        pInterSeq[Pi[k]] = pInpMtr[(Rate - 1) * D + InIdx];
                        InIdx++;
                }
        }
        OutIdx=0;
        from_inter_to_out:for (int k = NumDummy; k < K_pi; k++)
        {
#pragma HLS PIPELINE
                pOutMtr[(Rate - 1) * D + OutIdx] = pInterSeq[k];
                OutIdx++;
        }
**************************************************************************************************
It is sufficient to apply "PIPELINE" and "DEPENDENCE" to achieve II = 1 and increase the latency.




















In "Scrambler.cpp"
**************************************************************************************************
        Scrambling_label1:for (i = 0; i < n; i++)
        {
#pragma HLS PIPELINE
                pOutSeq[i] = (pInpSeq[i] + scramb_seq_int[i]) % 2;
        }
**************************************************************************************************
In the above loop, apply "PIPELINE" to increase latency of "mod" operation from 3 to 1.
**************************************************************************************************
In the following sub-function "GenScrambInt" code, apply "PIPELINE" is sufficient to achieve II = 1. 
**************************************************************************************************
        for (i = 0; i < 31; i++)
        {
#pragma HLS PIPELINE
                px1[i] = 0;
                px2[i] = n_init[i];
        }


        GenScrambInt_label2:for (i = 0; i < n + N_c - 31; i++)
        {
#pragma HLS PIPELINE
//              if(i >= n + N_c -31)
//                      break;
                px1_tmp3 = px1[i + 3];
                px1[i + 31] = (px1_tmp3 + px1_tmp0) % 2;
                px1_tmp0 = px1_tmp1;
                px1_tmp1 = px1_tmp2;
                px1_tmp2 = px1_tmp3;

                px2_tmp3 = px2[i + 3];
                px2[i + 31] = (px2_tmp3 + px2_tmp2 + px2_tmp1 + px2_tmp0) % 2;
                px2_tmp0 = px2_tmp1;
                px2_tmp1 = px2_tmp2;
                px2_tmp2 = px2_tmp3;
        }
        GenScrambInt_label3:for (i = 0; i < n; i++)
        {
#pragma HLS PIPELINE
//              if(i >= n)
//                      break;
                pScrambInt[i] = (px1[i + N_c] + px2[i + N_c]) % 2;
        }
**************************************************************************************************




















In "Turbo.cpp"
**************************************************************************************************
In the sub-function "set_generate_polynomials", the following loops is unrolled. Because the first loop bound variable "n_gens" is actually constant "2" when the sub-function is called in the top-function of "Turbo.cpp". The second loop bound is constant "N_STATES = 8". So "UNROLL" is applied to it. Two inner loops in the second loop are actually not loops as "n_gens" = 2. The code is like this (in the first loop replace "n_gens" with "2" in the head of loop and add loop-break condition):
**************************************************************************************************
        set_generator_polynomials_loop1:for (i = 0; i < 2 /*n_gens*/; i++)
        {
#pragma HLS UNROLL
                if(i >= n_gens)
                        break;
                g_gen_pols[i] = gens[i];
                g_rev_gen_pols[i] = reverse_int(K, gens[i]);
        }
        set_generator_polynomials_loop2:for (s_prim = 0; s_prim < N_STATES; s_prim++)
        {
#pragma HLS UNROLL
//              std::cout << s_prim << std::endl;
                s0 = calc_state_transition(s_prim, 0, p0);
//              std::cout << s0 << "\t";
                g_state_trans[s_prim * 2 + 0] = s0;
                g_rev_state_trans[s0 * 2 + 0] = s_prim;
                set_generator_polynomials_loop2_in1:for (j = 0; j < (n_gens - 1); j++)
                {
                        g_output_parity[s_prim * (n_gens - 1) * 2 + 2 * j + 0] = p0[j];
                        g_rev_output_parity[s0 * (n_gens - 1) * 2 + 2 * j + 0] = p0[j];
                }

                s1 = calc_state_transition(s_prim, 1, p1);
//              std::cout << s1 << std::endl;
                g_state_trans[s_prim * 2 + 1] = s1;
                g_rev_state_trans[s1 * 2 + 1] = s_prim;
                set_generator_polynomials_loop2_in2:for (j = 0; j < (n_gens - 1); j++)
                {
                        g_output_parity[s_prim * (n_gens - 1) * 2 + 2 * j + 1] = p1[j];
                        g_rev_output_parity[s1 * (n_gens - 1) * 2 + 2 * j + 1] = p1[j];
                }
//              std::cout << (int)p0[0] << "\t" << (int)p1[0] << std::endl;
        }
**************************************************************************************************
In the above loop, calc_state_transition is called twice. In the sub-function calc_state_transition, there are two loops:
**************************************************************************************************
        calc_state_transition_loop1:for (i = 0; i < CST_LEN; i++)
        {
                in = (temp & 1) ^ in;
                temp = temp >> 1;
        }
        in = in ^ input;

        calc_state_transition_loop2:for (j = 0; j < (N_GENS - 1); j++)
        {
                parity_temp = (instate | (in << (CST_LEN - 1))) & g_gen_pols[j + 1];
                parity_bit = 0;
                calc_state_transition_loop2_in:for (i = 0; i < CST_LEN; i++)
                {
                        parity_bit = (parity_temp & 1) ^ parity_bit;
                        parity_temp = parity_temp >> 1;
                }
                parity[j] = parity_bit;
        }
**************************************************************************************************
Their iteration latency is already 1. No optimization strategy is needed.
**************************************************************************************************
In the top-function "turbo_decoding" in "Turbo.cpp":
**************************************************************************************************
        // Clear data part of recv_syst2
        for (i = 0; i < N_UNCODED; i++)
        {
#pragma HLS UNROLL
                recv_syst2[i] = 0.0;
        }
**************************************************************************************************
The above loop is unrolled because it has a constant loop boundary. 
**************************************************************************************************
                for (i = 0; i < n_blocks; i++)
                {
                        cur_blk_len = (i != (n_blocks - 1)) ? BLOCK_SIZE : last_block_length;
                        // data part
                        for (j = 0; j < BLOCK_SIZE; j++)
                        {
        #pragma HLS PIPELINE
                                if(j >= cur_blk_len)
                                        break;
                                recv_syst1[j] = pInpData[out_bit_offset++];
                                for (k = 0; k < N_GENS - 1; k++)
                                {
                                        recv_parity1[j * (N_GENS - 1) + k] = pInpData[out_bit_offset++];
                                }
                                for (k = 0; k < N_GENS - 1; k++)
                                {
                                        recv_parity2[j * (N_GENS - 1) + k] = pInpData[out_bit_offset++];
                                }
                        }
                        // first tail
                        for (j = 0; j < N_TAIL; j++)
                        {
        #pragma HLS PIPELINE
                                recv_syst1[cur_blk_len + j] = pInpData[out_bit_offset++];
                                for (k = 0; k < N_GENS - 1; k++)
                                {
                                        recv_parity1[(cur_blk_len + j) * (N_GENS - 1) + k] = pInpData[out_bit_offset++];
                                }
                                for (k = 0; k < N_GENS - 1; k++)
                                {
                                        recv_parity2[j * (N_GENS - 1) + k] = pInpData[out_bit_offset++];
                                }
                        }
                        // first tail
                        for (j = 0; j < N_TAIL; j++)
                        {
        #pragma HLS PIPELINE
                                recv_syst1[cur_blk_len + j] = pInpData[out_bit_offset++];
                                for (k = 0; k < N_GENS - 1; k++)
                                {
                                        recv_parity1[(cur_blk_len + j) * (N_GENS - 1) + k] = pInpData[out_bit_offset++];
                                }
                        }
                        // second tail
                        for (j = 0; j < N_TAIL; j++)
                        {
        #pragma HLS PIPELINE
                                recv_syst2[cur_blk_len + j] = pInpData[out_bit_offset++];
                                for (k = 0; k < N_GENS - 1; k++)
                                {
                                        recv_parity2[(cur_blk_len + j) * (N_GENS - 1) + k] = pInpData[out_bit_offset++];
                                }
                        }

                        decode_block(recv_syst1, recv_syst2, recv_parity1, recv_parity2, pOutBits + out_block_offset, cur_blk_len, n_iters);
                        out_block_offset += cur_blk_len;
                }
**************************************************************************************************
The above loop is the second loop and the last loop in top-function "turbo_decoding". In the first inner loop, the array "pInpData" is accessed three times continously in the loop body. So apply "PARTITION cyclic"(just after the head of top-function) to the array to avoid memory port limitation and "PIPELINE" to the loop to achieve II = 1. While for the second and the third inner loop, apply "PIPELINE" is sufficient to achieve II = 1.
**************************************************************************************************
For the sub-function "decode_block", the 2nd, 3rd, 4th and 6th loop is applied "PIPELINE" and they achieve II = 1. For the 1st and 5th loops, they already have a latency of 1. The code is below:
**************************************************************************************************
        for (i = 0; i < n_tailed; i++)
                Le21[i] = 0.0;

        internal_interleaver(recv_syst1, int_recv_syst1, interleaver_size);
        internal_deinterleaver(recv_syst2, deint_recv_syst2, interleaver_size);

        // Combine the results from recv_syst1 and recv_syst2 (in case some bits are transmitted several times)
        for (i = 0; i < interleaver_size; i++)
        {
#pragma HLS PIPELINE
                recv_syst[i] = recv_syst1[i] + deint_recv_syst2[i];
        }
        for (i = 0; i < interleaver_size; i++)
        {
#pragma HLS PIPELINE
                int_recv_syst[i] = recv_syst2[i] + int_recv_syst1[i];
        }
        for (i = interleaver_size; i < n_tailed; i++)
        {
#pragma HLS PIPELINE
                recv_syst[i] = recv_syst1[i];
                int_recv_syst[i] = recv_syst2[i];
        }
        // do the iterative decoding
        for (i = 0; i < n_iters; i++)
        {
        //      map_decoder(recv_syst1, recv_parity1, Le21, Le12, interleaver_size);
                log_decoder(recv_syst, recv_parity1, Le21, Le12, interleaver_size);

                internal_interleaver(Le12, Le12_int, interleaver_size);

                //      memset(Le12_int + interleaver_size, 0, N_TAIL * sizeof(float));
                for (j = interleaver_size; j < interleaver_size + N_TAIL; j++)
                        Le12_int[j] = 0.0;
        //      map_decoder(recv_syst2, recv_parity2, Le12_int, Le21_int, interleaver_size);
                log_decoder(int_recv_syst, recv_parity2, Le12_int, Le21_int, interleaver_size);

                internal_deinterleaver(Le21_int, Le21, interleaver_size);
                //      memset(Le21 + interleaver_size, 0, N_TAIL * sizeof(float));
                for (j = interleaver_size; j < interleaver_size + N_TAIL; j++)
                        Le21_int[j] = 0.0;
        }

        for (i = 0; i < interleaver_size; i++)
        {
#pragma HLS PIPELINE
                L[i] = recv_syst[i] + Le21[i] + Le12[i];
        //      std::cout << recv_syst1[i] << "\t" << Le21[i] << "\t" << Le12[i] << std::endl;
                decoded_bits_i[i] = (L[i] > 0.0) ? 1 : -1;
        //      decoded_bits_i[i] = (L[i] >= 0.0) ? 1 : 0;
        }
**************************************************************************************************
For the sub-function "log-decoder" as below, there are 7 loops. Loop 1 has latency of 1. Loop 2 is modified into perfect loop to apply "FLATTEN". Loop 3 anbd loop 5 are initialization of an array, so just leave it. Apply "PIPELINE" to loop 4 and loop 6, but because of multiple access to arrays and it is not accessed linearly and it cannot be guaranteed that each element is accessed only once, "PARTITION" cannot be applied and buffer array doesn't work either. The II = 2. Loop 7 is applied "PIPELINE" but II = 4 because carried dependecy between "select operation" (comparison) and "fadd operation".
**************************************************************************************************
        loop1:for (k = 0; k <= block_length; k++)
        {
                denom[k] = -LOG_INFINITY;
        }

        loop2:for (k = 1; k <= block_length; k++)
        {
                kk = k - 1;

                for (s_prim = 0; s_prim < N_STATES; s_prim++)
                {
                        /*exp_temp0 = 0.0;
                        exp_temp1 = 0.0;*/

                        for (j = 0; j < (N_GENS - 1); j++)
                        {
#pragma HLS LOOP_FLATTEN
                                if(j == 0){
                                        exp_temp0 = 0.0;
                                        exp_temp1 = 0.0;
                                }

                                rp = recv_parity[kk * (N_GENS - 1) + j];
                                if (0 == g_output_parity[s_prim * (N_GENS - 1) * 2 + j * 2 + 0])
                                {
                                        exp_temp0 += rp;
                                }
                                else
                                {
                                        exp_temp0 -= rp;
                                }
                                if (0 == g_output_parity[s_prim * (N_GENS - 1) * 2 + j * 2 + 1])
                                {
                                        exp_temp1 += rp;
                                }
                                else
                                {
                                        exp_temp1 -= rp;
                                }
                               
				if(j == N_GENS - 2){
                                        gamma[(2 * s_prim + 0) * (block_length + 1) + k] =  0.5 * ((apriori[kk] + recv_syst[kk]) + exp_temp0);
                                        gamma[(2 * s_prim + 1) * (block_length + 1) + k] = -0.5 * ((apriori[kk] + recv_syst[kk]) - exp_temp1);
                                }
                        }

                        //gamma[(2 * s_prim + 0) * (block_length + 1) + k] =  0.5 * ((apriori[kk] + recv_syst[kk]) + exp_temp0);
                //      std::cout << gamma[(2 * s_prim + 0) * (block_length + 1) + k] << "\t";
                        //gamma[(2 * s_prim + 1) * (block_length + 1) + k] = -0.5 * ((apriori[kk] + recv_syst[kk]) - exp_temp1);
                //      std::cout << gamma[(2 * s_prim + 1) * (block_length + 1) + k] << std::endl;
                }
        }

        // Initiate alpha
        loop3:for (int i = 1; i < N_STATES; i++)
        {
                alpha[i * (block_length + 1) + 0] = -LOG_INFINITY;
        }
        alpha[0 * (block_length + 1) + 0] = 0.0;
        // Calculate alpha, going forward through the trellis
        loop4:for (k = 1; k <= block_length; k++)
        {
                for (s = 0; s < N_STATES; s++)
                {
#pragma HLS DEPENDENCE array inter false
#pragma HLS PIPELINE
                        s_prim0 = g_rev_state_trans[s * 2 + 0];
                        s_prim1 = g_rev_state_trans[s * 2 + 1];
                        temp0 = alpha[s_prim0 * (block_length + 1) + k - 1] + gamma[(2 * s_prim0 + 0) * (block_length + 1) + k];
                        temp1 = alpha[s_prim1 * (block_length + 1) + k - 1] + gamma[(2 * s_prim1 + 1) * (block_length + 1) + k];
                        //      alpha[s * (block_length + 1) + k] = com_log(temp0, temp1);
                        //      denom[k] = com_log(alpha[s * (block_length + 1) + k], denom[k]);
                        int temp = max_log(temp0, temp1);
                        alpha[s * (block_length + 1) + k] = temp;
                        //denom[k] = max_log(alpha[s * (block_length + 1) + k], denom[k]);
                        denom[k] = max_log(temp, denom[k]);
                }

                // Normalization of alpha
                for (l = 0; l < N_STATES; l++)
                {
#pragma HLS DEPENDENCE array inter false
#pragma HLS PIPELINE
                        alpha[l * (block_length + 1) + k] -= denom[k];
                }
        }

        // Initiate beta
        loop5:for (i = 1; i < N_STATES; i++)
        {
                beta[i * (block_length + 1) + block_length] = -LOG_INFINITY;
        }
        beta[0 * (block_length + 1) + block_length] = 0.0;

        // Calculate beta going backward in the trellis
        loop6:for (k = block_length; k >= 2; k--)
        {
#pragma HLS LOOP_TRIPCOUNT max=6147
                for (s_prim = 0; s_prim < N_STATES; s_prim++)
                {
#pragma HLS DEPENDENCE array inter false
#pragma HLS PIPELINE
                        s0 = g_state_trans[s_prim * 2 + 0];
                        s1 = g_state_trans[s_prim * 2 + 1];
                        //      beta[s_prim * (block_length + 1) + k - 1] = com_log(beta[s0 * (block_length + 1) + k] + gamma[(2 * s_prim + 0) * (block_length + 1) + k], beta[s1 * (block_length + 1) + k] + gamma[(2 * s_prim + 1) * (block_length + 1) + k]);
                        beta[s_prim * (block_length + 1) + k - 1] = max_log(beta[s0 * (block_length + 1) + k] + gamma[(2 * s_prim + 0) * (block_length + 1) + k], beta[s1 * (block_length + 1) + k] + gamma[(2 * s_prim + 1) * (block_length + 1) + k]);
                }
                // Normalization of beta
                for (l = 0; l < N_STATES; l++)
                {
#pragma HLS PIPELINE
#pragma HLS DEPENDENCE array inter false
                        beta[l * (block_length + 1) + k - 1] -= denom[k];
                }
        }

        // Calculate extrinsic output for each bit
        loop7:for (k = 1; k <= block_length; k++)
        {
#pragma HLS DEPENDENCE array inter false
              	kk = k - 1;
                nom = -LOG_INFINITY;
                den = -LOG_INFINITY;
                for (s_prim = 0; s_prim < N_STATES; s_prim++)
                {
                        s0 = g_state_trans[s_prim * 2 + 0];
                        s1 = g_state_trans[s_prim * 2 + 1];
                        exp_temp0 = 0.0;
                        exp_temp1 = 0.0;
                        for (j = 0; j < (N_GENS - 1); j++)
                        {
#pragma HLS DEPENDENCE array inter false
#pragma HLS PIPELINE
#pragma HLS LOOP_FLATTEN
                                rp = recv_parity[kk * (N_GENS - 1) + j];
                                if (0 == g_output_parity[s_prim * (N_GENS - 1) * 2 + j * 2 + 0])
                                {
                                        exp_temp0 += rp;
                                }
                                else
                                {
                                        exp_temp0 -= rp;
                                }
                                if (0 == g_output_parity[s_prim * (N_GENS - 1) * 2 + j * 2 + 1])
                                {
                                        exp_temp1 += rp;
                                }
                                else
                                {
                                        exp_temp1 -= rp;
                                }
                        
                        }
			nom = max_log(nom, alpha[s_prim * (block_length + 1) + kk] + 0.5 * exp_temp0 + beta[s0 * (block_length + 1) + k]);
			den = max_log(den, alpha[s_prim * (block_length + 1) + kk] + 0.5 * exp_temp1 + beta[s1 * (block_length + 1) + k]);
               
                }
		extrinsic[kk] = nom - den;
        }
**************************************************************************************************
In the following code in the sub-function "internal_interleaver", apply "UNROLL" to the first loop because of fixed and not-large boundary. Apply "PIPELINE" to the second loop to reduce latency from 2 to 1.
**************************************************************************************************
    for(i = 0; i < TURBO_INT_K_TABLE_SIZE; i++)
    {
#pragma HLS UNROLL
        if(m == TURBO_INT_K_TABLE[i])
        {
            f1 = TURBO_INT_F1_TABLE[i];
            f2 = TURBO_INT_F2_TABLE[i];
            break;
        }
    }

    for(i = 0; i < m; i++)
    {
#pragma HLS PIPELINE
                if ((0 == f1) && (0 == f2))
                {
                        idx = i;
                }
                else
                {
                        idx = (((f1 % m) * (i % m)) % m + ((((f2 % m) * (i % m)) % m) * (i % m)) % m) % m;
                }
        out[i] = in[idx];
    }
**************************************************************************************************
