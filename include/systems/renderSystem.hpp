#pragma once
#include "events/events.hpp"
#include "systems/system.hpp"

#include "components/instancedMeshComponent.hpp"
#include "components/meshComponent.hpp"
#include "components/transformationComponent.hpp"

#include <raylib.h>
#include <raymath.h>

#include "misc/rlights.h"

class RenderSystem : public System {
  protected:
    void init() override;

    Shader shader;
    Light lights[MAX_LIGHTS] = {0};

    // thumbnail rendering
    RenderTexture2D thumbnailTarget;
    Camera thumbnailCamera = {0};

    void DrawModelInternal(Model model, TransformationComponent const& trans, Color tint) const;

    template<typename... T>
    inline void renderScene (entt::exclude_t<T...> exclude = {}) const {
        registry.view<MeshComponent, TransformationComponent>(exclude)
            .each([&](const MeshComponent& mesh, const TransformationComponent& transform) {
                // Temporarily light this model
                for (int i = 0; i < mesh.mesh->model.materialCount; ++i) {
                    mesh.mesh->model.materials[i].shader = shader;
                }

                DrawModelInternal(mesh.mesh->model, transform, WHITE);

                // Note: scale is not typically passed around properly
                //DrawModelEx(mesh.mesh->model, transform.position, transform.rotationAxis, transform.rotationAngle, Vector3One(), WHITE);

                //DrawModel(mesh.mesh->model, transform.position, 1.0f, WHITE);
                //DrawModelWires(mesh.mesh->model, transform.position, 1.0f, BLACK);
            });
    }

    template<typename... T>
    inline void renderSceneInstanced(entt::exclude_t<T...> exclude = {}) const {
        registry.view<InstancedMeshComponent>(exclude)
            .each([&](const InstancedMeshComponent& mesh) {
                for (auto& trans : mesh.transformations) {
                    // Temporarily light this model
                    for (int i = 0; i < mesh.mesh->model.materialCount; ++i) {
                        mesh.mesh->model.materials[i].shader = shader;
                    }

                    DrawModel(mesh.mesh->model, trans.position, 1.0f, WHITE);
                    DrawModelWires(mesh.mesh->model, trans.position, 1.0f, BLACK);
                }
            });
    }

  public:
    RenderSystem(Game* app);

    void update(float dt) override;

    RenderTexture2D const& getThumbnail() const {
        return thumbnailTarget;
    }
};