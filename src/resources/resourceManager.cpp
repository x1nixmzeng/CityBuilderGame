#include "resources/resourceManager.hpp"

#include "resources/streetPack.hpp"

#include <filesystem>
#include <iostream>
#include <stdexcept>

#include <pugixml.hpp>

using namespace pugi;

ResourceManager::ResourceManager(const std::string& resourceDir) {
    loadResources(resourceDir);
}

ResourceManager::ResourceTypeException::ResourceTypeException(const char* message)
    : message(message) {
}

const char* ResourceManager::ResourceTypeException::what() const noexcept {
    return message;
}

template<>
void ResourceManager::loadResource<Geometry>(const std::string& id, const std::string& filename) {
    resources.emplace(std::piecewise_construct,
                      std::forward_as_tuple(id),
                      std::forward_as_tuple(std::type_index(typeid(Geometry)), ResourcePtr<Geometry>(new MeshGeometry(ModelLoader::load(filename)))));
}

template<>
void ResourceManager::loadResource<Shader>(const std::string& id, const std::string& filename) {
    const std::string& vertexPath = filename + ".vert";
    const std::string& fragmentPath = filename + ".frag";
    const std::string& geometryPath = filename + ".geo";

    ResourcePtr<Shader> shader;
    if (std::filesystem::exists(geometryPath)) {
        shader = ResourcePtr<Shader>(new Shader(vertexPath, fragmentPath, geometryPath));
    }
    else {
        shader = ResourcePtr<Shader>(new Shader(vertexPath, fragmentPath));
    }

    resources.emplace(std::piecewise_construct,
                      std::forward_as_tuple(id),
                      std::forward_as_tuple(std::type_index(typeid(Shader)), shader));
}

template<>
void ResourceManager::loadResource<Texture>(const std::string& id, const std::string& filename, bool alpha) {
    ResourcePtr<Texture> texture(alpha ? new Texture(filename, GL_RGBA) : new Texture(filename));

    resources.emplace(std::piecewise_construct,
                      std::forward_as_tuple(id),
                      std::forward_as_tuple(std::type_index(typeid(Texture)), texture));
}

void ResourceManager::loadResources(const std::string& resourceDir) {
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
        const std::string& filename = resourceDir + resourceNode.attribute("filename").as_string();

        if (type == "model") {
            loadResource<Geometry>(id, filename);
        }
        else if (type == "shader") {
            loadResource<Shader>(id, filename);
        }
        else if (type == "texture") {
            bool rgba = resourceNode.attribute("rgba").as_bool();
            loadResource<Texture>(id, filename, rgba);
        }
        else if (type == "streetPack") {
            const std::string& shaderId = resourceNode.child("shader").attribute("id").as_string();
            const std::string& textureId = resourceNode.child("texture").attribute("id").as_string();

            static struct {
                StreetType type;
                std::string filename;
            } filenames[] = {
                {StreetType::NOT_CONNECTED, "street_not_connected.obj"},
                {          StreetType::END,           "street_end.obj"},
                {        StreetType::CURVE,         "street_curve.obj"},
                {   StreetType::T_CROSSING,    "street_t_crossing.obj"},
                {     StreetType::CROSSING,      "street_crossing.obj"},
                {         StreetType::EDGE,      "street_straight.obj"},
                {   StreetType::CURVE_FULL,    "street_curve_full.obj"}
            };

            StreetPack* pack = new StreetPack();

            for (auto& [type, streetFilename] : filenames) {
                pack->streetGeometries.emplace(type, ModelLoader::load(filename + streetFilename));
            }

            pack->shader = getResource<Shader>(shaderId);
            pack->texture = getResource<Texture>(textureId);

            resources.emplace(std::piecewise_construct,
                              std::forward_as_tuple(id),
                              std::forward_as_tuple(std::type_index(typeid(StreetPack)), ResourcePtr<StreetPack>(pack)));
        }
    }
}

template<typename T>
std::shared_ptr<T> ResourceManager::getResource(const std::string& resourceId) const {
    const std::type_index type = std::type_index(typeid(T));
    const ResourceHolder& resource = resources.at(resourceId);

    if (type != resource.type) {
        std::string message = "Resource could not converted to ";

        throw ResourceTypeException(message.append(type.name()).c_str());
    }

    return std::reinterpret_pointer_cast<T>(resource.data);
}

template ResourcePtr<Texture> ResourceManager::getResource<Texture>(const std::string&) const;
template ResourcePtr<Shader> ResourceManager::getResource<Shader>(const std::string&) const;
template ResourcePtr<Geometry> ResourceManager::getResource<Geometry>(const std::string&) const;
template ResourcePtr<StreetPack> ResourceManager::getResource<StreetPack>(const std::string&) const;