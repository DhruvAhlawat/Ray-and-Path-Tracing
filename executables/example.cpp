#include "../src/scene.hpp"
#include "../src/image.hpp"

#include <SDL2/SDL.h>
#include <iostream>


void initializeObjects(Scene &scene)
{
    Object *unitSphere = new Object(new Sphere(glm::vec3(0, 0, -2), 0.5f), nullptr);
    scene.objects.push_back(unitSphere);
    Object *bigSphere = new Object(new Sphere(vec3(0, -101, -2), 100.0f), nullptr);
    scene.objects.push_back(bigSphere);
}


void sendRays(Camera &camera, Scene &scene, HDRImage &image)
{
    // Ray trace the image
    for (int j = 0; j < image.h; j++) {   
        for (int i = 0; i < image.w; i++) {
            float x = 2 * (i + 0.5f) / image.w - 1;
            float y = 1 - 2 * (j + 0.5f) / image.h;
            Ray ray = scene.camera->make_ray(x, y);
            color c = glm::normalize(ray.d) * 0.5f + 0.5f; //original color.
            HitRecord rec = getRayHit(ray, scene);
            if (rec.hit) 
            {
                // If the ray hits an object, get the color from the material
                // c = rec.mat->emission(rec, ray.d);
                c  = glm::normalize(rec.n) * 0.5f + 0.5f; // for now, just use the normal as color.
                // cout << rec.n.x << ", " << rec.n.y << ", " << rec.n.z << endl;


            }
            else
            {
                // c = glm::vec3(0, 0, 0);
            }
            image.pixel(i, j) = c;
        }
    }
}

int main() {
    int w = 800, h = 600;
    HDRImage image(w, h);
    Scene scene;
    Camera camera(w,h); 
    scene.camera = &camera;

    initializeObjects(scene);

    // Ray trace the image
    sendRays(camera, scene, image); 
    
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL init failed: " << SDL_GetError() << std::endl;
        return 1;
    }

    // Convert HDRImage to a simple RGBA buffer
    SDL_Surface* tempSurface = SDL_CreateRGBSurface(0, w, h, 32, 0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);
    tonemap(image, tempSurface, 2.2, 2.2);
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
