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
    color sky;
    Scene()
    {
        sky = color(0.5, 0.7, 1.0); // default sky color
    }
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
        glm::vec3 eye, view, up;
        glm::mat4 camToWorld;
    
        Camera(int w, int h, glm::vec3 eye, glm::vec3 target, glm::vec3 up) 
            : w(w), h(h), aspectRatio((float)w / (float)h), eye(eye), up(glm::normalize(up)) 
        {
            view = glm::normalize(target - eye);  // Compute the forward direction
            glm::vec3 right = glm::normalize(glm::cross(view, up));  // Right vector
            glm::vec3 newUp = glm::cross(right, view);  // Recomputed orthogonal up
    
            // Construct camera-to-world transformation matrix
            camToWorld = glm::mat4(
                glm::vec4(right, 0),
                glm::vec4(newUp, 0),
                glm::vec4(-view, 0),
                glm::vec4(eye, 1)
            );
        }
    
        Ray make_ray(float x, float y) const;
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
    glm::mat4 modelToWorld;
    glm::mat4 worldToModel;
    glm::mat4 normalTransform;

    Shape(glm::mat4 transform) : modelToWorld(transform) {
        worldToModel = glm::inverse(transform);
        normalTransform = glm::transpose(worldToModel);
    }

    virtual bool hit(Ray ray, Interval t_range, HitRecord &rec) const = 0;
};
    

class Sphere : public Shape {
public:
    glm::vec3 center;
    float radius;

    Sphere(glm::vec3 center, float radius, glm::mat4 transform)
        : Shape(transform), center(center), radius(radius) {}

    virtual bool hit(Ray ray, Interval t_range, HitRecord &rec) const override;
};
    

class Plane: public Shape {
    public:
        glm::vec3 normal;  // plane normal
        float d;           // signed distance from origin
    
        Plane(glm::vec3 normal, float d, glm::mat4 transform):
            Shape(transform), normal(glm::normalize(normal)), d(d) {} 
        
        virtual bool hit(Ray ray, Interval t_range, HitRecord &rec) const;
    };
    

class Box: public Shape {
    public:
        glm::vec3 min_corner;
        glm::vec3 max_corner;
    
        Box(glm::vec3 min_corner, glm::vec3 max_corner, glm::mat4 transform)
            : Shape(transform), min_corner(min_corner), max_corner(max_corner) {}
    
        virtual bool hit(Ray ray, Interval t_range, HitRecord &rec) const;
    };

class SquarePlane : public Shape {
public:
    glm::vec3 center;   // Center of square
    glm::vec3 n;   // Normal vector
    glm::vec3 u, v; // Basis vectors
    float s;       // Half-length of the square

    SquarePlane(glm::vec3 center, glm::vec3 normal, float size, mat4 transform)
        : center(center), n(glm::normalize(normal)), s(size), Shape(transform)
    {
        // Create two perpendicular basis vectors (u, v) for the plane
        glm::vec3 temp = (fabs(n.x) > 0.9f) ? glm::vec3(0, 1, 0) : glm::vec3(1, 0, 0);
        u = glm::normalize(glm::cross(temp, n));  // First basis vector
        v = glm::normalize(glm::cross(n, u));     // Second basis vector
    }

    virtual bool hit(Ray ray, Interval t_range, HitRecord &rec) const override;
};

class Material {
public:
    color albedo;
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
    virtual color brdf(const HitRecord &rec, glm::vec3 l, glm::vec3 v) const;
    virtual bool reflection(const HitRecord &rec, glm::vec3 v,
                            glm::vec3 &r, color &kr) const
    {
        r = reflect(v, rec.n);
        kr = albedo;
        return true;
    }
    Metallic(color albedo):
        albedo(albedo) {
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
color specularRadiance(Ray &ogRay, HitRecord &rec,  Scene &scene, int recursionDepth);

#endif
