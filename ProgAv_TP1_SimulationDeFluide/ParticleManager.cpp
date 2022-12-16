#include <iostream>
#include <vector>
#include <string>
#include <cassert>
#include "\10-ProjetVolumineux\ProgAvance\SDL2-devel-2.0.16-VC\SDL2-2.0.16\include\SDL.h"
#include "Constants.h"
#include "ParticleManager.h"
using namespace std;

Particle::Particle(double _x, double _y) :
    x(_x), y(_y),
    vx(0.f), vy(0.f),
    fx(0.f), fy(0.f),
    rho(0.f), p(0.f)
{
}

ParticleManager::ParticleManager()
{
    ax = 0;
    ay = GRAVITY;

    renderMode = RenderMode::PARTICLE;
    grid.resize(NB_CELLS);
}

void ParticleManager::init(unsigned long n)
{
    cout << "Init with " << n << " particles" << endl;

    particles.clear();
    particles.reserve(n);

    while (particles.size() < n) {

        double x = rand() / (double)(RAND_MAX)*SCREEN_WIDTH;
        double y = rand() / (double)(RAND_MAX)*SCREEN_HEIGHT;

        double centerDist = sqrt(
            pow(x - SCREEN_WIDTH / 2.0, 2) +
            pow(y - SCREEN_HEIGHT / 2.0, 2));

        if (centerDist < fmin(SCREEN_WIDTH, SCREEN_HEIGHT) * 0.25)
            particles.push_back(Particle(x, y));
    }
}

/**
 * Ajoute un bloc de 5*5 particules autour du centre donné en
 * paramètre.  Dans le cas où le centre est proche des bordures de la
 * fenêtre, on ajoute seulement les particules qui sont dans la
 * fenêtre.
 *
 * @return le nombre de particules ajouté
 */
int ParticleManager::addBlock(double center_x, double center_y)
{
    int nbParticlesAdded = 0;

    for (int i = 0; i <= 4; i++) {
        for (int j = 0; j <= 4; j++) {
            double x = center_x + (j - 2) * SCREEN_WIDTH * 0.04f + (rand() / (double)(RAND_MAX)) * H;
            double y = center_y + (i - 2) * SCREEN_HEIGHT * 0.04f + (rand() / (double)(RAND_MAX)) * H;

            if (x >= 0 && x < SCREEN_WIDTH && y >= 0 && y < SCREEN_HEIGHT)
            {
                particles.push_back(Particle(x, y));
                nbParticlesAdded++;
            }

        }
    }

    cout << particles.size() << " particles" << endl;

    return nbParticlesAdded;
}

void ParticleManager::removeParticles(int nb)
{
    if (nb > particles.size())
        nb = particles.size();

    particles.erase(particles.end() - nb, particles.end());
}

void ParticleManager::addOne(double x, double y)
{
    particles.push_back(Particle(x, y));
    cout << particles.size() << " particles" << endl;
}

void ParticleManager::setGravity(int direction)
{
    switch (direction) {
    case DOWN:
        ax = 0;
        ay = +GRAVITY;
        break;
    case UP:
        ax = 0;
        ay = -GRAVITY;
        break;
    case RIGHT:
        ax = +GRAVITY;
        ay = 0;
        break;
    default:
        ax = -GRAVITY;
        ay = 0;
    }
}

void ParticleManager::explode() {
    for (auto& p : particles) {
        p.vx = rand() / (double)RAND_MAX * 10000 - 5000;
        p.vy = rand() / (double)RAND_MAX * 10000 - 5000;
    }
}

void ParticleManager::integrate(double dt)
{
    for (auto& p : particles)
    {
        // forward Euler integration
        if (p.rho != 0 && p.fx == p.fx && p.fy == p.fy) {
            p.vx += dt * p.fx / p.rho;
            p.vy += dt * p.fy / p.rho;
        }

        p.x += dt * p.vx;
        p.y += dt * p.vy;

        // enforce boundary conditions
        if (p.x - PARTICLE_RADIUS < 0.0f)
        {
            p.vx *= BOUND_DAMPING;
            p.x = PARTICLE_RADIUS;
        }
        if (p.x + PARTICLE_RADIUS > SCREEN_WIDTH)
        {
            p.vx *= BOUND_DAMPING;
            p.x = SCREEN_WIDTH - PARTICLE_RADIUS;
        }
        if (p.y - PARTICLE_RADIUS < 0.0f)
        {
            p.vy *= BOUND_DAMPING;
            p.y = PARTICLE_RADIUS;
        }
        if (p.y + PARTICLE_RADIUS > SCREEN_HEIGHT)
        {
            p.vy *= BOUND_DAMPING;
            p.y = SCREEN_HEIGHT - PARTICLE_RADIUS;
        }
    }
}

void ParticleManager::computeDensityPressure()
{
    // Pour chaque particule
    for (auto& pi : particles)
    {
        pi.rho = 0.f;

        // Chercher toutes les particules qui contribuent à la
        // pression/densité
        long base_idx = idx(pi);

        for (int y = -1; y <= +1; y++) {
            for (int x = -1; x <= +1; x++) {
                long idx_j = base_idx + x + y * GRID_W;

                if (idx_j < 0 || idx_j >= NB_CELLS) continue;

                for (auto* pj : grid[idx_j]) {
                    float distance_squared =
                        (pj->x - pi.x) * (pj->x - pi.x) +
                        (pj->y - pi.y) * (pj->y - pi.y);

                    if (distance_squared < H * H)
                    {
                        // this computation is symmetric
                        float foo = H * H - distance_squared;
                        pi.rho += MASS * POLY6 * foo * foo * foo;
                    }
                }
            }
        }

        pi.p = GAS_CONST * (pi.rho - REST_DENS);
    }
}

