#ifndef MCHA_IIR_CLASS_H
#define MCHA_IIR_CLASS_H

#include <new>
#include <memory.h>

namespace mcha
{

using namespace std;

class MchaIIRClass
{
public:
	MchaIIRClass(): a(nullptr),
					b(nullptr),
					x(nullptr),
					y(nullptr),
					N(0),
					L(0)
	{ };

	virtual ~MchaIIRClass()
	{
		clearObjects();
	};

	bool	init(const float* taps, const size_t order);
	void	process(const float* src, const size_t len, float* dst);

private:
	float*	a;
	float*	b;
	float*	x;
	float*	y;
	size_t	N;
	size_t	L;

	void	clearObjects();
	bool	allocBuffer(float* &buf, size_t len);
	float	muladd(const float* src1, const float* src2, const size_t len);
	bool	norm(float* dat, const size_t len, const float coef);

	//MchaRecordPlayer* mcha;
};

}

#endif