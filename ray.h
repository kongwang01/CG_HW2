#ifndef RAY_H_INCLUDED
#define RAY_H_INCLUDED
#include "vec3.h"

class ray
{
    public:
        ray() {}
        ray(const vec3& a, const vec3& b) { O = a; D = b; }
        vec3 origin() const       { return O; }
        vec3 direction() const    { return D; }
        vec3 point_at_parameter(float t) const
        {
            vec3 unit_dir = unit_vector(this->direction());
            return this->origin() + unit_dir * t;
        }

    private:
        vec3 O;
        vec3 D;
};

#endif // RAY_H_INCLUDED
