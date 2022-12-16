#pragma once
#include "Game.h"
#include "ParticleManager.h"
#include "Commands.h"
#include <cstddef>
#include <vector>

class GameSPH : public Game
{
public:
    GameSPH();
    ~GameSPH();
    virtual void handleInput(const SDL_Event& event) override;
    virtual void update(unsigned long dt) override;
    virtual void render(SDL_Renderer* renderer) override;
private:
    void printHistory() const;
    void addCommand(ICommand*);
    void clearHistory();
    void undo();
    void redo();

    bool pause;
    unsigned long presets[9] = {1, 200, 400, 700, 900, 1500, 2000, 3000, 5000};
    ParticleManager particleManager;
    CommandManager commandManager;
    std::vector<ICommand*> history;
    size_t nextHistoryIdx;
};