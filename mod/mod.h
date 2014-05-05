#ifndef __MODULATOR_H
#define __MODULATOR_H

#include "types.h"
#include <complex>

/*
  enum {
  BPSK = 0,
  QPSK,
  16QAM,
  64QAM,
  } MODULATION_METHOD;
*/

/*
  typedef enum {
  LOGMAP,
  APPROX
  } Soft_Method;
*/

class Modulator
{
 public:
	// default constructor
	Modulator() {};
	// class constructor
	Modulator(int32_T m);
	// class destructor
	virtual ~Modulator();

	virtual void modulate_bits(const uint8_T *bits, uint32_T n_bits, std::complex<float_point> *output, uint32_T &n_signals) const;
	virtual void demodulate_bits(const std::complex<float_point> *signals, uint32_T n_signals, uint8_T *bits, uint32_T &n_bits) const;
	/*
	  virtual void demodulate_soft_bits(const std::complex<float_point> *signals, uint32_T n_signals, uint8_T *soft_bits, uint32_T &n_soft_bits, float_point N0, Soft_Method soft_method = LOGMAP) const;
	*/
	virtual void demodulate_soft_bits(const std::complex<float_point> *signals, uint32_T n_signals, float_point *soft_bits, uint32_T &n_soft_bits, float_point N0) const;

	
	inline void dec2bits(int32_T i, uint32_T n, uint8_T *bvec) const;
	inline float_point eudist(float_point a, float_point b, float_point c, float_point d) const;
	float_point vec_min(float_point *vec, uint32_T n) const;
	

 protected:
	// number of modulation symbols
	int32_T M;
	// number of bits per modulation symbol
	int32_T k;
	// modulation decimal to complex table [M * 2]
	float_point *m_mod_table;
	// modulation decimal to bits [M * k]
	uint8_T *bitmap;
};

class BPSK : public Modulator
{
 public:
	// default constructor
	BPSK();
	// class constructor
	//	BPSK(int m);
	// destructor
	virtual ~BPSK();

	//	void modulate_bits(const uint8_T *bits, uint32_T n_bits, std::complex<float_point> *output, uint32_T &n_signals) const;
	//	void demodulate_bits(const std::complex<float_point> *signals, uint32_T n_signals, uint8_T *bits, uint32_T &n_bits) const;
	//	void demodulate_soft_bits(const std::complex<float_point> *signals, uint32_T n_signals, uint8_T *soft_bits, uint32_T &n_soft_bits, float_point N0, Soft_Method soft_method = LOGMAP) const;

 private:
	float_point *m_BPSK_table;
};


class QPSK : public Modulator
{
 public:
	// default constructor
	QPSK();
	// class constructor
	//	QPSK(uint32_T m);
	// destructor
	virtual ~QPSK();

	//	void modulate_bits(const uint8_T *bits, uint32_T n_bits, std::complex<float_point> *output, uint32_T &n_signals) const;
	//	void demodulate_bits(const std::complex<float_point> *signals, uint32_T n_signals, uint8_T *bits, uint32_T &n_bits) const;
	//	void demodulate_soft_bits(const std::complex<float_point> *signals, uint32_T n_signals, uint8_T *soft_bits, uint32_T &n_soft_bits, float_point N0, Soft_Method soft_method = LOGMAP) const;

 private:
	float_point *m_QPSK_table;
};

#endif
