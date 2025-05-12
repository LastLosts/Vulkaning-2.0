#include "math/vec_functions.hpp"
#include <cmath>

#ifdef SIMD_CROSS
#include "math/vec4.hpp"
#endif

namespace ving
{
float dot(vec4 a, vec4 b)
{
    return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
}
vec3 cross(vec3 x, vec3 y)
{
    // Might be faster but i didn't bench it so i can't tell
    // also it doesn't produce right results
#ifdef SIMD_CROSS
    float aa[4]{x.x, x.y, x.z, x.z};
    float bb[4]{y.x, y.y, y.z, y.z};
    float result[4];

    // https://geometrian.com/resources/cross_product/
    __m128 vec0 = _mm_load_ps(aa);
    __m128 vec1 = _mm_load_ps(bb);

    __m128 tmp0 = _mm_shuffle_ps(vec0, vec0, _MM_SHUFFLE(3, 0, 2, 1));
    __m128 tmp1 = _mm_shuffle_ps(vec1, vec1, _MM_SHUFFLE(3, 1, 0, 2));
    __m128 tmp2 = _mm_mul_ps(tmp0, vec1);
    __m128 tmp3 = _mm_mul_ps(tmp0, tmp1);
    __m128 tmp4 = _mm_shuffle_ps(tmp2, tmp2, _MM_SHUFFLE(3, 0, 2, 1));

    __m128 tmp5 = _mm_sub_ps(tmp3, tmp4);

    _mm_store_ps(result, tmp5);

    return {result[0], result[1], result[2]};
#else
    vec3 result;
    result.x = x.y * y.z - y.y * x.z;
    result.y = x.z * y.x - y.z * x.x;
    result.z = x.x * y.y - y.x * x.y;
    return result;
#endif
}
vec2 normalize(vec2 v)
{
    float len = length(v);
    return vec2{};
}
vec3 normalize(vec3 v)
{
    float len = sqrt(v.x * v.x + v.y * v.y + v.z * v.z);

    return vec3{v.x / len, v.y / len, v.z / len};
}
float length(vec2 v)
{
    return sqrt(v.x * v.x + v.y * v.y);
}

} // namespace ving
