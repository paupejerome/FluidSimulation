#pragma once
#include "ParticleManager.h"
#include <cstddef> // size_t

class ICommand
{
public:
    ICommand(ParticleManager& particleManager, char representation);
    virtual ~ICommand() = default;

    virtual void execute() = 0;
    virtual void undo() = 0;

    char getRepresentation() const;
protected:
    ParticleManager& particleManager;
    const char representation;
};

class CommandAddOne : public ICommand
{
public:
    CommandAddOne(ParticleManager& particleManager, double x, double y);

    virtual void execute() override;
    virtual void undo() override;
private:
    double x, y;
};

class CommandAddBlock : public ICommand
{
public:
    CommandAddBlock(ParticleManager& particleManager, double x, double y);

    virtual void execute() override;
    virtual void undo() override;
private:
    double x, y;
    int nbParticles;
};

class CommandChangeColor : public ICommand
{
public:
    CommandChangeColor(ParticleManager& particleManager);

    virtual void execute() override;
    virtual void undo() override;
private:
    unsigned char colorR, colorG, colorB;
    unsigned char prevColorR, prevColorG, prevColorB;
};
