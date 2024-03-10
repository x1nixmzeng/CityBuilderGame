#include "components/transformationComponent.hpp"

#include <raymath.h>

BaseTransformationComponent::BaseTransformationComponent()
    : transform(MatrixIdentity()) {
}

BaseTransformationComponent::BaseTransformationComponent(Matrix finalTransform)
    : transform(finalTransform) {
}

void BaseTransformationComponent::assignToEntity(const entt::entity entity, entt::registry& registry) const {
    registry.emplace<BaseTransformationComponent>(entity, transform);
}

void TransformationComponent::unseal() {
    isSealed = false;
}

void TransformationComponent::seal() {
    Matrix matScale = MatrixScale(scale.x, scale.y, scale.z);
    Matrix matRotation = MatrixRotate(rotationAxis, rotationAngle * DEG2RAD);
    Matrix matTranslation = MatrixTranslate(position.x, position.y, position.z);

    transform = MatrixMultiply(MatrixMultiply(matScale, matRotation), matTranslation);
    isSealed = true;
}

TransformationComponent::TransformationComponent(const Vector3& position_, const Vector3& axis_, float angle_, const Vector3& scale_)
    : BaseTransformationComponent(), position(position_), rotationAxis(axis_), rotationAngle(angle_), scale(scale_) {
    seal();
}

void TransformationComponent::assignToEntity(const entt::entity entity, entt::registry& registry) const {
    registry.emplace<TransformationComponent>(entity, position, rotationAxis, rotationAngle, scale);
}
