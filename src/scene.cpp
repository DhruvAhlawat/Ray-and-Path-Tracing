#include "scene.hpp"

Ray Camera::make_ray(float x, float y) const {
    // FIXME! //brah what is there to fix?
    //ok so there is something to fix, the normalizisation of the direction possibly.
    float image_x = x * aspectRatio; // Adjust X based on aspect ratio
    float image_y = y;
    return Ray(glm::vec3(0,0,0), normalize(glm::vec3(image_x, image_y, -1))); //basically the image is displayed at a location of -z.

}


bool Object::hit(Ray ray, Interval t_range, HitRecord &rec) const
{
    // std::cout << "Object::hit" << std::endl;
    // Check if the ray hits the shape of the object.
    bool hit = shape->hit(ray, t_range, rec);
    if(!hit) return false;
    // otherwise also set the material.
    rec.mat = this->mat;
    rec.hit = true;
    return true;
}

bool Sphere::hit(Ray ray, Interval t_range, HitRecord &rec) const
{
    // Ray-sphere intersection. 
    //First we check distance from center of sphere and ray line. 
    //Then we check if the distance is less than the radius of the sphere.

    vec3 oc = ray.o - c; // vector from ray origin to sphere center
    float a = glm::dot(ray.d, ray.d);
    float b = 2.0f * glm::dot(oc, ray.d);
    float c = glm::dot(oc, oc) - r*r;   
    float discriminant = b*b - 4*a*c;

    if(discriminant < 0) return false; // no intersection
    float t1 = (-b - sqrt(discriminant)) / (2.0f * a); // first root
    float t2 = (-b + sqrt(discriminant)) / (2.0f * a); // second root
    if(t1 > t2) std::swap(t1, t2); // swap if t1 is greater than t2
    if(t1 < t_range.min) t1 = t2; // if t1 is less than min, use t2
    if(t1 < t_range.min || t1 > t_range.max) return false; // if t1 is out of range, return false
    rec.t = t1; // setting hit time

    rec.p = ray.at(t1); // setting hit point
    rec.n = normalize((rec.p - c)); // setting normal
    // rec.mat = mat; // setting material
    return true; // intersection found
}

HitRecord getRayHit(const Ray &ray, Scene &scene, Interval t_range)
{
    HitRecord rec;
    for(auto &object : scene.objects) 
    {
        if(object->hit(ray, t_range, rec)) //updates the hit record object.
        {
            t_range.max = rec.t; // we also update the range for checking next hits.
        }
    }
    return rec; // return the hit record object.
}

