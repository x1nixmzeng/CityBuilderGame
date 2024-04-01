#include "systems/movementSystem.hpp"

#include "resources/resourceManager.hpp"

#include "components/components.hpp"

#include "events/events.hpp"

#include <format>
#include <raymath.h>
#include <string_view>

MovementSystem::MovementSystem(Game* game)
    : System(game) {

    eventDispatcher.sink<OnLevelSpawned>()
        .connect<&MovementSystem::handleOnLevelSpawned>(*this);

    eventDispatcher.sink<OskMoveRequested>()
        .connect<&MovementSystem::handleOskMoveRequested>(*this);

    eventDispatcher.sink<OnStartBot>()
        .connect<&MovementSystem::handleOnStartBot>(*this);

    eventDispatcher.sink<OnLaraMoveEvent>()
        .connect<&MovementSystem::handleOnLaraMoveEvent>(*this);
}

void MovementSystem::handleOskMoveRequested(const OskMoveRequested& e) {
    handleOskKey(e.oskEvent);
}

void MovementSystem::handleOnLevelSpawned(const OnLevelSpawned& e) {

    state = MovementState::Idle;

    levelEntityRef = e.entity;

    LaraComponent& laraComponent = registry.get<LaraComponent>(e.entity);
    // ah this is all constant - boo
    spawnPoint = laraComponent.initialCell;
    laraOffset = laraComponent.offset;
    currentSurface = laraComponent.surface;
    pendingCreate = true;
}

void MovementSystem::handleOnStartBot(const OnStartBot& e) {
    // tiny tiny lengths
    interactLength *= 0.01f;
    walkSpeed *= 100.0f;
    fallSpeed *= 100.0f;
}

void MovementSystem::handleOnLaraMoveEvent(const OnLaraMoveEvent& e) {

    auto killLara = false;

    auto bladeView = registry.view<BladeComponent, TransformationComponent>();
    for (const auto& bladeEntity : bladeView) {
        auto& blade = bladeView.get<BladeComponent>(bladeEntity);

        // Test cell
        auto cell = blade.initialCell;

        // Test lara against her current position
        if (e.surface == blade.surface && e.pos == cell) {
            killLara = true;
            break;
        }

        handleBladeMove(cell, blade);

        // Can the blade move here
        if (!canMoveTo(cell, Surface::Ground, CanMoveSaw)) {

            // Test cell in other direction
            cell = blade.initialCell;
            blade.step *= -1;
            handleBladeMove(cell, blade);

            auto canMoveAgain = canMoveTo(cell, Surface::Ground, CanMoveSaw);
            if (!canMoveAgain) {
                // This is valid. The blade is constrained to one tile and cannot move
                // However, we must not move
                cell = blade.initialCell;

                // Keep blade.step inverted to show that something has happened
            }
        }

        // Apply this back to the blade
        blade.initialCell = cell;

        auto worldPos = CellToWorldPosition(cell);
        worldPos = Vector3Add(worldPos, Vector3(0.0f, 1.0f, 0.0f));

        auto& trans = bladeView.get<TransformationComponent>(bladeEntity);

        trans.unseal();
        switch (blade.pattern) {
            case MovementPattern::LeftRight:
                trans.rotationAxis = Vector3(1.0, 0.0f, 0.0f);
                trans.rotationAngle = 90.0f;
                break;
            case MovementPattern::ForwardBack:
                trans.rotationAxis = Vector3(1.0, 0.0f, 0.0f);
                trans.rotationAngle = 0.0f;
                break;
        }

        trans.position = worldPos;
        trans.seal();

        // Must match the same surface
        if (e.surface == blade.surface && e.pos == cell) {
            killLara = true;
            break;
        }
    }

    if (killLara) {
        OnLaraDiedEvent laraDiedEvent;
        game->raiseEvent(laraDiedEvent);
    }
}

