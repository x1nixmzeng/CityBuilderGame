#include "systems/movementSystem.hpp"

#include "resources/resourceManager.hpp"

#include "components/components.hpp"

#include "events/events.hpp"

#include <raymath.h>

MovementSystem::MovementSystem(Game* game)
    : System(game) {

    eventDispatcher.sink<OnLevelSpawned>()
        .connect<&MovementSystem::handleOnLevelSpawned>(*this);

    eventDispatcher.sink<OskMoveRequested>()
        .connect<&MovementSystem::handleOskMoveRequested>(*this);

    eventDispatcher.sink<OnStartBot>()
        .connect<&MovementSystem::handleOnStartBot>(*this);
}

void MovementSystem::handleOskMoveRequested(const OskMoveRequested& e) {
    handleOskKey(e.oskEvent);
}

void MovementSystem::handleOnLevelSpawned(const OnLevelSpawned& e) {

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
}

bool MovementSystem::canMoveTo(const CellPos& pos, const Surface& surfaceType, OskEvent const& dir) const {

    auto blockViews = registry.view<NavBlockComponent>();
    for (auto blockViewEntity : blockViews) {
        auto x = blockViews.get<NavBlockComponent>(blockViewEntity);
        if (x.data.cell == pos && x.data.surface == surfaceType) {
            return true;
        }
    }

    return false;
}

