#include <iostream>
#include <fstream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include "vec3.h"
#include "ray.h"

using namespace std;

vec3 colorlist[8] = { vec3(0.8, 0.3, 0.3), vec3(0.3, 0.8, 0.3), vec3(0.3, 0.3, 0.8),
 vec3(0.8, 0.8, 0.3), vec3(0.3, 0.8, 0.8), vec3(0.8, 0.3, 0.8),
 vec3(0.8, 0.8, 0.8), vec3(0.3, 0.3, 0.3) };
vec3 pointlight(-10, 10, 0);
vec3 lightintensity(1, 1, 1);

struct hit_record {
    float t;
    vec3 p;
    vec3 normal;
    float w_r;
    float w_t;
    vec3 matColor;
};

class sphere{
    public:
    sphere() {}
    //sphere(vec3 c, float r) : center(c), radius(r) {};
    //sphere(vec3 c, float r, float w_ri=0.0f, float w_ti=0.0f) : center(c), radius(r), w_r(w_ri), w_t(w_ti) {};
    sphere(vec3 c, float r, vec3 m_c, float w_ri=0.0f, float w_ti=0.0f) : center(c), radius(r), material_color(m_c), w_r(w_ri), w_t(w_ti) {};
    bool hit(const ray& r, float tmin, float tmax, hit_record& rec) const;

    vec3 center;
    float radius;
    float w_r;
    float w_t;
    vec3 material_color;
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
            rec.matColor = this->material_color;
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
        return 1;
    }
    else
    {
        return 0;
    }
}

vec3 Shading(vec3 lightposition,vec3 lightintensity, hit_record& rec)
{
    vec3 L = lightposition - rec.p;
    L.make_unit_vector();
    vec3 I = lightintensity;//intensity of lightsource

    hit_record temp_rec;
    temp_rec.t = 0.0;
    ray shadow_ray((rec.p+ L*0.0001), L);
    for (int i = 0; i < spheres.size() ; i++) //尋找該點和光源之間是否有障礙物
    {
        spheres[i].hit(shadow_ray, 0.01, 1000.0, temp_rec);
    }

    if(temp_rec.t != 0.0)
        return vec3(0,0,0);
    else
        return I * max(float(0.0) ,dot(L,rec.normal))*rec.matColor;
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
    float eta = 1.0f/1.79f;
    vec3 r_in(q.x(), q.y(), q.z());
    r_in.make_unit_vector();

    float N_dot_I = dot(n, r_in);
    float k = 1.0f - eta * eta * (1.0f - N_dot_I * N_dot_I);
    vec3 r_out;
    if (k < 0.0f)
        r_out = vec3(0.0f, 0.0f, 0.0f);
    else
        r_out = r_in * eta - n * (eta * N_dot_I + sqrtf(k));

    return r_out;
}

vec3 trace(vec3 p, vec3 d, int step)
{
    vec3 local, reflected, transmitted;
    vec3 q; //point
    vec3 n; //normal

    if(step > 5)
        return vec3(0, 0, 0);//black or background

    hit_record ht;
    ht.t = 0.0;

    int status = Intersect(p, d, ht);

    if(status == 0)
        return SkyColor(p, d);

    q = ht.p;
    n = ht.normal; //or get from hit_record
    vec3 r = Reflect(q, n); //計算反射光
    vec3 t = Transmit(q, n); //計算折射光
    local = Shading(pointlight, lightintensity, ht);
    reflected = trace(q, r, step+1);
    transmitted = trace(q, t, step+1);

    return(local*(1.0f - ht.w_r)+ reflected*ht.w_r) * (1.0f - ht.w_t)+ transmitted*ht.w_t;
}

