#include "resources/resourceManager.hpp"

#include "resources/configValue.hpp"
#include "resources/enemyTemplate.hpp"
#include "resources/meshLoader.hpp"
#include "resources/meshRes.hpp"
#include "resources/objectLoader.hpp"
#include "resources/tileTemplate.hpp"

#include <filesystem>
#include <iostream>
#include <stdexcept>
#include <utility>

#include <raylib.h>

#include <pugixml.hpp>
#include <string_view>

using namespace pugi;

ResourceManager::ResourceManager(const std::string& resourceDir)
    : resourceDir(resourceDir), objectLoader(*this) {

    loadResources();
}

ResourceManager::ResourceTypeException::ResourceTypeException(const char* message)
    : message(message) {
}

const char* ResourceManager::ResourceTypeException::what() const noexcept {
    return message;
}

void ResourceManager::loadResources() {
    xml_document doc;
    xml_parse_result result = doc.load_file((resourceDir + "resources.xml").c_str());

    if (!result) {
        std::cerr << "ResourceManager::loadResources "
                  << "cannot read xml file \"" << resourceDir << "\"" << std::endl;
        return;
    }

    for (const auto& resourceNode : doc.child("resources").children("resource")) {
        const std::string& type = resourceNode.attribute("type").as_string();

        const std::string& id = resourceNode.attribute("id").as_string();
        const std::string& filename = resourceNode.attribute("filename").as_string();

        if (type == "mesh") {

            auto meshPath = resourceDir + filename;
            MeshResPtr meshRes = MeshLoader::loadMesh(meshPath);
            meshRes->debugName = id;

            setResource(id, meshRes);
        }
        else if (type == "shader") {
            ConfigValue* shader = new ConfigValue();

            auto shaderPath = resourceDir + resourceNode.attribute("filename").as_string();
            shader->value = shaderPath;

            setResource(id, ConfigValuePtr(shader));
        }
        else if (type == "tile_template") {
            TileTemplate* tileTemplate = new TileTemplate();

            auto surfaceAttrib = resourceNode.attribute("surface").as_string();
            tileTemplate->surface = SurfaceFromString(std::string_view(surfaceAttrib));

            auto const allowForward = resourceNode.attribute("allow_forward").as_bool(true);
            auto const allowBackward = resourceNode.attribute("allow_backward").as_bool(true);
            auto const allowLeft = resourceNode.attribute("allow_left").as_bool(true);
            auto const allowRight = resourceNode.attribute("allow_right").as_bool(true);

            tileTemplate->navigation.allowForward = allowForward;
            tileTemplate->navigation.allowBackward = allowBackward;
            tileTemplate->navigation.allowLeft = allowLeft;
            tileTemplate->navigation.allowRight = allowRight;

            if (allowForward && allowBackward && allowLeft && allowRight) {
                std::cout << "Tile template '" << id << "' missing navigation hints" << std::endl;
            }

            tileTemplate->meshName = resourceNode.attribute("mesh").as_string();
            assert(tileTemplate->meshName.empty() == false);
            tileTemplate->meshCrackedName = resourceNode.attribute("mesh_cracked").as_string();

            setResource(id, TileTemplatePtr(tileTemplate));
        }
        else if (type == "enemy_template") {
            EnemyTemplate* enemyTemplate = new EnemyTemplate();

            // todo: type? do we need a type here
            auto surfaceAttrib = resourceNode.attribute("type").as_string();

            auto patternAttrib = resourceNode.attribute("pattern").as_string();
            enemyTemplate->pattern = MovementPatternFromString(patternAttrib);

            enemyTemplate->meshName = resourceNode.attribute("mesh").as_string();
            assert(enemyTemplate->meshName.empty() == false);

            setResource(id, EnemyTemplatePtr(enemyTemplate));
        }
        else if (type == "config_value") {
            ConfigValue* defaultLevel = new ConfigValue();
            defaultLevel->value = resourceNode.attribute("value").as_string();

            setResource(id, ConfigValuePtr(defaultLevel));
        }
        else {
            assert(false);
        }
    }

    // objects
    std::string buildingsPath = resourceDir + "objects";
    for (const auto& entry : std::filesystem::directory_iterator(buildingsPath)) {
        if (entry.path().extension() == ".xml") {
            std::string const id = "object." + entry.path().stem().string();
            ObjectPtr object = objectLoader.loadObject(entry.path().string());
            setResource(id, object);
        }
    }
}
