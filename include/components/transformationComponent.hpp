#pragma once
#include "component.hpp"

#include <raylib.h>

struct TransformationComponent : public AssignableComponent {
    Vector3 position;
    Quaternion rotation;
    Vector3 scale;

    Transform transform;

    TransformationComponent(const Vector3& position, const Quaternion& rotation, const Vector3& scale);

    void calculateTransform();

    void translate(const Vector3& translation);
    void setPosition(const Vector3& position);

    void rotate(const Vector3& axis, float angle);
    void setRotation(const Vector3& axis, float angle);
    void setRotation(const Vector3& eulerAngles);

    void addScale(const Vector3& scale);
    void setScale(const Vector3& scale);

    void assignToEntity(const entt::entity entity, entt::registry& registry) const override;
};