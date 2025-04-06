#include "../src/scene.hpp"
#include "../src/image.hpp"

#include <SDL2/SDL.h>
#include <iostream>

 std::random_device RandomGenerator::rd; //static so we dont keep creating new random devices yaknow.
//  std::mt19937 RandomGenerator::gen(rd());
std::mt19937 RandomGenerator::gen(14);

 std::uniform_real_distribution<float> RandomGenerator::uniform(0.0f, 1.0f);
 std::uniform_real_distribution<float> RandomGenerator::negpos(-1.0f,1.0f);



void demo_part1(Scene &scene)
{
    mat4 transform = mat4(1.0f);
    Object *unitSphere = new Object(new Sphere(glm::vec3(0, 0, -2), 1.0f), nullptr);
    scene.objects.push_back(unitSphere);


    Object *sphere2 = new Object(new Sphere(glm::vec3(0.0f, -101.0f, -2), 100.0f), nullptr);
    scene.objects.push_back(sphere2);
}

void specular_scene_better(Scene &scene)
{
    color intensity = color(12); // white light.
    scene.lights.emplace_back(vec3(-1,1.5,0), intensity); // white point light.
    scene.lights.emplace_back(vec3(0,1.5,-2), intensity); // white point light.
    
    Material *diffuse_red = new Lambertian(color(0.7, 0.1, 0.1));
    Material *diffuse_yellow = new Lambertian(color(0.8, 0.8, 0.1));
    Material *diffuse_grey = new Lambertian(color(0.8, 0.8, 0.8));
    Material *diffuse_blue = new Lambertian(color(0.1, 0.1, 0.8));
    
    Material *metallic_red = new Metallic(color(0.9, 0.1, 0.1));
    Material *metallic_green = new Metallic(color(0.1, 0.7, 0.1));
    Material *metallic_blue = new Metallic(color(0.1, 0.1, 0.7));
    
    Material *mirror = new SpecularMaterial(color(0.9, 0.9, 0.9));

    Material *greenish_mirror = new SpecularMaterial(color(0.4, 0.99, 0.4));
    
    Material *emissive_white = new Emissive(color(3));

    emissive_white->albedo = color(0.9,0.9,0.9);
    cout << "light color is " << emissive_white->albedo.x << endl;
    Object *lightPlane= new Object(new SquarePlane(vec3(0,1.5,-2), vec3(0,1,0), 1), emissive_white);
    scene.objects.push_back(lightPlane);

    Object *unitSphere = new Object(new Sphere(glm::vec3(0, 0, -3), 0.6), mirror);
    scene.objects.push_back(unitSphere);

    Object *unitSphere1 = new Object(new Sphere(glm::vec3(1.4, 0, -2.5), 0.6), diffuse_red);
    scene.objects.push_back(unitSphere1);

    Object *unitSphere2 = new Object(new Sphere(glm::vec3(-1.4, 0, -2.5), 0.6), greenish_mirror);
    scene.objects.push_back(unitSphere2);
    // create a box
    // Object *box = new Object(new Box(glm::vec3(-1.0, 0.0, -2.0), glm::vec3(0.0, 1.0, -1.0)), diffuse_red);
    // scene.objects.push_back(box);
    
    // Object *another = new Object(new Sphere(glm::vec3(1, 0, -3), 0.6), metallic_red);
    // scene.objects.push_back(another);
    // Object *sphere2 = new Object(new Sphere(glm::vec3(0.6, 0.6, -2), 0.2f), metallic_red);
    // scene.objects.push_back(sphere2);

    Object *ground = new Object(new SquarePlane(vec3(0,-1,-2), vec3(0,1,0), 3), diffuse_yellow);
    scene.objects.push_back(ground);

    // Object *box = new Object(new Box(vec3(0, 0, -2), vec3(0, 0.4, -2)), diffuse_blue);
    // scene.objects.push_back(box);
}


