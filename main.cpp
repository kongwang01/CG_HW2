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

    //�P�O�� D = b^2 - 4ac, a = 1,  b = 2d�E(O-C), c = (O-C)�E(O-C)-r^2
    vec3 OC = r.origin() - this->center;
    float dotOCD = dot(dir, OC);
    float OCdotOC = dot(OC, OC);
    float D = (2.0*dotOCD)*(2.0*dotOCD) - 4.0*dot(dir, dir)*(OCdotOC - this->radius*this->radius);

    if(D >= 0) //D�j�󵥩�0, �N�����I
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

int Intersect(vec3 p, vec3 d, hit_record& rec) //�D�@�����u�O�_��������馳���I�A�Ǧ^0�N��L���I
{
    ray trace_r(p, d);

    for (int i = 0; i < spheres.size() ; i++) //�M�����ur�����I�����餧���A�̪񪺨���
    {
        spheres[i].hit(trace_r, 0.01, 1000.0, rec);
    }

    if(rec.t != 0.0) //�N�����ur��Y�Ӫ��馳�I��
    {
        //return Shading(pointlight, lightintensity, ht);
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
    for (int i = 0; i < spheres.size() ; i++) //�M����I�M���������O�_����ê��
    {
        spheres[i].hit(shadow_ray, 0.01, 1000.0, temp_rec);
    }

    if(temp_rec.t != 0.0)
        return vec3(0,0,0);
    else
        return I * max(float(0.0) ,dot(L,rec.normal));

    //return color;
}

vec3 Reflect(vec3 q, vec3 n) //�p��Ϯg��
{
    vec3 r_in(-q.x(), -q.y(), -q.z());
    r_in.make_unit_vector();
    vec3 r_out = (n * 2.0) * dot(n, r_in);
    r_out = r_out - r_in;
    return r_out;
}

vec3 Transmit(vec3 q, vec3 n) //�p���g��
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
    //d.make_unit_vector();
    vec3 local, reflected, transmitted;
    vec3 q; //point
    vec3 n; //normal

    if(step > 5)
        return vec3(0, 0, 0);//black or background
        //return SkyColor(p, d);

    hit_record ht;
    ht.t = 0.0;

    int status = Intersect(p, d, ht);

    //if(status==light_source)
        //return(light_source_color);
    if(status == 0)
        return SkyColor(p, d);

    q = ht.p;
    n = ht.normal; //or get from hit_record
    vec3 r = Reflect(q, n); //�p��Ϯg��
    vec3 t = Transmit(q, n); //�p���g��
    local = Shading(pointlight, lightintensity, ht);
    reflected = trace(q, r, step+1);
    transmitted = trace(q, t, step+1);

    //return(local*(1.0f - ht.w_r)+ reflected*ht.w_r+ transmitted*0.0);//�u����g
    return(local*(1.0f - ht.w_r)+ reflected*ht.w_r) * (1.0f - ht.w_t)+ transmitted*ht.w_t;
}

