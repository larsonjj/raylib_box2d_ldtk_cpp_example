#pragma once

#include <LDtkLoader/Entity.hpp>
#include <box2d/box2d.h>

class Portal {
private:
    Texture2D sprite;
    b2Body *body{};

public:
    Portal(const ldtk::Entity *entity, b2World *physicsWorld);
    ~Portal();
};
