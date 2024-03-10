#pragma once
#include "component.hpp"

#include <raylib.h>
#include <vector>

struct BaseTransformationComponent : public AssignableComponent {
    Matrix transform;

    BaseTransformationComponent();
    BaseTransformationComponent(Matrix finalMatrix);

    void assignToEntity(const entt::entity entity, entt::registry& registry) const override;
};

struct TransformationComponent : public BaseTransformationComponent {
    bool isSealed = true;
    Vector3 position;
    Vector3 rotationAxis;
    float rotationAngle;
    Vector3 scale;

    void unseal();
    void seal();

    TransformationComponent(const Vector3& position, const Vector3& axis, float angle, const Vector3& scale);
    void assignToEntity(const entt::entity entity, entt::registry& registry) const override;
};