int main()
{
    //====  �H���гy10���y  ================
    srand((unsigned)time(0));
    int temp_r = 0, temp_x = 0, temp_y = 0, temp_z = 0;
    //vector<sphere> spheres;
    /*for(int i = 0 ; i < 3 ; i++)//�T�j�y
    {
        temp_r = 30;
        temp_x = -7 + (i*7);
        temp_y = 0;
        temp_z = -10;
        vec3 temp_v3((float)temp_x,(float)temp_y,(float)temp_z);
        sphere temp_sphere(temp_v3, ((float)temp_r)/10.0);
        spheres.push_back(temp_sphere);
    }*/
    for(int i = 0 ; i < 20 ; i++)//�ѤU�p�y
    {
        temp_r = rand()%10;
        temp_x = rand()%60;
        temp_y = rand()%2;
        temp_z = rand()%30;
        vec3 temp_v3((((float)temp_x)/10.0f)-3.0f, -0.4f,(((float)temp_z)/10.0f)-3.5f);
        sphere temp_sphere(temp_v3, 0.1f);
        spheres.push_back(temp_sphere);
    }

    //====  ��@ground  ====================
    sphere ground_sphere(vec3(0, -100.5, -2), 100);
    spheres.push_back(ground_sphere);
    //=======================================

    //====  ����  ====================
    sphere test_sphere(vec3(0, 0, -2), 0.5f, 0.0f, 0.9f);
    spheres.push_back(test_sphere);

    sphere test_sphere2(vec3(1, 0, -1.75), 0.5f, 0.9f);
    spheres.push_back(test_sphere2);

    sphere test_sphere3(vec3(-1, 0, -2.25), 0.5f);
    spheres.push_back(test_sphere3);

    sphere test_sphere4(vec3(0, 0, -4), 0.1f);
    spheres.push_back(test_sphere4);
    //=======================================

    int width = 400;
    int height = 200;

    vec3 lower_left_corner(-2, -1, -1);
    vec3 origin(0, 0, 0);
    vec3 horizontal(4, 0, 0);
    vec3 vertical(0, 2, 0);

    //=========  ��Xppm��  ================================
    /*fstream file;
    file.open("ray.ppm", ios::out);
    file << "P3\n" << width << " " << height << "\n255\n";
    for (int j = height - 1; j >= 0; j--) {
        for (int i = 0; i < width; i++) {
        float u = float(i) / float(width);
        float v = float(j) / float(height);
        ray r(origin, lower_left_corner + horizontal*u + vertical*v);
        //vec3 color = cal_color(r);
        vec3 color = trace(r.origin(), r.direction(), 0);

            file << int(color.r() * 255) << " " << int(color.g()  * 255) << " " << int(color.b() * 255) << "\n";
        }
    }*/
    //=========================================================
    //========  ��Xbmp��  ====================================
    //char* pFilename="54.ppm";
    //ifstream ifile;//input ppm file
    ofstream ofile;//output bmp file
    //ifile.open(pFilename,ios::binary);
    //if (!ifile) {
    //    cout<<"open error!"<<endl;
    //}
    ofile.open("rayBMP.bmp",ios::binary);


    string FileType = "p3";//file type-p6
    int Width=width,Height=height,Count = 255;
    //int Width,Height,Count;//count is the number of pixels
    int DataSize,HeadSize,FileSize;
    //ifile>>FileType>>Width>>Height>>Count;//get the para of the ppm file

    //cout<<FileType<<"!"<<Width<<"!"<<Height<<endl;

    DataSize=Width*Height*3;//every pixel need 3 byte to store
    HeadSize=0x36;//54 Bype
    FileSize=HeadSize+DataSize;
    Count=Width*Height;

    //cout<<Count<<endl;


    unsigned char BmpHead[54];

    for(int i=0;i<53;i++){
        BmpHead[i]=0;
    }

    BmpHead[0]=0x42;
    BmpHead[1]=0x4D;//type
    //cout<<FileSize<<endl;

    int SizeNum=2;//the size of bmp file
    while (FileSize!=0) {
        BmpHead[SizeNum++]=FileSize % 256;
        FileSize = FileSize / 256;
    }

    //cout<<BmpHead[2]<<BmpHead[3]<<BmpHead[4]<<BmpHead[5]<<endl;

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
    {ofile << BmpHead[i];
     //cout<<BmpHead[i]<<endl;
    }

    char blue,green,red;

    //ifile.ignore();
    //cout<<"ok" <<Count<<endl;
    /*for(int j = Count;j >=1 ;j--){
        //cout<<"begin"<<endl;
        ifile.get(blue);
        ifile.get(green);
        ifile.get(red);

        ofile<<hex;
        ofile.put(red);
        ofile.put(green);
        ofile.put(blue);
        //cout<<j<<endl;
    }*/

    for (int j = 0; j < height; j++) {
        for (int i = 0; i < width; i++) {
        float u = float(i) / float(width);
        float v = float(j) / float(height);
        ray r(origin, lower_left_corner + horizontal*u + vertical*v);
        vec3 color = trace(r.origin(), r.direction(), 0);

        blue = static_cast<char>(int(color.r() * 255));
        green= static_cast<char>(int(color.g() * 255));
        red= static_cast<char>(int(color.b() * 255));

        ofile<<hex;
        ofile.put(red);
        ofile.put(green);
        ofile.put(blue);
        }
    }

    //ifile.close();
    ofile.close();
    //======================================================

    return 0;
}
