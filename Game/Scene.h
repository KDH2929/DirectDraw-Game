#pragma once

class Scene {
public:
    virtual ~Scene() {}

    virtual void Initialize() = 0;
    virtual void Cleanup() = 0;
    virtual void Update(float deltaTime) = 0;
    virtual void Render() = 0;
};
