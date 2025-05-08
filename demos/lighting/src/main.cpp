#include "engine.hpp"
#include "glm/trigonometric.hpp"
#include "imgui_utils.hpp"
#include "mesh_loader.hpp"
#include "mesh_renderer.hpp"
#include "perspective_camera.hpp"

static constexpr float camera_rotate_speed = 40.0f;
static constexpr float camera_move_speed = 1.0f;

int main()
{
    ving::Engine engine{};
    ving::PerspectiveCamera camera{
        glm::radians(60.0f), (float)ving::Engine::initial_window_width / (float)ving::Engine::initial_window_height,
        0.01f, 10000.0f};
    ving::MeshRenderer render{engine.core()};

    std::vector<ving::Mesh> meshes{};

    meshes.push_back(ving::load_mesh(engine.core(), "./demos/meshes/DragonAttenuation.obj"));
    camera.position.z = 0.5f;

    while (engine.running())
    {
        glm::vec3 camera_right = glm::cross(camera.up(), camera.forward());
        if (glfwGetKey(engine.window().window(), GLFW_KEY_W))
            camera.position -= camera.forward() * camera_move_speed * engine.delta_time();
        if (glfwGetKey(engine.window().window(), GLFW_KEY_S))
            camera.position += camera.forward() * camera_move_speed * engine.delta_time();
        if (glfwGetKey(engine.window().window(), GLFW_KEY_A))
            camera.position -= camera_right * camera_move_speed * engine.delta_time();
        if (glfwGetKey(engine.window().window(), GLFW_KEY_D))
            camera.position += camera_right * camera_move_speed * engine.delta_time();

        if (glfwGetKey(engine.window().window(), GLFW_KEY_SPACE))
            camera.position -= camera.up() * camera_move_speed * engine.delta_time();
        if (glfwGetKey(engine.window().window(), GLFW_KEY_C))
            camera.position += camera.up() * camera_move_speed * engine.delta_time();

        if (glfwGetKey(engine.window().window(), GLFW_KEY_DOWN))
            camera.pitch += camera_rotate_speed * engine.delta_time();
        if (glfwGetKey(engine.window().window(), GLFW_KEY_UP))
            camera.pitch -= camera_rotate_speed * engine.delta_time();
        if (glfwGetKey(engine.window().window(), GLFW_KEY_LEFT))
            camera.yaw += camera_rotate_speed * engine.delta_time();
        if (glfwGetKey(engine.window().window(), GLFW_KEY_RIGHT))
            camera.yaw -= camera_rotate_speed * engine.delta_time();

        camera.update();

        ving::FrameInfo frame = engine.begin_frame();
        engine.begin_rendering(frame, true);

        render.render(frame, camera, meshes);
        engine.imgui_renderer().render(frame, {[&camera, &engine]() {
                                           ImGui::Text("%f", engine.delta_time());
                                           ImGui::Text("%f", engine.time());
                                           ImGui::Text("Forward %f %f %f", camera.forward().x, camera.forward().y,
                                                       camera.forward().z);
                                           ImGui::Text("Up %f %f %f", camera.up().x, camera.up().y, camera.up().z);
                                           ving::imgui_text_vec(camera.position);
                                       }});

        engine.end_rendering(frame);
        engine.end_frame(frame);
    }
}
