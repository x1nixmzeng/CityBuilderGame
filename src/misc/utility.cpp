#include "misc/utility.hpp"

template<typename T, glm::qualifier Q>
std::ostream& operator<<(std::ostream& os, const glm::vec<1, T, Q>& vec) {
    return os << "x: " << vec.x;
}

template<typename T, glm::qualifier Q>
std::ostream& operator<<(std::ostream& os, const glm::vec<2, T, Q>& vec) {
    return os << "x: " << vec.x << " y: " << vec.y;
}

template<typename T, glm::qualifier Q>
std::ostream& operator<<(std::ostream& os, const glm::vec<3, T, Q>& vec) {
    return os << "x: " << vec.x << " y: " << vec.y << " z: " << vec.z;
}

template<typename T, glm::qualifier Q>
std::ostream& operator<<(std::ostream& os, const glm::vec<4, T, Q>& vec) {
    return os << "x: " << vec.x << " y: " << vec.y << " z: " << vec.z << " w: " << vec.w;
}

std::ostream& operator<<(std::ostream& os, BuildingType type) {
    static std::string typeNames[] = {"DEFAULT", "ROAD"};

    return os << typeNames[(unsigned int)type - 1];
}

template std::ostream& operator<< <int, glm::packed_highp>(std::ostream&, const glm::vec<2, int, glm::packed_highp>&);