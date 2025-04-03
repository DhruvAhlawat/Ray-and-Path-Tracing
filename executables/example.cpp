#include "../src/scene.hpp"
#include "../src/image.hpp"

#include <SDL2/SDL.h>
#include <iostream>

 std::random_device RandomGenerator::rd; //static so we dont keep creating new random devices yaknow.
 std::mt19937 RandomGenerator::gen(rd());
 std::uniform_real_distribution<float> RandomGenerator::uniform(0.0f, 1.0f);
 std::uniform_real_distribution<float> RandomGenerator::negpos(-1.0f,1.0f);



void demo_part1(Scene &scene)
{

    Object *unitSphere = new Object(new Sphere(glm::vec3(0, 0, -2), 0.5f), nullptr);
    scene.objects.push_back(unitSphere);


    Object *sphere2 = new Object(new Sphere(glm::vec3(0.6, 0.6, -2), 0.2f), nullptr);
    scene.objects.push_back(sphere2);
}


void specular_scene(Scene &scene)
{
    color intensity = color(2, 2, 2); // white light.
    scene.lights.emplace_back(vec3(1,1,-1), intensity); // white point light.
    scene.lights.emplace_back(vec3(0,1,-1), intensity); // white point light.

    Material *diffuse_red = new Lambertian(color(0.7, 0.1, 0.1));
    Material *diffuse_yellow = new Lambertian(color(0.8, 0.8, 0.1));
    Material *diffuse_grey = new Lambertian(color(0.8, 0.8, 0.8));
    Material *diffuse_blue = new Lambertian(color(0.1, 0.1, 0.8));

    Material *metallic_red = new Metallic(color(0.9, 0.1, 0.1));
    Material *metallic_green = new Metallic(color(0.1, 0.7, 0.1));
    Material *metallic_blue = new Metallic(color(0.1, 0.1, 0.7));
    
    Material *mirror = new SpecularMaterial(color(0.9, 0.9, 0.9));

    Object *unitSphere = new Object(new Sphere(glm::vec3(0.3, 0.2, -2), 1), mirror);
    scene.objects.push_back(unitSphere);


    Object *sphere2 = new Object(new Sphere(glm::vec3(-0.6, -0.2, -0.9), 0.2f), metallic_red);
    scene.objects.push_back(sphere2);


    Object *ground = new Object(new SquarePlane(vec3(0,-1,0), vec3(0,1,0), 6), diffuse_yellow);
    scene.objects.push_back(ground);
}


void pathtrace_scene(Scene &scene)
{
    color intensity = color(2, 2, 2); // white light.
    scene.lights.emplace_back(vec3(1,1,-1), intensity); // white point light.
    scene.lights.emplace_back(vec3(0,1,-1), intensity); // white point light.

    
    Material *diffuse_red = new Lambertian(color(0.7, 0.1, 0.1));
    Material *diffuse_yellow = new Lambertian(color(0.8, 0.8, 0.1));
    Material *diffuse_grey = new Lambertian(color(0.8, 0.8, 0.8));
    Material *diffuse_blue = new Lambertian(color(0.1, 0.1, 0.8));
    
    Material *metallic_red = new Metallic(color(0.9, 0.1, 0.1));
    Material *metallic_green = new Metallic(color(0.1, 0.7, 0.1));
    Material *metallic_blue = new Metallic(color(0.1, 0.1, 0.7));
    
    Material *mirror = new SpecularMaterial(color(0.9, 0.9, 0.9));
    
    Material *emissive_white = new Emissive(color(0.9, 0.9, 0.9));

    emissive_white->albedo = color(0.9,0.9,0.9);
    cout << "light color is " << emissive_white->albedo.x << endl;
    Object *lightPlane= new Object(new SquarePlane(vec3(0,1.5,-2), vec3(0,1,0), 1), emissive_white);
    scene.objects.push_back(lightPlane);

    Object *unitSphere = new Object(new Sphere(glm::vec3(0, 0, -2), 0.6), diffuse_red);
    scene.objects.push_back(unitSphere);
    // Object *sphere2 = new Object(new Sphere(glm::vec3(0.6, 0.6, -2), 0.2f), metallic_red);
    // scene.objects.push_back(sphere2);

    Object *ground = new Object(new SquarePlane(vec3(0,-1,-2), vec3(0,1,0), 8), diffuse_yellow);
    scene.objects.push_back(ground);

    // Object *box = new Object(new Box(vec3(0, 0, -2), vec3(0, 0.4, -2)), diffuse_blue);
    // scene.objects.push_back(box);
}

