#include "model.hpp"
#include "math/trigonometry.hpp"
#include <cmath>

namespace ving
{
mat4 Model::mat()
{
    float pitch = rotate.x;
    float yaw = rotate.y;
    float roll = rotate.z;

    // float c1 = cos(radians(yaw)), c2 = cos(radians(pitch)), c3 = 1.0f;
    // float s1 = sin(radians(yaw)), s2 = sin(radians(pitch)), s3 = 0.0f;
    float c1 = cos(radians(yaw)), c2 = cos(radians(pitch)), c3 = cos(radians(roll));
    float s1 = sin(radians(yaw)), s2 = sin(radians(pitch)), s3 = sin(radians(roll));

    mat4 result{1.0f};
    // YXZ Rotation from wikipedia
    result[0][0] = scale * (c3 * c1 - s3 * s2 * s1);
    result[0][1] = s3 * c1 + c3 * s2 * s1;
    result[0][2] = -c2 * s1;
    result[1][0] = -s3 * c2;
    result[1][1] = scale * (c3 * c2);
    result[1][2] = s2;
    result[2][0] = c3 * s1 + s3 * s2 * c1;
    result[2][1] = s3 * s1 - c3 * s2 * c1;
    result[2][2] = scale * (c2 * c1);

    result[3] = vec4{position.x, position.y, position.z, 1.0f};

    return result;
}
} // namespace ving
