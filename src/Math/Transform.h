#ifndef __PBR_TRANSFORM_H__
#define __PBR_TRANSFORM_H__

#include <PBR.h>

namespace pbr {
namespace math {

class Vector3;
class Matrix4x4;

PBR_SHARED Matrix4x4 translation(const Vector3& tr);
PBR_SHARED Matrix4x4 skewX(float m);
PBR_SHARED Matrix4x4 scale(float x, float y, float z);
PBR_SHARED Matrix4x4 scale(const Vector3& scale);
PBR_SHARED Matrix4x4 rotationX(float rads);
PBR_SHARED Matrix4x4 rotationY(float rads);
PBR_SHARED Matrix4x4 rotationZ(float rads);
PBR_SHARED Matrix4x4 rotationAxis(float rads, const Vector3& axis);

PBR_SHARED Matrix4x4 orthographic(float l, float r, float b, float t, float n, float f);
PBR_SHARED Matrix4x4 perspective(float fov, float aspect, float n, float f);
PBR_SHARED Matrix4x4 lookAt(const Vector3& eye, const Vector3& center, const Vector3& up);
} // namespace math
} // namespace pbr

#endif