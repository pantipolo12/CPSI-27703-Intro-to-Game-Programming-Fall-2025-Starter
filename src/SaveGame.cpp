#include "SaveGame.h"
#include "Engine.h"
#include "Object.h"
#include "BodyComponent.h"
#include "SpriteComponent.h"
#include "CharacterComponent.h"
#include "GroundComponent.h"
#include "AnimateComponent.h"
#include "MissileComponent.h"
#include "tinyxml2.h"
#include <iostream>
#include <fstream>
#include <unordered_map>
#include <sstream>
#include <ctime>
#ifdef _WIN32
    #include <direct.h>
#else
    #include <sys/stat.h>
    #include <sys/types.h>
#endif

using namespace tinyxml2;

bool SaveGame::save(const std::string& filename, Engine& engine) {
    // Create save directory if it doesn't exist
    size_t lastSlash = filename.find_last_of("/\\");
    if (lastSlash != std::string::npos) {
        std::string dirPath = filename.substr(0, lastSlash);
        #ifdef _WIN32
            _mkdir(dirPath.c_str());
        #else
            mkdir(dirPath.c_str(), 0755);
        #endif
    }
    
    XMLDocument doc;
    
    // Create root element
    XMLElement* root = doc.NewElement("SaveGame");
    doc.InsertFirstChild(root);
    
    // Add metadata
    XMLElement* meta = doc.NewElement("Metadata");
    root->InsertEndChild(meta);
    
    // Save timestamp
    auto now = std::time(nullptr);
    char timeStr[100];
    std::strftime(timeStr, sizeof(timeStr), "%Y-%m-%d %H:%M:%S", std::localtime(&now));
    meta->SetAttribute("timestamp", timeStr);
    meta->SetAttribute("version", "1.0");
    
    // Save world size
    XMLElement* world = doc.NewElement("World");
    root->InsertEndChild(world);
    world->SetAttribute("width", engine.getWorldWidth());
    world->SetAttribute("height", engine.getWorldHeight());
    
    // Save all game objects
    XMLElement* objects = doc.NewElement("Objects");
    root->InsertEndChild(objects);
    
    // Track object IDs
    int objectCounter = 0;
    std::unordered_map<Object*, std::string> objectIdMap;
    
    for (auto& objPtr : engine.getObjects()) {
        Object* obj = objPtr.get();
        if (!obj) continue;
        
        // Generate ID if object doesn't have one
        std::string objId = obj->getId();
        if (objId.empty()) {
            // Try to identify by component type
            if (obj == engine.getPlayer()) {
                objId = "playerGIGI";
            } else if (obj->getComponent<GroundComponent>()) {
                objId = "ground_" + std::to_string(objectCounter++);
            } else if (obj->getComponent<BodyComponent>()) {
                objId = "object_" + std::to_string(objectCounter++);
            } else {
                objId = "object_" + std::to_string(objectCounter++);
            }
        }
        
        objectIdMap[obj] = objId;
        saveGameObject(objects, obj, objId);
    }
    
    // Save the file
    XMLError result = doc.SaveFile(filename.c_str());
    if (result == XML_SUCCESS) {
        std::cout << "Game saved successfully to: " << filename << std::endl;
        return true;
    } else {
        std::cerr << "Failed to save game to: " << filename << " (Error: " << result << ")" << std::endl;
        return false;
    }
}

