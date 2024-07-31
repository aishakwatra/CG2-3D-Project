#include "Projection.h"

#include <iostream>

Affine WorldToCamera(const Camera& cam) {

	return Inverse(CameraToWorld(cam));

}

Affine CameraToWorld(const Camera& cam) {

	Affine rotation(cam.Right(), cam.Up(), cam.Back(), Hcoords(0, 0, 0, 1));
	Affine translation(Vector(1, 0, 0), Vector(0, 1, 0), Vector(0, 0, 1), cam.Eye());

	return translation * rotation;

}

Matrix CameraToNDC(const Camera& cam) {

	float n = cam.NearDistance();
	float f = cam.FarDistance();
	
	Vector viewport = cam.ViewportGeometry();
	float W = viewport.x;
	float H = viewport.y;
	float D = viewport.z;

	Matrix camToNDC;

	camToNDC[0] = Hcoords((2 * D) / W, 0, 0, 0);
	camToNDC[1] = Hcoords(0, (2 * D) / H, 0, 0);
	camToNDC[2] = Hcoords(0, 0, (n + f) / (n - f), (2 * n * f) / (n - f));
	camToNDC[3] = Hcoords(0, 0, -1.0f, 0);

	return camToNDC;

}
