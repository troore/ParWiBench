
void map_decoder(float *recv_syst, float *recv_parity, float *apriori, float *extrinsic, int interleaver_size)
{
	float gamma_k_e, nom, den, temp0, temp1, exp_temp0, exp_temp1;
	int i, j, s0, s1, k, kk, s, s_prim, s_prim0, s_prim1;
	int block_length = (interleaver_size + N_TAIL);
	float alpha[N_STATES * (BLOCK_SIZE + N_TAIL + 1)];
	float beta[N_STATES * (BLOCK_SIZE + N_TAIL + 1)];
	float gamma[N_STATES * 2 * (BLOCK_SIZE + N_TAIL + 1)];
	float denom[BLOCK_SIZE + N_TAIL + 1];
	//Calculate gamma
	// std::cout << "gamma" << std::endl;
	// std::cout << block_length << std::endl;
	for (k = 1; k <= block_length; k++)
	{
		// std::cout << "k=" << k << std::endl;
		kk = k - 1;
		for (s_prim = 0; s_prim < N_STATES; s_prim++)
		{
			exp_temp0 = 0.0;
			exp_temp1 = 0.0;
			for (j = 0; j < (N_GENS - 1); j++)
			{
				exp_temp0 += 0.5 * Lc * recv_parity[kk * (N_GENS - 1) + j] * (float)(1 - 2 * g_output_parity[s_prim * (N_GENS - 1) * 2 + j * 2 + 0]); /* Is this OK? */
				exp_temp1 += 0.5 * Lc * recv_parity[kk * (N_GENS - 1) + j] * (float)(1 - 2 * g_output_parity[s_prim * (N_GENS - 1) * 2 + j * 2 + 1]);
			}
			// gamma(2*s_prim+0,k) = std::exp( 0.5*(extrinsic_input(kk) + Lc*rec_systematic(kk))) * std::exp( exp_temp0 );
			// gamma(2*s_prim+1,k) = std::exp(-0.5*(extrinsic_input(kk) + Lc*rec_systematic(kk))) * std::exp( exp_temp1 );
			std::cout << "s'=" << s_prim << std::endl;
			std::cout << "apriori[k]=" << apriori[kk] << std::endl;
			gamma[(2 * s_prim + 0) * (block_length + 1) + k] = exp(0.5 * (apriori[kk] + Lc * recv_syst[kk]) + exp_temp0);
			std::cout << "(s',0)\t" << gamma[(2 * s_prim + 0) * (block_length + 1) + k] << "\t";
			gamma[(2 * s_prim + 1) * (block_length + 1) + k] = exp(-0.5 * (apriori[kk] + Lc * recv_syst[kk]) + exp_temp1);
			std::cout << "(s',1)\t" << gamma[(2 * s_prim + 1) * (block_length + 1) + k] << "\n";
		}
	}
	// initiate alpha
	// alpha.set_col(0, zeros(Nstates));
	for (int i = 0; i < N_STATES; i++)
	{
		alpha[i * (block_length + 1) + 0] = 0.0;
	}
	alpha[0 * (block_length + 1) + 0] = 1.0;
	memset(denom, 0, sizeof(float) * (block_length + 1));
	//Calculate alpha and denom going forward through the trellis
	// std::cout << "alpha" << std::endl;
	for (k = 1; k <= block_length; k++)
	{
		// std::cout << "k=" << k << std::endl;
		for (s = 0; s < N_STATES; s++)
		{
			// std::cout << s << std::endl;
			s_prim0 = g_rev_state_trans[s * 2 + 0];
			s_prim1 = g_rev_state_trans[s * 2 + 1];
			// std::cout << s_prim0 << "\t" << s_prim1 << std::endl;
			temp0 = alpha[s_prim0 * (block_length + 1) + k - 1] * gamma[(2 * s_prim0 + 0) * (block_length + 1) + k];
			temp1 = alpha[s_prim1 * (block_length + 1) + k - 1] * gamma[(2 * s_prim1 + 1) * (block_length + 1) + k];
			alpha[s * (block_length + 1) + k] = temp0 + temp1;
			// std::cout << alpha[s * (block_length + 1) + k] << std::endl;
			denom[k] += temp0 + temp1;
		}
		// std::cout << "denom[" << k << "]=" << denom[k] << std::endl;
		// alpha.set_col(k, alpha.get_col(k) / denom(k));
		for (s = 0; s < N_STATES; s++)
		{
			alpha[s * (block_length + 1) + k] /= denom[k];
		}
	}
	// initiate beta
	// beta.set_col(block_length, zeros(Nstates));
	// beta(0, block_length) = 1.0;
	for (int i = 0; i < N_STATES; i++)
	{
		beta[i * (block_length + 1) + block_length] = 0.0;
	}
	beta[0 * (block_length + 1) + block_length] = 1.0;
	//Calculate beta going backward in the trellis
	// std::cout << "beta" << std::endl;
	for (k = block_length; k >= 2; k--)
	{
		// std::cout << "k=" << k << std::endl;
		for (s_prim = 0; s_prim < N_STATES; s_prim++)
		{
			s0 = g_state_trans[s_prim * 2 + 0];
			s1 = g_state_trans[s_prim * 2 + 1];
			temp0 = beta[s0 * (block_length + 1) + k] * gamma[(2 * s_prim + 0) * (block_length + 1) + k];
			temp1 = beta[s1 * (block_length + 1) + k] * gamma[(2 * s_prim + 1) * (block_length + 1) + k];
			beta[s_prim * (block_length + 1) + k - 1] = temp0 + temp1;
			// std::cout << beta[s_prim * (block_length + 1) + k - 1] << std::endl;
		}
		// beta.set_col(k - 1, beta.get_col(k - 1) / denom(k));
		for (s_prim = 0; s_prim < N_STATES; s_prim++)
		{
			beta[s_prim * (block_length + 1) + k - 1] /= denom[k];
		}
	}
	//Calculate extrinsic output for each bit
	// std::cout << "extrinsic" << std::endl;
	for (k = 1; k <= block_length; k++)
	{
		// std::cout << "k=" << k << std::endl;
		kk = k - 1;
		nom = 0.0;
		den = 0.0;
		for (s_prim = 0; s_prim < N_STATES; s_prim++)
		{
			// std::cout << s_prim << std::endl;
			s0 = g_state_trans[s_prim * 2 + 0];
			s1 = g_state_trans[s_prim * 2 + 1];
			// std::cout << s0 << "\t" << s1 << std::endl;
			exp_temp0 = 0.0;
			exp_temp1 = 0.0;
			for (j = 0; j < (N_GENS - 1); j++)
			{
				exp_temp0 += 0.5 * Lc * recv_parity[kk * (N_GENS - 1) + j] * (float)(1 - 2 * g_output_parity[s_prim * (N_GENS - 1) * 2 + j * 2 + 0]);
				exp_temp1 += 0.5 * Lc * recv_parity[kk * (N_GENS - 1) + j] * (float)(1 - 2 * g_output_parity[s_prim * (N_GENS - 1) * 2 + j * 2 + 1]);
			}
			// gamma_k_e = std::exp( exp_temp0 );
			gamma_k_e = exp(exp_temp0);
			// std::cout << alpha[s_prim * (block_length + 1) + k - 1] << "\t" << gamma_k_e << "\t" << beta[s0 * (block_length + 1) + k] << std::endl;
			nom += alpha[s_prim * (block_length + 1) + k - 1] * gamma_k_e * beta[s0 * (block_length + 1) + k];
			// gamma_k_e = std::exp( exp_temp1 );
			gamma_k_e = exp(exp_temp1);
			// std::cout << alpha[s_prim * (block_length + 1) + k - 1] << "\t" << gamma_k_e << "\t" << beta[s1 * (block_length + 1) + k] << std::endl;
			den += alpha[s_prim * (block_length + 1) + k - 1] * gamma_k_e * beta[s1 * (block_length + 1) + k];
		}
		// extrinsic_output(kk) = std::log(nom/den);
		// std::cout << "nom=" << nom << "\t" << "den=" << den << std::endl;
		extrinsic[kk] = log(nom / den);
		// std::cout << extrinsic[kk] << std::endl;
	}
}
