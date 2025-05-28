#include "engine.hpp"
#include "imgui_utils.hpp"
#include "math/vec_functions.hpp"
#include "mesh_generator.hpp"
#include "mesh_loader.hpp"
#include "model.hpp"
#include "model_renderer.hpp"
#include "perspective_camera.hpp"

#include "math/trigonometry.hpp"
#include "math/vec3.hpp"

#include "output_operators.hpp"
#include <iostream>

static constexpr float camera_rotate_speed = 40.0f;
static constexpr float camera_move_speed = 1.0f;

using ving::vec2;
using ving::vec3;
using ving::vec4;

int main()
{
    ving::Window window{ving::Engine::initial_window_width, ving::Engine::initial_window_height};
    ving::Engine engine{window};
    ving::PerspectiveCamera camera{
        ving::radians(60.0f), (float)ving::Engine::initial_window_width / (float)ving::Engine::initial_window_height,
        0.01f, 10000.0f};
    ving::ModelRenderer render{engine.core()};

    std::vector<ving::Mesh> meshes{};
    meshes.push_back(ving::load_mesh(engine.core(), "./demos/meshes/DragonAttenuation.obj"));

    std::vector<ving::Model> models{{&meshes[0]}};

    // meshes.push_back(ving::generate_quad(engine.core()));
    camera.position.z = -0.05f;

    while (!glfwWindowShouldClose(window.window()))
    {
        vec3 camera_right = ving::cross(camera.up(), camera.forward());
        if (window.get_key(GLFW_KEY_W))
            camera.position += camera.forward() * camera_move_speed * engine.delta_time();
        if (window.get_key(GLFW_KEY_S))
            camera.position -= camera.forward() * camera_move_speed * engine.delta_time();
        if (window.get_key(GLFW_KEY_A))
            camera.position -= camera_right * camera_move_speed * engine.delta_time();
        if (window.get_key(GLFW_KEY_D))
            camera.position += camera_right * camera_move_speed * engine.delta_time();

        if (window.get_key(GLFW_KEY_SPACE))
            camera.position -= camera.up() * camera_move_speed * engine.delta_time();
        if (window.get_key(GLFW_KEY_C))
            camera.position += camera.up() * camera_move_speed * engine.delta_time();

        if (window.get_key(GLFW_KEY_DOWN))
            camera.pitch -= camera_rotate_speed * engine.delta_time();
        if (window.get_key(GLFW_KEY_UP))
            camera.pitch += camera_rotate_speed * engine.delta_time();
        if (window.get_key(GLFW_KEY_LEFT))
            camera.yaw += camera_rotate_speed * engine.delta_time();
        if (window.get_key(GLFW_KEY_RIGHT))
            camera.yaw -= camera_rotate_speed * engine.delta_time();

        camera.update();

        ving::FrameInfo frame = engine.begin_frame(VkExtent2D{window.width(), window.height()});
        engine.begin_rendering(frame, true, {window.width(), window.height()});

        render.render(frame, camera, models);

        engine.imgui_renderer().render(frame, [&]() {
            ImGui::Text("%f", engine.delta_time());
            ImGui::Text("FPS: %f", 1.0f / engine.delta_time());
            ImGui::Text("%f", engine.time());
            ving::imgui_text_vec(camera.forward(), "forward:");
            ving::imgui_text_vec(camera.up(), "up");
            ving::imgui_text_vec(camera_right, "right");
            ving::imgui_text_vec(camera.position, "camera pos");
            ving::imgui_drag_model_transform(models[0]);
        });

        engine.end_rendering(frame);
        engine.end_frame(frame);
    }
}
