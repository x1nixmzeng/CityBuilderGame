#include "misc/streetGeometryGenerator.hpp"

#include "modelLoader.hpp"
#include "misc/utility.hpp"

std::unordered_map<StreetGraphNodeType, GeometryData> StreetGeometryGenerator::geometryData = {
    std::make_pair<StreetGraphNodeType, GeometryData>(StreetGraphNodeType::END_NOT_CONNECTED, ModelLoader::load("res/models/street_notConnected.obj")),
    std::make_pair<StreetGraphNodeType, GeometryData>(StreetGraphNodeType::END, ModelLoader::load("res/models/street_end.obj")),
    std::make_pair<StreetGraphNodeType, GeometryData>(StreetGraphNodeType::CURVE, ModelLoader::load("res/models/street_curve.obj")),
    std::make_pair<StreetGraphNodeType, GeometryData>(StreetGraphNodeType::T_CROSSING, ModelLoader::load("res/models/street_t_crossing.obj")),
    std::make_pair<StreetGraphNodeType, GeometryData>(StreetGraphNodeType::CROSSING, ModelLoader::load("res/models/street_crossing.obj")),
    std::make_pair<StreetGraphNodeType, GeometryData>(StreetGraphNodeType::EDGE, ModelLoader::load("res/models/street_straight.obj"))
};

GeometryData StreetGeometryGenerator::getNodeGeometry(const StreetGraphNode& node) {

    int connectionsCount = (node.connections[0] ? 1 : 0) + (node.connections[1] ? 1 : 0) + (node.connections[2] ? 1 : 0) + (node.connections[3] ? 1 : 0);

    StreetGraphNodeType type = static_cast<StreetGraphNodeType>(connectionsCount);
    int rotation;
    GeometryData data = geometryData[type];

    switch (type) {
    case StreetGraphNodeType::END:
        // north connected
        if (node.connections[0]) {
            rotation = 0;
        }
        // east connected
        else if (node.connections[1]) {
            rotation = 1;
        }
        // south connected
        else if (node.connections[2]) {
            rotation = 2;
        }
        // west connected
        else if (node.connections[3]) {
            rotation = 3;
        }

        break;
    case StreetGraphNodeType::CURVE:
        // north connected
        if (node.connections[0]) {
            // east connected
            if (node.connections[1]) {
                rotation = 0;
            }
            // west connected
            else if (node.connections[3]) {
                rotation = 3;
            }
        }
        // south connected
        else if (node.connections[2]) {
            // east connected
            if (node.connections[1]) {
                rotation = 1;
            }
            // west connected
            else if (node.connections[3]) {
                rotation = 2;
            }
        }
        break;
    case StreetGraphNodeType::T_CROSSING:
        // north and south connected
        if (node.connections[0] && node.connections[2]) {
            // east connected
            if (node.connections[1]) {
                rotation = 0;
            }
            else if (node.connections[3]) {
                rotation = 2;
            }
        }
        // east and west connected
        if (node.connections[1] && node.connections[3]) {
            // north connected
            if (node.connections[0]) {
                rotation = 3;
            }
            // south connected
            else if (node.connections[2]) {
                rotation = 1;
            }
        }
        break;
    default:
        rotation = 0;
        break;
    }

    // transform vertex positions
    const glm::vec3& worldCoords = utility::toWorldCoords(node.position) + gridCenterOffset;
    float angle = glm::radians(rotation * 90.0f);

    const glm::mat3& rotationMat = glm::mat3{
        static_cast<float>(cos(angle)), static_cast<float>(0), static_cast<float>(sin(angle)),
        static_cast<float>(0), static_cast<float>(1), static_cast<float>(0),
        static_cast<float>(-sin(angle)), static_cast<float>(0), static_cast<float>(cos(angle))};

    for (Vertex& vert : data.vertices) {
        vert.position = worldCoords + rotationMat * vert.position;
    }

    // return transformed data
    return data;
}

GeometryData StreetGeometryGenerator::getEdgeGeometry(const StreetGraphEdge& edge) {
    // get edge length and direction
    glm::ivec2 diff = edge.end - edge.start;

    int edgeLength = abs(diff.x != 0 ? diff.x : diff.y);
    if (edgeLength == 0) {
        return GeometryData{};
    }

    glm::ivec2 directionVec = diff / edgeLength;

    bool horizontal = directionVec.y != 0;
    glm::vec3 beginOffset = utility::toWorldCoords(edge.start) + gridCenterOffset;
    glm::vec3 stepOffset = utility::toWorldCoords(directionVec);

    // transform vertex data for each grid cell and merge them together
    GeometryData data;

    const GeometryData& edgeData = geometryData[StreetGraphNodeType::EDGE];

    if (horizontal) {

        const glm::mat3& rotation = glm::mat3{
            static_cast<float>(0), static_cast<float>(0), static_cast<float>(-1),
            static_cast<float>(0), static_cast<float>(1), static_cast<float>(0),
            static_cast<float>(1), static_cast<float>(0), static_cast<float>(0)};

        for (int i = 1; i < edgeLength; i++) {
            data.addData(GeometryData::transformVertices(edgeData, [&](const Vertex& vert) {
                return Vertex{
                    rotation * vert.position + beginOffset + static_cast<float>(i) * stepOffset,
                    vert.texCoord,
                    vert.normal};
            }));
        }
    }
    else {
        for (int i = 1; i < edgeLength; i++) {
            data.addData(GeometryData::transformVertices(edgeData, [&](const Vertex& vert) {
                return Vertex{
                    vert.position + beginOffset + static_cast<float>(i) * stepOffset,
                    vert.texCoord,
                    vert.normal};
            }));
        }
    }

    return data;
}

Geometry* StreetGeometryGenerator::create(const StreetGraph& graph) {
    GeometryData data;
    for (const auto& [position, node] : graph.nodes) {
        data.addData(getNodeGeometry(node));
    }

    for (const auto& edge : graph.edges) {
        data.addData(getEdgeGeometry(edge));
    }

    return new MeshGeometry(data);
}

Geometry* StreetGeometryGenerator::createDebug(const StreetGraph& graph) {
    std::vector<float> vertexData;
    std::vector<unsigned int> indices;

    unsigned int index = 0;
    for (auto& [pos, node] : graph.nodes) {
        float x = Configuration::gridSize * (pos.x + 0.5f);
        float z = Configuration::gridSize * (pos.y + 0.5f);

        vertexData.insert(vertexData.end(), {x, 0.0f, z, 0.0f, 1.0f, 0.0f, 1.0f,
                                             x, 2.0f, z, 0.0f, 1.0f, 0.0f, 1.0f});

        indices.insert(indices.end(), {index, index + 1});

        index += 2;
    }

    for (auto& [start, end] : graph.edges) {
        float x1 = Configuration::gridSize * (start.x + 0.5f);
        float z1 = Configuration::gridSize * (start.y + 0.5f);

        float x2 = Configuration::gridSize * (end.x + 0.5f);
        float z2 = Configuration::gridSize * (end.y + 0.5f);

        vertexData.insert(vertexData.end(), {x1, 1.0f, z1, 0.0f, 0.0f, 1.0f, 1.0f,
                                             x2, 1.0f, z2, 0.0f, 0.0f, 1.0f, 1.0f});

        indices.insert(indices.end(), {index, index + 1});

        index += 2;
    }

    Geometry* geometry = new Geometry(VertexAttributes{{{3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)0},
                                                        {4, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(3 * sizeof(float))}}},
                                      GL_LINES);

    geometry->fillBuffers(vertexData, indices);
    return geometry;
}
