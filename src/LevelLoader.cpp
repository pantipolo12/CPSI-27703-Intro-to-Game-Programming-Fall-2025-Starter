#include "LevelLoader.h"
#include "tinyxml2.h"
#include <iostream>
#include <unordered_map>

#include "BodyComponent.h"
#include "SpriteComponent.h"
#include "MissileComponent.h"
#include "CharacterComponent.h"
#include "GroundComponent.h"
#include "AnimateComponent.h"

using namespace tinyxml2;

bool LevelLoader::load(const std::string& filename, Engine& engine)
{
    XMLDocument doc;
    if (doc.LoadFile(filename.c_str()) != XML_SUCCESS) {
        std::cerr << "Failed to load level XML: " << filename << std::endl;
        return false;
    }

    XMLElement* level = doc.FirstChildElement("Level");
    if (!level) {
        std::cerr << " Missing <Level> root element." << std::endl;
        return false;
    }

    std::unordered_map<std::string, Object*> idMap;

    // Pass 1 — create all objects and add basic components
    for (XMLElement* objElem = level->FirstChildElement("GameObject");
         objElem; objElem = objElem->NextSiblingElement("GameObject"))
    {

        const char* id = objElem->Attribute("id");
        if (!id) continue;

        Object* obj = engine.addObject();
        idMap[id] = obj;

        // First pass: add GroundComponent before BodyComponent so we can check it
        for (XMLElement* comp = objElem->FirstChildElement();
             comp; comp = comp->NextSiblingElement())
        {
            std::string compName = comp->Name();
            if(compName == "GroundComponent") {
                obj->addComponent<GroundComponent>();
            }
        }
        
        // Second pass: add other components
        for (XMLElement* comp = objElem->FirstChildElement();
             comp; comp = comp->NextSiblingElement())
        {
            std::string compName = comp->Name();

            if (compName == "BodyComponent") {
                float x = comp->FloatAttribute("x", 0);
                float y = comp->FloatAttribute("y", 0);
                float w = comp->FloatAttribute("w", 50);
                float h = comp->FloatAttribute("h", 50);
                b2WorldId world = engine.getWorldId();
                float worldHeight = engine.getWorldHeight();
                
                // Check if dynamic attribute is explicitly set in XML
                bool isDynamic = false;
                if (comp->Attribute("dynamic")) {
                    isDynamic = comp->BoolAttribute("dynamic", false);
                } else {
                    // If not set, check if it's player or bee (they should be dynamic)
                    std::string objId = objElem->Attribute("id") ? objElem->Attribute("id") : "";
                    isDynamic = (objId == "playerGIGI" || objId == "bee");
                }
                
                obj->addComponent<BodyComponent>(world, x, y, w, h, isDynamic, worldHeight);
                // Initialize userData for the BodyComponent
                obj->initializeBodyComponentUserData();
            }
            else if (compName == "SpriteComponent") {
                const char* image = comp->Attribute("image");
                if (image) {
                    SpriteComponent* sprite = obj->addComponent<SpriteComponent>(image);
                    
                    // Parse position and size attributes
                    if (comp->Attribute("x")) {
                        sprite->setX(comp->FloatAttribute("x", 0));
                    }
                    if (comp->Attribute("y")) {
                        sprite->setY(comp->FloatAttribute("y", 0));
                    }
                    if (comp->Attribute("w")) {
                        sprite->setWidth(comp->FloatAttribute("w", 0));
                    }
                    if (comp->Attribute("h")) {
                        sprite->setHeight(comp->FloatAttribute("h", 0));
                    }
                    
                    // Parse parallax attribute
                    if (comp->Attribute("parallax")) {
                        float parallax = comp->FloatAttribute("parallax", 1.0f);
                        sprite->setParallax(parallax);
                    }
                }
            }
            else if (compName == "CharacterComponent") {
                obj->addComponent<CharacterComponent>();
            }
            else if (compName == "AnimateComponent") {
                const char* image = comp->Attribute("image");
                if (image) {
                    int frames = comp->IntAttribute("frames", 1);
                    float time = comp->FloatAttribute("time", 0.1f);
                    obj->addComponent<AnimateComponent>(image, frames, time);
                }
            }
        }

        if (std::string(id) == "playerGIGI")
            engine.setPlayer(obj);
    }

    // Pass 2 — link components with references (like missiles)
    for (XMLElement* objElem = level->FirstChildElement("GameObject");
         objElem; objElem = objElem->NextSiblingElement("GameObject"))
    {
        const char* id = objElem->Attribute("id");
        if (!id) continue;
        Object* obj = idMap[id];

        for (XMLElement* comp = objElem->FirstChildElement("MissileComponent");
             comp; comp = comp->NextSiblingElement("MissileComponent"))
        {
            const char* targetId = comp->Attribute("target");
            if (targetId && idMap.count(targetId))
                obj->addComponent<MissileComponent>(idMap[targetId]);
        }
    }
    std::cout << "Loaded level: " << filename << std::endl;
    return true;
}
