#include "engine.hpp"
#include "SDL3/SDL_events.h"

namespace ving
{

Engine::Engine() : m_window{400, 400}, m_core{m_window}
{
    // As long as nothing is allocated before throwing we should be fine
    // Even if we do OS will clean after us
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
}

} // namespace ving
