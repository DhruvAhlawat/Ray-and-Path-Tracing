#include "scene.hpp"
#include "../src/image.hpp"

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

    // Transform the ray into object space
    glm::vec3 new_origin = glm::vec3(worldToModel * glm::vec4(ray.o, 1.0));
    glm::vec3 new_direction = glm::normalize(glm::vec3(worldToModel * glm::vec4(ray.d, 0.0)));
    Ray localRay(new_origin, new_direction);

    bool hit = shape->hit(localRay, t_range, rec);

    if(!hit) return false;
    // otherwise also set the material.    
    rec.mat = this->mat;
    rec.hit = true;


    rec.p = glm::vec3(modelToWorld * vec4(rec.p, 1.0));
    rec.n = glm::normalize(glm::vec3(normalTransform * glm::vec4(rec.n, 0.0)));

    return true;
}

bool Sphere::hit(Ray ray, Interval t_range, HitRecord &rec) const {

    // Compute sphere intersection in object space
    glm::vec3 oc = ray.o - center;
    float a = glm::dot(ray.d, ray.d);
    float b = 2.0f * glm::dot(oc, ray.d);
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

    rec.t = t;
    rec.p = ray.o + t * ray.d;
    
    glm::vec3 localNormal = glm::normalize(ray.o + t * ray.d - center);
    rec.n = localNormal;

    return true;
}


bool Box::hit(Ray ray, Interval t_range, HitRecord &rec) const {
    float t_min = t_range.min, t_max = t_range.max;
    glm::vec3 invD = 1.0f / ray.d;  // Precompute inverse direction
    int hit_axis = -1;

    for (int i = 0; i < 3; i++) {
        float t0 = (min_corner[i] - ray.o[i]) * invD[i];
        float t1 = (max_corner[i] - ray.o[i]) * invD[i];

        if (invD[i] < 0.0f) std::swap(t0, t1);  // Ensure t0 < t1

        if (t0 > t_min) {
            t_min = t0;
            hit_axis = i;
        }
        t_max = std::min(t_max, t1);

        if (t_max <= t_min) return false;  // No intersection
    }

    if (t_min < t_range.min || t_min > t_range.max) return false; // Valid range check

    rec.t = t_min;
    rec.p = ray.o + t_min * ray.d;

    // Compute normal
    glm::vec3 normal(0.0f);
    if (hit_axis != -1) normal[hit_axis] = (rec.p[hit_axis] == min_corner[hit_axis]) ? -1.0f : 1.0f;
    rec.n = normal;

    return true;
}



bool Plane::hit(Ray ray, Interval t_range, HitRecord &rec) const {

    // Plane equation: (P • N) + d = 0
    float denom = glm::dot(normal, ray.d);
    if (fabs(denom) < 1e-6) return false; // Parallel ray

    float t = -(glm::dot(normal, ray.o) + d) / denom;
    if (t < t_range.min || t > t_range.max) return false;

    rec.t = t;
    rec.p = ray.o + t * ray.d;

    rec.n = normal;

    return true;
}

