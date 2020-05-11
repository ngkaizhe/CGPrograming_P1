#pragma once
#include"Common.h"
#include<vector>
#include <stdlib.h>     /* srand, rand */
#include"Shader.h"
#include"Model.h"
using namespace std;
using namespace glm;

class ParticleManager
{
public:
    static ParticleManager* getParticleManager();

    // get model from sphere obj
    void init();

    // draw the instance
    void Draw(Shader shader);

    // update all instance position
    void Update();

    // start shoot
    void startShoot(vec3 startPosition);
    void stopShoot();

    // remember to clear these var when switch to other state
    int totalInstance;
    vector<vec3> positions;

private:
    // private constuctor
    ParticleManager();
    // create singleton
    static ParticleManager* instance;

    // dir
    vector<vec3> directions;
    // start posiition
    vec3 startPosition;
    // check shot mode
    // if not shoot mode, discard all instances
    bool isShoot;
    // default speed for z axis
    float zSpeed;
    // model
    Model particleModel;

    // discard update
    void DiscardUpdate();
    // movement update
    void MovementUpdate();
    // create update
    void CreateUpdate();
};

