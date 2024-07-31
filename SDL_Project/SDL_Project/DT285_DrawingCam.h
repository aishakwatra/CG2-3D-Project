// cs250_h4.h
// -- display edges/faces of a mesh, second version
// cs250 1/15

#ifndef DT285_DRAWING_CAM_H
#define DT285_DRAWING_CAM_H

#include <cstdlib>
#include "Camera.h"
#include "Mesh.h"


void DisplayEdges(Mesh& mesh, const Affine& obj2world,
    const Camera& cam, const Vector& clr);

void DisplayFaces(Mesh& mesh, const Affine& obj2world,
    const Camera& cam, const Vector& clr);

void InitBuffer();

void UpdateBuffers();

#endif

