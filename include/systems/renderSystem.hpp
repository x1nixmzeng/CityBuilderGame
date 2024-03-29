#pragma once
#include "events/events.hpp"
#include "systems/system.hpp"

#include "components/instancedMeshComponent.hpp"
#include "components/meshComponent.hpp"
#include "components/transformationComponent.hpp"

#include <raylib.h>
#include <raymath.h>
#include <rlgl.h>

class RenderSystem : public System {
  protected:
    void init() override;

    Shader shader;

    // thumbnail rendering
    RenderTexture2D thumbnailTarget;
    Camera thumbnailCamera = {0};
    std::vector<entt::entity> lights;
    bool wireframe = true;

    void DrawModelInternal(Model model, TransformationComponent const& trans, Color tint) const;

    template<typename... T>
    inline void renderScene(entt::exclude_t<T...> exclude = {}) const {
        registry.view<MeshComponent, TransformationComponent>(exclude)
            .each([&](const MeshComponent& mesh, const TransformationComponent& transform) {
                // Temporarily light this model
                for (int i = 0; i < mesh.mesh->model.materialCount; ++i) {
                    mesh.mesh->model.materials[i].shader = shader;
                }

                DrawModelInternal(mesh.mesh->model, transform, WHITE);

                if (wireframe) {
                    rlEnableWireMode();
                    DrawModelInternal(mesh.mesh->model, transform, BLACK);
                    rlDisableWireMode();
                }
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

                    if (wireframe) {
                        DrawModelWires(mesh.mesh->model, trans.position, 1.0f, BLACK);
                    }
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