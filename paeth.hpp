#include <cmath>

#ifdef __SSE4_1__
    #include <emmintrin.h>
    #include <smmintrin.h>
    #include <tmmintrin.h>
    // for libpng
    #define PNG_INTEL_SSE_IMPLEMENTATION 3
#endif // __SSE4_1__

inline uint8_t paeth_simple(uint8_t a, uint8_t b, uint8_t c)  {
    int16_t p, pa, pb, pc;
    p = a + b - c;
    pa = abs(p - a);
    pb = abs(p - b);
    pc = abs(p - c);
    if (pa <= pb && pa <= pc)
        return a;
    else if (pb <= pc)
        return b;
    else
        return c;
}

#ifdef __SSE4_1__
// from libpng
static __m128i abs_i16(__m128i x) {
#if PNG_INTEL_SSE_IMPLEMENTATION >= 2
   return _mm_abs_epi16(x);
#else
   /* Read this all as, return x<0 ? -x : x.
   * To negate two's complement, you flip all the bits then add 1.
    */
   __m128i is_negative = _mm_cmplt_epi16(x, _mm_setzero_si128());

   /* Flip negative lanes. */
   x = _mm_xor_si128(x, is_negative);

   /* +1 to negative lanes, else +0. */
   x = _mm_sub_epi16(x, is_negative);
   return x;
#endif
}

// from libpng
static __m128i if_then_else(__m128i c, __m128i t, __m128i e) {
#if PNG_INTEL_SSE_IMPLEMENTATION >= 3
   return _mm_blendv_epi8(e,t,c);
#else
   return _mm_or_si128(_mm_and_si128(c, t), _mm_andnot_si128(c, e));
#endif
}

inline void paeth_before(uint8_t a_[8], uint8_t b_[8], uint8_t c_[8], uint8_t ans[8])
{
    __m128i zero, a, b, c, p, pa, pb, pc, smallest, nearest;

    zero = _mm_setzero_si128();

    // uint8_tの配列をロードして16ビット整数に拡張
    a = _mm_unpacklo_epi8(_mm_loadl_epi64(reinterpret_cast<const __m128i*>(a_)), zero);
    b = _mm_unpacklo_epi8(_mm_loadl_epi64(reinterpret_cast<const __m128i*>(b_)), zero);
    c = _mm_unpacklo_epi8(_mm_loadl_epi64(reinterpret_cast<const __m128i*>(c_)), zero);

    /* (p-a) == (a+b-c - a) == (b-c) */
    pa = _mm_sub_epi16(b,c);

    /* (p-b) == (a+b-c - b) == (a-c) */
    pb = _mm_sub_epi16(a,c);

    /* (p-c) == (a+b-c - c) == (a+b-c-c) == (b-c)+(a-c) */
    pc = _mm_add_epi16(pa,pb);

    pa = abs_i16(pa);  /* |p-a| */
    pb = abs_i16(pb);  /* |p-b| */
    pc = abs_i16(pc);  /* |p-c| */

    smallest = _mm_min_epi16(pc, _mm_min_epi16(pa, pb));

    /* Paeth breaks ties favoring a over b over c. */
    nearest  = if_then_else(_mm_cmpeq_epi16(smallest, pa), a,
               if_then_else(_mm_cmpeq_epi16(smallest, pb), b,
                                                           c));

    // 結果をuint8_tに縮小してストア
    __m128i ans8 = _mm_packus_epi16(nearest, zero);    // 16ビットから8ビットに縮小
    _mm_storel_epi64(reinterpret_cast<__m128i*>(ans), ans8); // 結果をメモリにストア
}

inline void paeth_after(uint8_t a_[8], uint8_t b_[8], uint8_t c_[8], uint8_t ans[8])
{
    __m128i zero, a, b, c, b_c, a_c, pa, pb, pc, mask_pa_gt_pb, mask_pa_gt_pc, mask_pb_gt_pc, mask1, result;

    zero = _mm_setzero_si128();

    // uint8_tの配列をロードして16ビット整数に拡張
    a = _mm_unpacklo_epi8(_mm_loadl_epi64(reinterpret_cast<const __m128i*>(a_)), zero);
    b = _mm_unpacklo_epi8(_mm_loadl_epi64(reinterpret_cast<const __m128i*>(b_)), zero);
    c = _mm_unpacklo_epi8(_mm_loadl_epi64(reinterpret_cast<const __m128i*>(c_)), zero);

    // p = a + b - c
    // pa = abs(p - a), pb = abs(p - b), pc = abs(p - c)
    // so, pa = abs(b - c), pb = abs(a - c), pc = abs(pa + pb)
    b_c = _mm_sub_epi16(b, c);
    a_c = _mm_sub_epi16(a, c);

    pa = abs_i16(b_c);
    pb = abs_i16(a_c);
    pc = abs_i16(_mm_add_epi16(b_c, a_c));

    // 条件マスクの計算
    mask_pa_gt_pb = _mm_cmpgt_epi16(pa, pb);
    mask_pa_gt_pc = _mm_cmpgt_epi16(pa, pc);
    mask_pb_gt_pc = _mm_cmpgt_epi16(pb, pc);

    mask1 = _mm_or_si128(mask_pa_gt_pb, mask_pa_gt_pc); // pa > pb || pa > pc
    result = if_then_else(mask_pb_gt_pc, c, b);
    result = if_then_else(mask1, result, a);

    // 結果をuint8_tに縮小してストア
    __m128i ans8 = _mm_packus_epi16(result, zero);    // 16ビットから8ビットに縮小
    _mm_storel_epi64(reinterpret_cast<__m128i*>(ans), ans8); // 結果をメモリにストア
}
#endif // __SSE4_1__
