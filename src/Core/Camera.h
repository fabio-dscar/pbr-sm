#ifndef PBR_CAMERA_H
#define PBR_CAMERA_H

#include <SceneObject.h>
#include <Ray.h>

namespace pbr {

struct CameraData {
    Mat4 viewMatrix;
    Mat4 projMatrix;
    Mat4 viewProjMatrix;
    alignas(16) Vec3 viewPos;
};

class Camera : public SceneObject {
public:
    Camera(int width, int height, const Vec3& eye, const Vec3& at, const Vec3& up,
           float n, float f);

    int width() const;
    int height() const;

    float aspect() const;
    float near() const;
    float far() const;

    Vec3 right() const;
    Vec3 front() const;
    Vec3 up() const;

    void lookAt(const Vec3& eye, const Vec3& at, const Vec3& up);
    void lookAt(const Vec3& at);

    const Mat4& viewMatrix() const;
    const Mat4& projMatrix() const;

    Mat4 viewProjMatrix() const;

    void updateOrientation(float dp, float dy);
    void updateViewMatrix();

    virtual void updateDimensions(int w, int h);

    Ray traceRay(const Vec2& pixel) const;

protected:
    Mat4 _projMatrix;

    int _width;
    int _height;

    float _far;
    float _near;

    float _pitch;
    float _yaw;
};

std::unique_ptr<Camera> CreateCamera(const ParameterMap& params);

} // namespace pbr

#endif