#include "systems/levelSystem.hpp"

#include "resources/meshRes.hpp"
#include "resources/resourceManager.hpp"
#include "resources/tileTemplate.hpp"

#include "events/events.hpp"

#include "resources/configValue.hpp"
#include "resources/enemyTemplate.hpp"

#include "components/components.hpp"
#include "events/resourceEvents.hpp"

#include "misc/cells.hpp"

#include <raymath.h>

#include <format>
#include <map>

LevelSystem::LevelSystem(Game* game)
    : System(game) {
    init();

    eventDispatcher.sink<ResourceUpdatedEvent>()
        .connect<&LevelSystem::handleResourceUpdatedEvent>(*this);

    eventDispatcher.sink<RequestLevelEvent>()
        .connect<&LevelSystem::handleRequestLevelEvent>(*this);

    eventDispatcher.sink<RequestToggleSwitchEvent>()
        .connect<&LevelSystem::handleRequestToggleSwitchEvent>(*this);

    eventDispatcher.sink<RequestLevelRestart>()
        .connect<&LevelSystem::handleRequestLevelRestart>(*this);
}

void LevelSystem::init() {
    pendingCreate = true;

    levelName = "level_1";

    ConfigValuePtr defaultLevel = resourceManager.getResource<ConfigValue>("DEFAULT_LEVEL");
    if (defaultLevel) {
        levelName = defaultLevel->value;
    }

    // Create static skybox
    auto skyEntity = registry.create();

    registry.emplace<TransformationComponent>(skyEntity, Vector3Zero(), Vector3Zero(), 0.0f, Vector3One());
    MeshResPtr skyMesh = resourceManager.getResource<MeshRes>("SKY_MESH");
    registry.emplace<MeshComponent>(skyEntity, skyMesh);
    registry.emplace<NoHitTestComponent>(skyEntity);
}

void LevelSystem::update(float dt) {
    if (pendingCreate) {
        // must wait for all components to be created - we should defer this to init on construction
        generateLevel();
        pendingCreate = false;
    }
}

void LevelSystem::handleResourceUpdatedEvent(const ResourceUpdatedEvent& e) {
    // todo: trash existing entity?
    pendingCreate = true;
}

void LevelSystem::handleRequestLevelEvent(const RequestLevelEvent& e) {

    levelName = e.name;
    generateLevel();
}

void LevelSystem::handleRequestLevelRestart(const RequestLevelRestart& e) {

    // restart whole level
    generateLevel();
}

void LevelSystem::handleRequestToggleSwitchEvent(const RequestToggleSwitchEvent& e) {

    std::string newState;

    // search for switch components
    auto blocks = registry.view<SwitchBlockComponent>();
    for (auto entity : blocks) {
        auto& blockComponent = registry.get<SwitchBlockComponent>(entity);
        if (blockComponent.key == e.name) {
            // state machine
            if (blockComponent.state == "false") {
                newState = blockComponent.state = "true";

                // set entity transform to true
                addSwitchOn(entity, blockComponent.position);
            }
            else if (blockComponent.state == "true") {
                newState = blockComponent.state = "false";

                // set entity transform to false
                addSwitchOff(entity, blockComponent.position);
            }
        }
    }

    std::map<CellPos, CellPos> movedCells;

    // search for switches
    auto view = registry.view<TransformationComponent, TriggerComponent, NavBlockComponent>();
    for (auto entity : view) {
        // find the named switch
        const auto& trigger = view.get<TriggerComponent>(entity);
        if (trigger.name == e.name) {

            for (const auto& data : trigger.triggerData) {
                // value checker
                if (data.value != newState) {
                    continue;
                }

                auto newCell = trigger.cell;
                newCell += data.cellOffset;

                // Update the"nav mesh
                NavBlockComponent& nav = view.get<NavBlockComponent>(entity);

                // Cache the moved cells; key = old cell, value = new cell
                movedCells[nav.data.cell] = newCell;

                nav.data.cell = newCell;

                // Update the transform
                auto& transform = view.get<TransformationComponent>(entity);
                transform.unseal();
                transform.position = CellToWorldPosition(newCell);
                transform.seal();
            }
        }
    }

    if (movedCells.size() > 0) {
        // Move all enemies to their new cells
        // TODO not moving any translations - is this ok?
        auto enemyView = registry.view<TransformationComponent, BladeComponent>();
        for (auto entity : enemyView) {
            auto& blade = enemyView.get<BladeComponent>(entity);

            for (auto const& p : movedCells) {
                if (blade.initialCell == p.first) {
                    blade.initialCell = p.second;
                    break;
                }
            }
        }
    }

    puts(std::format("Toggle switch {}: {}\n", e.name, newState).c_str());
}

