#include <iostream>
#include <vector>
#include "\10-ProjetVolumineux\ProgAvance\SDL2-devel-2.0.16-VC\SDL2-2.0.16\include\SDL.h"
#include "Constants.h"
#include "GameSPH.h"
#include "ParticleManager.h"
using namespace std;

GameSPH::GameSPH() :
    pause(false)
{
    particleManager.init(200);
}
GameSPH::~GameSPH()
{
    clearHistory();
}

void GameSPH::handleInput(const SDL_Event& event)
{
    const Uint8* state = SDL_GetKeyboardState(NULL);

    switch (event.type)
    {
    case SDL_MOUSEBUTTONDOWN:
    {
        int x, y;
        SDL_GetMouseState(&x, &y);

        if (event.button.button == SDL_BUTTON_LEFT)
            addCommand(new CommandAddBlock(particleManager, x, y));
        else if (event.button.button == SDL_BUTTON_RIGHT)
            addCommand(new CommandAddOne(particleManager, x, y));
        break;
    }
    case SDL_KEYUP:
        switch (event.key.keysym.sym)
        {
        case SDLK_LCTRL:
            control = false;
            break;
        case SDLK_LSHIFT:
            shift = false;
            break;
        }
    case SDL_KEYDOWN:
        switch (event.key.keysym.sym)
        {
        case SDLK_LCTRL:
            control = true;
            break;
        case SDLK_LSHIFT:
            shift = true;
            break;
        case SDLK_SPACE:
            particleManager.explode();
            break;
        case SDLK_LEFT:
            particleManager.setGravity(LEFT);
            break;
        case SDLK_RIGHT:
            particleManager.setGravity(RIGHT);
            break;
        case SDLK_UP:
            particleManager.setGravity(UP);
            break;
        case SDLK_DOWN:
            particleManager.setGravity(DOWN);
            break;
        case SDLK_c:
        {
            std::shared_ptr<Command> ccolor(new ChangeColorCommand(&particleManager));
            commandManager.executeCmd(ccolor);
            break;
        }
        case SDLK_z:
        {
            if (control)
            {
                commandManager.undo();
                if (shift)
                {
                    commandManager.redo();
                }
            }
            break;
        }
            // Display
        case SDLK_a:
            particleManager.renderMode = "renderMode:particles";
            break;

        case SDLK_s:
            particleManager.renderMode = "renderMode:particles+grid";
            break;

        case SDLK_d:
            particleManager.renderMode = "renderMode:grid";
            break;

            // Pause
        case SDLK_p:
            pause = !pause;
            break;

            // Number of particles
        case SDLK_1:
        case SDLK_2:
        case SDLK_3:
        case SDLK_4:
        case SDLK_5:
        case SDLK_6:
        case SDLK_7:
        case SDLK_8:
        case SDLK_9:
            particleManager.init(presets[event.key.keysym.sym - SDLK_1]);
            break;
        case SDLK_ESCAPE:
            keepPlaying = false;
            break;
        }
    }
}

void GameSPH::update(unsigned long dt)
{
    if(pause) return;

    particleManager.update(dt);
}

void GameSPH::render(SDL_Renderer* renderer)
{
    // -- Clear screen --
    SDL_SetRenderDrawColor(renderer, 220, 220, 220, 255);
    SDL_RenderClear(renderer);

    // Draw particles
    particleManager.render(renderer);

    SDL_RenderPresent(renderer);
}
