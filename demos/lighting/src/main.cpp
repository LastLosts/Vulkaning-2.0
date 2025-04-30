#include "engine.hpp"
#include "mesh_generator.hpp"
#include "mesh_loader.hpp"
#include "mesh_renderer.hpp"
#include "perspective_camera.hpp"

int main()
{
    ving::Engine engine{};
    ving::PerspectiveCamera camera{
        90.0f, (float)ving::Engine::initial_window_width / (float)ving::Engine::initial_window_height, 0.01f, 100.0f};
    ving::MeshRenderer render{engine.core()};

    std::vector<ving::Mesh> meshes{};
    meshes.resize(1);
    // meshes[0] = generate_quad(engine.core());
    meshes[0] = ving::load_mesh(engine.core(), "./demos/meshes/viking_room.obj");

    while (engine.running())
    {
        if (glfwGetKey(engine.window().window(), GLFW_KEY_W))
            camera.position -= camera.forward() * 0.5f * engine.delta_time();
        if (glfwGetKey(engine.window().window(), GLFW_KEY_S))
            camera.position += camera.forward() * 0.5f * engine.delta_time();
        if (glfwGetKey(engine.window().window(), GLFW_KEY_A))
            camera.position -= glm::vec3{1.0f, 0.0f, 0.0f} * 0.5f * engine.delta_time();
        if (glfwGetKey(engine.window().window(), GLFW_KEY_D))
            camera.position += glm::vec3{1.0f, 0.0f, 0.0f} * 0.5f * engine.delta_time();

        camera.update();

        ving::FrameInfo frame = engine.begin_frame();
        engine.begin_rendering(frame, true);

        render.render(frame, camera, meshes);
        engine.imgui_renderer().render(frame, {[&camera, &engine]() {
                                           ImGui::Text("%f", engine.delta_time());
                                           ImGui::Text("%f %f %f", camera.forward().x, camera.forward().y,
                                                       camera.forward().z);
                                           ImGui::Text("%f %f %f", camera.up().x, camera.up().y, camera.up().z);
                                       }});

        engine.end_rendering(frame);
        engine.end_frame(frame);
    }
}
