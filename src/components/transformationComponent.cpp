#include "components/transformationComponent.hpp"

#include <raymath.h>

TransformationComponent::TransformationComponent(const Vector3& position, const Quaternion& rotation, const Vector3& scale)
    : position(position), rotation(rotation), scale(scale) {
    calculateTransform();
}

void TransformationComponent::calculateTransform() {
    // no need to calculate this, just commit to the rotation
    transform.translation = position;
    transform.rotation = rotation;
    transform.scale = scale;
}

void TransformationComponent::translate(const Vector3& translation) {
    position = Vector3Add(position, translation);
}

void TransformationComponent::setPosition(const Vector3& position) {
    this->position = position;
}

void TransformationComponent::rotate(const Vector3& axis, float angle) {
    // glm::quat dRot = glm::angleAxis(angle, axis);
    //
    // rotation = dRot * rotation;
}

void TransformationComponent::setRotation(const Vector3& axis, float angle) {

    auto input = Vector3(1.0f, 0.0f, 0.0f);

//    rotation = Vector3RotateByAxisAngle(input, axis, angle);
}

void TransformationComponent::setRotation(const Vector3& eulerAngles) {
    // rotation = glm::quat(eulerAngles);
}

void TransformationComponent::addScale(const Vector3& scale) {
    this->scale = Vector3Multiply(this->scale, scale);
}

void TransformationComponent::setScale(const Vector3& scale) {
    this->scale = scale;
}

void TransformationComponent::assignToEntity(const entt::entity entity, entt::registry& registry) const {
    registry.emplace<TransformationComponent>(entity, position, rotation, scale);
}