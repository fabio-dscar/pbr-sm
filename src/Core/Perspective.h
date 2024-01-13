#ifndef __PBR_PERSPECTIVE_H__
#define __PBR_PERSPECTIVE_H__

#include <Camera.h>

namespace pbr {

class Perspective : public Camera {
public:
    Perspective(int width, int height, const Vec3& eye, const Vec3& at, const Vec3& up,
                float n, float f, float fov);

    float fov() const;

    void updateDimensions(int w, int h) override;

private:
    float _fov;
};

} // namespace pbr

#endif