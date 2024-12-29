#include "engine.hpp"
#include "SDL3/SDL_events.h"

namespace ving
{

Engine::Engine() : m_window{m_instance, 400, 400}, m_core{m_instance, m_window}, m_render_frames{m_core, m_window}
{
}
void Engine::run()
{
    m_running = true;
}
void Engine::update()
{
    SDL_Event event;

    while (SDL_PollEvent(&event))
    {
        if (event.type == SDL_EVENT_QUIT)
            m_running = false;
    }
    RenderFrames::FrameInfo frame = m_render_frames.begin_frame();

    m_render_frames.end_frame();
}

} // namespace ving
