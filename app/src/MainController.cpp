//
// Created by marko-cvijetinovic on 1/17/25.
//

#include <cmath>
#include <engine/core/Engine.hpp>
#include <engine/graphics/GraphicsController.hpp>

#include "MainController.hpp"

#include <future>
#include <chrono>
#include <thread>
#include <spdlog/spdlog.h>

#include "GUIController.hpp"

#include <random>

class MainPlatformEventObserver : public engine::platform::PlatformEventObserver {
public:
    void on_mouse_move(engine::platform::MousePosition position) override;
};

void MainPlatformEventObserver::on_mouse_move(engine::platform::MousePosition position) {
    //auto camera = engine::core::Controller::get<engine::graphics::GraphicsController>()->camera();
    //camera->process_mouse_movement(position.x, position.y, false);
}

void MainController::initialize() {
    auto platform = get<engine::platform::PlatformController>();
    platform->register_platform_event_observer(std::make_unique<MainPlatformEventObserver>());
    engine::graphics::OpenGL::enable_depth_testing();
}

bool MainController::loop() {
    auto platform = get<engine::platform::PlatformController>();
    if (platform->key(engine::platform::KeyId::KEY_ESCAPE).is_down())
        return false;

    return true;
}

void MainController::draw_phoenix() {
    auto resources = get<engine::resources::ResourcesController>();
    auto phoenix   = resources->model("phoenix");
    auto shader    = resources->shader("planet");
    shader->use();

    glm::mat4 model = glm::mat4(1.0f);
    model           = translate(model, glm::vec3(-2.0f, 0.0f, -3.0f));
    model           = scale(model, glm::vec3(0.8f));
    model           = rotate(model, glm::radians(-20.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    shader->set_mat4("model", model);

    phoenix->draw(shader);
}

void MainController::draw_spaceship() {
    auto resources = get<engine::resources::ResourcesController>();
    auto spaceship = resources->model("spaceship");
    auto shader    = resources->shader("planet");
    shader->use();

    glm::mat4 model = glm::mat4(1.0f);
    model           = translate(model, m_csillaPos + glm::vec3(0.1f, 0.2f, 1.4f));
    model           = scale(model, glm::vec3(0.001f));
    model           = rotate(model, glm::radians(20.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    shader->set_mat4("model", model);

    spaceship->draw(shader);
}

void MainController::configure_planet() {
    auto resources = get<engine::resources::ResourcesController>();
    auto shader    = resources->shader("planet");
    shader->use();

    auto graphics = get<engine::graphics::GraphicsController>();
    shader->set_mat4("projection", graphics->projection_matrix());
    shader->set_mat4("view", graphics->camera()->view_matrix());

    auto camera = graphics->camera();
    shader->set_vec3("viewPos", camera->Position);

    set_spot_light(shader);
    set_star_light(shader);
}

void MainController::draw_csilla() {
    auto resources = get<engine::resources::ResourcesController>();
    auto csilla    = resources->model("csilla");
    auto shader    = resources->shader("planet");
    shader->use();

    glm::mat4 model = glm::mat4(1.0f);
    model           = translate(model, m_csillaPos);
    model           = scale(model, glm::vec3(0.1f));
    shader->set_mat4("model", model);

    csilla->draw(shader);

    shader->set_float("lightModifier", 1.3f);
    for (int i = 0; i < 4; i++) {
        model = glm::mat4(1.0f);
        model = translate(model, m_starPos + glm::vec3(2.5f - i % 2 * 5.0f, 0.0f, 2.5f - i / 2 * 5.0f));
        model = scale(model, glm::vec3(0.1f));
        model = rotate(model, glm::radians(20.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        shader->set_mat4("model", model);

        csilla->draw(shader);
    }
    shader->set_float("lightModifier", 1.0f);
}

void MainController::draw_terran() {
    auto resources = get<engine::resources::ResourcesController>();
    auto mars      = resources->model("terran");
    auto shader    = resources->shader("planet");
    shader->use();

    glm::mat4 model = glm::mat4(1.0f);
    model           = translate(model, glm::vec3(4.0f, 0.0f, -2.0f));
    model           = scale(model, glm::vec3(m_terranScale));
    shader->set_mat4("model", model);

    mars->draw(shader);
}

void MainController::draw_star() {
    auto resources = get<engine::resources::ResourcesController>();
    auto star      = resources->model("star");
    auto shader    = resources->shader("star");
    shader->use();

    auto graphics = get<engine::graphics::GraphicsController>();
    shader->set_mat4("projection", graphics->projection_matrix());
    shader->set_mat4("view", graphics->camera()->view_matrix());
    glm::mat4 model = glm::mat4(1.0f);
    model           = translate(model, m_starPos);
    model           = scale(model, glm::vec3(0.6f));
    shader->set_mat4("model", model);

    auto camera = graphics->camera();
    shader->set_vec3("viewPos", camera->Position);

    star->draw(shader);
}

void MainController::set_spot_light(engine::resources::Shader *shader) {
    auto graphics = get<engine::graphics::GraphicsController>();
    auto camera   = graphics->camera();

    shader->set_vec3("light.position", camera->Position);
    shader->set_vec3("light.direction", camera->Front);
    shader->set_float("light.cutOff", glm::cos(glm::radians(10.5f)));
    shader->set_float("light.outerCutOff", glm::cos(glm::radians(12.5f)));

    // light properties
    shader->set_vec3("light.ambient", glm::vec3(0.1f, 0.1f, 0.1f));
    // we configure the diffuse intensity slightly higher; the right lighting conditions differ with each lighting method and environment.
    // each environment and lighting type requires some tweaking to get the best out of your environment.
    shader->set_vec3("light.diffuse", m_spotLightColor);
    shader->set_vec3("light.specular", m_spotLightColor);
    shader->set_float("light.constant", 1.0f);
    shader->set_float("light.linear", 0.35f);
    shader->set_float("light.quadratic", 0.44f);
}

void MainController::set_star_light(engine::resources::Shader *shader) {
    shader->set_vec3("lightPos", m_starPos);
    shader->set_vec3("lightColor", m_starColor);
}

void MainController::begin_draw() {
    engine::graphics::OpenGL::clear_buffers();

    configure_planet();
}

void MainController::draw_skybox() {
    auto resources = get<engine::resources::ResourcesController>();
    auto skybox    = resources->skybox("galaxy_skybox");
    auto shader    = resources->shader("skybox");
    auto graphics  = get<engine::graphics::GraphicsController>();
    graphics->draw_skybox(shader, skybox);
}

void MainController::draw() {
    configure_planet();
    draw_phoenix();
    draw_csilla();
    draw_spaceship();
    draw_terran();
    draw_star();
    draw_skybox();
    draw_star();
}

void MainController::end_draw() {
    auto platform = get<engine::platform::PlatformController>();
    platform->swap_buffers();
}

void MainController::update() {
    update_camera();
}

void MainController::update_camera() {
    auto gui = engine::core::Controller::get<GUIController>();
    if (gui->is_enabled()) {
        return;
    }
    auto platform = engine::core::Controller::get<engine::platform::PlatformController>();
    auto camera = engine::core::Controller::get<engine::graphics::GraphicsController>()->camera();
    float dt = platform->dt();
    if (platform->key(engine::platform::KEY_W)
                .state() == engine::platform::Key::State::Pressed) {
        camera->move_camera(engine::graphics::Camera::Movement::FORWARD, dt);
                }
    if (platform->key(engine::platform::KEY_S)
                .state() == engine::platform::Key::State::Pressed) {
        camera->move_camera(engine::graphics::Camera::Movement::BACKWARD, dt);
                }
    if (platform->key(engine::platform::KEY_A)
                .state() == engine::platform::Key::State::Pressed) {
        camera->move_camera(engine::graphics::Camera::Movement::LEFT, dt);
                }
    if (platform->key(engine::platform::KEY_D)
                .state() == engine::platform::Key::State::Pressed) {
        camera->move_camera(engine::graphics::Camera::Movement::RIGHT, dt);
                }
    auto mouse = platform->mouse();
    camera->rotate_camera(mouse.dx, mouse.dy);
    camera->zoom(mouse.scroll);
}

void MainController::poll_events() {
    const auto platform = get<engine::platform::PlatformController>();
    if (platform->key(engine::platform::KEY_F1).state() == engine::platform::Key::State::JustPressed) {
        cursor_enabled = !cursor_enabled;
        platform->set_enable_cursor(cursor_enabled);
    }

    if (platform->key(engine::platform::KeyId::KEY_J).is_down()) {
        m_spotLightColor[0] += 0.02f;
    }
    if (platform->key(engine::platform::KeyId::KEY_K).is_down()) {
        m_spotLightColor[1] += 0.02f;
    }
    if (platform->key(engine::platform::KeyId::KEY_L).is_down()) {
        m_spotLightColor[2] += 0.02f;
    }
    if (platform->key(engine::platform::KeyId::KEY_I).is_down()) {
        m_spotLightColor[0] = std::max(m_spotLightColor[0] - 0.02f, 0.0f);
    }
    if (platform->key(engine::platform::KeyId::KEY_O).is_down()) {
        m_spotLightColor[1] = std::max(m_spotLightColor[1] - 0.02f, 0.0f);
    }
    if (platform->key(engine::platform::KeyId::KEY_P).is_down()) {
        m_spotLightColor[2] = std::max(m_spotLightColor[2] - 0.02f, 0.0f);
    }
}
