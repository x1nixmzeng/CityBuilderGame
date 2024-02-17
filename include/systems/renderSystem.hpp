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

    template<typename... T>
    inline void renderScene(entt::exclude_t<T...> exclude = {}) const {
        registry.view<MeshComponent, TransformationComponent>(exclude)
            .each([&](const MeshComponent& mesh, const TransformationComponent& transform) {
                // fake the lighting
                for (int i = 0; i < mesh.mesh->model.materialCount; ++i) {
                    mesh.mesh->model.materials[i].shader = shader;
                }

                // todo: rip apart the transformations
                DrawModel(mesh.mesh->model, transform.position, 1.0f, WHITE);
            });
    }

    template<typename... T>
    inline void renderSceneInstanced(entt::exclude_t<T...> exclude = {}) const {
        registry.view<InstancedMeshComponent, TransformationComponent>(exclude)
            .each([&](const InstancedMeshComponent& mesh, const TransformationComponent& transform) {
                for (auto& trans : mesh.transformations) {
                    // todo: rip apart the transformations?
                    // Vector3 finalPos = Vector3Add(trans.position, transform.position);
                    for (int i = 0; i < mesh.mesh->model.materialCount; ++i) {
                        mesh.mesh->model.materials[i].shader = shader;
                    }

                    DrawModel(mesh.mesh->model, trans.position, 1.0f, WHITE);
                    DrawModelWires(mesh.mesh->model, trans.position, 1.0f, BLACK);
                }
            });

        // registry.view<MultiInstancedMeshComponent, TransformationComponent>(exclude)
        //     .each([&](const MultiInstancedMeshComponent& mesh, const TransformationComponent& transform) {
        //         shader->setMatrix4("model", transform.transform);

        //        for (const auto& [name, instances] : mesh.transforms) {
        //            mesh.mesh->renderObjectInstanced(shader, name, instances.instanceBuffer);
        //        }
        //    });
    }

  public:
    RenderSystem(Game* app);

    void update(float dt) override;
};