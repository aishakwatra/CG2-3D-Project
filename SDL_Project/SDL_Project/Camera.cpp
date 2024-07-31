#include "Camera.h"

Camera::Camera() : eye(Point(0, 0, 0)), right(Vector(1, 0, 0)), up(Vector(0, 1, 0)), back(Vector(0, 0, -1)),
width(1), height(1), distance(1), near(0.1f), far(10.0f) {}



Camera::Camera(const Point& E, const Vector& look, const Vector& up, float fov, float aspect, float near, float far) : eye(E), up(up), near(near), far(far) {

    back = -look;
    distance = near;
    width = 2 * distance * tan(fov / 2);
    height = width / aspect;
    right = cross(look, up);
    this->up = up;
}

Point Camera::Eye(void) const {

    return eye;

}

void Camera::SetLookVector(const Vector& look, const Vector& up) {

    Vector unitVec = look;
    unitVec.Normalize();
    this->back = -unitVec;

    this->up = up;  // Update the up vector
    this->up.Normalize();

    this->right = cross(this->up, this->back);// Recalculate the right vector
    this->right.Normalize();

}

void Camera::SetEyePosition(const Point& E) {
    eye = E;
}



Vector Camera::Right(void) const {

    Vector unitVec = right;
    unitVec.Normalize();

    return unitVec;

}


Vector Camera::Up(void) const {

    Vector unitVec = up;
    unitVec.Normalize();

    return unitVec;

}

Vector Camera::Back(void) const {

    Vector unitVec = back;
    unitVec.Normalize();

    return unitVec;

}

Vector Camera::ViewportGeometry(void) const {

    return Vector(width, height, distance);

}

float Camera::NearDistance(void) const {

    return near;

}

float Camera::FarDistance(void) const {

    return far;

}

Camera& Camera::Pitch(float angle) {

    float cosA = cos(angle);
    float sinA = sin(angle);

    //about u vector
    //rotate the up and back vectors about the right vector
    Vector newUp = cosA * up + sinA * back;
    Vector newBack = -sinA * up + cosA * back;

    up = newUp;
    back = newBack;

    return *this;

}

Camera& Camera::Yaw(float angle) {

    float cosA = cos(angle);
    float sinA = sin(angle);

    //about v vector
    //rotate the right and back vectors about the up vector
    Vector newRight = cosA * right - sinA * back;
    Vector newBack = sinA * right + cosA * back;

    right = newRight;
    back = newBack;

    return *this;

}

Camera& Camera::Roll(float angle) {

    float cosA = cos(angle);
    float sinA = sin(angle);

    //about n vector
    //rotate the right and up vectors about the back vector
    Vector newRight = cosA * right + sinA * up;
    Vector newUp = sinA * -right + cosA * up;

    right = newRight;
    up = newUp;

    return *this;
}

Camera& Camera::Forward(float distance) {

    // Update the eye position by moving along the back vector
    eye.x -= distance * back.x;
    eye.y -= distance * back.y;
    eye.z -= distance * back.z;

    return *this;

}


Camera& Camera::Zoom(float factor) {

    width *= factor;
    height *= factor;

    return *this;
}

