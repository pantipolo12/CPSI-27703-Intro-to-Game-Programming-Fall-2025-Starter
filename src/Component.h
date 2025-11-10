#pragma once

#include <string>

// Forward declaration to avoid circular dependency
class Object;

class Component {
protected:
    Object* object{nullptr};
public:
    Component() = default;
    virtual ~Component() = default;
    
    virtual void update() {}
    virtual void render() {}

    void setObject(Object* object);
    Object* getObject() { return object; }

};
