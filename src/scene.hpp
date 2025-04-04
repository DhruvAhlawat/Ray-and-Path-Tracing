#ifndef SCENE_HPP
#define SCENE_HPP

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <vector>
#include<iostream>
#include <glm/glm.hpp>
#include <random>
#include <glm/gtc/random.hpp>

#include "../src/image.hpp"

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


class RandomGenerator
{
    static std::random_device rd; //static so we dont keep creating new random devices yaknow.
    static std::mt19937 gen; //(rd());
    static std::uniform_real_distribution<float> uniform; //(0.0f, 1.0f);
    static std::uniform_real_distribution<float> negpos;

    public:
    static float randomFloat() {
        return uniform(gen);
    }
    static float randomNegPos() {
        return negpos(gen);
    }
    static vec3 randomVec3() {
        return vec3(randomNegPos(), randomNegPos(), randomNegPos());
    }
    static vec3 randomHemisphere() {
        return vec3(randomNegPos(), randomNegPos(), randomFloat());
    }

};

class Scene {
public:
    Camera *camera;
    std::vector<Object*> objects;
    std::vector<Material> materials;
    std::vector<PointLight> lights;


    color sky;
    Scene()
    {
        // sky = color(0.5, 0.7, 1.0); // default sky color
        sky = color(0.2, 0.4, 0.7); // default sky color
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

    glm::mat4 modelToWorld;
    glm::mat4 worldToModel;
    glm::mat4 normalTransform;

    Object(Shape *shape, Material *mat, glm::mat4 model_to_world = glm::mat4(1.0f)) 
        : shape(shape), mat(mat), modelToWorld(model_to_world) 
    {
        worldToModel = glm::inverse(modelToWorld);
        normalTransform = glm::transpose(worldToModel);
    }

    bool hit(Ray ray, Interval t_range, HitRecord& rec) const;
};


class Shape {
public:
    virtual bool hit(Ray ray, Interval t_range, HitRecord &rec) const = 0;
};
    

class Sphere : public Shape {
public:
    glm::vec3 center;
    float radius;

    Sphere(glm::vec3 center, float radius)
        : center(center), radius(radius) {}

    virtual bool hit(Ray ray, Interval t_range, HitRecord &rec) const override;
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
        :min_corner(min_corner), max_corner(max_corner) {}

    virtual bool hit(Ray ray, Interval t_range, HitRecord &rec) const override;
};

class SquarePlane : public Shape {
public:
    glm::vec3 center;   // Center of square
    glm::vec3 n;   // Normal vector
    glm::vec3 u, v; // Basis vectors
    float s;       // Half-length of the square

    SquarePlane(glm::vec3 center, glm::vec3 normal, float size)
        : center(center), n(glm::normalize(normal)), s(size)
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
    // virtual color emission(const HitRecord &rec, glm::vec3 v) const {
    //     return glm::vec3(0.0);
    // }
    virtual color brdf(const HitRecord &rec, glm::vec3 l, glm::vec3 v) const = 0;
    virtual bool reflection(const HitRecord &rec, glm::vec3 v,
                            glm::vec3 &r, color &kr) const = 0;

    virtual bool emission() const = 0;
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

    virtual bool emission() const
    {
        return false;
    }
    Lambertian(color albedo):
        albedo(albedo) {
    }
};

class SpecularMaterial: public Material {
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
    virtual bool emission() const
    {
        return false;
    }
    SpecularMaterial(color albedo):
        albedo(albedo) {
    }
};


class Metallic: public Material 
{
    public:
    color albedo;
    float roughness;

    virtual color brdf(const HitRecord &rec, glm::vec3 l, glm::vec3 v) const;
    virtual bool reflection(const HitRecord &rec, glm::vec3 v, glm::vec3 &r, color &kr) const override {
        glm::vec3 perfectReflection = glm::reflect(-v, rec.n); // Ideal mirror reflection

        if (roughness > 0.0f) {
            // Sample random direction around perfect reflection
            perfectReflection = sampleBlinnPhong(perfectReflection, roughness);
        }

        r = glm::normalize(perfectReflection);

        // Fresnel-Schlick approximation
        float cosTheta = glm::max(glm::dot(glm::normalize(v), glm::normalize(rec.n)), 0.0f);
        color F0 = albedo; // Metals use their color as base reflectance
        kr = F0 + (1.0f - F0) * pow(1.0f - cosTheta, 5.0f);

        return true;
    }

    virtual bool emission() const
    {
        return false;
    }

    Metallic(color albedo, float roughness=0.0f):
        albedo(albedo),
        roughness(roughness) {
    }

private:
    glm::vec3 sampleBlinnPhong(glm::vec3 perfectReflection, float roughness) const {
        // Sample a random vector around the perfect reflection using a Blinn-Phong distribution
        float alpha = 2.0f / (roughness * roughness) - 2.0f;
        float phi = glm::linearRand(0.0f, 2.0f * static_cast<float>(M_PI));
        float cosTheta = pow(glm::linearRand(0.0f, 1.0f), 1.0f / (alpha + 1.0f));
        float sinTheta = sqrt(1.0f - cosTheta * cosTheta);

        // Convert spherical coordinates to cartesian
        glm::vec3 sample(
            sinTheta * cos(phi),
            sinTheta * sin(phi),
            cosTheta
        );

        return glm::normalize(perfectReflection + roughness * sample);
}
};

class Emissive: public Material 
{
    public:
    color albedo;
    virtual color brdf(const HitRecord &rec, glm::vec3 l, glm::vec3 v) const
    {
        // float costheta = (glm::dot(v, rec.n)/(glm::length(v) * glm::length(rec.n)));
        // return albedo * abs(costheta); // purely emissive material. from any direction it emits albedo 
        return albedo;
    }
    virtual bool reflection(const HitRecord &rec, glm::vec3 v,
                            glm::vec3 &r, color &kr) const
    {
        return false;
    }
    virtual bool emission() const
    {
        return true;
    }

    Emissive(color albedo)
    {
        this->albedo = albedo;
    }
};

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
color PathTracing(Ray &ogRay, HitRecord &rec, Scene &scene, int recursion_depth, const float continueProb);

color singleBouncePixelColor(Ray &ray, Scene &scene);
void sendRays(Camera &camera, Scene &scene, HDRImage &image);
inline color trace_paths(Ray &ray, Scene &scene, int num_samples);
void run_pathTrace(Camera &camera, Scene &scene, HDRImage &image);
void run_pathTrace_iterative(Camera &camera, Scene &scene, HDRImage &image,  string saveFolder, int saveEvery);

#endif
