#include "Engine.h"
#include "Object.h"
#include <iostream>
#include "ImageDevice.h"
#include "BodyComponent.h"
#include "SpriteComponent.h"
#include "MissileComponent.h"
#include "BounceComponent.h"
#include "PhysicsComponent.h"
#include "CharacterComponent.h"
#include "ControlerComponent.h"
//Todo: MISSLE TRACK
//MISSLE TEXTURE
//MISSLECOMPONENT





int main(int argc, char* argv[]) 
{
    float grassHeight = 100; // choose the height of your ground

    Engine e;
    ImageDevice::load("playerGIGI", "assets/playerGIGI.png");
    ImageDevice::load("grass", "assets/grass.png");
    ImageDevice::load("tree", "assets/tree.png");
    ImageDevice::load("tree1", "assets/tree1.png");
    ImageDevice::load("crate", "assets/crate.png");
    ImageDevice::load("background", "assets/backgroundSky.png");
    ImageDevice::load("bee", "assets/bee.png");
    // ImageDevice::load("dude", "assets/dude.png");

    Object* background = e.addObject();
    background->addComponent<BodyComponent>(400, 300, 800, 600);
    background->addComponent<SpriteComponent>("background");
    
    Object* tree = e.addObject();
    tree->addComponent<BodyComponent>(400, 425, 1100, 400);
    tree->addComponent<SpriteComponent>("tree");


    Object* playerGIGI = e.addObject();
    playerGIGI->addComponent<BodyComponent>(0, 0, 100, 150);
    playerGIGI->addComponent<SpriteComponent>("playerGIGI");
    playerGIGI->addComponent<BounceComponent>();
    playerGIGI->addComponent<CharacterComponent>();
    playerGIGI->addComponent<ControllerComponent>();
    //playerGIGI->addComponent<PhysicsComponent>(0, 0.5f);
    // Object* tree = e.addObject();
    // tree->addComponent<BodyComponent>(0, 0, 50, 50);
    // tree->addComponent<SpriteComponent>("tree");



    Object* crate = e.addObject();
    crate->addComponent<BodyComponent>(675, 455, 250, 200);
    crate->addComponent<SpriteComponent>("crate");

    Object* grass = e.addObject();
    grass->addComponent<BodyComponent>(400, 550, 1000, 400);
    grass->addComponent<SpriteComponent>("grass");
    e.setGroundY(e.getHeight() - grassHeight);

    Object* bee = e.addObject();
    bee->addComponent<BodyComponent>(100, 100, 100, 100);
    bee->addComponent<SpriteComponent>("bee");
    bee->addComponent<MissileComponent>(playerGIGI);

    Object* tree1 = e.addObject();
    tree1->addComponent<BodyComponent>(200, 400, 1250, 600);
    tree1->addComponent<SpriteComponent>("tree1");


    // Object* missile = e.addObject();
    // missile->addComponent<BodyComponent>(100, 100, 200, 40);
    // missile->addComponent<SpriteComponent>("missile");
    // missile->addComponent<MissileComponent>(target);
    // obj->addComponent<ControllerComponent>();

    int lastTime = SDL_GetTicks();
    double fps = 0.0;
    while(true)   
    {
        int frameStart = SDL_GetTicks();

        e.update();
        //e.setView(dude->getX()-400+50, dude->getY()-300+50);

        int currentTime = SDL_GetTicks();
     
        fps = 1000.0f / (currentTime - lastTime);
        lastTime = currentTime;
        // std::cout << "FPS: " << fps << std::endl;
        // std::cout << "Frame Time: " << (currentTime-frameStart) << std::endl;
        SDL_Delay((1000/200) - (currentTime-frameStart)/1000);

    }
    return 0;
}
