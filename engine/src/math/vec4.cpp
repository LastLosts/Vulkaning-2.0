#include "math/vec4.hpp"

#include <cassert>

namespace ving
{
float &vec4::operator[](int i)
{
    assert(i < 4 && i > -1);
    switch (i)
    {
    default:
    case 0:
        return x;
    case 1:
        return y;
    case 2:
        return z;
    case 3:
        return w;
    }
}
const float &vec4::operator[](int i) const
{
    assert(i < 4 && i > -1);
    switch (i)
    {
    default:
    case 0:
        return x;
    case 1:
        return y;
    case 2:
        return z;
    case 3:
        return w;
    }
}
} // namespace ving
