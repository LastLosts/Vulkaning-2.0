#include "engine.hpp"
#include "mesh_generator.hpp"
#include "mesh_renderer.hpp"
#include "perspective_camera.hpp"

int main()
{
    ving::Engine engine{};
    ving::PerspectiveCamera camera{90.0f, ving::Engine::initial_window_width / ving::Engine::initial_window_height,
                                   0.01f, 100.0f};
    ving::MeshRenderer render{engine.core()};

    // std::array<Vertex, 4> vertices{
    //     Vertex{{-0.5f, 0.5f, 0.0f}, 0.0f, {}, 0.0f},  // Bottom Left
    //     Vertex{{-0.5f, -0.5f, 0.0f}, 0.0f, {}, 1.0f}, //
    //     Vertex{{0.5f, -0.5f, 0.0f}, 1.0f, {}, 1.0f},  //
    //     Vertex{{0.5f, 0.5f, 0.0f}, 1.0f, {}, 0.0f},   // Bottom Right
    // };
    //
    // std::array<uint32_t, 6> indices{0, 1, 2, 2, 3, 0};

    std::vector<ving::Mesh> meshes{};
    meshes.resize(1);
    meshes[0] = generate_quad(engine.core());

    while (engine.running())
    {
        camera.update();

        ving::FrameInfo frame = engine.begin_frame();
        engine.begin_rendering(frame, true);

        render.render(frame, camera, meshes);
        engine.imgui_renderer().render(frame, {[]() { ImGui::Text("Hello Lighting"); }});

        engine.end_rendering(frame);
        engine.end_frame(frame);
    }
}
