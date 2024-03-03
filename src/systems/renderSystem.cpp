#include "systems/renderSystem.hpp"

#include "components/components.hpp"
#include <resources/configValue.hpp>

#define RLIGHTS_IMPLEMENTATION
#include "misc/rlights.h"

// #if defined(PLATFORM_DESKTOP)
// #define GLSL_VERSION 330
// #else // PLATFORM_ANDROID, PLATFORM_WEB
// #define GLSL_VERSION 100
// #endif

void RenderSystem::init() {

    ConfigValuePtr fs = resourceManager.getResource<ConfigValue>("FOG_SHADER_FS");
    ConfigValuePtr vs = resourceManager.getResource<ConfigValue>("LIGHTING_SHADER_VS");

    // Load basic lighting shader
    shader = LoadShader(vs->value.c_str(), fs->value.c_str());

    // Get some required shader locations
    shader.locs[SHADER_LOC_VECTOR_VIEW] = GetShaderLocation(shader, "viewPos");
    // NOTE: "matModel" location name is automatically assigned on shader loading,
    // no need to get the location again if using that uniform name
    // shader.locs[SHADER_LOC_MATRIX_MODEL] = GetShaderLocation(shader, "matModel");

    // Ambient light level (some basic lighting)
    int ambientLoc = GetShaderLocation(shader, "ambient");

    float data[4] = {1.0f, 0.1f, 0.1f, 1.0f};
    SetShaderValue(shader, ambientLoc, data, SHADER_UNIFORM_VEC4);

    float fogDensity = 0.06f;
    int fogDensityLoc = GetShaderLocation(shader, "fogDensity");
    SetShaderValue(shader, fogDensityLoc, &fogDensity, SHADER_UNIFORM_FLOAT);

    // Create lights
    lights[0] = CreateLight(LIGHT_POINT, {-5, 5, -5}, Vector3Zero(), YELLOW, shader);
    lights[1] = CreateLight(LIGHT_POINT, {5, 5, 5}, Vector3Zero(), RED, shader);
    lights[2] = CreateLight(LIGHT_POINT, {-5, 5, 5}, Vector3Zero(), GREEN, shader);
    lights[3] = CreateLight(LIGHT_POINT, {5, 5, -5}, Vector3Zero(), BLUE, shader);

    thumbnailTarget = LoadRenderTexture(400, 300);

    thumbnailCamera.position = {-3.0f, 3.0f, 3.0f};  // Camera position
    thumbnailCamera.target = {0.0f, 0.4f, 0.0f};     // Camera looking at point
    thumbnailCamera.up = {0.0f, 1.0f, 0.0f};         // Camera up vector (rotation towards target)
    thumbnailCamera.fovy = 45.0f;                    // Camera field-of-view Y
    thumbnailCamera.projection = CAMERA_PERSPECTIVE; // Camera projection type
}

RenderSystem::RenderSystem(Game* game)
    : System(game) {
    init();
}

