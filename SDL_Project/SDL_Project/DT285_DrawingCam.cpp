#include "DT285_DrawingCam.h"
#include <SDL.h>
#include <GL/glew.h>
#include <GL/gl.h>
#include <glm/glm.hpp>
#include <iostream>

#include <vector>
#include "Camera.h"
#include "Projection.h"

using namespace std;

vector<float> vertices;
vector<float> colors;
vector<int> indices;

std::vector<Point> temp_verts;

GLuint program_id;
GLuint vertex_shader_id, fragment_shader_id, p;
GLuint VBO, VAO, VCO;

const char* vertex_shader_str = "#version 330 core\n"
"layout (location = 0) in vec3 a_pos;\n"
"layout (location = 1) in vec3 c_pos;\n"
"out vec4 vertex_color;\n"
"void main() {\n"
"  gl_Position = vec4(a_pos, 1.0);\n"
"  vertex_color = vec4(c_pos, 1.0);\n"
"}\n";

const char* fragment_shader_str = "#version 330 core\n"
"out vec4 frag_color;\n"
"in vec4 vertex_color;\n"
"void main() {\n"
"  frag_color = vertex_color;\n"
"}\n";


void InitBuffer() {

    vertices.clear();
    indices.clear();
    colors.clear();

    program_id = glCreateProgram();

    vertex_shader_id = glCreateShader(GL_VERTEX_SHADER);
    fragment_shader_id = glCreateShader(GL_FRAGMENT_SHADER);

    glShaderSource(vertex_shader_id, 1, &vertex_shader_str, NULL);
    glShaderSource(fragment_shader_id, 1, &fragment_shader_str, NULL);

    glCompileShader(vertex_shader_id);
    glCompileShader(fragment_shader_id);

    glAttachShader(program_id, vertex_shader_id);
    glAttachShader(program_id, fragment_shader_id);
    glLinkProgram(program_id);
    glUseProgram(program_id);

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &VCO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, VCO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);


}

void UpdateBuffers() {


    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    glBindBuffer(GL_ARRAY_BUFFER, VCO);
    glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(float), colors.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

}


float Magnitude(Vector v) {

    return std::sqrt((v.x * v.x) + (v.y * v.y) + (v.z * v.z));

}


void DisplayEdges(Mesh& mesh, const Affine& obj2world, const Camera& cam, const Vector& clr) {

    Matrix viewMatrix = WorldToCamera(cam) * obj2world;
    Matrix projMatrix = CameraToNDC(cam) * viewMatrix;

    vertices.clear();
    indices.clear();
    colors.clear();

    for (int i = 0; i < mesh.EdgeCount(); ++i) {
        Mesh::Edge edge = mesh.GetEdge(i);
        Hcoords p1Cam = viewMatrix * mesh.GetVertex(edge.index1);
        Hcoords p2Cam = viewMatrix * mesh.GetVertex(edge.index2);

        if (p1Cam.z >= 0)
            continue;
        if (p2Cam.z >= 0)
            continue;

        Hcoords P1 = projMatrix * mesh.GetVertex(edge.index1);
        Hcoords P2 = projMatrix * mesh.GetVertex(edge.index2);

        P1.x = P1.x / P1.w;
        P1.y = P1.y / P1.w;
        P1.z = P1.z / P1.w;

        P2.x = P2.x / P2.w;
        P2.y = P2.y / P2.w;
        P2.z = P2.z / P2.w;

        unsigned int startIndex = static_cast<unsigned int>(vertices.size() / 3);

        vertices.push_back(P1.x);
        vertices.push_back(P1.y);
        vertices.push_back(P1.z);

        vertices.push_back(P2.x);
        vertices.push_back(P2.y);
        vertices.push_back(P2.z);

        for (int j = 0; j < 2; j++) {
            colors.push_back(clr.x);
            colors.push_back(clr.y);
            colors.push_back(clr.z);
        }

        indices.push_back(startIndex);
        indices.push_back(startIndex + 1);

    }


    UpdateBuffers();

    glDrawArrays(GL_LINES, 0, indices.size());

}


void DisplayFaces(Mesh& mesh, const Affine& obj2world, const Camera& cam, const Vector& clr) {

    vertices.clear();
    indices.clear();
    colors.clear();

    temp_verts.clear();
    temp_verts.resize(mesh.VertexCount());

    Matrix camToNDC = CameraToNDC(cam);
    Matrix worldToCam = WorldToCamera(cam);

    Point cameraPosition = cam.Eye();
    Vector light = cam.Back();
    light.Normalize();

    for (int i = 0; i < mesh.VertexCount(); ++i) {
        temp_verts[i] = obj2world * mesh.GetVertex(i);
    }

    for (int i = 0; i < mesh.FaceCount(); ++i) {

        Mesh::Face face = mesh.GetFace(i);

        //Face Vertices
        Point p1 = temp_verts.at(face.index1);
        Point p2 = temp_verts.at(face.index2);
        Point p3 = temp_verts.at(face.index3);

        // Transform vertices to camera coordinates
        Point p1Camera = worldToCam * p1;
        Point p2Camera = worldToCam * p2;
        Point p3Camera = worldToCam * p3;

        if (p1Camera.z >= 0 || p2Camera.z >= 0 || p3Camera.z >= 0) {
            continue;
        }

        //Face Normal
        Vector normal = cross(p2 - p1, p3 - p1);
        normal.Normalize();

        Vector viewVector = cameraPosition - p1;
        viewVector.Normalize();

        if (dot(normal, viewVector) > 0) {

            float color_constant = ((dot(light, normal)) / (Magnitude(light) * Magnitude(normal)));
            Vector faceColor = color_constant * clr;

            Hcoords P = camToNDC * p1Camera;
            Hcoords Q = camToNDC * p2Camera;
            Hcoords R = camToNDC * p3Camera;

            P.x = P.x / P.w;
            P.y = P.y / P.w;
            P.z = P.z / P.w;

            Q.x = Q.x / Q.w;
            Q.y = Q.y / Q.w;
            Q.z = Q.z / Q.w;

            R.x = R.x / R.w;
            R.y = R.y / R.w;
            R.z = R.z / R.w;

            //Add indices

            int indice_index = (vertices.size() / 3);
            indices.push_back(indice_index + 0);
            indices.push_back(indice_index + 1);
            indices.push_back(indice_index + 2);

            //Add vertices

            vertices.push_back(P.x);
            vertices.push_back(P.y);
            vertices.push_back(P.z);

            vertices.push_back(Q.x);
            vertices.push_back(Q.y);
            vertices.push_back(Q.z);

            vertices.push_back(R.x);
            vertices.push_back(R.y);
            vertices.push_back(R.z);

            for (int i = 0; i < 3; ++i) {
                colors.push_back(faceColor.x);
                colors.push_back(faceColor.y);
                colors.push_back(faceColor.z);
            }

        }

    }

    UpdateBuffers();

    glDrawArrays(GL_TRIANGLES, 0, indices.size());


}


