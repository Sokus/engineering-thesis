#include "bullet.h"

#include <glm/glm.hpp>
#include <cmath>

namespace Game {

    void BulletType::SetDrag(float dragCoefficient) {
        ln1MinusDragCoefficient = log(1 - dragCoefficient);
    }

    float interpolate(float progress, const std::vector<float> &keyframes) {
        
        if(keyframes.size() == 1)
            return keyframes.front();

        if(progress < 0) 
            return keyframes.front();

        if(progress > 1)
            return keyframes.back();

        int intPart = progress*(keyframes.size()-1);
        float realPart = progress*(keyframes.size()-1) - intPart;

        if(intPart >= keyframes.size() - 1) {
            intPart = keyframes.size() - 2;
            realPart = 1;
        }

        return glm::mix(keyframes[intPart], keyframes[intPart+1], realPart);
    }

    Bullet::Bullet() {}

    Bullet::Bullet(const BulletType *type, const ReferenceFrame &referenceFrame, float damage) :
        type(type), referenceFrame(referenceFrame), damage(damage)
    {}

    void Bullet::Update(float dt) {
        referenceFrame.Update(dt);
        referenceFrame.velocity.y += type->gravity * dt;
        referenceFrame.velocity *= 1 + type->ln1MinusDragCoefficient*dt;
        lifetime += dt;
    }

    void Bullet::Draw() const {

        const int totalAnimationFrames = type->animationFrames.x * type->animationFrames.y;
        const int frameNo = static_cast<int>(lifetime / type->animationLength * totalAnimationFrames) % totalAnimationFrames;
        const glm::vec2 frameSize = glm::vec2(type->texture.width, type->texture.height) / glm::vec2(type->animationFrames);

        // 0 when spawned, 1 when at max lifetime
        const float lifetimeProgress = lifetime/type->maxLifetime;
        const float sizeMultiplier = interpolate(lifetimeProgress, type->sizeKeyframes);
        const float alphaMultiplier = interpolate(lifetimeProgress, type->alphaKeyframes);

        Rectangle source, dest;

        source.x = frameSize.x * (frameNo % type->animationFrames.x);
        source.y = frameSize.y * (frameNo / type->animationFrames.x);;
        source.width = frameSize.x;
        source.height = frameSize.y;

        dest.x = referenceFrame.position.x;
        dest.y = referenceFrame.position.y;
        dest.width = type->visibleSize.x*sizeMultiplier;
        dest.height = type->visibleSize.y*sizeMultiplier;

        DrawTexturePro(
            type->texture, 
            source, dest, 
            {type->visibleSize.x*sizeMultiplier/2, type->visibleSize.y*sizeMultiplier/2}, 
            referenceFrame.rotation, 
            {255,255,255,static_cast<unsigned char>(255*alphaMultiplier)}
        );
    }

    bool Bullet::IsAlive() const {
        return lifetime < type->maxLifetime;
    }

    const BulletType *Bullet::GetType() const {
        return type;
    }
}