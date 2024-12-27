#include "engine.hpp"
#include <iostream>

int main()
{
    try
    {
        ving::Engine engine{};

        engine.run();

        while (engine.running())
        {
            engine.update();
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what();
    }
    catch (...)
    {
        std::cerr << "Idk";
    }
    return 0;
}
