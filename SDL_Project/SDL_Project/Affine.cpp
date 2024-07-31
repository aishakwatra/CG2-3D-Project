//Aisha Kwatra 65360501808

#include "Affine.h"

Hcoords::Hcoords() : x(0.0f), y(0.0f), z(0.0f), w(0.0f){}

Hcoords::Hcoords(float X, float Y, float Z, float W) : x(X), y(Y), z(Z), w(W){}

Point::Point(void) : Hcoords(0.0f, 0.0f, 0.0f, 1.0f){}

Point::Point(float X, float Y, float Z): Hcoords(X, Y, Z, 1.0f){}

Vector::Vector(void): Hcoords(0.0f, 0.0f, 0.0f, 0.0f){}

Vector::Vector(float X, float Y, float Z): Hcoords(X, Y, Z, 0.0f){}

bool Vector::Normalize(void){

    float length = sqrt((x*x) + (y*y) + (z*z));

    if(Near(length, 0)){
        return false;
    }

    x = x/length;
    y = y/length;
    z = z/length;
    w = 0.0f;

    return true;

}

Matrix::Matrix(){

    for(int i = 0; i < 4; i++) {
        row[i] = Hcoords(0.0f, 0.0f, 0.0f, 0.0f);
    }   

}

Affine::Affine(){
 
    row[0] = Hcoords(0.0f, 0.0f, 0.0f, 0.0f);
    row[1] = Hcoords(0.0f, 0.0f, 0.0f, 0.0f);
    row[2] = Hcoords(0.0f, 0.0f, 0.0f, 0.0f);
    row[3] = Hcoords(0.0f, 0.0f, 0.0f, 1.0f);
    
}

Affine::Affine(const Vector& Lx, const Vector& Ly, const Vector& Lz, const Point& D){

    row[0] = Hcoords(Lx.x, Ly.x, Lz.x, D.x);
    row[1] = Hcoords(Lx.y, Ly.y, Lz.y, D.y);
    row[2] = Hcoords(Lx.z, Ly.z, Lz.z, D.z);
    row[3] = Hcoords(Lx.w, Ly.w, Lz.w, D.w);

}

Hcoords operator+(const Hcoords& u, const Hcoords& v){
    
    return Hcoords(u.x + v.x, u.y + v.y, u.z + v.z, u.w + v.w);

}

Hcoords operator-(const Hcoords& u, const Hcoords& v){
    
    return Hcoords(u.x - v.x, u.y - v.y, u.z - v.z, u.w - v.w);

}

Hcoords operator-(const Hcoords& v){

    return Hcoords(-v.x, -v.y, -v.z, -v.w);

}


Hcoords operator*(float r, const Hcoords& v){
    
    return Hcoords(r * v.x, r * v.y, r * v.z, r * v.w);

}

Hcoords operator*(const Matrix& A, const Hcoords& v){

    Hcoords result;

    for (int i = 0; i < 4; ++i) {

        result[i] = 0.0f;

        for (int j = 0; j < 4; ++j) {

            result[i] += A.row[i][j] * v[j]; 
            
        }
    }

    return result;

}

Matrix operator*(const Matrix& A, const Matrix& B) {

    Matrix result;

    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {

            result[i][j] = 0;

            for (int k = 0; k < 4; ++k) {
                result[i][j] += A[i][k] * B[k][j];
            }

        }

    }

    return result;
}

float dot(const Vector& u, const Vector& v) {

    return (u.x * v.x) + (u.y * v.y) + (u.z * v.z);
}


float abs(const Vector& v) {

    return std::sqrt((v.x * v.x) + (v.y * v.y) + (v.z * v.z));

}


Vector cross(const Vector& u, const Vector& v){

    Vector result;

    result = Vector(u.y * v.z - u.z * v.y, 
                    u.z * v.x - u.x * v.z, 
                    u.x * v.y - u.y * v.x);

    return result;

}


Affine Rot(float t, const Vector& v){

    float cosT = cos(t);
    float sinT = sin(t);
    float K = 1.0f - cosT;

    Vector axis = v;

    if (!axis.Normalize()) {
        assert(false && "Rotation axis cannot be a zero vector.");
    }

    Affine result;

    result[0] = Hcoords((axis.x * axis.x * K) + cosT, (axis.x * axis.y * K) - (axis.z * sinT), (axis.x * axis.z * K) + (axis.y * sinT), 0.0f);
    result[1] = Hcoords((axis.y * axis.x * K) + (axis.z * sinT), (axis.y * axis.y * K) + cosT, (axis.y * axis.z * K) - (axis.x * sinT), 0.0f);
    result[2] = Hcoords((axis.z * axis.x * K) - (axis.y * sinT), (axis.z * axis.y * K) + (axis.x * sinT), (axis.z * axis.z * K) + cosT, 0.0f);
    result[3] = Hcoords(0.0f, 0.0f, 0.0f, 1.0f);

    return result;

}


Affine Trans(const Vector& v){

    Affine result;

    result[0] = Hcoords(1.0f, 0.0f, 0.0f, v.x); 
    result[1] = Hcoords(0.0f, 1.0f, 0.0f, v.y); 
    result[2] = Hcoords(0.0f, 0.0f, 1.0f, v.z); 
    result[3] = Hcoords(0.0f, 0.0f, 0.0f, 1.0f);

    return result;

}

Affine Scale(float r){

    Affine result;

    result[0] = Hcoords(r, 0.0f, 0.0f, 0.0f); 
    result[1] = Hcoords(0.0f, r, 0.0f, 0.0f); 
    result[2] = Hcoords(0.0f, 0.0f, r, 0.0f); 
    result[3] = Hcoords(0.0f, 0.0f, 0.0f, 1.0f); 

    return result;

}

Affine Scale(float rx, float ry, float rz){

    Affine result;

    result[0] = Hcoords(rx, 0.0f, 0.0f, 0.0f); 
    result[1] = Hcoords(0.0f, ry, 0.0f, 0.0f); 
    result[2] = Hcoords(0.0f, 0.0f, rz, 0.0f); 
    result[3] = Hcoords(0.0f, 0.0f, 0.0f, 1.0f); 

    return result;

}

Affine Inverse(const Affine& A){

    float det = A[0][0] * A[1][1] * A[2][2]
        + A[0][1] * A[1][2] * A[2][0]
        + A[0][2] * A[1][0] * A[2][1]
        - A[2][0] * A[1][1] * A[0][2]
        - A[2][1] * A[1][2] * A[0][0]
        - A[2][2] * A[1][0] * A[0][1];

    Affine inverseTran{ {1, 0, 0}, {0, 1, 0}, {0, 0, 1}, {-A[0][3], -A[1][3], -A[2][3]} };

        Affine adj{
            (1 / det) * Vector {
     A[1][1] * A[2][2] - A[2][1] * A[1][2],    -(A[1][0] * A[2][2] - A[2][0] * A[1][2]),   A[1][0] * A[2][1] - A[2][0] * A[1][1]
    },
    (1 / det) * Vector {
    -(A[0][1] * A[2][2] - A[2][1] * A[0][2]), A[0][0] * A[2][2] - A[2][0] * A[0][2],      -(A[0][0] * A[2][1] - A[2][0] * A[0][1])
    },
    (1 / det) * Vector {
    A[0][1] * A[1][2] - A[1][1] * A[0][2],    -(A[0][0] * A[1][2] - A[1][0] * A[0][2]),   A[0][0] * A[1][1] - A[1][0] * A[0][1]
    },
    {}
    };

    return adj * inverseTran;

    
}










