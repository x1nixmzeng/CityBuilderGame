#include "gui/menus/inspectorPanel.hpp"

#include "gui/components/button.hpp"

#include "application.hpp"
#include "game.hpp"
#include "systems/renderSystem.hpp"

#include "events/oskEvents.hpp"

#include "application.hpp"

InspectorPanel::InspectorPanel(Gui* gui)
    : StackPanel("inspector_menu", gui, StackOrientation::COLUMN, colors::anthraziteGrey, ItemAligment::BEGIN) {
    constraints.x = AbsoluteConstraint(0);
    constraints.y = AbsoluteConstraint(0);
    constraints.width = RelativeConstraint(0.3);
    cornerRadius = 0.0f;

    // todo: refactor this nonsense
    iconTexture = gui->getApp()->getGame()->renderSystem->getThumbnail().texture;

    // Confusing api. Last 2 params are the wrong way around.
    /* activeIcon = new IconButton("inspector_menu.icon", gui, colors::anthraziteGrey, &iconTexture);
     activeIcon->constraints.height = AbsoluteConstraint(height);
     activeIcon->constraints.width = AbsoluteConstraint(width);

     addChild(activeIcon);*/
}

void InspectorPanel::render() const {
    Widget::render();

    Rectangle box = getBox();

    auto px = box.width / 2 - static_cast<float>(iconTexture.width) / 2;

    // NOTE: Render texture must be y-flipped due to default OpenGL coordinates (left-bottom)
    DrawTextureRec(iconTexture, {0.0f, 0.0f, (float)iconTexture.width, (float)-iconTexture.height}, {px, box.y}, WHITE);

    DrawText("Press 5 to cycle", box.x, iconTexture.height, 32, BLACK);
}
