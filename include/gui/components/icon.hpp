#pragma once
#include "widget.hpp"
#include "gui/colors.hpp"

#include <raylib.h>

class Icon : public virtual Widget {
  protected:
    Texture* texture;

  public:
    Icon(const std::string& id, Gui* gui, Texture* texture, const Color& backgroundColor = colors::transparent);

    void render() const override;
};