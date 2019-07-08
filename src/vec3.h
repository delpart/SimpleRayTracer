// *** ADDED BY HEADER FIXUP ***
#include <istream>
// *** END ***
#ifndef VEC3H
#define VEC3H

#include <math.h>
#include <stdlib.h>
#include <iostream>

class vec3{
    public:
        vec3(){}
        vec3(float x, float y, float z){
            val[0] = x;
            val[1] = y;
            val[2] = z;
        }
        
        inline float x() const {
            return val[0];
        }

        inline float y() const{
            return val[1];
        }

        inline float z() const{
            return val[2];
        }

        inline float r() const {
            return val[0];
        }

        inline float g() const{
            return val[1];
        }

        inline float b() const{
            return val[2];
        }

        inline const vec3& operator+() const{
            return *this;
        }

        inline vec3 operator-() const{
            return vec3(-val[0], -val[1], -val[2]);
        }

        inline float operator[](int i) const {
            return val[i];
        }

        inline float& operator[](int i){
            return val[i];
        }

        inline vec3& operator+=(const vec3 &v2);
        inline vec3& operator-=(const vec3 &v2);
        inline vec3& operator*=(const vec3 &v2);
        inline vec3& operator/=(const vec3 &v2);
        inline vec3& operator*=(const float t);
        inline vec3& operator/=(const float t);
        
        inline float length() const{
            return sqrt(val[0]*val[0] + val[1]*val[1] + val[2]*val[2]);
        }

        inline float squaredLength() const{
            return val[0]*val[0] + val[1]*val[1] + val[2]*val[2];
        }
        
        inline vec3 abs() const{
            return vec3(std::abs(val[0]), std::abs(val[1]), std::abs(val[2]));
        } 

        inline void toUnitVector();

        float val[3];
};

inline std::istream& operator>>(std::istream &is, vec3 &t) {
    is >> t.val[0] >> t.val[1] >> t.val[2];
    return is;
}

inline std::ostream& operator<<(std::ostream &os, const vec3 &t) {
    os << t.val[0] << " " << t.val[1] << " " << t.val[2];
    return os;
}

inline void vec3::toUnitVector() {
    float k = 1.0 / sqrt(val[0]*val[0] + val[1]*val[1] + val[2]*val[2]);
    val[0] *= k; val[1] *= k; val[2] *= k;
}

inline vec3 operator+(const vec3 &v1, const vec3 &v2) {
    return vec3(v1.val[0] + v2.val[0], v1.val[1] + v2.val[1], v1.val[2] + v2.val[2]);
}

inline vec3 operator-(const vec3 &v1, const vec3 &v2) {
    return vec3(v1.val[0] - v2.val[0], v1.val[1] - v2.val[1], v1.val[2] - v2.val[2]);
}

inline vec3 operator*(const vec3 &v1, const vec3 &v2) {
    return vec3(v1.val[0] * v2.val[0], v1.val[1] * v2.val[1], v1.val[2] * v2.val[2]);
}

inline vec3 operator/(const vec3 &v1, const vec3 &v2) {
    return vec3(v1.val[0] / v2.val[0], v1.val[1] / v2.val[1], v1.val[2] / v2.val[2]);
}

inline vec3 operator*(float t, const vec3 &v) {
    return vec3(t*v.val[0], t*v.val[1], t*v.val[2]);
}

inline vec3 operator/(vec3 v, float t) {
    return vec3(v.val[0]/t, v.val[1]/t, v.val[2]/t);
}

inline vec3 operator*(const vec3 &v, float t) {
    return vec3(t*v.val[0], t*v.val[1], t*v.val[2]);
}

inline float dot(const vec3 &v1, const vec3 &v2) {
    return v1.val[0] * v2.val[0] + v1.val[1] * v2.val[1]  + v1.val[2] * v2.val[2];
}

inline vec3 cross(const vec3 &v1, const vec3 &v2) {
    return vec3( (v1.val[1]*v2.val[2] - v1.val[2]*v2.val[1]),
                (-(v1.val[0]*v2.val[2] - v1.val[2]*v2.val[0])),
                (v1.val[0]*v2.val[1] - v1.val[1]*v2.val[0]));
}


inline vec3& vec3::operator+=(const vec3 &v){
    val[0]  += v.val[0];
    val[1]  += v.val[1];
    val[2]  += v.val[2];
    return *this;
}

inline vec3& vec3::operator*=(const vec3 &v){
    val[0]  *= v.val[0];
    val[1]  *= v.val[1];
    val[2]  *= v.val[2];
    return *this;
}

inline vec3& vec3::operator/=(const vec3 &v){
    val[0]  /= v.val[0];
    val[1]  /= v.val[1];
    val[2]  /= v.val[2];
    return *this;
}

inline vec3& vec3::operator-=(const vec3& v) {
    val[0]  -= v.val[0];
    val[1]  -= v.val[1];
    val[2]  -= v.val[2];
    return *this;
}

inline vec3& vec3::operator*=(const float t) {
    val[0]  *= t;
    val[1]  *= t;
    val[2]  *= t;
    return *this;
}

inline vec3& vec3::operator/=(const float t) {
    float k = 1.0/t;
    
    val[0]  *= k;
    val[1]  *= k;
    val[2]  *= k;
    return *this;
}

inline vec3 unitVector(vec3 v) {
    return v / v.length();
}

#endif