void MovementSystem::handleBladeMove(CellPos& cell, BladeComponent const& blade) {
    // blade cannot not be moving
    assert(blade.step != 0);

    switch (blade.pattern) {
        case MovementPattern::ForwardBack:
            if (blade.step > 0) {
                // move left
                cell.z += 1;
            }
            else {
                cell.z -= 1;
            }
            break;

        case MovementPattern::LeftRight:
            if (blade.step > 0) {
                // move forwards
                cell.x += 1;
            }
            else {
                cell.x -= 1;
            }
            break;
    }
}

bool MovementSystem::CanMoveLara(const NavBlockComponent& navBlock, const CellPos& pos, const Surface& surfaceType) {
    auto const cellMatch = navBlock.data.cell == pos;
    auto const surfaceMatch = navBlock.data.surface == surfaceType;
    if (cellMatch && surfaceMatch) {
        return true;
    }

    return false;
}

bool MovementSystem::CanMoveSaw(const NavBlockComponent& navBlock, const CellPos& pos, const Surface& surfaceType) {
    auto const cellMatch = navBlock.data.cell == pos;
    auto const surfaceMatch = navBlock.data.surface == surfaceType;
    if (navBlock.data.allowSaw && cellMatch && surfaceMatch) {
        return true;
    }

    return false;
}

bool MovementSystem::canMoveTo(const CellPos& pos, const Surface& surfaceType, CanMoveLambda const& checkCanMove) const {

    auto blockViews = registry.view<NavBlockComponent>();
    for (auto blockViewEntity : blockViews) {
        auto x = blockViews.get<NavBlockComponent>(blockViewEntity);

        if (checkCanMove(x, pos, surfaceType)) {
            return true;
        }
    }

    return false;
}

bool MovementSystem::tryMoveLara(const CellPos& pos, const Surface& surfaceType, OskEvent const oskEvent) {

    if (canMoveTo(pos, surfaceType, CanMoveLara)) {
        setLaraTarget(pos, surfaceType, oskEvent);
        return true;
    }
    return false;
}

Vector3 MovementSystem::getWorldPosition(const CellPos& pos, const Surface& surfaceType) const {

    constexpr float tileDim = 2.0f;

    Vector3 vec3;
    vec3.x = pos.x;
    vec3.y = pos.y;
    vec3.z = pos.z;

    auto worldPos = Vector3Scale(vec3, tileDim);

    switch (currentSurface) {
        case Surface::Ground:
            // nada
            break;
        case Surface::Wall_Front: {
            static volatile float XOFFSET = 0.0f;
            static volatile float YOFFSET = -1.0f;
            static volatile float ZOFFSET = 1.5f;

            Vector3 offset;
            offset.x = XOFFSET;
            offset.y = YOFFSET;
            offset.z = ZOFFSET;
            worldPos = Vector3Add(worldPos, offset);
        } break;

        case Surface::Wall_Side: {
            static volatile float XOFFSET = -1.5f;
            static volatile float YOFFSET = -1.0f;
            static volatile float ZOFFSET = 0.0f;

            Vector3 offset;
            offset.x = XOFFSET;
            offset.y = YOFFSET;
            offset.z = ZOFFSET;
            worldPos = Vector3Add(worldPos, offset);
        } break;
    }

    return worldPos;
}

void MovementSystem::init() {
}

void MovementSystem::update(float dt) {

    if (pendingCreate) {
        createLara();
        pendingCreate = false;
    }

    if (state == MovementState::Interacting) {
        // delay
        interactDelay -= dt;
        if (interactDelay < 0.0f) {

            // mark "end of motion" - counts interaction as a move
            OnLaraMoveEvent laraMoveEvent(lara, currentSurface);
            game->raiseEvent(laraMoveEvent);

            state = MovementState::Idle;
        }
    }

    updateMovement(dt);

    {
        TransformationComponent& laraTransformation = registry.get<TransformationComponent>(laraEntity);
        // hack to move spotlight with character
        registry.view<LightComponent>()
            .each([&](LightComponent& light) {
                auto lightPos = light.light.position;

                // Negate the light offset
                lightPos = Vector3Add(lightPos, Vector3(0.0f, -3.0f, 0.0f));

                // Project a little
                auto v1 = Vector3Subtract(laraTransformation.position, lightPos);
                const float speed = 0.3f;
                auto finalPos = Vector3Add(lightPos, Vector3Multiply(v1, Vector3(speed, speed, speed)));

                // Reapply the offset
                light.light.position = Vector3Add(finalPos, Vector3(0.0f, 3.0f, 0.0f));
            });
    }
}

