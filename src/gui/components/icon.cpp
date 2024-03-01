#include "gui/components/icon.hpp"

#include "gui/gui.hpp"

Icon::Icon(const std::string& id, Gui* gui, Texture* texture, const Color& backgroundColor)
    : Widget(id, gui, backgroundColor), texture(texture) {
}

void Icon::render() const {

    Widget::render();

    if (texture) {
        Rectangle box = getBox();

        Rectangle source = {0.0f, 0.0f, (float)texture->width, (float)texture->height};
        Rectangle dest = {box.x, box.y, box.width, box.height};
        Vector2 origin = {0.0f, 0.0f};

        DrawTexturePro(*texture, source, dest, origin, 0.0f, backgroundColor);
    }
}