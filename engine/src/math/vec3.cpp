#include "math/vec3.hpp"

namespace ving
{
void vec3::operator+=(vec3 b)
{
    *this = *this + b;
}

void vec3::operator-=(vec3 b)
{
    *this = *this - b;
}
} // namespace ving