void MovementSystem::handleOskKey(const OskEvent& oskEvent) {

    // early out when this incomplete
    if (state != MovementState::Idle)
        return;

    // todo: check our current cell - what are we allowed to do

    bool allowMove = false;

    {
        auto blockViews = registry.view<NavBlockComponent>();
        for (auto blockViewEntity : blockViews) {
            auto x = blockViews.get<NavBlockComponent>(blockViewEntity);
            if (x.data.cell == lara && x.data.surface == currentSurface) {
                // nav hints prevent certain movements (blocked routes)
                switch (oskEvent) {
                    case OskEvent::MoveLeft:
                        allowMove = x.data.blocks.allowLeft;
                        break;
                    case OskEvent::MoveRight:
                        allowMove = x.data.blocks.allowRight;
                        break;
                    case OskEvent::MoveForward:
                        allowMove = x.data.blocks.allowForward;
                        break;
                    case OskEvent::MoveBackward:
                        allowMove = x.data.blocks.allowBackward;
                        break;
                    case OskEvent::Interact:
                        allowMove = true;
                        break;
                }

                break;
            }
        }
    }

    auto oskName = [oskEvent]() -> std::string_view {
        switch (oskEvent) {
            case OskEvent::MoveLeft:
                return "left";
            case OskEvent::MoveRight:
                return "right";
            case OskEvent::MoveForward:
                return "forward";
            case OskEvent::MoveBackward:
                return "back";
            case OskEvent::Interact:
                return "interact";
        }

        std::unreachable();
    }();

    if (!allowMove) {
        puts(std::format("Current cell prevents move {}\n", oskName).c_str());
        return;
    }

    bool handled = true;

    switch (oskEvent) {
        case OskEvent::MoveLeft: {
            if (currentSurface == Surface::Ground) {
                // check for front walls on current cell

                // Moving back to a front wall is NOT supported - todo: check side walls

                auto target = lara;
                target.x -= 1;
                if (!tryMoveLara(target, Surface::Ground, oskEvent)) {
                    target = lara;
                    if (!tryMoveLara(target, Surface::Wall_Side, oskEvent)) {
                        handled = false;
                    }
                }
            }
            else if (currentSurface == Surface::Wall_Front) {
                auto target = lara;
                target.x -= 1;
                if (!tryMoveLara(target, Surface::Wall_Front, oskEvent)) {
                    handled = false;
                }
            }
            else if (currentSurface == Surface::Wall_Side) {

                auto target = lara;
                target.y -= 1;
                if (!tryMoveLara(target, Surface::Wall_Side, oskEvent)) {
                    target.x -= 1;
                    if (!tryMoveLara(target, Surface::Ground, oskEvent)) {
                        handled = false;
                    }
                }
            }
        } break;

        case OskEvent::MoveRight: {
            if (currentSurface == Surface::Ground) {
                // check for front walls on current cell

                auto target = lara;
                target.x += 1;
                if (!tryMoveLara(target, Surface::Ground, oskEvent)) {
                    target.y += 1;
                    if (!tryMoveLara(target, Surface::Wall_Side, oskEvent)) {
                        handled = false;
                    }
                }
            }
            else if (currentSurface == Surface::Wall_Front) {
                auto target = lara;
                target.x += 1;
                if (!tryMoveLara(target, Surface::Wall_Front, oskEvent)) {
                    handled = false;
                }
            }
            else if (currentSurface == Surface::Wall_Side) {
                auto target = lara;
                target.y += 1;
                if (!tryMoveLara(target, Surface::Wall_Side, oskEvent)) {
                    target = lara;
                    if (!tryMoveLara(target, Surface::Ground, oskEvent)) {
                        handled = false;
                    }
                }
            }

        } break;

        case OskEvent::MoveBackward: {
            if (currentSurface == Surface::Ground) {
                // Moving back up a wall - is this supported?
                auto target = lara;
                target.y += 1;
                target.z -= 1;

                if (!tryMoveLara(target, Surface::Wall_Front, oskEvent)) {
                    target = lara;
                    target.z -= 1;
                    tryMoveLara(target, Surface::Ground, oskEvent);
                }
            }
            else if (currentSurface == Surface::Wall_Front) {

                // try going "up" in y
                auto target = lara;

                // TODO: CHECK FOR GROUND

                // move down
                target.y += 1;
                if (!tryMoveLara(target, Surface::Wall_Front, oskEvent)) {
                    // check for transition to ground
                    tryMoveLara(lara, Surface::Ground, oskEvent);
                }
            }
            else if (currentSurface == Surface::Wall_Side) {
                auto target = lara;
                target.z -= 1;
                if (!tryMoveLara(target, Surface::Wall_Side, oskEvent)) {
                    // not sure what else can be done here
                    handled = false;
                }
            }
            else {
                assert(false);
            }

        } break;

        case OskEvent::MoveForward: {
            if (currentSurface == Surface::Ground) {
                // check for front walls on current cell

                auto target = lara;
                if (!tryMoveLara(target, Surface::Wall_Front, oskEvent)) {
                    target.z += 1;

                    tryMoveLara(target, Surface::Ground, oskEvent);
                }
            }
            else if (currentSurface == Surface::Wall_Front) {

                // continue to climb down climb if possible
                auto target = lara;
                target.y -= 1;
                if (!tryMoveLara(target, Surface::Wall_Front, oskEvent)) {
                    target.z += 1;

                    // check for transition to ground
                    tryMoveLara(target, Surface::Ground, oskEvent);
                }
            }
            else if (currentSurface == Surface::Wall_Side) {
                auto target = lara;
                target.z += 1;
                if (!tryMoveLara(target, Surface::Wall_Side, oskEvent)) {
                    // not sure what else can be done here
                    handled = false;
                }
            }
            else {
                assert(false);
            }

        } break;

        case OskEvent::Interact: {
            // todo: check we can interact with anything

            if (state == MovementState::Idle) {

                if (switchTrigger) {
                    interactDelay = interactLength;
                    state = MovementState::Interacting;

                    RequestToggleSwitchEvent switchEvent(*switchTrigger);
                    game->raiseEvent(switchEvent);
                }
                // todo: check we can interact with anything in this position
            }

        } break;
    }

    if (!handled) {
        puts(std::format("Unable to find valid target for move {}\n", oskName).c_str());
        return;
    }
}

