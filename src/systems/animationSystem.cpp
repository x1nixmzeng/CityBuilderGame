#include "systems/animationSystem.hpp"
#include "components/bladeComponent.hpp"
#include "components/meshComponent.hpp"
#include "components/transformationComponent.hpp"
#include <raymath.h>

AnimationSystem::AnimationSystem(Game* game)
    : System(game) {

    init();
}

void AnimationSystem::update(float dt) {

    // Update all anims
    auto meshView = registry.view<MeshComponent>();
    for (auto& meshEntity : meshView) {
        MeshComponent& mesh = meshView.get<MeshComponent>(meshEntity);

        if (mesh.mesh->animCount > 0) {
            ModelAnimation anim = mesh.mesh->anims[0];
            assert(anim.frameCount > 0);

            mesh.mesh->animDelta += dt;

            auto frame = static_cast<int>(std::floorf(mesh.mesh->animDelta * 60.0f));
            UpdateModelAnimation(mesh.mesh->model, anim, frame);
        }
    }

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

        t.isSealed = false;

        t.rotationAxis = Vector3(0.0f, 0.0f, 1.0f);
        t.rotationAngle = x;

        // Step is signed, and will rotate towards movement
        Matrix matRotation = MatrixRotateZ(blade.step * x * DEG2RAD);

        if (blade.pattern == MovementPattern::ForwardBack) {

            // todo: rotate along some axis
            // MatrixRotateZ(x);

            t.rotationAxis = Vector3(0.0f, 1.0f, 0.0f);
            t.rotationAngle = 90.0f;

            Matrix matOrientation = MatrixRotate(Vector3(0.0f, 1.0f, 0.0f), 90.0f * DEG2RAD);

            matRotation = MatrixMultiply(matRotation, matOrientation);
        }
        else {

            // Do nothing. 0 deg around Y axis
        }

        // Recalculate the "seal" ourselves

        Matrix matScale = MatrixScale(t.scale.x, t.scale.y, t.scale.z);
        Matrix matTranslation = MatrixTranslate(t.position.x, t.position.y, t.position.z);

        t.transform = MatrixMultiply(MatrixMultiply(matScale, matRotation), matTranslation);
        t.isSealed = true;
    }
}
