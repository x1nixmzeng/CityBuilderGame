#pragma once
#include "../components/stackPanel.hpp"

#include <raylib.h>

class InspectorPanel : public StackPanel {
  public:
    InspectorPanel(Gui* gui);

    Texture2D iconTexture;

  protected:
    void render() const override;
};
