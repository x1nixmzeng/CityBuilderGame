#include "gui/components/label.hpp"
#include "gui/gui.hpp"

#include <raylib.h>

Label::Label(const std::string& id, Gui* gui, const Color& backgroundColor, const std::string& text, const Color& textColor)
    : Widget(id, gui, backgroundColor), text(text), textColor(textColor) {
}

void Label::render() const {
    if (!visible) {
        return;
    }

    Widget::render();

    const Rectangle& box = getBox();

    auto fontSize = 28;

    switch (textAlign) {
        case TextAlign::CENTER: {
            int size = MeasureText(text.c_str(), fontSize);
            auto startX = box.x + (box.width * 0.5f) - (size * 0.5f);
            auto startY = box.y + (box.height * 0.5f) - (fontSize * 0.5f);
            DrawText(text.c_str(), startX, startY, fontSize, textColor);
        } break;

        default:
            DrawText(text.c_str(), box.x, box.y, fontSize, textColor);
    }
}
