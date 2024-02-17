#include "gui/components/icon.hpp"

#include "gui/gui.hpp"

Icon::Icon(const std::string& id, Gui* gui, Texture* texture, const Color& backgroundColor)
    : Widget(id, gui, backgroundColor), texture(texture) {
}

void Icon::render() const {

    Widget::render();

    /*

    glActiveTexture(GL_TEXTURE0);

    const Rectangle& box = getBox();

    Shader* guiShader = gui->getShader();
    guiShader->setInt("tex", 0);
    guiShader->setBool("useTexture", true);

    texture->use(0);

    gui->getRenderQuad().draw(box.x, box.y, box.width, box.height);

    guiShader->setBool("useTexture", false);*/
}