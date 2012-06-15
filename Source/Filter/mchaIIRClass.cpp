/*  
	MCHA - Multichannel Audio Playback and Recording Library
    Copyright (C) 2011  Roman Kosobrodov

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/

#include "mchaIIRClass.h"
#include "fftw3.h"

namespace mcha
{

/* ---------------------------------------------------------------------- */
bool	MchaIIRClass::init(const float* taps, const size_t order)
{
	if ((taps == nullptr)||(order < 1))
		return false;
		
	clearObjects();

	N = order;

	if ( !allocBuffer(a, N) )		return false;
	if ( !allocBuffer(b, N+1) )		return false;
	if ( !allocBuffer(x, 2*N) )		return false;
	if ( !allocBuffer(y, 2*N) )		return false;
	
	/* copy N coefficients a and N+1 coefficients b */
	memcpy(b, taps, (N+1)*sizeof(float));
	memcpy(a, taps+N+2, N*sizeof(float));

	/* normalise by a(0) if required */
	if ( !norm(a, N, taps[N+1]) )	return false;
	if ( !norm(b, N+1, taps[N+1]) )	return false;

	//mcha = MchaRecordPlayer::getInstance();

	return true;
};

/* ---------------------------------------------------------------------- */
void	MchaIIRClass::process(const float* src, const size_t len, float* dst)
{
	L = (N <= len) ? N : len; 
	size_t bytesToCopy = L*sizeof(float);

	/* copy N samples from the source to the state variable x */
	memcpy( x+N, src, bytesToCopy );
	
	/* process first N samples using input and output states saved from the previous call */
	for (size_t i = 0; i < L; ++i)
	{
		y[i+N] = muladd( x+i, b, N+1 ) - muladd( y+i, a, N );
	}

	/* copy first L output samples to the destination buffer */ 
	memcpy(dst, y+N, bytesToCopy);

	/* process remaining len-N samples using source and destination buffers */
	for (size_t i = L; i<len; ++i)
	{
		dst[i] = muladd( src+i-N, b, N+1 ) - muladd( dst+i-N, a, N);
	}

	/* update the state buffers */
	if (L==N)
	{
		memcpy(x, src + len - L, bytesToCopy);
		memcpy(y, dst + len - L, bytesToCopy);
	}
	else
	{
		memcpy( x, x + L, N*sizeof(float) );
		memcpy( y, y + L, N*sizeof(float) );		
	}
};


/* ---------------------------------------------------------------------- */
float	MchaIIRClass::muladd(const float* dat, const float* coef, const size_t len)
{
	float	res(0.0f);
	size_t	i(0);
	size_t	j(len-1);

	while (i<len)
		res += dat[j--] * coef[i++];
	
	return res;
}

/* ---------------------------------------------------------------------- */
bool	MchaIIRClass::norm(float* dat, const size_t len, const float coef)
{
	if (coef == 1.0f)	/* do nothing */
		return true;

	if (coef == 0.0f)	/* wrong input */
		return false;

	for(size_t n=0; n<len; ++n)
	{
		dat[n]  = dat[n] / coef;
	}

	return true;
}


/* ---------------------------------------------------------------------- */
void MchaIIRClass::clearObjects()
{ 
	fftwf_free(a);
	fftwf_free(b);
	fftwf_free(x);
	fftwf_free(y);
	N = 0;
}

/* ---------------------------------------------------------------------- */
bool MchaIIRClass::allocBuffer(float* &buf, size_t len)
{
	try
	{
		buf = fftwf_alloc_real(len);
	}
	catch (std::bad_alloc)
	{
		return false;
	}

	memset( buf, 0, len*sizeof(float) );

	return true;
}

}