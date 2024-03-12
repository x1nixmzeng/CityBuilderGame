#include "resources/objectLoader.hpp"

#include "components/components.hpp"
#include "resources/meshLoader.hpp"
#include "resources/resourceManager.hpp"

#include "misc/cells.hpp"

#include <raylib.h>
#include <raymath.h>

#include <sstream>
#include <string>

using namespace pugi;

ObjectLoader::ObjectLoader(ResourceManager& resourceManager)
    : resourceManager(resourceManager) {
}

CellPos ReadI3(const xml_node& node, const char* name) {
    const std::string& position = node.attribute(name).as_string();

    CellPos i3;
    std::stringstream posStream(position);
    posStream >> i3.x;
    posStream >> i3.y;
    posStream >> i3.z;

    return i3;
}

Vector3 ReadF3(const xml_node& node, const char* name) {
    const std::string& position = node.attribute(name).as_string();

    Vector3 f3;
    std::stringstream posStream(position);
    posStream >> f3.x;
    posStream >> f3.y;
    posStream >> f3.z;

    return f3;
}

Surface ReadSurface(const xml_node& node) {
    std::string surfaceAttrib = node.attribute("surface").as_string();

    return SurfaceFromString(surfaceAttrib);
}

template<>
LaraComponent ObjectLoader::loadComponent<LaraComponent>(const xml_node& node) {

    auto initialPos = ReadI3(node, "cell");
    auto offset = ReadF3(node, "offset");
    auto surface = ReadSurface(node);

    return LaraComponent(initialPos, offset, surface);
}

template<>
MeshComponent ObjectLoader::loadComponent<MeshComponent>(const xml_node& node) {
    const std::string& filename = node.attribute("filename").as_string();

    auto meshPath = resourceManager.resourceDir + filename;

    MeshResPtr meshRes = MeshLoader::loadMesh(meshPath);
    return MeshComponent(meshRes);
}

void ReadTrigger(RouteNode& n, const xml_node& node) {

    Trigger triggerData;
    triggerData.value = std::string(node.attribute("value").as_string());

    for (const xml_node& childNode : node.children()) {
        auto name = std::string_view(childNode.name());
        if (name == "cell") {
            triggerData.cellOffset = ReadI3(childNode, "offset");
        }
        else {
            assert(false);
        }
    }

    n.triggers.push_back(triggerData);
}

void ReadRouteNode(RouteNode& n, const xml_node& tileNode) {

    auto key = ReadI3(tileNode, "cell");
    // RouteNode& navPoint = points[key];

    std::string templateString = tileNode.attribute("template").as_string();

    n.templateName = templateString;
    n.cell = key;

    auto triggerNameAttrib = tileNode.attribute("trigger");
    if (triggerNameAttrib) {
        n.trigger = std::string(triggerNameAttrib.as_string());
    }

    auto trapAttrib = tileNode.attribute("is_trap");
    if (trapAttrib) {
        n.trap = trapAttrib.as_bool();
    }

    auto sawAttrib = tileNode.attribute("allow_saw");
    if (sawAttrib) {
        n.allowSaw = sawAttrib.as_bool();
    }

    for (const xml_node& childNode : tileNode.children()) {
        auto name = std::string_view(childNode.name());
        if (name == "trigger_camera") {
            n.cameraTrigger = ReadF3(childNode, "position");
        }
        else if (name == "trigger_level") {
            n.levelTrigger = std::string(childNode.attribute("target").as_string());
        }
        else if (name == "trigger_switch") {
            n.switchTrigger = std::string(childNode.attribute("toggle").as_string());
        }
        else if (name == "enemy") {
            n.enemy = std::string(childNode.attribute("template").as_string());
        }
        else if (name == "trigger") {
            ReadTrigger(n, childNode);
        }
    }
}

template<>
RouteComponent ObjectLoader::loadComponent<RouteComponent>(const xml_node& node) {
    std::vector<RouteNode> tiles;

    for (const xml_node& tileNode : node.children("tile")) {
        RouteNode n;
        ReadRouteNode(n, tileNode);
        tiles.push_back(n);
    }

    return RouteComponent(tiles);
}

template<>
AutoNavComponent ObjectLoader::loadComponent<AutoNavComponent>(const xml_node& node) {
    std::vector<OskEvent> events;

    for (const xml_node& eventNode : node.children("event")) {
        auto id = std::string_view(eventNode.attribute("id").as_string());
        if (id == "left") {
            events.push_back(OskEvent::MoveLeft);
        }
        else if (id == "right") {
            events.push_back(OskEvent::MoveRight);
        }
        else if (id == "forward") {
            events.push_back(OskEvent::MoveForward);
        }
        else if (id == "backward") {
            events.push_back(OskEvent::MoveBackward);
        }
        else if (id == "interact") {
            events.push_back(OskEvent::Interact);
        }
        else {
            assert(false);
        }
    }

    return AutoNavComponent(events);
}

ObjectPtr ObjectLoader::loadObject(const std::string& filename) {
    xml_document doc;
    xml_parse_result result = doc.load_file(filename.c_str());

    if (!result) {
        std::string message = "Failed to load file " + filename;
        std::cout << message << std::endl;

        throw std::runtime_error(message);
    }

    Object* object = new Object();
    for (const auto& node : doc.child("object")) {
        const std::string& name = node.name();

        if (name == "name") {
            object->name = node.text().as_string();
        }
        else if (name == "mesh") {
            object->addComponent<MeshComponent>(loadComponent<MeshComponent>(node));
        }
        else if (name == "route") {
            object->addComponent<RouteComponent>(loadComponent<RouteComponent>(node));
        }
        else if (name == "auto_nav") {
            object->addComponent<AutoNavComponent>(loadComponent<AutoNavComponent>(node));
        }
        else if (name == "lara") {
            object->addComponent<LaraComponent>(loadComponent<LaraComponent>(node));
        }
    }

    return ObjectPtr(object);
}
