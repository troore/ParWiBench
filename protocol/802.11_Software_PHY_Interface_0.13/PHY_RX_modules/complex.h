#ifndef _COMPLEX_
#define _COMPLEX_


typedef struct Complex
{
	float real;
	float imag;
}Complex;

Complex Complex_init(float a, float b)
{
	Complex ans;
	ans.real = a;
	ans.imag = b;
	return ans;
}

Complex Complex_add(Complex a, Complex b)
{
	Complex ans;
	ans.real = a.real + b.real;
	ans.imag = a.imag + b.imag;
	return ans;
}

Complex Complex_mul(Complex a, Complex b)
{
	Complex ans;
	ans.real = a.real*b.real - a.imag*b.imag;
	ans.imag = a.imag*b.real + a.real*b.imag;
	return ans;
}

Complex Complex_mul_const(Complex a, float b)
{
	Complex ans;
	ans.real = a.real*b;
	ans.imag = a.imag*b;
	return ans;
}

Complex Complex_sub(Complex a, Complex b)
{
	Complex ans;
	ans.real = a.real - b.real;
	ans.imag = a.imag - b.imag;
	return ans;
}

Complex Complex_div(Complex a, Complex b)
{
	Complex ans;
	float t;
	t = b.real*b.real+b.imag*b.imag;
	ans.real = (a.real*b.real+a.imag*b.imag)/t;
	ans.imag = (a.imag*b.real-a.real*b.imag)/t;
	return ans;
}

Complex Complex_conj(Complex a)
{
	return Complex_init(a.real, 0.0-a.imag); 
}

Complex Complex_mul_conj(Complex a, Complex b)
{
	return Complex_mul(a, Complex_conj(b));
}

void Print_complex_6(Complex a)
{
	printf("%.6f", a.real);
	if(a.imag >= 0)
		printf("+");
	printf("%.6f\n", a.imag);
}

void Print_complex_3(Complex a)
{
	printf("%.3f", a.real);
	if(a.imag >= 0)
		printf("+");
	printf("%.3f\n", a.imag);
}


float Complex_angle(Complex x)
{
	Complex a;
	a=x;
	return atan(a.imag/a.real);
}

Complex Complex_expi(float x)
{
	return Complex_init(cos(x), sin(x)); 
} 

#endif
