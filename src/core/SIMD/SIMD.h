#ifndef _SIMD_H_
#define _SIMD_H_

#include "../renderer.h"

namespace GGUI{

    #if defined(__AVX512F__)
        #include <immintrin.h>

        inline const unsigned int MAX_SIMD_SIZE = 16;
    #elif defined(__AVX__)
        #include <immintrin.h>

        inline const unsigned int MAX_SIMD_SIZE = 8;
    #elif defined(__SSE__)
        #include <xmmintrin.h>

        inline const unsigned int MAX_SIMD_SIZE = 4;
    #else
        inline const unsigned int MAX_SIMD_SIZE = 1;
    #endif

    // The number represents how many 32 bit float value pairs can it calculate at the same time.
    void simdDivision4(float* a, float* b, float* c);
    void simdDivision8(float* a, float* b, float* c);
    void simdDivision16(float* a, float* b, float* c);

    // Calls the right division SIMD operator depending on the length
    void operateSIMDDivision(float* dividend, float* divider, float* result, int length){
        if(length == 4){
            simdDivision4(dividend, divider, result);
        }else if(length == 8){
            simdDivision8(dividend, divider, result);
        }else if(length == 16){
            simdDivision16(dividend, divider, result);
        }else{
            INTERNAL::reportStack("Calling SIMD division with longer sequence than allowed: " + std::to_string(length) + " elements.");
        }
    }

    void operateSIMDModulo(float* dividend, float* divider, float* result, int length){
        // Uses the division variants and then calculates for each the modulo
        operateSIMDDivision(dividend, divider, result, length);

        for(int i = 0; i < length; i++){
            // by the formula: a - b * floor(a / b)
            result[i] = dividend[i] - divider[i] * floor(result[i]);
        }
    }

    #if defined(__SSE__)
        void simdDivision4(float* a, float* b, float* c) {
            __m128 va = _mm_loadu_ps(a);
            __m128 vb = _mm_loadu_ps(b);
            __m128 vc = _mm_div_ps(va, vb);
            _mm_storeu_ps(c, vc);
        }
    #else
        void simdDivision4(float* a, float* b, float* c) {
            *c = *a / *b;
            *(c + 1) = *(a + 1) / *(b + 1);
            *(c + 2) = *(a + 2) / *(b + 2);
            *(c + 3) = *(a + 3) / *(b + 3);
        }
    #endif

    #if defined(__AVX__)
        void simdDivision8(float* a, float* b, float* c) {
            __m256 va = _mm256_loadu_ps(a);
            __m256 vb = _mm256_loadu_ps(b);
            __m256 vc = _mm256_div_ps(va, vb);
            _mm256_storeu_ps(c, vc);
        }
    #else
        void simdDivision8(float* a, float* b, float* c) {
            // use the one stage lower SIMD function variant.
            simdDivision4(a, b, c);
            simdDivision4(a + 4, b + 4, c + 4);
        }
    #endif

    #if defined(__AVX512F__)
        void simdDivision16(float* a, float* b, float* c) {
            __m512 va = _mm512_loadu_ps(a);
            __m512 vb = _mm512_loadu_ps(b);
            __m512 vc = _mm512_div_ps(va, vb);
            _mm512_storeu_ps(c, vc);
        }
    #else
        void simdDivision16(float* a, float* b, float* c) {
            // use the one stage lower SIMD function variant.
            simdDivision8(a, b, c);
            simdDivision8(a + 8, b + 8, c + 8);
        }
    #endif
}

#endif