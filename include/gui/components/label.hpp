#pragma once
#include "../colors.hpp"
#include "widget.hpp"
#include "../colors.hpp"

#include <string>
#include <raylib.h>

enum class TextAlign {
    BEGIN,
    CENTER,
    END
};

class Label : public virtual Widget {
  public:
    std::string text;
    Color textColor;

    TextAlign textAlign = TextAlign::CENTER;

    Label(const std::string& id, Gui* gui, const Color& backgroundColor, const std::string& text, const Color& textColor = colors::white);

    virtual void render() const override;
};
