#pragma once
#include "../components/stackPanel.hpp"

class IconButton;
class Texture;

class BuildMenu : public StackPanel {
  private:
    Texture* liftTerrainButtonTexture;
    Texture* lowerTerrainButtonTexture;
    Texture* streetButtonTexture;
    Texture* parkingLotButtonTexture;

    IconButton* liftTerrainButton;
    IconButton* lowerTerrainButton;
    IconButton* streetButton;
    IconButton* parkingLotButton;

  public:
    BuildMenu(Gui* gui);
};