void specular_scene(Scene &scene)
{
    color intensity = color(12); // white light.
    scene.lights.emplace_back(vec3(-1,1.5,0), intensity); // white point light.
    scene.lights.emplace_back(vec3(0,1.5,-2), intensity); // white point light.
    
    Material *diffuse_red = new Lambertian(color(0.7, 0.1, 0.1));
    Material *diffuse_yellow = new Lambertian(color(0.8, 0.8, 0.1));
    Material *diffuse_grey = new Lambertian(color(0.8, 0.8, 0.8));
    Material *diffuse_blue = new Lambertian(color(0.1, 0.1, 0.8));
    
    Material *metallic_red = new Metallic(color(0.9, 0.2, 0.3), 0.9);
    Material *metallic_green = new Metallic(color(0.4, 0.9, 0.4));
    Material *metallic_blue = new Metallic(color(0.1, 0.1, 0.7));
    
    Material *mirror = new SpecularMaterial(color(0.9, 0.9, 0.9));

    Material *greenish_mirror = new SpecularMaterial(color(0.4, 0.99, 0.4));
    
    Material *emissive_white = new Emissive(color(3));

    emissive_white->albedo = color(0.9,0.9,0.9);
    cout << "light color is " << emissive_white->albedo.x << endl;
    Object *lightPlane= new Object(new SquarePlane(vec3(0,1.5,-2), vec3(0,1,0), 1), emissive_white);
    scene.objects.push_back(lightPlane);

    Object *unitSphere = new Object(new Sphere(glm::vec3(0, 0, -3), 0.6), mirror);
    scene.objects.push_back(unitSphere);

    Object *unitSphere1 = new Object(new Sphere(glm::vec3(1.4, 0, -2.5), 0.6), greenish_mirror);
    scene.objects.push_back(unitSphere1);

    Object *unitSphere2 = new Object(new Sphere(glm::vec3(-1.4, 0, -2.5), 0.6), diffuse_red);
    scene.objects.push_back(unitSphere2);
    // create a box
    // Object *box = new Object(new Box(glm::vec3(-1.0, 0.0, -2.0), glm::vec3(0.0, 1.0, -1.0)), diffuse_red);
    // scene.objects.push_back(box);
    
    // Object *another = new Object(new Sphere(glm::vec3(1, 0, -3), 0.6), metallic_red);
    // scene.objects.push_back(another);
    // Object *sphere2 = new Object(new Sphere(glm::vec3(0.6, 0.6, -2), 0.2f), metallic_red);
    // scene.objects.push_back(sphere2);

    Object *ground = new Object(new SquarePlane(vec3(0,-1,-2), vec3(0,1,0), 3), diffuse_yellow);
    scene.objects.push_back(ground);

    // Object *box = new Object(new Box(vec3(0, 0, -2), vec3(0, 0.4, -2)), diffuse_blue);
    // scene.objects.push_back(box);
}

void part2(Scene &scene){
    mat4 transform = mat4(1.0f);
    
    Object *box = new Object(new Box(glm::vec3(1.0,1.0,-2.0), glm::vec3(2.0,2.0,-4.0)), nullptr );
    Object *plane = new Object(new Plane(glm::vec3(0.0, -1.0, 0.0), -1), nullptr );
    scene.objects.push_back(box);
    scene.objects.push_back(plane);
}


void create_random_sphere(Scene &scene)
{
    vec3 center = vec3(1.5f * (RandomGenerator::randomNegPos()), 2 * RandomGenerator::randomNegPos(),-2.5 + 1.5f * RandomGenerator::randomNegPos());
    float size = 0.7 * RandomGenerator::randomFloat();
    color randcolor = RandomGenerator::randomVec3();
    Material *curmat = new Metallic(randcolor, 0.5*RandomGenerator::randomFloat());
    Object *sphere = new Object(new Sphere(center, size), curmat);
    scene.objects.push_back(sphere);
}