bool MovementSystem::tryMoveTo(const CellPos& pos, const Surface& surfaceType, OskEvent const& dir) {

    // todo: check target tile has access from this direction

    if (canMoveTo(pos, surfaceType, dir)) {
        setLaraTarget(pos, surfaceType);
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

    if (!allowMove) {
        puts("Current cell prevents this action\n");
        return;
    }

    switch (oskEvent) {
        case OskEvent::MoveLeft: {
            if (currentSurface == Surface::Ground) {
                // check for front walls on current cell

                // Moving back to a front wall is NOT supported - todo: check side walls

                auto target = lara;
                target.x -= 1;
                if (!tryMoveTo(target, Surface::Ground, oskEvent)) {
                    target = lara;
                    if (!tryMoveTo(target, Surface::Wall_Side, oskEvent)) {
                        // assert?
                    }
                }
            }
            else if (currentSurface == Surface::Wall_Front) {
                auto target = lara;
                target.x -= 1;
                if (!tryMoveTo(target, Surface::Wall_Front, oskEvent)) {
                    // assert?
                }
            }
            else if (currentSurface == Surface::Wall_Side) {

                auto target = lara;
                target.y -= 1;
                if (!tryMoveTo(target, Surface::Wall_Side, oskEvent)) {
                    target.x -= 1;
                    if (!tryMoveTo(target, Surface::Ground, oskEvent)) {
                        // assert?
                    }
                }
            }
        } break;

        case OskEvent::MoveRight: {
            if (currentSurface == Surface::Ground) {
                // check for front walls on current cell

                auto target = lara;
                target.x += 1;
                if (!tryMoveTo(target, Surface::Ground, oskEvent)) {
                    target.y += 1;
                    if (!tryMoveTo(target, Surface::Wall_Side, oskEvent)) {
                        // assert?
                    }
                }
            }
            else if (currentSurface == Surface::Wall_Front) {
                auto target = lara;
                target.x += 1;
                if (!tryMoveTo(target, Surface::Wall_Front, oskEvent)) {
                    // assert?
                }
            }
            else if (currentSurface == Surface::Wall_Side) {
                auto target = lara;
                target.y += 1;
                if (!tryMoveTo(target, Surface::Wall_Side, oskEvent)) {
                    target = lara;
                    if (!tryMoveTo(target, Surface::Ground, oskEvent)) {
                        // assert?
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

                if (!tryMoveTo(target, Surface::Wall_Front, oskEvent)) {
                    target = lara;
                    target.z -= 1;
                    tryMoveTo(target, Surface::Ground, oskEvent);
                }
            }
            else if (currentSurface == Surface::Wall_Front) {

                // try going "up" in y
                auto target = lara;

                // TODO: CHECK FOR GROUND

                // move down
                target.y += 1;
                if (!tryMoveTo(target, Surface::Wall_Front, oskEvent)) {
                    // check for transition to ground
                    tryMoveTo(lara, Surface::Ground, oskEvent);
                }
            }
            else if (currentSurface == Surface::Wall_Side) {
                auto target = lara;
                target.z -= 1;
                if (!tryMoveTo(target, Surface::Wall_Side, oskEvent)) {
                    // not sure what else can be done here
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
                if (!tryMoveTo(target, Surface::Wall_Front, oskEvent)) {
                    target.z += 1;

                    tryMoveTo(target, Surface::Ground, oskEvent);
                }
            }
            else if (currentSurface == Surface::Wall_Front) {

                // continue to climb down climb if possible
                auto target = lara;
                target.y -= 1;
                if (!tryMoveTo(target, Surface::Wall_Front, oskEvent)) {
                    target.z += 1;

                    // check for transition to ground
                    tryMoveTo(target, Surface::Ground, oskEvent);
                }
            }
            else if (currentSurface == Surface::Wall_Side) {
                auto target = lara;
                target.z += 1;
                if (!tryMoveTo(target, Surface::Wall_Side, oskEvent)) {
                    // not sure what else can be done here
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
}

void MovementSystem::setLaraInternal(const Vector3& pos) {
    TransformationComponent& laraTransformation = registry.get<TransformationComponent>(laraEntity);

    auto finalPos = Vector3Add(pos, laraOffset);

    laraTransformation.setPosition(finalPos);
    laraTransformation.setRotation(laraDirection, 90.0f);//, glm::radians(90.0f));
    laraTransformation.calculateTransform();
}

void MovementSystem::setLaraTarget(const CellPos& pos, const Surface& surfaceType) {

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
                            // skip for now
                            // state = MovementState::WalkToDeath;
                        }
                        else {
                            // fall down?! how do we code this - how far can lara fall
                        }
                    }
                }

                {
                    auto end = getWorldPosition(laraTarget, laraTargetSurface);
                    // kick off the animation transition
                    CameraTargetComponent& camTarget = registry.get<CameraTargetComponent>(game->camera);
                    camTarget.target = end;
                    // camTarget.up = // ignored
                    camTarget.valid = true;

                    RequestCameraLookAt lookAt;
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

        laraDirection = {};

        // todo: offset for position?
        switch (laraTargetSurface) {
            case Surface::Ground:
                // nothing
                break;
            case Surface::Wall_Front:
                laraDirection.x = -1.0f;
                break;
            case Surface::Wall_Side:
                laraDirection.z = -1.0f;
                break;
        }

        // we need to set this correctly at least once - and not at the end
        // for now, we do not interpolate the rotation (todo: play anim sequence to jump down?)
        currentSurface = laraTargetSurface;
        setLaraInternal(laraPos);

        if (laraDelta >= 1.0f) {
            if (state == MovementState::WalkToDeath) {
                // death!
                assert(false); // lara has died
            }

            state = MovementState::Idle;
            lara = laraTarget;
            currentSurface = laraTargetSurface;

            OnLaraMoveEvent laraMoveEvent(lara, currentSurface);
            game->raiseEvent(laraMoveEvent);

            //// todo: lookat component?
            // CameraTargetComponent& camTarget = registry.get<CameraTargetComponent>(game->camera);
            // camTarget.target = laraPos;
            ////camTarget.up = // ignored
            // camTarget.valid = true;

            // RequestCameraLookAt lookAt;
            // game->raiseEvent(lookAt);

            // if (cameraTrigger) {
            //     // todo: move camera

            //    //TransformationComponent& cameraTransform = registry.get<TransformationComponent>(game->camera);
            //    //cameraTransform.position = *cameraTrigger;

            //    RequestCameraLookAt lookAt;
            //    game->raiseEvent(lookAt);
            //}

            // handle events
            if (levelTrigger) {
                RequestLevelEvent newLevel(*levelTrigger);
                game->raiseEvent(newLevel);
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

    registry.emplace<TransformationComponent>(laraEntity, Vector3Zero(), Quaternion(), Vector3(1.0f));
    MeshResPtr laraMesh = resourceManager.getResource<MeshRes>("LARA_MESH");
    registry.emplace<MeshComponent>(laraEntity, laraMesh);

    setLaraInternal(getWorldPosition(spawnPoint, Surface::Ground));
    lara = spawnPoint;
}
