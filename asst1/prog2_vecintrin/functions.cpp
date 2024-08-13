#include <stdio.h>
#include <algorithm>
#include <math.h>
#include "CMU418intrin.h"
#include "logger.h"
using namespace std;


void absSerial(float* values, float* output, int N) {
    for (int i=0; i<N; i++) {
	float x = values[i];
	if (x < 0) {
	    output[i] = -x;
	} else {
	    output[i] = x;
	}
    }
}

// implementation of absolute value using 15418 instrinsics
void absVector(float* values, float* output, int N) {
    __cmu418_vec_float x;
    __cmu418_vec_float result;
    __cmu418_vec_float zero = _cmu418_vset_float(0.f);
    __cmu418_mask maskAll, maskIsNegative, maskIsNotNegative;

    //  Note: Take a careful look at this loop indexing.  This example
    //  code is not guaranteed to work when (N % VECTOR_WIDTH) != 0.
    //  Why is that the case?
    for (int i=0; i<N; i+=VECTOR_WIDTH) {

	// All ones
	maskAll = _cmu418_init_ones();

	// All zeros
	maskIsNegative = _cmu418_init_ones(0);

	// Load vector of values from contiguous memory addresses
	_cmu418_vload_float(x, values+i, maskAll);               // x = values[i];

	// Set mask according to predicate
	_cmu418_vlt_float(maskIsNegative, x, zero, maskAll);     // if (x < 0) {

	// Execute instruction using mask ("if" clause)
	_cmu418_vsub_float(result, zero, x, maskIsNegative);      //   output[i] = -x;

	// Inverse maskIsNegative to generate "else" mask
	maskIsNotNegative = _cmu418_mask_not(maskIsNegative);     // } else {

	// Execute instruction ("else" clause)
	_cmu418_vload_float(result, values+i, maskIsNotNegative); //   output[i] = x; }

	// Write results back to memory
	_cmu418_vstore_float(output+i, result, maskAll);
    }
}

// Accepts an array of values and an array of exponents
// For each element, compute values[i]^exponents[i] and clamp value to
// 4.18.  Store result in outputs.
// Uses iterative squaring, so that total iterations is proportional
// to the log_2 of the exponent
void clampedExpSerial(float* values, int* exponents, float* output, int N) {
    for (int i=0; i<N; i++) {
	float x = values[i];
	float result = 1.f;
	int y = exponents[i];
	float xpower = x;
	while (y > 0) {
	    if (y & 0x1) {
			result *= xpower;
		}
	    xpower = xpower * xpower;
	    y >>= 1;
	}
	if (result > 4.18f) {
	    result = 4.18f;
	}
	output[i] = result;
    }
}

void clampedExpVector(float* values, int* exponents, float* output, int N) {
    // Implement your vectorized version of clampedExpSerial here
    //  ...
	__cmu418_vec_float valuesVector;
	__cmu418_vec_float resultVector;
	__cmu418_vec_float maxResultVector;
	__cmu418_vec_int exponentsVector;
	__cmu418_vec_int oneAll;
	__cmu418_vec_int zeroAll;
	__cmu418_vec_int exponentsFlag;
	__cmu418_mask maskResult;
	__cmu418_mask maskExponents;
	__cmu418_mask maskZero;
	__cmu418_mask maskAll = _cmu418_init_ones();

	_cmu418_vset_int(oneAll, 1, maskAll);
	_cmu418_vset_int(zeroAll, 0, maskAll);
	_cmu418_vset_float(maxResultVector, 4.18f, maskAll);

	for(int i = 0; i < N; i += VECTOR_WIDTH) {

		if (i + VECTOR_WIDTH > N) {
			maskZero = maskResult = maskExponents = maskAll = _cmu418_init_ones(N - i);
		}

		_cmu418_vset_float(resultVector, 1, maskAll);  //result = 1;
		
		_cmu418_vload_float(valuesVector, values + i, maskAll);
		_cmu418_vload_int(exponentsVector, exponents + i, maskAll);

		auto check = [&exponentsVector, &maskZero, &zeroAll, &maskAll] {
			_cmu418_vgt_int(maskZero, exponentsVector, zeroAll, maskAll);
			return (_cmu418_cntbits(maskZero) > 0);
		};

		while(check()) {	

			_cmu418_vbitand_int(exponentsFlag, exponentsVector, oneAll, maskAll); 
			_cmu418_veq_int(maskExponents, exponentsFlag, oneAll, maskAll);

			_cmu418_vmult_float(resultVector, resultVector, valuesVector, maskExponents); // if(y & 1) result *= xpower
			
			_cmu418_vmult_float(valuesVector, valuesVector, valuesVector, maskAll); //xpower = xpower * xpower;
 
			_cmu418_vshiftright_int(exponentsVector, exponentsVector, oneAll, maskAll); //y >>= 1
			
		}

		_cmu418_vgt_float(maskResult, resultVector, maxResultVector, maskAll); //if (result > 4.18f)

		_cmu418_vset_float(resultVector, 4.18f, maskResult);  //result = 4.18f

		_cmu418_vstore_float(output + i, resultVector, maskAll);

	}
}


float arraySumSerial(float* values, int N) {
    float sum = 0;
    for (int i=0; i<N; i++) {
	sum += values[i];
    }

    return sum;
}

// Assume N % VECTOR_WIDTH == 0
// Assume VECTOR_WIDTH is a power of 2
float arraySumVector(float* values, int N) {
    // Implement your vectorized version here
    //  ...

	__cmu418_vec_float valuesVector;
	__cmu418_vec_float resultVector;
	__cmu418_mask maskAll = _cmu418_init_ones();

	_cmu418_vset_float(resultVector, 0, maskAll);

	for(int i = 0; i < N; i += VECTOR_WIDTH) {
		_cmu418_vload_float(valuesVector, values + i, maskAll);
		_cmu418_vadd_float(resultVector, resultVector, valuesVector, maskAll);
	}

	for(int i = 1; i < VECTOR_WIDTH; i <<= 1) {
		_cmu418_interleave_float(resultVector, resultVector);
		_cmu418_hadd_float(resultVector, resultVector);
	}
	return resultVector.value[0];
}
