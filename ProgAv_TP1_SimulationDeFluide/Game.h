#pragma once
class Game
{
public:
    static const unsigned int MS_PER_STEP = 30;

    Game();
    void loop(SDL_Renderer* renderer);

    virtual void handleInput(const SDL_Event& event) = 0;
    virtual void update(unsigned long dt) = 0;
    virtual void render(SDL_Renderer* renderer) = 0;

protected:
    bool keepPlaying;
};