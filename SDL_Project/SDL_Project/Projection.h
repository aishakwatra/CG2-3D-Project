// Projection.h
// -- world-to-camera, and camera-to-ndc transforms
// cs250 5/15

#ifndef DT285_PROJECTION_H
#define DT285_PROJECTION_H

#include "Camera.h"


Affine CameraToWorld(const Camera& cam);
Affine WorldToCamera(const Camera& cam);
Matrix CameraToNDC(const Camera& cam);


#endif

