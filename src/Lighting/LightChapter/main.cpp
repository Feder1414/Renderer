#include <format>
#include <fstream>
#include <iostream>
#include "OpenGL.h"
#include "Shader.h"
#include "Texture.h"
#include <vector>

#include "Engine.h"

int main()
{
    std::unique_ptr<Engine> engine = std::make_unique<Engine>();
    engine->Initialize();
    engine->SetScene();
    engine->GameLoop();
    // try
    // {
    //     engine->Initialize();
    //     engine->SetScene();
    //     engine->GameLoop();
    // }
    // catch (const std::exception& e)
    // {
    //     std::cerr << "Caught exceptio initializing engine" << e.what() << std::endl;
    // }


    return 0;
}