int main()
{
    //====  隨機創造10顆球  ================
    srand((unsigned)time(0));
    int temp_r = 0, temp_x = 0, temp_y = 0, temp_z = 0, cindex = 0;
    sphere temp_sphere;
    for(int i = 0 ; i < 20 ; i++)//小球
    {
        temp_r = rand()%10;
        temp_x = rand()%60;
        temp_y = rand()%2;
        temp_z = rand()%30;
        cindex = rand() % 8;
        float rand_reflec = ((float)rand() / (float)(RAND_MAX));
        float rand_refrac = ((float)rand() / (float)(RAND_MAX));
        vec3 temp_v3((((float)temp_x)/10.0f)-3.0f, -0.4f,(((float)temp_z)/10.0f)-3.5f);
        if((i%8) == 0)
            temp_sphere = sphere(temp_v3, 0.1f, colorlist[cindex], 0.0f, rand_refrac);
        else if((i%9) == 0)
            temp_sphere = sphere(temp_v3, 0.1f, colorlist[cindex], rand_reflec);
        else
            temp_sphere = sphere(temp_v3, 0.1f, colorlist[cindex]);

        spheres.push_back(temp_sphere);
    }

    //====  當作ground  ====================
    sphere ground_sphere(vec3(0, -100.5, -2), 100, vec3(1.0f, 1.0f, 1.0f));
    spheres.push_back(ground_sphere);
    //=======================================

    //====  大球測試  ====================
    sphere test_sphere(vec3(0, 0, -2), 0.5f, vec3(1.0f, 1.0f, 1.0f), 0.0f, 0.9f);
    spheres.push_back(test_sphere);

    sphere test_sphere2(vec3(1, 0, -1.75), 0.5f, vec3(1.0f, 1.0f, 1.0f), 0.9f);
    spheres.push_back(test_sphere2);

    sphere test_sphere3(vec3(-1, 0, -2.25), 0.5f, vec3(0.2f, 0.2f, 0.7f));
    spheres.push_back(test_sphere3);

    sphere test_sphere4(vec3(0, 0, -4), 0.1f, vec3(1.0f, 1.0f, 1.0f));
    spheres.push_back(test_sphere4);
    //=======================================

    int width = 400;
    int height = 200;

    vec3 lower_left_corner(-2, -1, -1);
    vec3 origin(0, 0, 0);
    vec3 horizontal(4, 0, 0);
    vec3 vertical(0, 2, 0);

    //========  輸出bmp檔  ====================================
    ofstream ofile;//output bmp file
    ofile.open("rayBMP.bmp",ios::binary);


    string FileType = "p3";//file type-p6
    int Width=width,Height=height,Count = 255;
    //int Width,Height,Count;//count is the number of pixels
    int DataSize,HeadSize,FileSize;

    DataSize=Width*Height*3;//every pixel need 3 byte to store
    HeadSize=0x36;//54 Bype
    FileSize=HeadSize+DataSize;
    Count=Width*Height;


    unsigned char BmpHead[54];

    for(int i=0;i<53;i++){
        BmpHead[i]=0;
    }

    BmpHead[0]=0x42;
    BmpHead[1]=0x4D;//type

    int SizeNum=2;//the size of bmp file
    while (FileSize!=0) {
        BmpHead[SizeNum++]=FileSize % 256;
        FileSize = FileSize / 256;
    }


    BmpHead[0x0A]=0x36;//data begin here
    BmpHead[0x0E]=0x28;//size of bitmap information head

    int FileWidthCount = 0x12;//width
    while (Width != 0)
    {

        BmpHead[FileWidthCount++] = Width % 256;
        Width = Width / 256;

    }
    int FileHeightCount = 0x16;//height
    while (Height != 0)
    {

        BmpHead[FileHeightCount++] = Height % 256;
        Height = Height / 256;
    }

    BmpHead[0x1A]=0x1;//device
    BmpHead[0x1C]=0x18;//every pixel need 3 byte


    int FileSizeCount=0x22;
    while (DataSize!=0) {
        BmpHead[FileSizeCount++]=DataSize%256;
        DataSize=DataSize/256;
    }

    int i,j;

    for (i=0; i<54; i++)
    {
        ofile << BmpHead[i];
    }

    char blue,green,red;

    for (int j = 0; j < height; j++) {
        for (int i = 0; i < width; i++) {
        float u = float(i) / float(width);
        float v = float(j) / float(height);
        ray r(origin, lower_left_corner + horizontal*u + vertical*v);
        vec3 color = trace(r.origin(), r.direction(), 0);
        //cout << color.r() << " , " << color.g() << " , " << color.b() << endl;

        blue = static_cast<char>(int(color.r() * 255));
        green= static_cast<char>(int(color.g() * 255));
        red= static_cast<char>(int(color.b() * 255));

        ofile<<hex;
        ofile.put(red);
        ofile.put(green);
        ofile.put(blue);
        }
    }

    ofile.close();
    //======================================================

    return 0;
}
