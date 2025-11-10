#pragma once

#include "Component.h"
#include <vector>
#include <memory>
#include <unordered_map>
#include <string>
#include <iostream>

class Object {
public:
    // Constructor with position and size parameters
    Object(){};
    virtual ~Object(){};
    
    // Core object methods
    virtual void update();
    virtual void render();

    template<typename T, typename... Args>
    T* addComponent(Args&&... args) {
        std::unique_ptr<T> component = std::make_unique<T>(std::forward<Args>(args)...);
        std::string name = typeid(T).name();
        components[name] = std::move(component);
        components[name]->setObject(this);
        return static_cast<T*>(components[name].get());
    }

    template<typename T>
    T* getComponent() {
        std::string name = typeid(T).name();  // Get the component name
        // std::cout << "Getting component: " << name << std::endl;
        if(components.find(name) == components.end()) {
            return nullptr;
        }
        return static_cast<T*>(components[name].get());
    }


 private:  
    std::unordered_map<std::string, std::unique_ptr<Component>> components;


};
