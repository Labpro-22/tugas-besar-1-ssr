#pragma once

class EntityRenderer {
public:
    EntityRenderer() = default;
    virtual void render() = 0;
};