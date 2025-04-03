#ifndef SCENE_HPP
#define SCENE_HPP

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <vector>
#include<iostream>

using namespace glm; 
using namespace std;
using color = glm::vec3;

class Ray;
class Interval;
class Shape;
class HitRecord;
class Object;
class Material;
class PointLight;
class Camera;

class Scene {
public:
    Camera *camera;
    std::vector<Object*> objects;
    std::vector<Material> materials;
    std::vector<PointLight> lights;
};

class Ray {
public:
    glm::vec3 o, d;
    Ray(glm::vec3 origin, glm::vec3 direction):
        o(origin),
        d(direction) {
    }
    glm::vec3 at(float t) const {
        return o + t*d;
    }
};

class Camera {
public:
    int w, h;
    float aspectRatio;
    Camera (int w, int h)
    {
        this->w = w;
        this->h = h;
        this->aspectRatio = (float)w / (float)h;
    }
    Ray make_ray(float x, float y) const; // screen coordinates in [-1, 1]
};

class Interval {
public:
    float min, max;
    Interval(float min, float max):
        min(min),
        max(max) {
    }
    bool contains(float x) const {
        return min <= x && x <= max;
    }
};

class HitRecord {
public:
    float t;
    bool hit;
    glm::vec3 p, n;
    Material *mat;

    HitRecord()
    {
        hit = false; t = 0; 
    }
};

class Object {
public:
    Shape *shape;
    Material *mat;
    Object(Shape *shape, Material *mat, glm::mat4 M=glm::mat4(1.0)):
        shape(shape),
        mat(mat) {
    }
    bool hit(Ray ray, Interval t_range, HitRecord &rec) const;
};

class Shape {
public:
    virtual bool hit(Ray ray, Interval t_range, HitRecord &rec) const = 0;
};

class Sphere: public Shape {
public:
    glm::vec3 c;
    float r;
    Sphere(glm::vec3 center, float radius):
        c(center),
        r(radius) {
    }
    virtual bool hit(Ray ray, Interval t_range, HitRecord &rec) const;
};

class Plane: public Shape {
    public:
        glm::vec3 normal;  // plane normal
        float d;           // signed distance from origin
    
        Plane(glm::vec3 normal, float d):
            normal(glm::normalize(normal)), d(d) {} 
        
        virtual bool hit(Ray ray, Interval t_range, HitRecord &rec) const;
    };
    

class Box: public Shape {
    public:
        glm::vec3 min_corner;
        glm::vec3 max_corner;
    
        Box(glm::vec3 min_corner, glm::vec3 max_corner)
            : min_corner(min_corner), max_corner(max_corner) {}
    
        virtual bool hit(Ray ray, Interval t_range, HitRecord &rec) const;
    };
    

class Material {
public:
    virtual color emission(const HitRecord &rec, glm::vec3 v) const {
        return glm::vec3(0.0);
    }
    virtual color brdf(const HitRecord &rec, glm::vec3 l, glm::vec3 v) const = 0;
    virtual bool reflection(const HitRecord &rec, glm::vec3 v,
                            glm::vec3 &r, color &kr) const = 0;
};

class Lambertian: public Material {
public:
    color albedo;
    virtual color brdf(const HitRecord &rec, glm::vec3 l, glm::vec3 v) const;
    virtual bool reflection(const HitRecord &rec, glm::vec3 v,
                            glm::vec3 &r, color &kr) const
    {

        return false;
    }

    Lambertian(color albedo):
        albedo(albedo) {
    }

};

class Metallic: public Material 
{
    public:
    color albedo;
    float fuzz;
    virtual color brdf(const HitRecord &rec, glm::vec3 l, glm::vec3 v) const;
    virtual bool reflection(const HitRecord &rec, glm::vec3 v,
                            glm::vec3 &r, color &kr) const
    {
        r = reflect(v, rec.n);
        kr = albedo;
        return true;
    }
    Metallic(color albedo, float fuzz):
        albedo(albedo),
        fuzz(fuzz) {
    }
};

// class Emissive: public Material {
// };

class PointLight {
public:
    glm::vec3 location;
    color intensity;

    PointLight(glm::vec3 location, color intensity):
        location(location),
        intensity(intensity) {}

};


HitRecord getRayHit(const Ray &ray, Scene &scene, Interval t_range = Interval(0, MAXFLOAT));
color getFixedIrradiance(vec3 point, vec3 normal,  Scene &scene);
// color getFixedRadiance(vec3 point, vec3 normal,  Scene &scene, Material *mat);
color getFixedRadiance(Ray &ogRay, HitRecord &rec,  Scene &scene);


#endif