void LevelSystem::generateLevel() {

    for (const entt::entity entity : dynamicEntities) {
        if (registry.valid(entity)) {
            registry.destroy(entity);
        }
    }

    dynamicEntities.clear();

    std::string resourceName;
    resourceName = std::format("object.{}", levelName);

    ObjectPtr level1 = resourceManager.getResource<Object>(resourceName);

    // do we need this as a registry item?
    auto level1Instance = level1->create(registry);

    std::map<std::string, std::vector<TransformationComponent>> instances;

    RouteComponent& test = registry.get<RouteComponent>(level1Instance);

    for (const auto& node : test.routeNodes) {
        Vector3 position = CellToWorldPosition(node.cell);

        // TODO: We cannot have switched on a trigger at the moment
        // See how we are handling blades
        if (node.switchTrigger) {
            entt::entity e = registry.create();
            dynamicEntities.push_back(e);

            // add a new switch
            registry.emplace<SwitchBlockComponent>(e, *node.switchTrigger, "false", position);

            // mark this as a debug component
            registry.emplace<NoHitTestComponent>(e);

            addSwitchOff(e, position);
        }

        // main entry?
        auto entity = registry.create();
        dynamicEntities.push_back(entity);

        registry.emplace<MetaBlockComponent>(entity, node);

        auto tileTemplate = resourceManager.getResource<TileTemplate>(node.templateName);
        assert(tileTemplate);

        NavBlockType blockType = NavBlockType::Standard;
        if (node.trap) {
            blockType = NavBlockType::Trap;
        }

        NavBlockData navBlockData;
        navBlockData.cell = node.cell;
        navBlockData.surface = tileTemplate->surface;
        navBlockData.type = blockType;
        navBlockData.allowSaw = node.allowSaw;

        // copy out blocks
        navBlockData.blocks.allowForward = tileTemplate->navigation.allowForward;
        navBlockData.blocks.allowBackward = tileTemplate->navigation.allowBackward;
        navBlockData.blocks.allowLeft = tileTemplate->navigation.allowLeft;
        navBlockData.blocks.allowRight = tileTemplate->navigation.allowRight;

        CellPos finalCellPos = node.cell;

        if (node.trigger.empty()) {

            registry.emplace<NavBlockComponent>(entity, navBlockData);

            // position already calculated from finalCellPos

            instances[tileTemplate->meshName].emplace_back(position, Vector3Zero(), 0.0f, Vector3One());
        }
        else {
            // common trigger data between all surfaces
            std::vector<TriggerData> triggerData;
            triggerData.reserve(node.triggers.size());
            for (const auto& t : node.triggers) {
                TriggerData data;
                data.value = t.value;
                data.cellOffset = t.cellOffset;
                triggerData.push_back(data);
            }

            for (const auto& td : triggerData) {
                if (td.value == "false") {
                    finalCellPos += td.cellOffset;
                }
            }

            // register a trigger
            registry.emplace<TriggerComponent>(entity, node.trigger, node.cell, triggerData);

            // register a nav block at the starting offset (could be anything)
            navBlockData.cell = finalCellPos;

            registry.emplace<NavBlockComponent>(entity, navBlockData);

            MeshResPtr wbMesh = resourceManager.getResource<MeshRes>(tileTemplate->meshName);

            // add single transformation
            auto pos = CellToWorldPosition(finalCellPos);

            registry.emplace<TransformationComponent>(entity, pos, Vector3Zero(), 0.0f, Vector3One());
            registry.emplace<MeshComponent>(entity, wbMesh);
        }

        if (node.enemy) {
            // temp hack around "ground" offsets

            auto finalPosition = CellToWorldPosition(finalCellPos);

            auto enemyPosition = Vector3Add(finalPosition, Vector3(0.0f, 1.0f, 0.0f));

            auto enemyTemplate = resourceManager.getResource<EnemyTemplate>(*node.enemy);

            switch (enemyTemplate->type) {
                case EnemyType::Saw: {
                    auto sawEntity = registry.create();
                    dynamicEntities.push_back(sawEntity);
                    registry.emplace<TransformationComponent>(sawEntity, enemyPosition, Vector3Zero(), 0.0f, Vector3One());
                    MeshResPtr skyMesh = resourceManager.getResource<MeshRes>(enemyTemplate->meshName);
                    registry.emplace<MeshComponent>(sawEntity, skyMesh);
                    registry.emplace<NoHitTestComponent>(sawEntity);
                    registry.emplace<BladeComponent>(sawEntity, finalCellPos, enemyTemplate->pattern, enemyTemplate->surface);
                } break;
                case EnemyType::Snake: {
                    auto snakeEntity = registry.create();
                    dynamicEntities.push_back(snakeEntity);
                    registry.emplace<TransformationComponent>(snakeEntity, enemyPosition, Vector3Zero(), 0.0f, Vector3One());
                    MeshResPtr skyMesh = resourceManager.getResource<MeshRes>(enemyTemplate->meshName);
                    registry.emplace<MeshComponent>(snakeEntity, skyMesh);
                    registry.emplace<NoHitTestComponent>(snakeEntity);
                    registry.emplace<SnakeComponent>(snakeEntity, finalCellPos, enemyTemplate->pattern, enemyTemplate->surface);
                } break;
            }
        }
    }

    MeshComponent* meshComponent = registry.try_get<MeshComponent>(level1Instance);
    if (meshComponent) {
        auto entity3 = registry.create();
        dynamicEntities.push_back(entity3);
        // Mark this as debug
        registry.emplace_or_replace<NoHitTestComponent>(entity3);
        registry.emplace<TransformationComponent>(entity3, Vector3Zero(), Vector3Zero(), 0.0f, Vector3One());
        meshComponent->assignToEntity(entity3, registry);
    }

    for (const auto& instance : instances) {
        assert(instance.second.size() > 0);

        auto entity2 = registry.create();
        dynamicEntities.push_back(entity2);

        if (instance.second.size() == 1) {
            MeshResPtr meshResource = resourceManager.getResource<MeshRes>(instance.first);
            registry.emplace<MeshComponent>(entity2, meshResource);
            registry.emplace<TransformationComponent>(entity2, instance.second[0]);
        }
        else {
            MeshResPtr meshResource = resourceManager.getResource<MeshRes>(instance.first);
            registry.emplace<InstancedMeshComponent>(entity2, meshResource, instance.second);
        }
    }

    OnLevelSpawned e{level1Instance, levelName};
    game->raiseEvent(e);

    // we keep this around for nav mesh purposes
    registry.destroy(level1Instance);
}

void LevelSystem::addSwitchOn(const entt::entity& entity, const Vector3& pos) {
    MeshResPtr wbTriggerOn = resourceManager.getResource<MeshRes>("WB_TRIGGER_ON");

    registry.emplace_or_replace<MeshComponent>(entity, wbTriggerOn);
    registry.emplace_or_replace<TransformationComponent>(entity, pos, Vector3Zero(), 0.0f, Vector3One());
}

void LevelSystem::addSwitchOff(const entt::entity& entity, const Vector3& pos) {
    MeshResPtr wbTriggerOff = resourceManager.getResource<MeshRes>("WB_TRIGGER_OFF");

    registry.emplace_or_replace<MeshComponent>(entity, wbTriggerOff);
    registry.emplace_or_replace<TransformationComponent>(entity, pos, Vector3Zero(), 0.0f, Vector3One());
}
