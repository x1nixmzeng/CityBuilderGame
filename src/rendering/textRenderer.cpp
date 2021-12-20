#include "rendering/textRenderer.hpp"

#include <GL/glew.h>
#include <iostream>

#include <glm/gtc/matrix_transform.hpp>

void TextRenderer::init() {
    FT_Library ft;
    if (FT_Init_FreeType(&ft)) {
        std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
    }

    FT_Face face;
    if (FT_New_Face(ft, "/usr/share/fonts/TTF/Hack-Regular.ttf", 0, &face)) {
        std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;
    }

    FT_Set_Pixel_Sizes(face, 0, 48);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    for (unsigned char c = 0; c < 128; c++) {
        if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
            std::cout << "ERROR::FREETYPE: Failed to load Glyph" << std::endl;
            continue;
        }

        Character character;
        glGenTextures(1, &character.textureId);
        glBindTexture(GL_TEXTURE_2D, character.textureId);
        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RED,
            face->glyph->bitmap.width,
            face->glyph->bitmap.rows,
            0,
            GL_RED,
            GL_UNSIGNED_BYTE,
            face->glyph->bitmap.buffer);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        character.size = glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows);
        character.bearing = glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top);
        character.advance = face->glyph->advance.x;

        characters[c] = character;
    }
    glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
    glBindTexture(GL_TEXTURE_2D, 0);

    FT_Done_Face(face);
    FT_Done_FreeType(ft);

    textShader = new Shader("res/shaders/renderQuad.vert", "res/shaders/text.frag");
}

void TextRenderer::setScreenSize(float width, float height) {
    screenWidth = width;
    screenHeight = height;
}

float TextRenderer::getWidth(const std::string& text, float scale) const {
    float width = 0;
    for (auto it = text.begin(); it != text.end(); it++) {
        const Character& character = characters.at(*it);
        width += (character.advance >> 6) * scale;
    }

    return width;
}

float TextRenderer::getHeight(const std::string& text, float scale, float* baselineOffset) const {
    *baselineOffset = 0;

    float height = 0.0f;
    for (auto it = text.begin(); it != text.end(); it++) {
        const Character& character = characters.at(*it);
        if (height < character.size.y) {
            height = character.size.y;
        }

        float glyphBaselineOffset = (character.size.y - character.bearing.y);
        if (glyphBaselineOffset > *baselineOffset) {
            *baselineOffset = glyphBaselineOffset;
        }
    }

    *baselineOffset *= scale;
    return height * scale;
}

void TextRenderer::renderText(const std::string& text, const Rectangle& rect, float maxScale) const {
    // get width and height of non scaled text
    float baselineOffset;
    float scale = maxScale;

    float width = getWidth(text, scale);
    float height = getHeight(text, scale, &baselineOffset);

    // adjust text size if it not fits into the rectangle
    if (width > rect.width) {
        scale = rect.width / width;

        width *= scale;
        height *= scale;
    }

    if (height > rect.height) {
        // scale = glm::max(scale, rect.height / height);
        scale = rect.height / height;
        
        width *= scale;
        height *= scale;
    }

    float currentX = rect.x + (rect.width - width) * 0.5f;
    float currentY = rect.y + baselineOffset + (rect.height - height) * 0.5f;

    textShader->use();
    textShader->setVector3("textColor", textColor);
    textShader->setInt("text", 0);
    textShader->setMatrix4("projection", glm::ortho(0.0f, screenWidth, 0.0f, screenHeight));

    glActiveTexture(GL_TEXTURE0);

    for (auto it = text.begin(); it != text.end(); it++) {
        Character character = characters.at(*it);

        float xPos = currentX + character.bearing.x * scale;
        float yPos = currentY - (character.size.y - character.bearing.y) * scale;

        float charHeight = character.size.y * scale;
        float charWidth = character.size.x * scale;

        glBindTexture(GL_TEXTURE_2D, character.textureId);

        quad.draw(xPos, yPos, charWidth, charHeight);
        currentX += (character.advance >> 6) * scale;
    }
}