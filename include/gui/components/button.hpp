#pragma once

#include "label.hpp"
#include "widget.hpp"
#include "icon.hpp"

#include <raylib.h>

class Button : public virtual Widget {
  public:
    EventDispatcher<MouseButtonEvent> onClick;

    Button(const std::string& id, Gui* gui, const Color& backgroundColor);

    void handleMouseButtonEvent(MouseButtonEvent& e) override;
};

class TextButton : public Label, public Button {
  public:
    TextButton(const std::string& id, Gui* gui, const Color& backgroundColor, const std::string& text);
};

class IconButton : public Icon, public Button {
  public:
   IconButton(const std::string& id, Gui* gui, const Color& backgroundColor, Texture* icon);
};
