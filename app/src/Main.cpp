#include <engine/core/Engine.hpp>

#include <MyApp.hpp>

/**
 * Start here...
 */
auto main(int argc, char **argv) -> int {
    auto app = std::make_unique<MyApp>();
    return app->run(argc, argv);
}
