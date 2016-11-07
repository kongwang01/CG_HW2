#include <iostream>
#include <fstream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include "vec3.h"
#include "ray.h"
using namespace std;


struct hit_record {
    float t;
    vec3 p;
    vec3 normal;
    //??
};

class sphere{
    public:
    sphere() {}
    sphere(vec3 c, float r) : center(c), radius(r) {};
    bool hit(const ray& r, float tmin, float tmax, hit_record& rec) const;

    vec3 center;
    float radius;
};

bool sphere::hit(const ray& r, float tmin, float tmax, hit_record& rec) const {
    float t = 0.0;
    vec3 dir = unit_vector(r.direction());

    //判別式 D = b^2 - 4ac, a = 1,  b = 2d‧(O-C), c = (O-C)‧(O-C)-r^2
    vec3 OC = r.origin() - this->center;
    float dotOCD = dot(dir, OC);
    float OCdotOC = dot(OC, OC);
    float D = (2.0*dotOCD)*(2.0*dotOCD) - 4.0*dot(dir, dir)*(OCdotOC - this->radius*this->radius);

    if(D >= 0) //D大於等於0, 代表有交點
    {
        D = sqrt(D/4.0);
        t = -dotOCD - D;
        //cout << t << endl;
        if(((t > tmin) && (rec.t > t)) || ((t > tmin) && (rec.t == 0.0)))
        {
            vec3 N = r.point_at_parameter(t);
            N = N - center;
            N.make_unit_vector();

            rec.t = t;
            rec.normal = N;
            rec.p = r.point_at_parameter(t);
        }

        return true;
    }
    else
    {
        t = 0.0;
        return false;
    }
}
vector<sphere> spheres;


float hit_sphere(const vec3 &center, float radius, const ray& r)
{
    float t = 0.0;
    vec3 dir = unit_vector(r.direction());

    //判別式 D = b^2 - 4ac, a = 1,  b = 2d‧(O-C), c = (O-C)‧(O-C)-r^2
    vec3 OC = r.origin() - center;
    float dotOCD = dot(dir, OC);
    float OCdotOC = dot(OC, OC);
    float D = (2.0*dotOCD)*(2.0*dotOCD) - 4.0*dot(dir, dir)*(OCdotOC - radius*radius);

    if(D >= 0) //D大於等於0, 代表有交點
    {
        D = sqrt(D/4.0);
        t = -dotOCD - D;
    }
    else
    {
        t = 0.0;
    }
    return t;
}


vec3 cal_color(const ray& r)
{
    vec3 pointlight(-10, 10, 0);

    hit_record ht;
    ht.t = 0.0;

    for (int i = 0; i < spheres.size() ; i++)
    {
        spheres[i].hit(r, 0.01, 1000.0, ht);
    }
    //cout << ht.t << endl;
    if(ht.t != 0.0) //代表此光線r跟某個物體有碰撞
    {
        vec3 L = pointlight - ht.p;
        L.make_unit_vector();
        vec3 I = vec3(1, 1, 1);//intensity of lightsource
        return I * max(float(0.0) ,dot(L,ht.normal))  ;
    }

    /*
    vec3 center(0, 0, -1);
    float t = hit_sphere(center, 0.5, r);
    if (t > 0.0)
    {
        vec3 N = r.point_at_parameter(t);
        N = N - center;
        N.make_unit_vector();

        vec3 L = pointlight - r.point_at_parameter(t);
        L.make_unit_vector();
        vec3 I = vec3(1, 1, 1);//intensity of lightsource
        return I * max(float(0.0) ,dot(L,N))  ;

    }*/


    /*vec3 center4(0, -500, -10);
    float t = 0.0;
    t = hit_sphere(center4, 500, r);
    if (t > 0.0)
    {
        vec3 N = r.point_at_parameter(t);
        N = N - center4;
        N.make_unit_vector();

        vec3 L = pointlight - r.point_at_parameter(t);
        L.make_unit_vector();
        vec3 I = vec3(1, 1, 1);//intensity of lightsource
        return I * max(float(0.0) ,dot(L,N))   ;
    }*/

    r.direction().make_unit_vector();
    vec3 unit_direction = r.direction();
    float t= 0.5*(unit_direction.y() + 1.0);

    return vec3(1, 1, 1) * (1.0-t) + vec3(0.5, 0.7, 1.0) * t;
}



int main()
{
    //====  隨機創造10顆球  ================
    srand((unsigned)time(0));
    int temp_r = 0, temp_x = 0, temp_y = 0, temp_z = 0;
    //vector<sphere> spheres;
    for(int i = 0 ; i < 3 ; i++)//三大球
    {
        temp_r = 30;
        temp_x = -7 + (i*7);
        temp_y = 0;
        temp_z = -10;
        vec3 temp_v3((float)temp_x,(float)temp_y,(float)temp_z);
        sphere temp_sphere(temp_v3, ((float)temp_r)/10.0);
        spheres.push_back(temp_sphere);
    }
    for(int i = 0 ; i < 7 ; i++)//剩下小球
    {
        temp_r = rand()%10;
        temp_x = rand()%30;
        temp_y = rand()%2;
        temp_z = rand()%10;
        vec3 temp_v3(((float)temp_x)-10.0,((float)temp_y)-5.0,(-((float)temp_z)-5.0));
        sphere temp_sphere(temp_v3, ((float)temp_r)/10.0);
        spheres.push_back(temp_sphere);
    }

    //====  當作ground  ====================
    temp_r = 1000;
    temp_x = 0;
    temp_y = -990;
    temp_z = -200;
    vec3 temp_v3((float)temp_x,(float)temp_y,(float)temp_z);
    sphere temp_sphere(temp_v3, (float)temp_r);
    spheres.push_back(temp_sphere);


    for(int i = 0 ; i < 10 ; i++) //輸出存入的值，確認儲存正確
    {
        cout << spheres[i].center.z() << endl;
    }
    //=======================================

    int width = 400;
    int height = 200;

    fstream file;
    file.open("ray.ppm", ios::out);

    vec3 lower_left_corner(-2, -1, -1);
    vec3 origin(0, 0, 1);
    vec3 horizontal(4, 0, 0);
    vec3 vertical(0, 2, 0);
    file << "P3\n" << width << " " << height << "\n255\n";
    for (int j = height - 1; j >= 0; j--) {
        for (int i = 0; i < width; i++) {
        float u = float(i) / float(width);
        float v = float(j) / float(height);
        ray r(origin, lower_left_corner + horizontal*u + vertical*v);
        vec3 color = cal_color(r);

            file << int(color.r() * 255) << " " << int(color.g()  * 255) << " " << int(color.b() * 255) << "\n";
        }
    }


    return 0;
}
