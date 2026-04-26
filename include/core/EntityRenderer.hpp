#pragma once

class EntityRenderer {
public:
    EntityRenderer() = default;
    virtual void render() = 0;
    virtual ~EntityRenderer() = default;
};