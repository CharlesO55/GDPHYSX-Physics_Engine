#include "particle_collisions.h"
#include <iostream>

/*COLLISIONS
* Contains ParticleContact to resolve collisions between particles.
*   resolve() - Main logic. Will call collision check and resolution
*       checkCollision()  - Collision checker
*       resolveVelocity() - Alters velocity as two particles collide/bounce
*       resolveInterpenetration - Repositions the particles apart
*
* ParticleLinker to link particles and keep them distance apart. FillContact calcs said distance and passes it to collision resolution
*   Rods - Keeps particles at certain distance between another
*   Cables - Keeps particles to not exceed a certain distance between another
*/


//PARTICLE CONTACT
ParticleContact::ParticleContact() {
    part[0] = NULL;   part[1] = NULL;       //2 Particles
    k = 1.f;                                  //Adjust bounciness
    elasticity = 1.2f;
    contactNormal = { 0.f, 0.f, 0.f };      //Direction
    deltaTime = 0.f;
    penetrationDepth = 1.f;
    rbCube = NULL;
}


    //MAIN LOGIC FOR COLLISIONS
    void ParticleContact::resolve(Particle* part0, Particle* part1, RigidBody* rbCube) {
        this->deltaTime = deltaTime;    //Save the time
        this->part[0] = part0;
        this->part[1] = part1;
        this->rbCube = rbCube;


        if (checkCollision()) {     //If collision occurs
            std::cout << "COLLISION\n";
            //Store the direction
            if (part[1]) {
                contactNormal = glm::normalize(part[0]->partPos - part[1]->partPos);
            }
            resolveVelocity();
            resolveInterpenetration();
            //Despawn the projectile
            part[1]->partType = INACTIVE;
            part[1]->despawnParticle(1000);
        }
    }


        //CHECKS COLLISION AND DEPTH
        int ParticleContact::checkCollision() {
            float posDistance = glm::length(rbCube->partPos - part[1]->partPos);
            float radSum = rbCube->fLength + part[1]->radius * 3;
            penetrationDepth = posDistance - radSum;

            /*if (part[1]) {
                float posDistance = glm::length(part[0]->partPos - part[1]->partPos);
                float radSum = part[0]->radius + part[1]->radius;
                penetrationDepth = posDistance - radSum;    //Actual distance between particles - Min distance between circles
            }*/
            if (penetrationDepth <= 0) {
                return 1;
            }
            return 0;
        }

        //CALCULATE THE VELOCITY AFTER COLLIDING
        void ParticleContact::resolveVelocity() {
            //Check if they're already separating

            float sepVel = glm::dot((rbCube->partVel/*part[0]->partVel*/ - part[1]->partVel), contactNormal) - rbCube->fLength * 2;
            if (sepVel > 0) {
                return;
            }

            //Start finding new velocities
            float impulse = (1.f + k) * -sepVel;
            impulse /= (1 / (part[0]->mass * 8)) + (1 / part[1]->mass);   //Make it proportional to their masses

            //Directly edit vel
            //part[0]->partVel += contactNormal * impulse / part[0]->mass;
            rbCube->partVel -= contactNormal * impulse / (part[0]->mass * 8);
            rbCube->angularVel -= impulse / part[0]->mass;
            part[1]->partVel -= contactNormal * impulse * elasticity / part[1]->mass;   //Need additional amplifier since adding -vel to curr will just negate bullet's vel
        }


        //RESOLVE INTERPENETRATIONS POSITION
        void ParticleContact::resolveInterpenetration() {
            //Check if there's movable mass
            float totalInverseMass = 1 / part[0]->mass;
            if (part[1]) {
                totalInverseMass += 1 / part[1]->mass;
            }
            if (totalInverseMass <= 0) {
                return;
            }
            std::cout << "PEN\n";
            //Find the proportional amount of movement to mass
            glm::vec3 movePerMass = (-penetrationDepth / totalInverseMass) * contactNormal;    //Magnitude to move by * Direction

            //Reposition the particles
            part[0]->partPos += movePerMass * part[0]->mass;        //Move according to normal
            if (part[1]) {
                part[1]->partPos += -movePerMass * part[1]->mass;   //Move in opposite pos
            }
        }



//PARENT CLASS FOR RODS AND CABLES
ParticleLinker::ParticleLinker() {
    part[0] = NULL;     part[1] = NULL;
    lengthLimit = 0.f;
    currLength = 0.f;
    contactNormal = { 0.f,0.f,0.f };
}
    unsigned ParticleLinker::fillContact(ParticleContact* contact) {
        std::cout << "NO CONTACT PARTICLE LINKER\n";
        return 0;
    }

/*RODS CONSTRUCTOR
Rod::Rod() {
    part[0] = NULL;
}
Rod::Rod(Particle* part0, Particle* part1) {
    part[0] = part0;
    part[1] = part1;
    this->lengthLimit = glm::length(part[0]->partPos - part[1]->partPos);       //Remember the distance between each part
    this->contactNormal = glm::normalize(part[0]->partPos - part[1]->partPos);  //Remember the between angle each part
    currLength = 0.f;
}*/
    //CALCULATE AND FILL THE CONTACT WITH NEEDED VALUES
    /*unsigned Rod::fillContact(ParticleContact* contact) {
        currLength = glm::length(part[0]->partPos - part[1]->partPos);
        if (currLength == lengthLimit) {
            return 0;
        }

        //Just directly change the position since we know the angle and length they were init in
        part[1]->partPos = part[0]->partPos + (lengthLimit * -contactNormal);


        contact->part[0] = part[0];
        contact->part[1] = part[1];
        contact->k = 0.f;*/

        /*
        //contactNormal = glm::normalize(part[0]->partPos - part[1]->partPos);    //If need current angle
        if (currLength > lengthLimit) {
            contact->contactNormal = -contactNormal;
            contact->penetrationDepth = (currLength - lengthLimit);
        }

        else if (currLength < lengthLimit) {
            contact->contactNormal = contactNormal;
            contact->penetrationDepth = (currLength - lengthLimit);
        }*/
        //  return 1;
      //}
