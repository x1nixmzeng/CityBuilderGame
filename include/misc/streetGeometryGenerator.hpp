#pragma once
#include "misc/configuration.hpp"
#include "rendering/geometry.hpp"
#include "streetGraph.hpp"

#include <map>

class StreetGeometryGenerator {
  private:    
    static std::unordered_map<StreetGraphNodeType, GeometryData> geometryData;

    struct Directions {
        static constexpr glm::ivec2 North{1, 0};
        static constexpr glm::ivec2 East{0, 1};
        static constexpr glm::ivec2 South{-1, 0};
        static constexpr glm::ivec2 West{0, -1};
    };

    static constexpr glm::vec3 gridCenterOffset = static_cast<float>(Configuration::gridSize) * glm::vec3{0.5, 0, 0.5};        

    static GeometryData getNodeGeometry(const StreetGraphNode& node);
    static GeometryData getEdgeGeometry(const StreetGraphEdge& edge);    

  public:
    static Geometry* create(const StreetGraph& graph);

    static Geometry* createDebug(const StreetGraph& graph);
};