void RenderSystem::update(float dt) {
    CameraComponent& cameraComponent = registry.get<CameraComponent>(game->camera);

    // Update the shader with the camera view vector (points towards { 0.0f, 0.0f, 0.0f })
    float cameraPos[3] = {cameraComponent.camera.position.x, cameraComponent.camera.position.y, cameraComponent.camera.position.z};
    SetShaderValue(shader, shader.locs[SHADER_LOC_VECTOR_VIEW], cameraPos, SHADER_UNIFORM_VEC3);

    if (IsKeyPressed(KEY_ONE)) {
        lights[0].enabled = !lights[0].enabled;
    }
    if (IsKeyPressed(KEY_TWO)) {
        lights[1].enabled = !lights[1].enabled;
    }
    if (IsKeyPressed(KEY_THREE)) {
        lights[2].enabled = !lights[2].enabled;
    }
    if (IsKeyPressed(KEY_FOUR)) {
        lights[3].enabled = !lights[3].enabled;
    }

    static int subMesh = 0;

    if (IsKeyPressed(KEY_FIVE)) {
        subMesh++;
    }

    bool oldLightEnabled[MAX_LIGHTS];

    for (int i = 0; i < MAX_LIGHTS; i++) {
        oldLightEnabled[i] = lights[i].enabled;
        lights[i].enabled = false;
        UpdateLightValues(shader, lights[i]);
    }

    BeginTextureMode(thumbnailTarget); // Enable drawing to texture
    {
        ClearBackground(WHITE); // Clear texture background

        BeginMode3D(thumbnailCamera); // Begin 3d mode drawing

        // Returns a sorted list of all meshes
        auto allMeshes = resourceManager.getAllResources<MeshRes>();

        if (subMesh >= allMeshes.size()) {
            subMesh = 0;
        }

        auto it = allMeshes.begin();
        std::advance(it, subMesh);

        DrawCube(Vector3Zero(), 1.99f, 1.99f, 1.99f, GRAY);
        DrawCubeWires(Vector3Zero(), 1.99f, 1.99f, 1.99f, BLACK);

        //// Disable the shader for this model (do not do this)
        // for (int i = 0; i < it->second->model.materialCount; ++i) {
        //     it->second->model.materials[i].shader = {};
        // }

        DrawModel(it->second->model, Vector3Zero(), 1.0f, WHITE);
        DrawGrid(10, 1.0f); // Draw a grid

        EndMode3D(); // End 3d mode drawing, returns to orthographic 2d mode

        DrawText(it->first.c_str(), 0, 0, 32, BLACK);

        auto text = std::format("{} of {}", subMesh + 1, allMeshes.size());
        DrawText(text.c_str(), 0, 32, 32, BLACK);
    }
    EndTextureMode(); // End drawing to texture (now we have a texture available for next passes)

    for (int i = 0; i < MAX_LIGHTS; i++) {
        lights[i].enabled = oldLightEnabled[i];
        UpdateLightValues(shader, lights[i]);
    }

    BeginMode3D(cameraComponent.camera);

    renderScene(entt::exclude<DebugComponent>);
    renderSceneInstanced(entt::exclude<DebugComponent>);

    // Get ray and test against objects
    auto ray = GetMouseRay(GetMousePosition(), cameraComponent.camera);
    RayCollision collision = {0};
    std::string collisionName;

    {
        registry.view<MeshComponent, TransformationComponent>(entt::exclude<NoHitTestComponent>)
            .each([&](const MeshComponent& mesh, const TransformationComponent& transform) {
                auto& model = mesh.mesh->model;

                auto finalPos = MatrixMultiply(model.transform, MatrixTranslate(transform.position.x, transform.position.y, transform.position.z));

                for (int i = 0; i < model.meshCount; ++i) {
                    auto meshHitInfo = GetRayCollisionMesh(ray, model.meshes[i], finalPos);
                    if (meshHitInfo.hit) {
                        // Save the closest hit mesh
                        if ((!collision.hit) || (collision.distance > meshHitInfo.distance)) {
                            collision = meshHitInfo;
                            collisionName = mesh.mesh->debugName;
                        }

                        break;
                    }
                }
            });

        registry.view<InstancedMeshComponent>(entt::exclude<NoHitTestComponent>)
            .each([&](const InstancedMeshComponent& mesh) {
                auto& model = mesh.mesh->model;
                for (int i = 0; i < model.meshCount; ++i) {

                    for (const auto& t : mesh.transformations) {
                        auto finalPos = MatrixMultiply(model.transform, MatrixTranslate(t.position.x, t.position.y, t.position.z));
                        auto meshHitInfo = GetRayCollisionMesh(ray, model.meshes[i], finalPos);
                        if (meshHitInfo.hit) {
                            // Save the closest hit mesh
                            if ((!collision.hit) || (collision.distance > meshHitInfo.distance)) {
                                collision = meshHitInfo;
                                collisionName = mesh.mesh->debugName;
                            }

                            break;
                        }
                    }
                }
            });

        if (collision.hit) {
            DrawCube(collision.point, 0.3f, 0.3f, 0.3f, BLUE);
            DrawCubeWires(collision.point, 0.3f, 0.3f, 0.3f, RED);

            Vector3 normalEnd;
            normalEnd.x = collision.point.x + collision.normal.x;
            normalEnd.y = collision.point.y + collision.normal.y;
            normalEnd.z = collision.point.z + collision.normal.z;

            DrawLine3D(collision.point, normalEnd, RED);
        }
    }

    for (int i = 0; i < MAX_LIGHTS; i++) {
        if (lights[i].enabled)
            DrawSphereEx(lights[i].position, 0.2f, 8, 8, lights[i].color);
        else
            DrawSphereWires(lights[i].position, 0.2f, 8, 8, ColorAlpha(lights[i].color, 0.3f));
    }

    EndMode3D();

    if (collision.hit) {
        DrawCube(collision.point, 0.3f, 0.3f, 0.3f, BLUE);
        DrawCubeWires(collision.point, 0.3f, 0.3f, 0.3f, RED);

        Vector3 normalEnd;
        normalEnd.x = collision.point.x + collision.normal.x;
        normalEnd.y = collision.point.y + collision.normal.y;
        normalEnd.z = collision.point.z + collision.normal.z;

        DrawLine3D(collision.point, normalEnd, RED);

        // Draw the text
        auto screenPosition = GetWorldToScreen(collision.point, cameraComponent.camera);

        screenPosition.y -= 24 + 100;

        auto textSize = MeasureText(collisionName.c_str(), 24);
        screenPosition.x -= textSize * 0.5f;

        DrawText(collisionName.c_str(), screenPosition.x, screenPosition.y, 24, RED);
    }

    // NOTE: Render texture must be y-flipped due to default OpenGL coordinates (left-bottom)
    // DrawTextureRec(thumbnailTarget.texture, {0, 0, (float)thumbnailTarget.texture.width, (float)-thumbnailTarget.texture.height}, {0, 0}, WHITE);
}
