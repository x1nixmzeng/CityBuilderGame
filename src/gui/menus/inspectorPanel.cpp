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

    auto info1 = new Label("inspector_menu.info_1", gui, colors::transparent, "Press 5 to cycle", colors::black);
    info1->constraints.height = AbsoluteConstraint(30);
    info1->constraints.width = RelativeConstraint(0.9);
    addChild(info1);

    auto info2 = new Label("inspector_menu.info_2", gui, colors::transparent, "Press 6 to generate", colors::black);
    info2->constraints.height = AbsoluteConstraint(30);
    info2->constraints.width = RelativeConstraint(0.9);
    addChild(info2);
}

void InspectorPanel::render() const {
    StackPanel::render();

    Rectangle box = getBox();

    auto px = box.width / 2 - static_cast<float>(iconTexture.width) / 2;

    // NOTE: Render texture must be y-flipped due to default OpenGL coordinates (left-bottom)
    DrawTextureRec(iconTexture, {0.0f, 0.0f, (float)iconTexture.width, (float)-iconTexture.height}, {px, box.y}, WHITE);
}
