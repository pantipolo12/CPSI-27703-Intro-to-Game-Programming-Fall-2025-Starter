#include "LevelLoader.h"
#include "tinyxml2.h"
#include <iostream>
#include <unordered_map>

#include "BodyComponent.h"
#include "SpriteComponent.h"
#include "MissileComponent.h"
#include "CharacterComponent.h"
#include "GroundComponent.h"

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

        for (XMLElement* comp = objElem->FirstChildElement();
             comp; comp = comp->NextSiblingElement())
        {
            std::string compName = comp->Name();

            if (compName == "BodyComponent") {
                float x = comp->FloatAttribute("x", 0);
                float y = comp->FloatAttribute("y", 0);
                float w = comp->FloatAttribute("w", 50);
                float h = comp->FloatAttribute("h", 50);
                obj->addComponent<BodyComponent>(x, y, w, h);
            }
            else if (compName == "SpriteComponent") {
                const char* image = comp->Attribute("image");
                if (image) obj->addComponent<SpriteComponent>(image);
            }
            else if (compName == "CharacterComponent") {
                obj->addComponent<CharacterComponent>();
            }
            else if(compName == "GroundComponent") {
                obj->addComponent<GroundComponent>();
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