void SaveGame::saveGameObject(XMLElement* parent, Object* obj, const std::string& id) {
    XMLDocument* doc = parent->GetDocument();
    XMLElement* objElem = doc->NewElement("GameObject");
    parent->InsertEndChild(objElem);
    
    objElem->SetAttribute("id", id.c_str());
    
    // Save BodyComponent
    if (auto* body = obj->getComponent<BodyComponent>()) {
        XMLElement* bodyElem = doc->NewElement("BodyComponent");
        objElem->InsertEndChild(bodyElem);
        
        bodyElem->SetAttribute("x", body->getX());
        bodyElem->SetAttribute("y", body->getY());
        bodyElem->SetAttribute("w", body->getWidth());
        bodyElem->SetAttribute("h", body->getHeight());
        // Check if body is dynamic by checking if it has velocity or is the player
        // We'll save this as an attribute that can be determined during load
        bodyElem->SetAttribute("vx", body->getVx());
        bodyElem->SetAttribute("vy", body->getVy());
        bodyElem->SetAttribute("angle", body->getAngle());
    }
    
    // Save SpriteComponent
    if (auto* sprite = obj->getComponent<SpriteComponent>()) {
        XMLElement* spriteElem = doc->NewElement("SpriteComponent");
        objElem->InsertEndChild(spriteElem);
        
        spriteElem->SetAttribute("image", sprite->getTextureName().c_str());
        spriteElem->SetAttribute("x", sprite->getX());
        spriteElem->SetAttribute("y", sprite->getY());
        spriteElem->SetAttribute("w", sprite->getWidth());
        spriteElem->SetAttribute("h", sprite->getHeight());
        spriteElem->SetAttribute("parallax", sprite->getParallax());
    }
    
    // Save CharacterComponent 
    if (obj->getComponent<CharacterComponent>()) {
        XMLElement* charElem = doc->NewElement("CharacterComponent");
        objElem->InsertEndChild(charElem);
    }
    
    // Save GroundComponent 
    if (obj->getComponent<GroundComponent>()) {
        XMLElement* groundElem = doc->NewElement("GroundComponent");
        objElem->InsertEndChild(groundElem);
    }
    
    // Save AnimateComponent
    if (auto* animate = obj->getComponent<AnimateComponent>()) {
        XMLElement* animateElem = doc->NewElement("AnimateComponent");
        objElem->InsertEndChild(animateElem);
        
        animateElem->SetAttribute("image", animate->getTextureName().c_str());
      
    }
}

bool SaveGame::load(const std::string& filename, Engine& engine) {
    // Check if file exists
    if (!exists(filename)) {
        std::cerr << "Save file does not exist: " << filename << std::endl;
        return false;
    }
    
    XMLDocument doc;
    if (doc.LoadFile(filename.c_str()) != XML_SUCCESS) {
        std::cerr << "Failed to load save file: " << filename << std::endl;
        return false;
    }
    
    XMLElement* root = doc.FirstChildElement("SaveGame");
    if (!root) {
        std::cerr << "Invalid save file format: missing <SaveGame> root element" << std::endl;
        return false;
    }
    
    // Load world size
    XMLElement* world = root->FirstChildElement("World");
    if (world) {
        int width = world->IntAttribute("width", 5000);
        int height = world->IntAttribute("height", 1200);
        engine.setWorldSize(width, height);
    }
    
    // Clear existing objects (except we might want to keep some)
    // For now, we'll assume the level is already loaded and we're just updating positions
    // In a full implementation, you might want to clear and reload everything
    
    // Load objects
    XMLElement* objects = root->FirstChildElement("Objects");
    if (!objects) {
        std::cerr << "Invalid save file: missing <Objects> element" << std::endl;
        return false;
    }
    
    std::unordered_map<std::string, Object*> idMap;
    
    // First pass: create/update objects
    for (XMLElement* objElem = objects->FirstChildElement("GameObject");
         objElem; objElem = objElem->NextSiblingElement("GameObject")) {
        
        const char* id = objElem->Attribute("id");
        if (!id) continue;
        
        // Try to find existing object by ID
        Object* obj = engine.findObjectById(id);
        
        if (!obj) {
            // Object doesn't exist, create it
            obj = engine.addObject();
            obj->setId(id);
        }
        
        idMap[id] = obj;
        loadGameObject(objElem, engine, idMap);
    }
    
    // Second pass: restore relationships (like MissileComponent targets)
    for (XMLElement* objElem = objects->FirstChildElement("GameObject");
         objElem; objElem = objElem->NextSiblingElement("GameObject")) {
        
        const char* id = objElem->Attribute("id");
        if (!id) continue;
        
        Object* obj = idMap[id];
        if (!obj) continue;
        
        // Load MissileComponent with target reference
        for (XMLElement* comp = objElem->FirstChildElement("MissileComponent");
             comp; comp = comp->NextSiblingElement("MissileComponent")) {
            const char* targetId = comp->Attribute("target");
            if (targetId && idMap.count(targetId)) {
                obj->addComponent<MissileComponent>(idMap[targetId]);
            }
        }
    }
    
    // Restore player reference
    XMLElement* playerElem = objects->FirstChildElement("GameObject");
    while (playerElem) {
        const char* id = playerElem->Attribute("id");
        if (id && std::string(id) == "playerGIGI") {
            if (idMap.count(id)) {
                engine.setPlayer(idMap[id]);
            }
            break;
        }
        playerElem = playerElem->NextSiblingElement("GameObject");
    }
    
    std::cout << "Game loaded successfully from: " << filename << std::endl;
    return true;
}

