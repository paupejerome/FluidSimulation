#pragma once
#include <vector>
#include <string>
#include "\10-ProjetVolumineux\ProgAvance\SDL2-devel-2.0.16-VC\SDL2-2.0.16\include\SDL.h"
#include "Constants.h"
/**
 * Smoothed-particle hydrodynamics simulation (SPH), basé sur
 * l'article de Matthias Müller
 * https://matthias-research.github.io/pages/publications/sca03.pdf
 *
 * Le code est dérivé de l'implantation sous licence MIT par Lucas
 * V. Schuermann :
 * https://github.com/cerrno/mueller-sph/tree/d24d025ce496db89de62ad4359bf89b175c712ed
 *
 * Writeup :
 * https://lucasschuermann.com/writing/implementing-sph-in-2d
 *
 */

enum class RenderMode {
    PARTICLE,
    GRID,
    PARTICLE_GRID
};

// Particle data structure
class Particle {
public:
    Particle(double, double);
    double x, y;   // Position
    double vx, vy; // Velocity
    double fx, fy; // Total forces
    double rho;    // Density
    double p;      // Pressure
};

class ParticleManager {
public:
    const double REST_DENS = 200.0; // rest density
    const double GAS_CONST = 200.0; // const for equation of state
    const double H = 16.0; // kernel radius
    // const double HSQ = pow(H, 2.0); // radius^2 for optimization
    const double MASS = 65.0; // assume all particles have the same mass
    const double VISC = 25.0; // viscosity constant
    const double GRAVITY = 20000;

    // size of a particle
    const double PARTICLE_RADIUS = H / 4.0;

    // smoothing kernels defined in Müller and their gradients
    const double POLY6 = 315.0 / (65.0 * M_PI * pow(H, 9.0));
    const double SPIKY_GRAD = -45.0 / (M_PI * pow(H, 6.0));
    const double VISC_LAP = 45.0 / (M_PI * pow(H, 6.0));

    // simulation parameters
    const double BOUND_DAMPING = -0.9;

    ParticleManager();

    void init(unsigned long);

    void addOne(double, double);
    int addBlock(double, double);
    void removeParticles(int nb);

    void setGravity(int);
    void explode();

    void setColor(unsigned char colorR, unsigned char colorG, unsigned char colorB);
    unsigned char getR() const;
    unsigned char getG() const;
    unsigned char getB() const;

    void update(unsigned long);
    void render(SDL_Renderer*);

    RenderMode renderMode;
private:
    double ax, ay; // Gravity
    void integrate(double dt);

    void computeDensityPressure();
    void computeForces();
    std::vector<Particle> particles;

    void renderParticles(SDL_Renderer*);

    void renderGrid(SDL_Renderer*);
    void renderCells(SDL_Renderer*);

    // TODO: utiliser une grille pour accélérer la simulation
    std::vector<std::vector<Particle*>> grid;


    unsigned char colorR = 230, colorG = 120, colorB = 0;

    long idx(Particle& p);

    const int CELL_SIZE = H;
    const int GRID_W = ceil(SCREEN_WIDTH / CELL_SIZE);
    const int GRID_H = ceil(SCREEN_HEIGHT / CELL_SIZE);
    const int NB_CELLS = GRID_W * GRID_H;
};
