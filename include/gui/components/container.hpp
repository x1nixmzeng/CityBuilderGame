#pragma once
#include "../colors.hpp"
#include "widget.hpp"

#include <vector>

// class ContainerItem : public Widget {
//   protected:
//     Widget* element;

//   public:
//     ContainerItem(const std::string& id, Gui* gui, Widget* element, const glm::vec4 backgroundColor = colors::transparent);

//     virtual void show() override;
//     virtual void hide() override;

//     void render() const override;
//     Rectangle getBox() const override;

//     void handleMouseButtonEvent(const MouseButtonEvent& e) override;
//     void handleMouseMoveEvent(const MouseMoveEvent& e) override;
// };

class Container : public Widget {
  protected:
    std::vector<Widget*> children;

  public:
    Container(const std::string& id, Gui* gui, const glm::vec4& backgroundColor);
    virtual void addChild(Widget* child);

    virtual void show() override;
    virtual void hide() override;

    void render() const override;

    virtual void setChildConstraints() = 0;

    void handleMouseButtonEvent(const MouseButtonEvent& e) override;
    void handleMouseMoveEvent(const MouseMoveEvent& e) override;
};
