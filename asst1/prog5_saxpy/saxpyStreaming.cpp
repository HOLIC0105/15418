#include <smmintrin.h> // For _mm_stream_load_si128
#include <emmintrin.h> // For _mm_mul_ps
#include <assert.h>
#include <stdint.h>

extern void saxpySerial(int N,
			float scale,
			float X[],
			float Y[],
			float result[]);


void saxpyStreaming(int N,
                    float scale,
                    float X[],
                    float Y[],
                    float result[])
{
    // Replace this code with ones that make use of the streaming instructions
    __m128 valueScale = _mm_set1_ps(scale);
    for(int i = 0; i < N; i += 4) {
        *reinterpret_cast<__m128*>(result + i) = _mm_add_ps(_mm_mul_ps(*reinterpret_cast<__m128*>(X + i), valueScale), *reinterpret_cast<__m128*>(Y + i));
    }

}