void ParticleManager::computeForces()
{
    // Pour chaque particule
    for (auto& pi : particles) {
        float pressure_x = 0.f;
        float pressure_y = 0.f;

        float viscosity_x = 0.f;
        float viscosity_y = 0.f;

        // Calculer la somme des forces de viscosité et pression
        // appliquées par les autres particules
        long base_idx = idx(pi);

        for (int y = -1; y <= +1; y++) {
            for (int x = -1; x <= +1; x++) {
                long idx_j = base_idx + x + y * GRID_W;

                if (idx_j < 0 || idx_j >= NB_CELLS) continue;

                for (auto* pj : grid[idx_j]) {

                    if (&pi == pj)
                        continue;

                    float r = sqrt(
                        (pj->x - pi.x) * (pj->x - pi.x) +
                        (pj->y - pi.y) * (pj->y - pi.y));

                    if (r < H) {
                        // compute pressure force contribution
                        float fpress = MASS * (pi.p + pj->p) / (2.0 * pj->rho) * SPIKY_GRAD * (H - r) * (H - r);
                        pressure_x += (pi.x - pj->x) / r * fpress;
                        pressure_y += (pi.y - pj->y) / r * fpress;

                        // compute viscosity force contribution
                        viscosity_x += VISC * MASS * (pj->vx - pi.vx) / pj->rho * VISC_LAP * (H - r);
                        viscosity_y += VISC * MASS * (pj->vy - pi.vy) / pj->rho * VISC_LAP * (H - r);
                    }
                }
            }
        }

        pi.fx = pressure_x + viscosity_x + ax * pi.rho;
        pi.fy = pressure_y + viscosity_y + ay * pi.rho;
    }
}

long ParticleManager::idx(Particle& p) {
    long x = (long)p.x / CELL_SIZE;
    long y = (long)p.y / CELL_SIZE;

    long idx = y * GRID_W + x;

    assert(idx >= 0 && idx < NB_CELLS);

    return idx;
}

void ParticleManager::update(unsigned long dt)
{
    for (int i = 0; i < NB_CELLS; i++) {
        grid[i].clear();
    }

    for (auto& p : particles) {
        grid[idx(p)].push_back(&p);
    }

    computeDensityPressure();
    computeForces();
    integrate(dt / 10000.0f);
}

void ParticleManager::setColor(unsigned char colorR, unsigned char colorG, unsigned char colorB)
{
    this->colorR = colorR;
    this->colorG = colorG;
    this->colorB = colorB;
}

unsigned char ParticleManager::getR() const
{
    return colorR;
}

unsigned char ParticleManager::getG() const
{
    return colorG;
}

unsigned char ParticleManager::getB() const
{
    return colorB;
}

void ParticleManager::renderParticles(SDL_Renderer* renderer)
{
    SDL_SetRenderDrawColor(renderer, colorR, colorG, colorB, 100);
    SDL_Rect r;

    // Draw particles
    for (long unsigned int i = 0; i < particles.size(); i++) {
        r.x = (int)(particles[i].x - PARTICLE_RADIUS);
        r.y = (int)(particles[i].y - PARTICLE_RADIUS);
        r.w = (int)(PARTICLE_RADIUS * 2);
        r.h = (int)(PARTICLE_RADIUS * 2);
        SDL_RenderFillRect(renderer, &r);
    }
}

void ParticleManager::renderGrid(SDL_Renderer* renderer)
{
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 100);

    for (int i = 0; i < GRID_W; i++) {
        SDL_RenderDrawLine(renderer, i * CELL_SIZE, 0, i * CELL_SIZE, SCREEN_HEIGHT);
    }

    for (int i = 0; i < GRID_H; i++) {
        SDL_RenderDrawLine(renderer, 0, i * CELL_SIZE, SCREEN_WIDTH, i * CELL_SIZE);
    }
}

void ParticleManager::renderCells(SDL_Renderer* renderer)
{

    SDL_Rect r;
    r.w = r.h = CELL_SIZE;

    for (int i = 0; i < NB_CELLS; i++) {
        int nb_particles = grid[i].size();
        int alpha = nb_particles * 255 / 5;
        if (alpha > 255) alpha = 255;
        SDL_SetRenderDrawColor(renderer, 0, 0, 255, alpha);

        int x = i % GRID_W;
        int y = i / GRID_W;

        r.x = x * CELL_SIZE;
        r.y = y * CELL_SIZE;

        SDL_RenderFillRect(renderer, &r);
    }
}

void ParticleManager::render(SDL_Renderer* renderer)
{
    if (renderMode == RenderMode::PARTICLE || renderMode == RenderMode::PARTICLE_GRID) {
        renderParticles(renderer);
    }

    if (renderMode == RenderMode::GRID || renderMode == RenderMode::PARTICLE_GRID) {
        renderCells(renderer);
        renderGrid(renderer);
    }
}
