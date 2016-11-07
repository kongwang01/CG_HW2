#include <iostream>
#include <fstream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include "vec3.h"
#include "ray.h"
using namespace std;

vec3 pointlight(-10, 10, 0);
vec3 lightintensity(1, 1, 1);

struct hit_record {
    float t;
    vec3 p;
    vec3 normal;
    float w_r;
    float w_t;
    //??
};

class sphere{
    public:
    sphere() {}
    //sphere(vec3 c, float r) : center(c), radius(r) {};
    sphere(vec3 c, float r, float w_ri=0.0f, float w_ti=0.0f) : center(c), radius(r), w_r(w_ri), w_t(w_ti) {};
    bool hit(const ray& r, float tmin, float tmax, hit_record& rec) const;

    vec3 center;
    float radius;
    float w_r;
    float w_t;
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
            rec.w_r = this->w_r;
            rec.w_t = this->w_t;
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

vec3 Shading(vec3 lightposition,vec3 lightintensity, hit_record& rec)
{
    vec3 L = lightposition - rec.p;
    L.make_unit_vector();
    vec3 I = lightintensity;//intensity of lightsource
    return I * max(float(0.0) ,dot(L,rec.normal))  ;
}

vec3 SkyColor(vec3 p, vec3 d)
{
    ray r(p, d);

    r.direction().make_unit_vector();
    vec3 unit_direction = r.direction();
    float t= 0.5*(unit_direction.y() + 1.0);

    return vec3(1, 1, 1) * (1.0-t) + vec3(0.5, 0.7, 1.0) * t;
}

int Intersect(vec3 p, vec3 d, hit_record& rec) //求一條光線是否有跟任何物體有交點，傳回0代表無交點
{
    ray trace_r(p, d);

    for (int i = 0; i < spheres.size() ; i++) //尋找跟光線r有交點的物體之中，最近的那個
    {
        spheres[i].hit(trace_r, 0.01, 1000.0, rec);
    }

    if(rec.t != 0.0) //代表此光線r跟某個物體有碰撞
    {
        //return Shading(pointlight, lightintensity, ht);
        return 1;
    }
    else
    {
        return 0;
    }
}

vec3 Reflect(vec3 q, vec3 n) //計算反射光
{
    vec3 r_in(-q.x(), -q.y(), -q.z());
    r_in.make_unit_vector();
    vec3 r_out = (n * 2.0) * dot(n, r_in);
    r_out = r_out - r_in;
    return r_out;
}

vec3 Transmit(vec3 q, vec3 n) //計算折射光
{
    //vec3 r_in(-q.x(), -q.y(), -q.z());

    //return r_out;
}

vec3 trace(vec3 p, vec3 d, int step)
{
    vec3 local, reflected, transmitted;
    vec3 q; //point
    vec3 n; //normal

    if(step > 3)
        //return vec3(0, 0, 0);//black or background
        return SkyColor(p, d);

    hit_record ht;
    ht.t = 0.0;

    int status = Intersect(p, d, ht);

    //if(status==light_source)
        //return(light_source_color);
    if(status == 0)
        return SkyColor(p, d);

    q = ht.p;
    n = ht.normal; //or get from hit_record
    vec3 r = Reflect(q, n);
    //vec3 t = Transmit(q, n);
    local = Shading(pointlight, lightintensity, ht);
    reflected = trace(q, r, step+1);
    //transmitted = trace(q, t, step+1);

    //cout << local.r() << " , "<< local.g() << " , "<< local.b() << endl;
    //return(w_l*local+ w_r*reflected+ w_t*transmitted);

    //if(ht.w_r != 0.0)
    //    cout << reflected.x() << " , "<< reflected.y() << " , "<< reflected.z() << endl;

    return(local*(1.0f - ht.w_r)+ reflected*ht.w_r+ transmitted*0.0);//只有折射
    //return(local*(1.0f - ht.w_r)+ reflected*ht.w_r+ transmitted*ht.w_t) * (1.0f - ht.w_t);
}

vec3 cal_color(const ray& r)
{
    hit_record ht;
    ht.t = 0.0;

    for (int i = 0; i < spheres.size() ; i++) //尋找跟光線r有交點的物體之中，最近的那個
    {
        spheres[i].hit(r, 0.01, 1000.0, ht);
    }

    if(ht.t != 0.0) //代表此光線r跟某個物體有碰撞
    {
        //return Shading(pointlight, lightintensity, ht);
        return trace(r.origin(), r.direction(), 0);
    }

    return SkyColor(r.origin(), r.direction()); //光線沒碰到任何物體，回傳天空背景
}


int main()
{
    //====  隨機創造10顆球  ================
    //srand((unsigned)time(0));
    int temp_r = 0, temp_x = 0, temp_y = 0, temp_z = 0;
    //vector<sphere> spheres;
    /*for(int i = 0 ; i < 3 ; i++)//三大球
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
    }*/

    //====  當作ground  ====================
    sphere ground_sphere(vec3(0, -100.5, -2), 100);
    spheres.push_back(ground_sphere);
    //=======================================

    //====  測試  ====================
    sphere test_sphere(vec3(0, 0, -2), 0.5f, 0.0f, 0.9f);
    spheres.push_back(test_sphere);

    sphere test_sphere2(vec3(1, 0, -1.75), 0.5f, 0.9f);
    spheres.push_back(test_sphere2);

    sphere test_sphere3(vec3(-1, 0, -2.25), 0.5f);
    spheres.push_back(test_sphere3);
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
