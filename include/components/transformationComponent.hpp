#pragma once
#include "component.hpp"

#include <raylib.h>

struct TransformationComponent : public AssignableComponent {
    Vector3 position;
    
    Vector3 rotationAxis;
    float rotationAngle;
    Vector3 scale;

    TransformationComponent(const Vector3& position, const Vector3& axis, float angle, const Vector3& scale);

    void translate(const Vector3& translation);
    void setPosition(const Vector3& position);

    void rotate(const Vector3& axis, float angle);
    void setRotation(const Vector3& axis, float angle);
    void setRotation(const Vector3& eulerAngles);

    void addScale(const Vector3& scale);
    void setScale(const Vector3& scale);

    void assignToEntity(const entt::entity entity, entt::registry& registry) const override;
};