#pragma once
#include "gui/colors.hpp"
#include "gui/rectangle.hpp"
#include "renderQuad.hpp"
#include "shader.hpp"

#include <ft2build.h>
#include FT_FREETYPE_H
#include <glm/glm.hpp>

#include <map>

enum class TextAlign {
    BEGIN,
    CENTER,
    END
};

class TextRenderer {
  private:
    struct Character {
        unsigned int textureId;
        glm::ivec2 size;
        glm::ivec2 bearing;
        unsigned int advance;
    };

    RenderQuad quad;
    float screenWidth, screenHeight;

    std::map<char, Character> characters;

  public:
    glm::vec3 textColor = colors::white;

    void init();
    void setScreenSize(float width, float height);

    float getWidth(const std::string& text, float scale) const;
    float getHeight(const std::string& text, float scale, float* baseLineOffset) const;

    void renderText(const std::string& text, const Rectangle& rect, float maxScale, TextAlign align) const;
};