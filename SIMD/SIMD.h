#ifndef _SIMD_H_
#define _SIMD_H_

#include "../Renderer.h"

namespace GGUI{

    #if defined(__AVX__)
        #include <immintrin.h>

        inline const unsigned int MAX_SIMD_SIZE = 8;
    #elif defined(__SSE2__)
        #include <emmintrin.h>

        inline const unsigned int MAX_SIMD_SIZE = 4;
    #elif defined(__SSE__)
        #include <xmmintrin.h>

        inline const unsigned int MAX_SIMD_SIZE = 2;
    #else
        inline const unsigned int MAX_SIMD_SIZE = 1;
    #endif

    // The number represents how many 32 bit float value pairs can it calculate at the same time.
    void simd_division_2(float* a, float* b, float* c);
    void simd_division_4(float* a, float* b, float* c);
    void simd_division_8(float* a, float* b, float* c);

    // Calls the right division SIMD operator depending on the length
    void Operate_SIMD_Division(float* dividend, float* divider, float* result, int length){
        if(length == 2){
            simd_division_2(dividend, divider, result);
        }else if(length == 4){
            simd_division_4(dividend, divider, result);
        }else if(length == 8){
            simd_division_8(dividend, divider, result);
        }else{
            Report_Stack("Calling SIMD division with longer sequence than allowed: " + std::to_string(length) + " elements.");
        }
    }

    void Operate_SIMD_Modulo(float* dividend, float* divider, float* result, int length){
        // Uses the division variants and then calculates for each the modulo
        Operate_SIMD_Division(dividend, divider, result, length);

        for(int i = 0; i < length; i++){
            // by the formula: a - b * floor(a / b)
            result[i] = dividend[i] - divider[i] * floor(result[i]);
        }
    }

    #if defined(__SSE__)
        void simd_division_2(float* a, float* b, float* c) {
            __m128 va = _mm_loadu_ps(a);
            __m128 vb = _mm_loadu_ps(b);
            __m128 vc = _mm_div_ps(va, vb);
            _mm_storeu_ps(c, vc);
        }
    #else
        void simd_division_2(float* a, float* b, float* c) {
            *c = *a / *b;
            *(c + 1) = *(a + 1) / *(b + 1);
        }
    #endif

    #if defined(__SSE2__)
        void simd_division_4(float* a, float* b, float* c) {
            __m128 va = _mm_loadu_ps(a);
            __m128 vb = _mm_loadu_ps(b);
            __m128 vc = _mm_div_ps(va, vb);
            _mm_storeu_ps(c, vc);
        }
    #else
        void simd_division_4(float* a, float* b, float* c) {
            // use the one stage lower SIMD function variant.
            simd_division_2(a, b, c);
            simd_division_2(a + 2, b + 2, c + 2);
        }
    #endif

    #if defined(__AVX__)
        void simd_division_8(float* a, float* b, float* c) {
            __m256 va = _mm256_loadu_ps(a);
            __m256 vb = _mm256_loadu_ps(b);
            __m256 vc = _mm256_div_ps(va, vb);
            _mm256_storeu_ps(c, vc);
        }
    #else
        void simd_division_8(float* a, float* b, float* c) {
            // use the one stage lower SIMD function variant.
            simd_division_4(a, b, c);
            simd_division_4(a + 4, b + 4, c + 4);
        }
    #endif
}

#endif