void MovementSystem::setLaraInternal(const Vector3& pos, const Vector3& rotation) {
    TransformationComponent& laraTransformation = registry.get<TransformationComponent>(laraEntity);

    laraTransformation.unseal();
    auto finalPos = Vector3Add(pos, laraOffset);

    laraTransformation.position = finalPos;

    // TODO: Would be good for "lara" to be facing the right direction here
    laraTransformation.rotationAxis = Vector3(0.0f, 1.0f, 0.0f);
    laraTransformation.rotationAngle = rotation.z;

    laraTransformation.seal();

    //// hack to move spotlight with character
    //registry.view<LightComponent>()
    //    .each([&](LightComponent& light) {
    //        light.light.position = Vector3Add(finalPos, Vector3(0.0f, 3.0f, 0.0f));
    //    });
}

void MovementSystem::setLaraTarget(const CellPos& pos, const Surface& surfaceType, OskEvent const oskEvent) {

    if (state == MovementState::Idle) {
        // here we find the route component (the "level setup") to query the nav
        // we should be using the level system to do this for us
        // however we don't allow cross system containaimation - should we have an interface on game??

        // todo: we need to know the direction too - how can we infer this?

        auto v = registry.view<NavBlockComponent, MetaBlockComponent>();
        for (auto entity : v) {
            auto& navBlock = registry.get<NavBlockComponent>(entity);
            if (navBlock.data.cell == pos && navBlock.data.surface == surfaceType) {
                auto const& metaBlock = registry.get<MetaBlockComponent>(entity);

                laraInitial = lara;
                laraTarget = pos;
                laraTargetSurface = surfaceType;
                laraDelta = 0.0f;

                laraRotation = 0.0f;

                if (surfaceType == Surface::Ground) {
                    switch (oskEvent) {
                        case OskEvent::MoveRight:
                            laraRotation = 90.0f;
                            break;
                        case OskEvent::MoveLeft:
                            laraRotation = -90.0f;
                            break;
                        case OskEvent::MoveForward:
                            laraRotation = 0.0f;
                            break;
                        case OskEvent::MoveBackward:
                            laraRotation = 180.0f;
                            break;
                    }
                }
                else if (surfaceType == Surface::Wall_Front) {
                    laraRotation = 180.0f;
                }
                else if (surfaceType == Surface::Wall_Side) {
                    laraRotation = 90.0f;
                }

                switchTrigger = metaBlock.node.switchTrigger;
                levelTrigger = metaBlock.node.levelTrigger;
                cameraTrigger = metaBlock.node.cameraTrigger;

                targetEntity = entity;

                state = MovementState::WalkTo;

                // if lara is walking over a trap, check this isn't to her death
                if (navBlock.data.type == NavBlockType::Trap) {
                    navBlock.data.steps++;

                    // todo: this only logs to file! move to console too
                    game->log(std::format("lara has stepped on a trap! steps = {}", navBlock.data.steps));

                    if (navBlock.data.steps > 1) {
                        // ground tile - fall to death
                        if (surfaceType == Surface::Ground) {
                            state = MovementState::WalkToDeath;
                        }
                        else if (surfaceType == Surface::Wall_Front) {
                            // assume lara can fall? check for ground immediately?
                            state = MovementState::Falling;

                            laraTarget.y -= 1;
                            laraTarget.z += 1;
                        }
                        else if (surfaceType == Surface::Wall_Side) {
                            // assume lara can fall? check for ground immediately?
                            state = MovementState::Falling;

                            laraTarget.y -= 1;
                            laraTarget.x -= 1;
                        }
                        else {
                            assert(false);
                        }
                    }
                }

                {
                    auto end = getWorldPosition(laraTarget, laraTargetSurface);

                    CameraRequestLookAt lookAt;
                    lookAt.target = end;
                    game->raiseEvent(lookAt);
                }

                break;
            }
        }
    }
}

