#pragma once
#include "system.hpp"

#include "events/oskEvents.hpp"
#include "misc/cells.hpp"

#include <raylib.h>
#include <optional>

struct OnLevelSpawned;
struct OnStartBot;

class MovementSystem : public System {
  protected:
    entt::entity laraEntity = entt::null;

    bool pendingCreate = false;

    void createLara();

    enum class MovementState {
        Idle,
        WalkTo,
        Interacting,
        WalkToDeath,
        Falling,
    };

    entt::entity levelEntityRef = entt::null;

    MovementState state = MovementState::Idle;

    void handleOskKey(const OskEvent& oskEvent);

    void setLaraInternal(const Vector3& pos);
    void setLaraTarget(const CellPos& pos, const Surface& surfaceType);
    void updateMovement(float dt);

    CellPos spawnPoint; // spawn point
    Vector3 laraOffset;
    Vector3 laraDirection{};

    CellPos lara;
    Surface currentSurface;

    CellPos laraInitial;
    CellPos laraTarget;
    Surface laraTargetSurface;

    entt::entity targetEntity = entt::null;

    std::optional<std::string> switchTrigger;
    std::optional<std::string> levelTrigger;
    std::optional<Vector3> cameraTrigger;

    float laraDelta = 0.0f;

    float interactDelay = 0.0f;

    void handleOskMoveRequested(const OskMoveRequested& e);
    void handleOnLevelSpawned(const OnLevelSpawned& e);
    void handleOnStartBot(const OnStartBot& e);

    float walkSpeed = 3.0f;
    float fallSpeed = 6.0f;
    float interactLength = 0.1f;

    bool canMoveTo(const CellPos& pos, const Surface& surfaceType, OskEvent const& dir) const;
    bool tryMoveTo(const CellPos& pos, const Surface& surfaceType, OskEvent const& dir);

    Vector3 getWorldPosition(const CellPos& pos, const Surface& surfaceType) const;

  public:
    MovementSystem(Game* game);

    void init() override;
    void update(float dt) override;
};