void part3(Scene &scene)
{
    demo_part1(scene); //sets up sphere objects.
    color intensity = color(2, 2, 2); // white light.
    scene.lights.emplace_back(vec3(1,1,-1), intensity); // white point light.
    scene.lights.emplace_back(vec3(0,1,-1), intensity); // white point light.
}


color singleBouncePixelColor(Ray &ray, Scene &scene)
{
    color c = scene.sky; //= glm::normalize(ray.d) * 0.5f + 0.5f; //original color.
    HitRecord rec = getRayHit(ray, scene);
    if (rec.hit) 
    {
        // cout << "hit at: " << rec.p.x << " " << rec.p.y << " " << rec.p.z << endl;
        // c  = glm::normalize(rec.n) * 0.5f + 0.5f; // for now, just use the normal as color.
        // Now, for no indirect lighting, we can first directly get the radiance from direct scene illumination.
        // c = getFixedRadiance(ray, rec, scene);
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
color trace_paths(Ray &ray, Scene &scene, int num_samples)
{
    color acc = color(0.0);
    int recursion_depth = 10;
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
    if(acc != color(0.0f))
    // cout << "color: " << acc.x << " " << acc.y << " " << acc.z << endl;
    return acc;
}
void run_pathTrace(Camera &camera, Scene &scene, HDRImage &image)
{
    // Ray trace the image
    int num_samples = 100;
    for (int j = 0; j < image.h; j++) {   
        for (int i = 0; i < image.w; i++) {
            float x = 2 * (i + 0.5f) / image.w - 1;
            float y = 1 - 2 * (j + 0.5f) / image.h;
            Ray ray = scene.camera->make_ray(x, y);
            image.pixel(i, j) = trace_paths(ray, scene, num_samples); // get the color from the ray.
            // image.pixel(i, j) = acc/num_samples; // get the color from the ray.
        }
    }
}

int main() {
    int w = 800, h = 600;
    HDRImage image(w, h);
    Scene scene;
    Camera camera(w,h); 
    scene.camera = &camera;

    pathtrace_scene(scene); //sets up the scene with objects and lights for the specular part.
    // Ray trace the image
    // sendRays(camera, scene, image); 
    run_pathTrace(camera, scene, image); 
    
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL init failed: " << SDL_GetError() << std::endl;
        return 1;
    }

    // Convert HDRImage to a simple RGBA buffer
    SDL_Surface* tempSurface = SDL_CreateRGBSurface(0, w, h, 32, 0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);
    tonemap(image, tempSurface, 1, 2.2);
    IMG_SavePNG(tempSurface, "out/out.png"); //make a folder "out" that is untracked in git.
    SDL_FreeSurface(tempSurface);



    // SDL_Window* window = SDL_CreateWindow("Image Display", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, w, h, SDL_WINDOW_SHOWN);
    // SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    // SDL_Texture* texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, w, h);


    // Update texture from surface
    // SDL_UpdateTexture(texture, nullptr, tempSurface->pixels, tempSurface->pitch);
    // Render loop
    // bool quit = false;
    // SDL_Event e;
    // while (!quit) {
    //     while (SDL_PollEvent(&e)) {
    //         if (e.type == SDL_QUIT) quit = true;
    //     }
    //     SDL_RenderClear(renderer);
    //     SDL_RenderCopy(renderer, texture, nullptr, nullptr);
    //     SDL_RenderPresent(renderer);
    //     SDL_Delay(16); // ~60fps idle
    // }

    // SDL_DestroyTexture(texture);
    // SDL_DestroyRenderer(renderer);
    // SDL_DestroyWindow(window);
    // SDL_Quit();
    return 0;
}