void MovementSystem::updateMovement(float dt) {

    if (state == MovementState::WalkTo || state == MovementState::WalkToDeath) {
        laraDelta = std::clamp<float>(laraDelta + dt * walkSpeed, 0.0f, 1.0f);

        auto start = getWorldPosition(laraInitial, currentSurface);
        auto end = getWorldPosition(laraTarget, laraTargetSurface);

        Vector3 laraPos;

        laraPos.x = std::lerp(start.x, end.x, laraDelta);
        laraPos.y = std::lerp(start.y, end.y, laraDelta);
        laraPos.z = std::lerp(start.z, end.z, laraDelta);

        // we need to set this correctly at least once - and not at the end
        // for now, we do not interpolate the rotation (todo: play anim sequence to jump down?)
        currentSurface = laraTargetSurface;
        setLaraInternal(laraPos, Vector3(0.0f, 0.0f, laraRotation));

        if (laraDelta >= 1.0f) {
            if (state == MovementState::WalkToDeath) {
                // death!
                OnLaraDiedEvent laraDiedEvent;
                game->raiseEvent(laraDiedEvent);
            }
            else if (cameraTrigger) {
                CameraRequestPosition cameraPosition;
                cameraPosition.target = *cameraTrigger;
                game->raiseEvent(cameraPosition);
            }

            state = MovementState::Idle;
            lara = laraTarget;
            currentSurface = laraTargetSurface;

            OnLaraMoveEvent laraMoveEvent(lara, currentSurface);
            game->raiseEvent(laraMoveEvent);

            // handle events
            if (levelTrigger) {
                RequestLevelEvent newLevel(*levelTrigger);
                game->raiseEvent(newLevel);
            }
        }
    }

    else if (state == MovementState::Falling) {
        laraDelta = std::clamp<float>(laraDelta + dt * fallSpeed, 0.0f, 1.0f);

        // always start from the ground
        auto start = getWorldPosition(laraInitial, Surface::Ground);
        auto end = getWorldPosition(laraTarget, Surface::Ground);

        Vector3 laraPos;

        laraPos.x = std::lerp(start.x, end.x, laraDelta);
        laraPos.y = std::lerp(start.y, end.y, laraDelta);
        laraPos.z = std::lerp(start.z, end.z, laraDelta);

        currentSurface = Surface::Ground;

        setLaraInternal(laraPos, Vector3(0.0f, 0.0f, laraRotation));

        if (laraDelta >= 1.0f) {

            // check again for target??

            if (canMoveTo(laraTarget, Surface::Ground, CanMoveLara)) {

                // lara has landed

                state = MovementState::Idle;
                lara = laraTarget;
                currentSurface = laraTargetSurface;

                OnLaraMoveEvent laraMoveEvent(lara, currentSurface);
                game->raiseEvent(laraMoveEvent);

                auto end = getWorldPosition(lara, currentSurface);

                CameraRequestLookAt lookAt;
                lookAt.target = end;
                game->raiseEvent(lookAt);
            }
            else {
                lara = laraTarget;

                // Keep moving down
                laraInitial = lara;
                laraTarget = lara;
                laraTarget.y -= 1;

                if (laraTarget.y < -2) {
                    // Fallen to our death
                    OnLaraDiedEvent laraDiedEvent;
                    game->raiseEvent(laraDiedEvent);
                }

                laraTargetSurface = Surface::Ground;
                laraDelta = 0.0f;
            }
        }
    }
}

