#include "Commands.h"
#include "ParticleManager.h"


ICommand::ICommand(ParticleManager& particleManager, char representation)
    : particleManager{ particleManager }
    , representation{ representation }
{}


char ICommand::getRepresentation() const
{
    return representation;
}

CommandAddOne::CommandAddOne(ParticleManager& particleManager, double x, double y)
    : ICommand{ particleManager, '1' }
    , x{ x }, y{ y }
{}

void CommandAddOne::execute()
{
    particleManager.addOne(x, y);
}

void CommandAddOne::undo()
{
    particleManager.removeParticles(1);
}



CommandAddBlock::CommandAddBlock(ParticleManager& particleManager, double x, double y)
    : ICommand{ particleManager, 'N' }
    , x{ x }, y{ y }
    , nbParticles{ 0 }
{}

void CommandAddBlock::execute()
{
    nbParticles = particleManager.addBlock(x, y);
}

void CommandAddBlock::undo()
{
    particleManager.removeParticles(nbParticles);
}



CommandChangeColor::CommandChangeColor(ParticleManager& particleManager)
    : ICommand{ particleManager, 'C' }
{
    colorR = rand() % 255;
    colorG = rand() % 255;
    colorB = rand() % 255;

    prevColorR = particleManager.getR();
    prevColorG = particleManager.getG();
    prevColorB = particleManager.getB();
}

void CommandChangeColor::execute()
{
    particleManager.setColor(colorR, colorG, colorB);
}

void CommandChangeColor::undo()
{
    particleManager.setColor(prevColorR, prevColorG, prevColorB);
}
