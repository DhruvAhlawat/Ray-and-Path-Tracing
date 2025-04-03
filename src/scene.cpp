#include "scene.hpp"


Ray Camera::make_ray(float x, float y) const {
    float image_x = x * aspectRatio;
    float image_y = y;
    
    glm::vec3 ray_dir = glm::normalize(glm::vec3(image_x, image_y, -1));  // Camera space ray
    ray_dir = glm::vec3(camToWorld * glm::vec4(ray_dir, 0.0f));  // Transform to world space

    return Ray(eye, ray_dir);
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

bool Sphere::hit(Ray ray, Interval t_range, HitRecord &rec) const {
    // Transform the ray into object space
    glm::vec3 new_origin = glm::vec3(worldToModel * glm::vec4(ray.o, 1.0));
    glm::vec3 new_direction = glm::normalize(glm::vec3(worldToModel * glm::vec4(ray.d, 0.0)));
    Ray localRay(new_origin, new_direction);

    // Compute sphere intersection in object space
    glm::vec3 oc = localRay.o - center;
    float a = glm::dot(localRay.d, localRay.d);
    float b = 2.0f * glm::dot(oc, localRay.d);
    float c_val = glm::dot(oc, oc) - radius * radius;
    float D = b * b - 4 * a * c_val;

    if (D < 0) return false;
    float sqrtD = sqrt(D);

    float t = (-b - sqrtD) / (2.0f * a);
    if (t < t_range.min || t > t_range.max) {
        t = (-b + sqrtD) / (2.0f * a);
        if (t < t_range.min || t > t_range.max) {
            return false;
        }
    }

    // Convert intersection point back to world space
    rec.t = t;
    rec.p = glm::vec3(modelToWorld * glm::vec4(localRay.o + t * localRay.d, 1.0));
    
    // Transform the normal using normalTransform
    glm::vec3 localNormal = glm::normalize(localRay.o + t * localRay.d - center);
    rec.n = glm::normalize(glm::vec3(normalTransform * glm::vec4(localNormal, 0.0)));

    return true;
}


bool Box::hit(Ray ray, Interval t_range, HitRecord &rec) const {    // without transform
    float t_min = t_range.min;
    float t_max = t_range.max;
    int hit_axis = -1;

    for (int i = 0; i < 3; i++) {
        if (ray.d[i] == 0.0f) {  // ray parallel to axis
            if (ray.o[i] < min_corner[i] || ray.o[i] > max_corner[i]) {
                return false;
            }
            continue;   
        }

        float invD = 1.0f / ray.d[i];
        float t0 = (min_corner[i] - ray.o[i]) * invD;
        float t1 = (max_corner[i] - ray.o[i]) * invD;

        if (invD < 0) std::swap(t0, t1);

        if (t0 > t_min) {
            t_min = t0;
            hit_axis = i;
        }
        t_max = std::min(t_max, t1);
        if (t_max <= t_min) return false;
    }

    rec.t = t_min;
    rec.p = ray.o + t_min * ray.d;

    glm::vec3 normal(0.0f);
    if (hit_axis != -1) {
        normal[hit_axis] = (rec.p[hit_axis] == min_corner[hit_axis]) ? -1.0f : 1.0f;
    }
    rec.n = normal;

    return true;
}

bool Plane::hit(Ray ray, Interval t_range, HitRecord &rec) const {
    // Transform ray into object space
    glm::vec3 new_origin = glm::vec3(worldToModel * glm::vec4(ray.o, 1.0));
    glm::vec3 new_direction = glm::normalize(glm::vec3(worldToModel * glm::vec4(ray.d, 0.0)));
    Ray localRay(new_origin, new_direction);

    // Plane equation: (P • N) + d = 0
    float denom = glm::dot(normal, localRay.d);
    if (fabs(denom) < 1e-6) return false; // Parallel ray

    float t = -(glm::dot(normal, localRay.o) + d) / denom;
    if (t < t_range.min || t > t_range.max) return false;

    // Convert intersection point back to world space
    rec.t = t;
    rec.p = glm::vec3(modelToWorld * glm::vec4(localRay.o + t * localRay.d, 1.0));

    // Transform normal using inverse transpose
    rec.n = glm::normalize(glm::vec3(normalTransform * glm::vec4(normal, 0.0)));

    return true;
}

bool SquarePlane::hit(Ray ray, Interval t_range, HitRecord &rec) const {
    // Transform ray to object space
    glm::vec3 new_origin = glm::vec3(worldToModel * glm::vec4(ray.o, 1.0));
    glm::vec3 new_direction = glm::normalize(glm::vec3(worldToModel * glm::vec4(ray.d, 0.0)));
    Ray localRay(new_origin, new_direction);

    // Ray-plane intersection
    float denom = glm::dot(n, localRay.d);
    if (fabs(denom) < 1e-6) return false;  // Ray is parallel

    float t = glm::dot(center - localRay.o, n) / denom;
    if (t < t_range.min || t > t_range.max) return false;

    // Compute intersection point
    glm::vec3 p = localRay.o + t * localRay.d;
    
    // Convert to local plane coordinates
    glm::vec3 d = p - center;
    float u_proj = glm::dot(d, u);
    float v_proj = glm::dot(d, v);

    // Check if within square bounds
    if (fabs(u_proj) > s || fabs(v_proj) > s) return false;

    // Convert intersection point back to world space
    rec.t = t;
    rec.p = glm::vec3(modelToWorld * glm::vec4(p, 1.0));

    // Transform normal using inverse transpose
    rec.n = glm::normalize(glm::vec3(normalTransform * glm::vec4(n, 0.0)));

    return true;
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

color Lambertian::brdf(const HitRecord &rec, glm::vec3 l, glm::vec3 v) const
{
    return albedo;
}

color Metallic::brdf(const HitRecord &rec, glm::vec3 l, glm::vec3 v) const
{
    color center = albedo;
    vec3 radius = reflect(v, rec.n);
    return center * glm::max(glm::dot(radius, l), 0.0f); // specular reflection.

}

color SpecularMaterial::brdf(const HitRecord &rec, glm::vec3 l, glm::vec3 v) const
{
    color c = albedo;
    vec3 r = reflect(v, rec.n);
    return c * glm::max(glm::dot(r, l), 0.0f); // specular reflection.

}


color specularRadiance(Ray &ogRay, HitRecord &rec,  Scene &scene, int recursion_depth)
{
    if(recursion_depth <= 0) return scene.sky; //no computations here.
    color result(0.0f), refColor(0.0f);
    float bias = 0.01; // bias to avoid self-shadowing
    vec3 l;
    bool reflection = rec.mat->reflection(rec, ogRay.d, l, refColor); // reflect the ray direction.
    if(!reflection) 
    {
        return getFixedRadiance(ogRay, rec, scene); //if no reflection on this material, then just return radiance from the fixed light sources. FOR NOW. TO CHANGE LATER.
    }
    //otherwise we needa do some reflection, so cast a new ray.

    Ray ray(rec.p + bias * rec.n, l); // first move a little in that direction.
    HitRecord newRec = getRayHit(ray, scene, Interval(bias, MAXFLOAT));
    if(newRec.hit)
    {
        // color incident = rec.mat->brdf(rec, l, ogRay.d) * specularRadiance(ray, newRec, scene, recursionDepth - 1); // get the color from the ray.
        result = refColor * specularRadiance(ray, newRec, scene, recursion_depth - 1); 
    }
    else
    {
        result = refColor * scene.sky; // if no hit, return the sky color.
    }
    return result;
}



color getFixedRadiance(Ray &ogRay, HitRecord &rec,  Scene &scene)
{
    color result(0.0);
    float bias = 0.1; // bias to avoid self-shadowing
    for(auto &light : scene.lights)
    {

        vec3 l = light.location - rec.p;
        float distance = glm::length(l);
        l = normalize(l);
        // HitRecord rec;
        Ray ray(rec.p, l); //first move a little in that direction.
        HitRecord newRec = getRayHit(ray, scene, Interval(bias, distance - bias));
        if(!newRec.hit) // check if the ray hits any object.
        {
            color radiance = light.intensity * glm::max(glm::dot(rec.n, l), 0.0f) / (distance * distance); // add the light color to the result.
            {  // cout << "light intensity: " << light.intensity.x << " " << light.intensity.y << " " << light.intensity.z << endl;
            // cout << "normal: " << normal.x << " " << normal.y << " " << normal.z << endl;
            // cout << "light direction : " << l.x << " " << l.y << " " << l.z << endl;
            // cout << "ray hit, : color = " << radiance.x << " " << radiance.y << " " << radiance.z << endl;
            }
            result += rec.mat->brdf(rec, l, -ogRay.d) * radiance; // multiply the light color with the material color.
            // result += radiance;
        }
    }
    // cout << "result color = " << result.x << " " << result.y << " " << result.z << endl;
    return result;
}

color getFixedIrradiance(vec3 point, vec3 normal,  Scene &scene)
{
    color result(0.0);
    float bias = 0.01; // bias to avoid self-shadowing
    for(auto &light : scene.lights)
    {
        vec3 l = light.location - point;
        float distance = glm::length(l);
        l = normalize(l);
        HitRecord rec;
        Ray ray(point, l); //first move a little in that direction.
        if(!getRayHit(ray, scene, Interval(bias, distance - bias)).hit) // check if the ray hits any object.
        {
            // result += (light.intensity / (distance * distance)) ; // add the light color to the result.
            result += light.intensity * glm::max(glm::dot(normal, l), 0.0f) / (distance * distance); // add the light color to the result.
        }
    }
    return result;
}

glm::vec3 alignToNormal(const glm::vec3 &sample, const glm::vec3 &normal) {
    // Step 1: Construct an orthonormal basis (Tangent, Bitangent, Normal)
    glm::vec3 tangent = glm::normalize(glm::cross(
        (fabs(normal.x) > 0.1f ? glm::vec3(0, 1, 0) : glm::vec3(1, 0, 0)), 
        normal
    ));
    glm::vec3 bitangent = glm::cross(normal, tangent);

    // Step 2: Transform the sample from the Z+ hemisphere to the normal's hemisphere
    return sample.x * tangent + sample.y * bitangent + sample.z * normal;
}

glm::vec3 sampleHemisphereUniform(vec3 &normal) {
    

    // float xi1 = dist(gen);
    // float xi2 = dist(gen);
    
    // float theta = 2.0f * M_PI * xi1;
    // float z = xi2;  // Height
    // float r = sqrt(1.0f - z * z);
    // float x = r * cos(theta);
    // float y = r * sin(theta);
    vec3 out = RandomGenerator::randomHemisphere();
    while(glm::dot(out, out) > 1.0f) // keep generating until we get a point in the hemisphere.
    {
        out = RandomGenerator::randomHemisphere();
    }
    // float x = negpos(gen);
    // float y = negpos(gen);
    // float z = uniform(gen); // Height
    // while(x*x + y*y + z*z > 1.0f) // keep generating until we get a point in the hemisphere.
    // {
    //     x = negpos(gen);
    //     y = negpos(gen);
    //     z = dist(gen); // Height
    // }
    // Normalize the vector to ensure it lies on the unit sphere
    out = normalize(out);  // Returns a vector in the +Z hemisphere
    return alignToNormal(out, normal); // Align the sample to the normal direction instead.
}

color PathTracing(Ray &ogRay, HitRecord &rec, Scene &scene, int recursion_depth, const float continueProb)
{
    color result(0.0f);
    float bias = 0.01; // bias to avoid self-shadowing
    if(rec.mat->emission())
    {
        // cout << "hit a light! with albedo: " << rec.mat->albedo.x << endl;
        return rec.mat->albedo; //ez. just return the color of the emissive material. 
    }

    if(recursion_depth >= 3)
    {
        //check for end case then. 
        if(RandomGenerator::randomFloat() > continueProb) return result; // terminate the path with some probability.
    }
    vec3 l = sampleHemisphereUniform(rec.n); //this is the direction we want to sample in this instance.
    // now we sample in this direction, simple as that.

    Ray ray(rec.p + bias * rec.n, l); // first move a little in that direction.
    HitRecord newRec = getRayHit(ray, scene, Interval(bias, MAXFLOAT));
    if(newRec.hit)
    {
        color lighting = PathTracing(ray, newRec, scene, recursion_depth + 1, continueProb); 
        // if(lighting.x != 0)
        // {
        //     // cout << "illuminated" << endl;
        // }
        result = rec.mat->brdf(rec, l, ogRay.d) * lighting;
    }
    else
    {
        // result += getFixedRadiance(ray, newRec, scene); //gets radiance from the fixed light sources then.
        //then it stays dark.
    }
    return result;
}