void MovementSystem::createLara() {

    if (registry.valid(laraEntity)) {
        registry.destroy(laraEntity);
        laraEntity = entt::null;
    }

    laraEntity = registry.create();

    registry.emplace<TransformationComponent>(laraEntity, Vector3Zero(), Vector3(1.0f, 0.0f, 0.0f), 0.0f, Vector3One());
    MeshResPtr laraMesh = resourceManager.getResource<MeshRes>("LARA_MESH");
    registry.emplace<MeshComponent>(laraEntity, laraMesh);
    registry.emplace<NoHitTestComponent>(laraEntity);

    setLaraInternal(getWorldPosition(spawnPoint, Surface::Ground), Vector3(0.0f, 0.0f, 0.0f));
    lara = spawnPoint;

    auto foundInitialCam = false;

    // Find initial camera position
    auto metaView = registry.view<MetaBlockComponent>();
    for (auto metaEntity : metaView) {
        MetaBlockComponent const& metaBlock = registry.get<MetaBlockComponent>(metaEntity);
        if (metaBlock.node.cell == lara) {
            if (metaBlock.node.cameraTrigger) {
                CameraRequestPosition cameraPosition;
                cameraPosition.target = *metaBlock.node.cameraTrigger;
                game->raiseEvent(cameraPosition);
                foundInitialCam = true;
            }

            break;
        }
    }

    assert(foundInitialCam);

    // Reset look at
    {
        auto end = getWorldPosition(lara, Surface::Ground);

        CameraRequestLookAt lookAt;
        lookAt.target = end;
        game->raiseEvent(lookAt);
    }
}