bool SquarePlane::hit(Ray ray, Interval t_range, HitRecord &rec) const {

    // Ray-plane intersection
    float denom = glm::dot(n, ray.d);
    if (fabs(denom) < 1e-6) return false;  // Ray is parallel

    float t = glm::dot(center - ray.o, n) / denom;
    if (t < t_range.min || t > t_range.max) return false;

    // Compute intersection point
    glm::vec3 p = ray.o + t * ray.d;
    
    // Convert to local plane coordinates
    glm::vec3 d = p - center;
    float u_proj = glm::dot(d, u);
    float v_proj = glm::dot(d, v);

    // Check if within square bounds
    if (fabs(u_proj) > s || fabs(v_proj) > s) return false;

    rec.t = t;
    rec.p = p;

    rec.n = n;

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

color Metallic::brdf(const HitRecord &rec, glm::vec3 l, glm::vec3 v) const {
    glm::vec3 reflectDir = glm::reflect(-v, rec.n);

    // Calculate the Fresnel-Schlick term
    float cosTheta = glm::max(glm::dot(glm::normalize(v), glm::normalize(rec.n)), 0.0f);
    color F0 = albedo;  // Metals use their color as base reflectance
    color fresnel = F0 + (color(1.0f) - F0) * pow(1.0f - cosTheta, 5.0f);

    // You can return the Fresnel term directly, or multiply it with the BRDF value
    // In the case of specular reflection, the BRDF is simply a constant multiplied by the Fresnel term
    return fresnel * glm::max(glm::dot(reflectDir, l), 0.0f);
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

    Ray ray(rec.p + bias * l, l); // first move a little in that direction.
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

// new

color singleBouncePixelColor(Ray &ray, Scene &scene)
{
    color c = scene.sky; //= glm::normalize(ray.d) * 0.5f + 0.5f; //original color.
    HitRecord rec = getRayHit(ray, scene);
    if (rec.hit) 
    {
        // cout << "hit at: " << rec.p.x << " " << rec.p.y << " " << rec.p.z << endl;
        // c  = glm::normalize(rec.n) * 0.5f + 0.5f; // for now, just use the normal as color.
        // Now, for no indirect lighting, we can first directly get the radiance from direct scene illumination.
        c = getFixedRadiance(ray, rec, scene);
        c = specularRadiance(ray, rec, scene, 10); // get the color from the ray.
        // c = radiance;
        // Now, we can get the color from the material.
        // c = rec.mat->brdf(rec, glm::normalize(scene.lights[0].location - rec.p), glm::normalize(-ray.d));
    }
    return c;
}

void sendRays(Camera &camera, Scene &scene, HDRImage &image)
{
    // Ray trace the image
    for (int j = 0; j < image.h; j++) {   
        for (int i = 0; i < image.w; i++) {
            float x = 2 * (i + 0.5f) / image.w - 1;
            float y = 1 - 2 * (j + 0.5f) / image.h;
            Ray ray = scene.camera->make_ray(x, y);
            image.pixel(i, j) = singleBouncePixelColor(ray, scene); // get the color from the ray.
        }
    }
}

inline color trace_paths(Ray &ray, Scene &scene, int num_samples)
{
    color acc = color(0.0);
    int recursion_depth = 4;
    float continue_prob = 1 - (1.0f / (float)recursion_depth);
    for(int k = 0; k < num_samples; k++)
    {
        // color c = scene.sky; //= glm::normalize(ray.d) * 0.5f + 0.5f; //original color.
        HitRecord rec = getRayHit(ray, scene);
        if (rec.hit) 
        {
            // cout << "hit at: " << rec.p.x << " " << rec.p.y << " " << rec.p.z << endl;
            // c  = glm::normalize(rec.n) * 0.5f + 0.5f; // for now, just use the normal as color.
            // Now, for no indirect lighting, we can first directly get the radiance from direct scene illumination.
            // c = getFixedRadiance(ray, rec, scene);
            // c = specularRadiance(ray, rec, scene, 10); // get the color from the ray.
            acc += PathTracing(ray, rec,  scene, 0, continue_prob); // get the color from the ray.
            // c = radiance;
            // Now, we can get the color from the material.
            // c = rec.mat->brdf(rec, glm::normalize(scene.lights[0].location - rec.p), glm::normalize(-ray.d));
        }
    }
    // if(acc != color(0.0f))
    // cout << "color: " << acc.x << " " << acc.y << " " << acc.z << endl;
    return acc;
}
void run_pathTrace(Camera &camera, Scene &scene, HDRImage &image)
{
    // Ray trace the image
    int num_samples = 100;
    for (int j = 0; j < image.h; j++) {   
        std::cout<<"\rPercentage completed: " << (float)j/(float)image.h * 100.0f << "%" << std::flush;
        for (int i = 0; i < image.w; i++) {
            float x = 2 * (i + 0.5f) / image.w - 1;
            float y = 1 - 2 * (j + 0.5f) / image.h;
            Ray ray = scene.camera->make_ray(x, y);
            image.pixel(i, j) = trace_paths(ray, scene, num_samples); // get the color from the ray.
            // image.pixel(i, j) = acc/num_samples; // get the color from the ray.
        }
    }
}

void run_pathTrace_iterative(Camera &camera, Scene &scene, HDRImage &image,  string saveFolder, int saveEvery = 20)
{
    // Ray trace the image
    int num_samples = 100;
    for(int sample = 0; sample < num_samples; sample++)
    {
        for (int j = 0; j < image.h; j++) 
        {   
            for (int i = 0; i < image.w; i++) {
                float x = 2 * (i + 0.5f) / image.w - 1;
                float y = 1 - 2 * (j + 0.5f) / image.h;
                Ray ray = scene.camera->make_ray(x, y);
                image.pixel(i, j) += trace_paths(ray, scene, 1); // get the color from the ray.
                // image.pixel(i, j) = acc/num_samples; // get the color from the ray.
            }
        }
        if((sample + 1) % saveEvery == 0)
        {
            for (int j = 0; j < image.h; j++) 
            {   
                for (int i = 0; i < image.w; i++) {
                    // float x = 2 * (i + 0.5f) / image.w - 1;
                    // float y = 1 - 2 * (j + 0.5f) / image.h;
                    // Ray ray = scene.camera->make_ray(x, y);
                    image.pixel(i, j) /= (sample + 1); // get the color from the ray.
                    // image.pixel(i, j) = acc/num_samples; // get the color from the ray.
                }
            }
            SDL_Surface* tempSurface = SDL_CreateRGBSurface(0, image.w, image.h, 32, 0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);
            tonemap(image, tempSurface, 1, 2.2);
            string savepath = saveFolder + "/outavg_" + to_string(sample + 1) + ".png";
            IMG_SavePNG(tempSurface, savepath.c_str()); //make a folder "out" that is untracked in git.
            SDL_FreeSurface(tempSurface);

            for (int j = 0; j < image.h; j++) 
            {   
                for (int i = 0; i < image.w; i++) {
                    // float x = 2 * (i + 0.5f) / image.w - 1;
                    // float y = 1 - 2 * (j + 0.5f) / image.h;
                    // Ray ray = scene.camera->make_ray(x, y);
                    image.pixel(i, j) *= (sample + 1); // get the color from the ray. //to get back to the sum.
                    // image.pixel(i, j) = acc/num_samples; // get the color from the ray.
                }
            }

        }
    }
}
