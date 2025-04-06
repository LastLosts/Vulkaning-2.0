#include "engine.hpp"

int main()
{
    ving::Engine engine{};

    while (engine.running())
    {
        ving::FrameInfo frame = engine.begin_frame();
        engine.begin_rendering(frame, true);

        engine.imgui_renderer().render(frame, {[]() { ImGui::Text("Hello Lighting"); }});

        engine.end_rendering(frame);
        engine.end_frame(frame);
    }
}
