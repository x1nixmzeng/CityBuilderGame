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

    ConfigValuePtr fs = resourceManager.getResource<ConfigValue>("LIGHTING_SHADER_FS");
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

    // Create lights
    lights[0] = CreateLight(LIGHT_POINT, {-5, 5, -5}, Vector3Zero(), YELLOW, shader);
    lights[1] = CreateLight(LIGHT_POINT, {5, 5, 5}, Vector3Zero(), RED, shader);
    lights[2] = CreateLight(LIGHT_POINT, {-5, 5, 5}, Vector3Zero(), GREEN, shader);
    lights[3] = CreateLight(LIGHT_POINT, {5, 5, -5}, Vector3Zero(), BLUE, shader);
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

    for (int i = 0; i < MAX_LIGHTS; i++) {
        UpdateLightValues(shader, lights[i]);
    }

    BeginMode3D(cameraComponent.camera);

    // DrawGrid(10, 1.0f); // Draw a grid

    renderScene(entt::exclude<DebugComponent>);
    renderSceneInstanced(entt::exclude<DebugComponent>);

    for (int i = 0; i < MAX_LIGHTS; i++) {
        if (lights[i].enabled)
            DrawSphereEx(lights[i].position, 0.2f, 8, 8, lights[i].color);
        else
            DrawSphereWires(lights[i].position, 0.2f, 8, 8, ColorAlpha(lights[i].color, 0.3f));
    }

    EndMode3D();
}
