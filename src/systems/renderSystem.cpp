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

    ConfigValuePtr fs = resourceManager.getResource<ConfigValue>("TOON_SHADER_FS");
    ConfigValuePtr vs = resourceManager.getResource<ConfigValue>("TOON_SHADER_VS");

    // Load basic lighting shader
    shader = LoadShader(vs->value.c_str(), fs->value.c_str());

    // Get some required shader locations
    shader.locs[SHADER_LOC_VECTOR_VIEW] = GetShaderLocation(shader, "viewPos");
    // NOTE: "matModel" location name is automatically assigned on shader loading,
    // no need to get the location again if using that uniform name
    shader.locs[SHADER_LOC_MATRIX_MODEL] = GetShaderLocation(shader, "matModel");

    // Ambient light level (some basic lighting)
    const float ambient = 0.1f;
    int ambientLoc = GetShaderLocation(shader, "ambient");

    float data[4] = {ambient, ambient, ambient, 1.0f};
    SetShaderValue(shader, ambientLoc, data, SHADER_UNIFORM_VEC4);

    //    // light clamp
    // auto light_clamp_loc = raylib.GetShaderLocation(shader, "light_clamp");
    // raylib.SetShaderValue(shader, light_clamp_loc, &light_clamp,
    //                      raylib.ShaderUniformDataType.SHADER_UNIFORM_FLOAT);
    //// specular shine
    // auto shine_loc = raylib.GetShaderLocation(shader, "shine");
    // raylib.SetShaderValue(shader, shine_loc, &shine_amount,
    //                       raylib.ShaderUniformDataType.SHADER_UNIFORM_FLOAT);

    //// light quantize
    // auto light_quantize_loc = raylib.GetShaderLocation(shader, "light_quantize");
    // raylib.SetShaderValue(shader, light_quantize_loc, &light_quantize,
    //                       raylib.ShaderUniformDataType.SHADER_UNIFORM_INT);

    // float fogDensity = 0.03f; // 0.06f;
    // int fogDensityLoc = GetShaderLocation(shader, "fogDensity");
    // SetShaderValue(shader, fogDensityLoc, &fogDensity, SHADER_UNIFORM_FLOAT);

    lights.clear();


    {
        auto light1 = CreateLight(LIGHT_POINT, {5, 5, 5}, Vector3Zero(), RED, shader);
        auto lightEntity = registry.create();
        registry.emplace<LightComponent>(lightEntity, light1);
        lights.push_back(lightEntity);
    }

  

    thumbnailTarget = LoadRenderTexture(400, 300);

    thumbnailCamera.position = {-3.0f, 3.0f, 3.0f};  // Camera position
    thumbnailCamera.target = {0.0f, 0.4f, 0.0f};     // Camera looking at point
    thumbnailCamera.up = {0.0f, 1.0f, 0.0f};         // Camera up vector (rotation towards target)
    thumbnailCamera.fovy = 45.0f;                    // Camera field-of-view Y
    thumbnailCamera.projection = CAMERA_PERSPECTIVE; // Camera projection type
}

