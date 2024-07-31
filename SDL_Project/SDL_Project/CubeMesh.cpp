#include "CubeMesh.h"

int CubeMesh::VertexCount(void) { return 8; }
Point CubeMesh::GetVertex(int i) { return vertices[i]; }
Vector CubeMesh::Dimensions(void) { return Vector(2, 2, 2); }
Point CubeMesh::Center(void) { return Point(0, 0, 0); }
int CubeMesh::FaceCount(void) { return 12; }
Mesh::Face CubeMesh::GetFace(int i) { return faces[i]; }
int CubeMesh::EdgeCount(void) { return 12; }
Mesh::Edge CubeMesh::GetEdge(int i) { return edges[i]; }

const Mesh::Face CubeMesh::faces[12] = {

	Face(0, 1, 2),
	Face(3, 0, 2),

	Face(6, 7, 4),
	Face(4, 5, 6),

	Face(0, 1, 5),
	Face(0, 5, 4),

	Face(6, 2, 3),
	Face(6, 3, 7),

	Face(5, 1, 2), 
	Face(2, 6, 5),

	Face(4, 7, 3), 
	Face(3, 0, 4)
};

const Mesh::Edge CubeMesh::edges[12] = {

	Edge(0, 1),
	Edge(1, 2),
	Edge(2, 3),
	Edge(3, 0),
	Edge(4, 5),
	Edge(5, 6),
	Edge(6, 7),
	Edge(7, 4),
	Edge(0, 4),
	Edge(1, 5),
	Edge(2, 6),
	Edge(3, 7)

};


const Point CubeMesh::vertices[8] = {

	Point(-1.0f, -1.0f, -1.0f),   // Vertex 0
	Point(1.0f, -1.0f, -1.0f),    // Vertex 1
	Point(1.0f, 1.0f, -1.0f),     // Vertex 2
	Point(-1.0f, 1.0f, -1.0f),    // Vertex 3
	Point(-1.0f, -1.0f, 1.0f),    // Vertex 4
	Point(1.0f, -1.0f, 1.0f),     // Vertex 5
	Point(1.0f, 1.0f, 1.0f),      // Vertex 6
	Point(-1.0f, 1.0f, 1.0f)      // Vertex 7

};




