#include "components/transformationComponent.hpp"

#include <raymath.h>

TransformationComponent::TransformationComponent(const Vector3& position, const Vector3& axis, float angle, const Vector3& scale)
    : position(position), rotationAxis(axis), rotationAngle(angle), scale(scale) {
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
    assert(false);
}

void TransformationComponent::setRotation(const Vector3& axis, float angle) {
    rotationAxis = axis;
    rotationAngle = angle;
}

void TransformationComponent::setRotation(const Vector3& eulerAngles) {
    // rotation = glm::quat(eulerAngles);
    assert(false);
}

void TransformationComponent::addScale(const Vector3& scale) {
    this->scale = Vector3Multiply(this->scale, scale);
}

void TransformationComponent::setScale(const Vector3& scale) {
    this->scale = scale;
}

void TransformationComponent::assignToEntity(const entt::entity entity, entt::registry& registry) const {
    registry.emplace<TransformationComponent>(entity, position, rotationAxis, rotationAngle, scale);
}