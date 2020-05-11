#include "ParticleManager.h"

ParticleManager* ParticleManager::instance = new ParticleManager();

ParticleManager::ParticleManager() {
    totalInstance = 0;
    positions = vector<vec3>(0);
    directions = vector<vec3>(0);
    zSpeed = 0.1f;
    startPosition = vec3(0.0);
}

ParticleManager* ParticleManager::getParticleManager() {
    if (!instance) instance = new ParticleManager();
    return instance;
}

// get model from sphere obj
void ParticleManager::init() {
    // load obj
    particleModel = Model("../Assets/objects/sphere/sphere.obj");
}

// draw the instance
void ParticleManager::Draw(Shader shader) {
    if(isShoot)
        particleModel.Draw(shader, true);
}

// update all instance position
void ParticleManager::Update() {
    CreateUpdate();
    MovementUpdate();
    DiscardUpdate();
}

// start shoot
void ParticleManager::startShoot(vec3 startPosition) {
    this->startPosition = startPosition;
    isShoot = true;
}

// stop shoot
void ParticleManager::stopShoot() {
    positions.clear();
    directions.clear();
    isShoot = false;

}

// discard update
void ParticleManager::DiscardUpdate() {
    // if the obj run out to certain position
    for (int i = this->positions.size() - 1; i >= 0; i--) {
        if (positions[i].z > 20.0f) {
            positions.erase(positions.begin() + i);
            directions.erase(directions.begin() + i);
        }
    }
}

// movement update
void ParticleManager::MovementUpdate() {
    // update movement
    for (int i = 0; i < this->positions.size(); i++) {
        // x axis
        positions[i].x += directions[i].x;
        // y axis
        positions[i].y += directions[i].y;
        // z axis
        positions[i].z += zSpeed;
    }
}

// create update
void ParticleManager::CreateUpdate() {
    // create a instance every time it get called
    // if it is in the shoot state
    if (isShoot) {
        int total = 10;
        for (int i = 0; i < total; i++) {
            positions.push_back(vec3(startPosition));
            float dx = (rand() % 1000) / 100000.0;
            float dy = (rand() % 1000) / 100000.0;
            // get random
            directions.push_back(vec3(dx, dy, 0));
        }
    }
}