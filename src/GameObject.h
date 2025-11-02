#pragma once
#include <vector>
#include <memory>
#include <type_traits>
#include <typeinfo>
#include <SDL.h>
#include "Component.h"

class Input;

class GameObject {
    private:
        std::vector<std::unique_ptr<Component>> components;
    public:
        // add a component of type T
        template<typename T, typename... Args>
        T* add(Args&&... args) {
            static_assert(std::is_base_of<Component, T>::value, "T must derive from Component");
            T* comp = new T(std::forward<Args>(args)...);
            comp->setParent(this);
            components.emplace_back(comp);
            return comp;
        }

        // Get a component of Type T
        template<typename T>
        T* get() {
            for (auto& comp : components) {
                if(T* casted = dynamic_cast<T*>(comp.get())) 
                    return casted;
            }
            return nullptr;
        }

        void update(float dt, const Input& input) {
            for(auto& comp : components){
                comp->update(dt, input);
            }
        }

        void draw(SDL_Renderer* renderer) {
            for(auto& comp : components)
                comp->draw(renderer);
        }
};