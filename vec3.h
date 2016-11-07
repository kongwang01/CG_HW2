#ifndef VEC3_H_INCLUDED
#define VEC3_H_INCLUDED

#include <cmath>

class vec3 {
public:
    vec3() {}
    vec3(float e0, float e1, float e2) { e[0] = e0; e[1] = e1; e[2] = e2; }
    float x() const { return e[0]; }
    float y() const { return e[1]; }
    float z() const { return e[2]; }
    float r() const { return e[0]; }
    float g() const { return e[1]; }
    float b() const { return e[2]; }

    inline vec3& operator+=(const vec3 &v2);
    inline vec3& operator-=(const vec3 &v2);

    inline vec3& operator*=(const float t);
    inline vec3& operator/=(const float t);

    inline vec3 operator+(const vec3 &v2);
    inline vec3 operator-(const vec3 &v2);

    inline vec3 operator*(const float t);
    inline vec3 operator/(const float t);

    inline vec3 operator*(const vec3 &v2);

    inline float length() const { return sqrt(e[0] * e[0] + e[1] * e[1] + e[2] * e[2]); }
    inline float squared_length() const { return e[0] * e[0] + e[1] * e[1] + e[2] * e[2]; }
    inline void make_unit_vector();

    float e[3];
};

inline float dot(const vec3 &v1, const vec3 &v2) //¤º¿n
{
    float result;
    result = v1.x() * v2.x() + v1.y() * v2.y() + v1.z() * v2.z();
    return result;
}
inline vec3 cross(const vec3 &v1, const vec3 &v2) //¥~¿n
{
    vec3 cross_result;
    cross_result.e[0] = (v1.y() * v2.z()) - (v1.z() * v2.y());
    cross_result.e[1] = (v1.z() * v2.x()) - (v1.x() * v2.z());
    cross_result.e[2] = (v1.x() * v2.y()) - (v1.y() * v2.x());

    return cross_result;
}

inline vec3& vec3::operator+=(const vec3 &v2)
{
    this->e[0] = this->x() + v2.x();
    this->e[1] = this->y() + v2.y();
    this->e[2] = this->z() + v2.z();
}
inline vec3& vec3::operator-=(const vec3 &v2)
{
    this->e[0] = this->x() - v2.x();
    this->e[1] = this->y() - v2.y();
    this->e[2] = this->z() - v2.z();
}

inline vec3& vec3::operator*=(const float t)
{
    this->e[0] = this->x() * t;
    this->e[1] = this->y() * t;
    this->e[2] = this->z() * t;
}
inline vec3& vec3::operator/=(const float t)
{
    this->e[0] = this->x() / t;
    this->e[1] = this->y() / t;
    this->e[2] = this->z() / t;
}

//======================================================
inline vec3 vec3::operator+(const vec3 &v2)
{
    vec3 add;
    add.e[0] = this->x() + v2.x();
    add.e[1] = this->y() + v2.y();
    add.e[2] = this->z() + v2.z();

    return add;
}
inline vec3 vec3::operator-(const vec3 &v2)
{
    vec3 sub;
    sub.e[0] = this->x() - v2.x();
    sub.e[1] = this->y() - v2.y();
    sub.e[2] = this->z() - v2.z();

    return sub;
}

inline vec3 vec3::operator*(const float t)
{
    vec3 mul;
    mul.e[0] = this->x() * t;
    mul.e[1] = this->y() * t;
    mul.e[2] = this->z() * t;

    return mul;
}
inline vec3 vec3::operator/(const float t)
{
    vec3 div;
    div.e[0] = this->x() / t;
    div.e[1] = this->y() / t;
    div.e[2] = this->z() / t;

    return div;
}

inline vec3 vec3::operator*(const vec3 &v2)
{
    vec3 vec_mul;
    vec_mul.e[0] = this->x() * v2.x();
    vec_mul.e[1] = this->y() * v2.y();
    vec_mul.e[2] = this->z() * v2.z();

    return vec_mul;
}
//================================================================

inline void vec3::make_unit_vector()
{
    float l = this->length();
    this->e[0] = this->e[0] / l;
    this->e[1] = this->e[1] / l;
    this->e[2] = this->e[2] / l;
}

inline vec3 unit_vector(vec3 v) {
    vec3 unit_vec = v;
    float l = unit_vec.length();
    unit_vec.e[0] = unit_vec.e[0] / l;
    unit_vec.e[1] = unit_vec.e[1] / l;
    unit_vec.e[2] = unit_vec.e[2] / l;

    return unit_vec;
}

#endif // VEC3_H_INCLUDED