void SaveGame::loadGameObject(XMLElement* objElem, Engine& engine, std::unordered_map<std::string, Object*>& idMap) {
    const char* id = objElem->Attribute("id");
    if (!id) return;
    
    Object* obj = idMap[id];
    if (!obj) return;
    
    b2WorldId world = engine.getWorldId();
    float worldHeight = engine.getWorldHeight();
    
    // Load BodyComponent
    XMLElement* bodyElem = objElem->FirstChildElement("BodyComponent");
    if (bodyElem) {
        float x = bodyElem->FloatAttribute("x", 0);
        float y = bodyElem->FloatAttribute("y", 0);
        float w = bodyElem->FloatAttribute("w", 50);
        float h = bodyElem->FloatAttribute("h", 50);
        bool isDynamic = bodyElem->BoolAttribute("dynamic", false);
        
        // Check if BodyComponent already exists
        BodyComponent* body = obj->getComponent<BodyComponent>();
        if (!body) {
            // Determine if dynamic based on object type or velocity
            if (!bodyElem->Attribute("dynamic")) {
                // Default: player and objects with velocity are dynamic
                std::string objId = obj->getId();
                isDynamic = (objId == "playerGIGI" || objId == "bee" || 
                            bodyElem->FloatAttribute("vx", 0) != 0 || 
                            bodyElem->FloatAttribute("vy", 0) != 0);
            }
            body = obj->addComponent<BodyComponent>(world, x, y, w, h, isDynamic, worldHeight);
            obj->initializeBodyComponentUserData();
        } else {
            // Update existing body
            body->setX(x);
            body->setY(y);
            body->setVx(bodyElem->FloatAttribute("vx", 0));
            body->setVy(bodyElem->FloatAttribute("vy", 0));
            body->setAngle(bodyElem->FloatAttribute("angle", 0));
        }
    }
    
    // Load SpriteComponent
    XMLElement* spriteElem = objElem->FirstChildElement("SpriteComponent");
    if (spriteElem) {
        const char* image = spriteElem->Attribute("image");
        if (image) {
            SpriteComponent* sprite = obj->getComponent<SpriteComponent>();
            if (!sprite) {
                sprite = obj->addComponent<SpriteComponent>(image);
            }
            
            if (spriteElem->Attribute("x")) {
                sprite->setX(spriteElem->FloatAttribute("x", 0));
            }
            if (spriteElem->Attribute("y")) {
                sprite->setY(spriteElem->FloatAttribute("y", 0));
            }
            if (spriteElem->Attribute("w")) {
                sprite->setWidth(spriteElem->FloatAttribute("w", 0));
            }
            if (spriteElem->Attribute("h")) {
                sprite->setHeight(spriteElem->FloatAttribute("h", 0));
            }
            if (spriteElem->Attribute("parallax")) {
                sprite->setParallax(spriteElem->FloatAttribute("parallax", 1.0f));
            }
        }
    }
    
    // Load CharacterComponent
    if (objElem->FirstChildElement("CharacterComponent")) {
        if (!obj->getComponent<CharacterComponent>()) {
            obj->addComponent<CharacterComponent>();
        }
    }
    
    // Load GroundComponent
    if (objElem->FirstChildElement("GroundComponent")) {
        if (!obj->getComponent<GroundComponent>()) {
            obj->addComponent<GroundComponent>();
        }
    }
    
    // Load AnimateComponent
    XMLElement* animateElem = objElem->FirstChildElement("AnimateComponent");
    if (animateElem) {
        const char* image = animateElem->Attribute("image");
        if (image) {
            // Note: Frame count/time should come from level.xml or be saved
            // For now, we'll use defaults
            AnimateComponent* animate = obj->getComponent<AnimateComponent>();
            if (!animate) {
                animate = obj->addComponent<AnimateComponent>(image, 1, 0.1f);
            } else {
                animate->setTexture(image);
            }
        }
    }
}

bool SaveGame::exists(const std::string& filename) {
    std::ifstream file(filename);
    return file.good();
}

