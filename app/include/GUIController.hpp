//
// Created by marko-cvijetinovic on 1/18/25.
//

#ifndef GUICONTROLLER_HPP
#define GUICONTROLLER_HPP

#include <engine/core/Engine.hpp>

class GUIController : public engine::core::Controller {
public:
    [[nodiscard]] auto name() const -> std::string_view override {
        return "GUIController";
    }

    void initialize() override;

    void draw() override;

    void poll_events() override;
};

#endif//GUICONTROLLER_HPP
