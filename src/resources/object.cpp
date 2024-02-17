#include "resources/object.hpp"

#include "components/components.hpp"

template<AssignableComponentType T>
void Object::addComponent(const T& component) {    
    components[typeid(T)] = std::make_shared<T>(component);
}

entt::entity Object::create(entt::registry& registry) const {
    const entt::entity entity = registry.create();

    for (const auto& [type, component] : components) {        
        component->assignToEntity(entity, registry);
    }

    return entity;
}

template void Object::addComponent<MeshComponent>(const MeshComponent&);
template void Object::addComponent<TransformationComponent>(const TransformationComponent&);
template void Object::addComponent<RouteComponent>(const RouteComponent&);
template void Object::addComponent<LaraComponent>(const LaraComponent&);
template void Object::addComponent<AutoNavComponent>(const AutoNavComponent&);