void pathtrace_scene2(Scene &scene)
{
    color intensity = color(12); // white light.
    scene.lights.emplace_back(vec3(-1,1.5,0), intensity); // white point light.
    scene.lights.emplace_back(vec3(0,1.5,-2), intensity); // white point light.
    
    Material *diffuse_red = new Lambertian(color(0.7, 0.1, 0.1));
    Material *diffuse_yellow = new Lambertian(color(0.8, 0.8, 0.1));
    Material *diffuse_grey = new Lambertian(color(0.8, 0.8, 0.8));
    Material *diffuse_blue = new Lambertian(color(0.1, 0.1, 0.8));
    
    Material *metallic_red = new Metallic(color(0.9, 0.2, 0.3), 0.4f);
    Material *metallic_green = new Metallic(color(0.4, 0.9, 0.4));
    Material *metallic_blue = new Metallic(color(0.4, 0.4, 0.9), 0.2f);
    
    Material *mirror = new SpecularMaterial(color(0.9, 0.9, 0.9));
    Material *actual_mirror = new SpecularMaterial(color(0.85, 0.99, 0.85));
    Material *greenish_mirror = new SpecularMaterial(color(0.4, 0.99, 0.4));
    
    Material *emissive_white = new Emissive(color(3)); //DOESNT WORK and DOESNT SET ALBEDO FOR SOME REASON. Maybe cause overwrites.
    emissive_white->albedo = color(100);
    cout << "light color is " << emissive_white->albedo.x << endl;

    float groundpos = -1;
    Object *ground = new Object(new SquarePlane(vec3(0,groundpos,-2), vec3(0,1,0), 5), diffuse_yellow);
    scene.objects.push_back(ground);

    Object *lightPlane= new Object(new SquarePlane(vec3(0,3,-2), vec3(0,-1,0), 5), emissive_white);
    scene.objects.push_back(lightPlane);

    // Object *lightPlane= new Object(new SquarePlane(vec3(0,1.5,-2), vec3(0,-1,0), 0.5), emissive_white);
    // scene.objects.push_back(lightPlane);

    Object *wall = new Object(new SquarePlane(vec3(3,1.5,-2), vec3(-1,0,0), 10), diffuse_grey);
    scene.objects.push_back(wall);

    Object *wall2 = new Object(new SquarePlane(vec3(-3,1.5,-2), vec3(1,0,0), 10), diffuse_grey);
    scene.objects.push_back(wall2);

    Object *backwall = new Object(new SquarePlane(vec3(0,1.5,-7), vec3(0,0,1), 10), actual_mirror);
    scene.objects.push_back(backwall);
    
    Object *frontwall = new Object(new SquarePlane(vec3(0,1.5,1), vec3(0,0,-1), 10), actual_mirror);
    scene.objects.push_back(frontwall);

    float rad = 0.4;
    Object *unitSphere = new Object(new Sphere(glm::vec3(0, groundpos + rad, -3), 0.5), mirror);
    // scene.objects.push_back(unitSphere);

    // Object *unitSphere1 = new Object(new Sphere(glm::vec3(1.4, 0, -2.5), 0.6), metallic_red);
    float rad1 = 0.6;
    Object *unitSphere1 = new Object(new Sphere(glm::vec3(1.4, groundpos + rad1, -2.5), rad1), metallic_red);
    scene.objects.push_back(unitSphere1);

    float rad2 = 0.4;
    Object *unitSphere2 = new Object(new Sphere(glm::vec3(-1.4, groundpos + rad2, -2.5), 0.4), metallic_green);
    // Object *unitSphere2 = new Object(new Sphere(glm::vec3(-1.4, 0, -2.5), 0.6), greenish_mirror);
    scene.objects.push_back(unitSphere2);

  
    vec3 boxCenter(0, groundpos + 0.2, -2);
    vec3 boxDim(0.2,0.2,0.2);
    Object *box = new Object(new Box(boxCenter - boxDim, boxCenter + boxDim), diffuse_blue);
    scene.objects.push_back(box);
    
    // Object *another = new Object(new Sphere(glm::vec3(1, 0, -3), 0.6), metallic_red);
    // scene.objects.push_back(another);
    // Object *sphere2 = new Object(new Sphere(glm::vec3(0.6, 0.6, -2), 0.2f), metallic_red);
    // scene.objects.push_back(sphere2);

  

    // Object *box = new Object(new Box(vec3(0, 0, -2), vec3(0, 0.4, -2)), diffuse_blue);
    // scene.objects.push_back(box);
}
void pathtrace_scene(Scene &scene)
{
    color intensity = color(12); // white light.
    scene.lights.emplace_back(vec3(-1,1.5,0), intensity); // white point light.
    scene.lights.emplace_back(vec3(0,1.5,-2), intensity); // white point light.
    
    Material *diffuse_red = new Lambertian(color(0.7, 0.1, 0.1));
    Material *diffuse_yellow = new Lambertian(color(0.8, 0.8, 0.1));
    Material *diffuse_grey = new Lambertian(color(0.8, 0.8, 0.8));
    Material *diffuse_blue = new Lambertian(color(0.1, 0.1, 0.8));
    
    Material *metallic_red = new Metallic(color(0.9, 0.1, 0.1));
    Material *metallic_green = new Metallic(color(0.1, 0.7, 0.1));
    Material *metallic_blue = new Metallic(color(0.1, 0.1, 0.7));
    
    Material *mirror = new SpecularMaterial(color(0.9, 0.9, 0.9));

    Material *greenish_mirror = new SpecularMaterial(color(0.4, 0.99, 0.4));
    
    Material *emissive_white = new Emissive(color(3));

    emissive_white->albedo = color(0.9,0.9,0.9);
    cout << "light color is " << emissive_white->albedo.x << endl;
    Object *lightPlane= new Object(new SquarePlane(vec3(0,1.5,-2), vec3(0,1,0), 1), emissive_white);
    scene.objects.push_back(lightPlane);

    Object *unitSphere = new Object(new Sphere(glm::vec3(0, 0, -3), 0.6), mirror);
    scene.objects.push_back(unitSphere);

    Object *unitSphere1 = new Object(new Sphere(glm::vec3(1.4, 0, -2.5), 0.6), diffuse_red);
    scene.objects.push_back(unitSphere1);

    Object *unitSphere2 = new Object(new Sphere(glm::vec3(-1.4, 0, -2.5), 0.6), greenish_mirror);
    scene.objects.push_back(unitSphere2);
    // create a box
    // Object *box = new Object(new Box(glm::vec3(-1.0, 0.0, -2.0), glm::vec3(0.0, 1.0, -1.0)), diffuse_red);
    // scene.objects.push_back(box);
    
    // Object *another = new Object(new Sphere(glm::vec3(1, 0, -3), 0.6), metallic_red);
    // scene.objects.push_back(another);
    // Object *sphere2 = new Object(new Sphere(glm::vec3(0.6, 0.6, -2), 0.2f), metallic_red);
    // scene.objects.push_back(sphere2);

    Object *ground = new Object(new SquarePlane(vec3(0,-1,-2), vec3(0,1,0), 3), diffuse_yellow);
    scene.objects.push_back(ground);

    // Object *box = new Object(new Box(vec3(0, 0, -2), vec3(0, 0.4, -2)), diffuse_blue);
    // scene.objects.push_back(box);
}