void RenderSystem::DrawModelInternal(Model model, TransformationComponent const& trans, Color tint) const {
    // Ensure the translation matrix is pre-calculated
    assert(trans.isSealed);

    // Combine model transformation matrix (model.transform) with matrix generated by transformation
    model.transform = MatrixMultiply(model.transform, trans.transform);

    for (int i = 0; i < model.meshCount; i++) {
        Color color = model.materials[model.meshMaterial[i]].maps[MATERIAL_MAP_DIFFUSE].color;

        Color colorTint = WHITE;
        colorTint.r = (unsigned char)((((float)color.r / 255.0f) * ((float)tint.r / 255.0f)) * 255.0f);
        colorTint.g = (unsigned char)((((float)color.g / 255.0f) * ((float)tint.g / 255.0f)) * 255.0f);
        colorTint.b = (unsigned char)((((float)color.b / 255.0f) * ((float)tint.b / 255.0f)) * 255.0f);
        colorTint.a = (unsigned char)((((float)color.a / 255.0f) * ((float)tint.a / 255.0f)) * 255.0f);

        model.materials[model.meshMaterial[i]].maps[MATERIAL_MAP_DIFFUSE].color = colorTint;
        DrawMesh(model.meshes[i], model.materials[model.meshMaterial[i]], model.transform);
        model.materials[model.meshMaterial[i]].maps[MATERIAL_MAP_DIFFUSE].color = color;
    }
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
        auto& light = registry.get<LightComponent>(lights[0]);
        light.light.enabled = !light.light.enabled;
    }

    if (IsKeyPressed(KEY_NINE)) {
        wireframe = !wireframe;
    }

    static bool exportAllImages = false;

    if (IsKeyPressed(KEY_SIX)) {
        exportAllImages = true;
    }

    static int subMesh = 0;

    if (IsKeyPressed(KEY_FIVE)) {
        subMesh++;
    }

    // Disable all lighting
    registry.view<LightComponent>()
        .each([&](LightComponent& light) {
            light.oldEnabled = light.light.enabled;
            light.light.enabled = false;
        });

    if (exportAllImages) {

        // Returns a sorted list of all meshes
        auto allMeshes = resourceManager.getAllResources<MeshRes>();

        int i = 0;

        for (auto& m : allMeshes) {

            BeginTextureMode(thumbnailTarget); // Enable drawing to texture
            {
                ClearBackground(WHITE); // Clear texture background

                BeginMode3D(thumbnailCamera); // Begin 3d mode drawing

                DrawCube(Vector3Zero(), 1.99f, 1.99f, 1.99f, GRAY);
                DrawCubeWires(Vector3Zero(), 1.99f, 1.99f, 1.99f, BLACK);

                //// Disable the shader for this model (do not do this)
                // for (int i = 0; i < it->second->model.materialCount; ++i) {
                //     it->second->model.materials[i].shader = {};
                // }

                DrawModel(m.second->model, Vector3Zero(), 1.0f, WHITE);
                DrawGrid(10, 1.0f); // Draw a grid

                EndMode3D(); // End 3d mode drawing, returns to orthographic 2d mode

                DrawText(m.first.c_str(), 0, 0, 32, BLACK);
            }
            EndTextureMode(); // End drawing to texture (now we have a texture available for next passes)

            // Convert to cpu image
            Image dlImage = LoadImageFromTexture(thumbnailTarget.texture);
            ImageFlipVertical(&dlImage);

            auto fileName = std::format("{}.png", m.first);
            ExportImage(dlImage, fileName.c_str());

            // Destroy temp cpu image
            UnloadImage(dlImage);

            ++i;
        }

        exportAllImages = false;
    }

    registry.view<LightComponent>()
        .each([&](LightComponent& light) {
            light.light.enabled = light.oldEnabled;
            UpdateLightValues(shader, light.light);
        });

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

    registry.view<LightComponent>()
        .each([&](const LightComponent& light) {
            if (light.light.enabled)
                DrawSphereEx(light.light.position, 0.2f, 8, 8, light.light.color);
            else
                DrawSphereWires(light.light.position, 0.2f, 8, 8, ColorAlpha(light.light.color, 0.3f));
        });

    // Draw any saw paths
    registry.view<NavBlockComponent>(entt::exclude<NoHitTestComponent>)
        .each([&](const NavBlockComponent& navBlock) {
            if (navBlock.data.allowSaw) {
                Vector3 worldPos = CellToWorldPosition(navBlock.data.cell);
                // Offset slightly towards normal
                worldPos = Vector3Add(worldPos, Vector3(0.0f, 1.0f, 0.0f));
                DrawSphereEx(worldPos, 0.2f, 8, 8, BLACK);
            }
        });

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
