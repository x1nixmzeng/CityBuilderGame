#include "systems/animationSystem.hpp"
#include "components/bladeComponent.hpp"
#include "components/transformationComponent.hpp"

AnimationSystem::AnimationSystem(Game* game)
    : System(game) {

    init();
}

void AnimationSystem::update(float dt) {

    auto bladeView = registry.view<BladeComponent, TransformationComponent>();
    for (const auto& bladeEntity : bladeView) {
        BladeComponent const& blade = bladeView.get<BladeComponent>(bladeEntity);
        TransformationComponent& t = bladeView.get<TransformationComponent>(bladeEntity);

        static float x = 0.0f;
        x += dt;
        x = fmodf(x + dt * blade.bladeSpeed, 360);

        // TODO: Support multiple rotations
        // Need to rotate along Z for spin:
        // Need to orientate the blade for left/right
        t.setRotation(Vector3(0.0f, 0.0f, 1.0f), x);
    }
}