void part3(Scene &scene)
{
    demo_part1(scene); //sets up sphere objects.
    scene.objects[0]->mat = new Lambertian(color(0.7, 0.1, 0.1));
    scene.objects[1]->mat = new Lambertian(color(0.8, 0.8, 0.1));
    color intensity = color(2, 2, 2); // white light.
    scene.lights.emplace_back(vec3(1,1,1), intensity); // white point light.
    scene.lights.emplace_back(vec3(0,1,-1), intensity); // white point light.
}


int main() {
    int w = 800, h = 600;
    HDRImage image(w, h);
    Scene scene;
    // vec3 target(0,0,-2), eye (0,1,0), up(0,1,0);
    Camera camera(w,h, vec3(0,0,0), vec3(0,0,-1), vec3(0,1,0)); 
    // Camera camera(w,h, target, eye, up); 
    scene.camera = &camera;
    setup_signal_handler();
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL init failed: " << SDL_GetError() << std::endl;
        return 1;
    }
    
    // part3(scene);
    pathtrace_scene2(scene); //sets up the scene with objects and lights for the specular part.
    // specular_scene_better(scene); //sets up the scene with objects and lights for the specular part.
    // specular_scene(scene);
    // Ray trace the image
    sendRays(camera, scene, image, 7); 
    //  run_pathTrace(camera, scene, image); 
    // run_pathTrace_iterative(camera, scene, image, "out/iterative", "metallic", 1000, 20); // make a folder out/iterative (untracked).
    

    // Convert HDRImage to a simple RGBA buffer
    SDL_Surface* tempSurface = SDL_CreateRGBSurface(0, w, h, 32, 0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);
    tonemap(image, tempSurface, 1, 2.2);
    string savepath = "out/yo.png";
    IMG_SavePNG(tempSurface, savepath.c_str()); //make a folder "out" that is untracked in git.
    cout << "saved image to " << savepath << endl;
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
