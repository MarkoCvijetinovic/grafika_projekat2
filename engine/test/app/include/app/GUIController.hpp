
#ifndef GUICONTROLLER_HPP
#define GUICONTROLLER_HPP

#include <engine/core/Engine.hpp>

namespace engine::test::app {
class GUIController final : public engine::core::Controller {
public:
    [[nodiscard]] auto name() const -> std::string_view override {
        return "test::app::GUIController";
    }

private:
    void initialize() override;

    void poll_events() override;

    void draw() override;
};
}
#endif //GUICONTROLLER_HPP
