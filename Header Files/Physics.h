#pragma once

#ifndef PHYSICS_H
#define PHYSICS_H

#include "BlockSearch.h"

using namespace std;

extern vector<vector<Chunk>> Chunks;
extern BlocksMap blocksMap;
extern vector<vector<Block>> Blocks;
extern std::vector<std::string> currentTextures;


extern float deltaTime;
extern float gravity;

extern Block helpBlock;

extern vector<Audio> Audios;
extern int currentAudioId;

extern void PlayAudio(std::string filePath, bool wholePath, int vol);

class Entity
{
public:

    vec3 position = vec3(0, 0, 0);

    vector<vec3> Collider;

    vector<vec3> PhysicsCorrections;

    bool walking = false;
	bool running = false;

    float footStepsTimer = 0.2f;
	float footStepsWalkFrec = 0.55f;
	float footStepsRunFrec = 0.3f;
    int footStepsVolume = 20;

    bool falling = false;
    bool wasFalling = false;

    float ySpeed = 0.0f;
    const float ySpeedMultiplier = 1.2f;
    float jumpImpulse = 14.3f;
    float fallSpeed = 15.0f;
    float fallTime = 0.4f;

    Entity(vec3 position, vector<vec3> Collider) : position(position), Collider(Collider) {};

    vec3 RelativeColliderCenter()
    {
        vec3 vec = vec3
        (
            (Collider[0].x + Collider[1].x) / 2,
            (Collider[0].y + Collider[1].y) / 2,
            (Collider[0].z + Collider[1].z) / 2
        );

        return vec;
    }

    vec3 ColliderRadius()
    {
        vec3 vec = vec3
        (
            (Collider[0].x + Collider[1].x) / 2,
            (Collider[0].y + Collider[1].y) / 2,
            (Collider[0].z + Collider[1].z) / 2
        );

        return vec;
    }

    vec3 ColliderCenter()
    {
        vec3 vec = vec3
        (
            position.x + Collider[0].x - ColliderRadius().x,
            position.y + Collider[0].y - ColliderRadius().y,
            position.z + Collider[0].z - ColliderRadius().z
        );

        return vec;
    }

    void CorrectCollision()
    {
        if (PhysicsCorrections.size() > 0)
        {
            float maxX = PhysicsCorrections[0].x;
            float maxY = PhysicsCorrections[0].y;
            float maxZ = PhysicsCorrections[0].z;

            for (int i = 0; i < PhysicsCorrections.size(); i++)
            {
                if (PhysicsCorrections[i].x > maxX)
                {
                    maxX = PhysicsCorrections[i].x;
                }
                if (PhysicsCorrections[i].y > maxY)
                {
                    maxY = PhysicsCorrections[i].y;
                }
                if (PhysicsCorrections[i].z > maxZ)
                {
                    maxZ = PhysicsCorrections[i].z;
                }
            }

            float minX = PhysicsCorrections[0].x;
            float minY = PhysicsCorrections[0].y;
            float minZ = PhysicsCorrections[0].z;

            for (int i = 0; i < PhysicsCorrections.size(); i++)
            {
                if (PhysicsCorrections[i].x < minX)
                {
                    minX = PhysicsCorrections[i].x;
                }
                if (PhysicsCorrections[i].y < minY)
                {
                    minY = PhysicsCorrections[i].y;
                }
                if (PhysicsCorrections[i].z < minZ)
                {
                    minZ = PhysicsCorrections[i].z;
                }
            }

            vec3 maxCorrection = vec3(maxX, maxY, maxZ);
            vec3 minCorrection = vec3(minX, minY, minZ);

            if (minX == maxX)
            {
                minCorrection.x = 0;
            }
            if (minY == maxY)
            {
                minCorrection.y = 0;
            }
            if (minZ == maxZ)
            {
                minCorrection.z = 0;
            }

            //position += maxCorrection;
            //position += minCorrection;


            PhysicsCorrections.clear();
        } 
    }

    void UpdateGravity()
    {
        falling = true;

        if (falling)
        {
            fallTime += deltaTime / 2;

            position.y += ySpeed * deltaTime * ySpeedMultiplier;

            ySpeed -= fallSpeed * deltaTime * ySpeedMultiplier;

            if (ySpeed < 0)
            {
                ySpeed = 0;
            }

            if (fallTime > 1)
            {
                falling = 1;
            }

            position.y -= fallSpeed * (fallTime < 2 ? fallTime : 2) * deltaTime * ySpeedMultiplier;
        }
        else
        {
            fallTime = 0.4f;
        }
    }

    void Land()
    {
        if (falling)
        {
            falling = false;
            ySpeed = 0;
            fallTime = 0.4f;
        }
    }

    void PlayLandSound()
    {
        int rnd = rand() % (5 - 1) + 1;

        PlayAudio("step/grass" + to_string(rnd), false, footStepsVolume);

        Audios[currentAudioId] = Audio("step/grass" + to_string(rnd), false, footStepsVolume);
        Audios[currentAudioId].Play();
    }

    void CeilingLand()
    {
        ySpeed = 0;
    }

    void UpdateFootSteps(vec3& oldPos)
    {
        if (walking && !falling && oldPos != position)
        {
            footStepsTimer += deltaTime;

            if (footStepsTimer >= (running ? footStepsRunFrec : footStepsWalkFrec))
            {
                int rnd = rand() % (5 - 1) + 1;

                PlayAudio("step/grass" + to_string(rnd), false, footStepsVolume);

                footStepsTimer = 0;
            }
        }
        else if (!walking && !running)
        {
            footStepsTimer = (running ? footStepsRunFrec : footStepsWalkFrec) - 0.1f;
        }
    }



    // Operador de asignación
    Entity& operator=(const Entity& other)
    {
        // Evitar la autoasignación
        if (this != &other)
        {
            this->position = other.position;
            this->Collider = other.Collider;
            // Copiar otros miembros si es necesario
        }
        return *this;
    }
};

static bool CheckBoxCollision(vec3& position, vec3& oldPosition, vec3 colliderCenter, vec3 colliderScale, vec3 cubeCenter, vec3 cubeScale, float collisionOffset)
{
    bool xCol = (abs(colliderCenter.x - cubeCenter.x) < colliderScale.x + cubeScale.x + collisionOffset);
    bool yCol = (abs(colliderCenter.y - cubeCenter.y) < colliderScale.y + cubeScale.y + collisionOffset);
    bool zCol = (abs(colliderCenter.z - cubeCenter.z) < colliderScale.z + cubeScale.z + collisionOffset);

    return (xCol && yCol && zCol);
}

static void CheckEntityCollision(Entity& entity, vec3& oldPosition, vec3 cubeCenter, vec3 cubeScale, float collisionOffset)
{
    vec3 originalPos = entity.position;

    if (CheckBoxCollision(entity.position, oldPosition, entity.ColliderCenter(), entity.ColliderRadius(), cubeCenter, cubeScale, collisionOffset))
    {
        entity.position = originalPos;
        entity.position.x = oldPosition.x;
    }
    if (CheckBoxCollision(entity.position, oldPosition, entity.ColliderCenter(), entity.ColliderRadius(), cubeCenter, cubeScale, collisionOffset))
    {
        entity.position = originalPos;
        entity.position.y = oldPosition.y;
    }

    if (CheckBoxCollision(entity.position, oldPosition, entity.ColliderCenter(), entity.ColliderRadius(), cubeCenter, cubeScale, collisionOffset))
    {
        entity.position = originalPos;
        entity.position.z = oldPosition.z;
    }

    if (CheckBoxCollision(entity.position, oldPosition, entity.ColliderCenter(), entity.ColliderRadius(), cubeCenter, cubeScale, collisionOffset))
    {
        entity.position = originalPos;
        entity.position.x = oldPosition.x;
        entity.position.y = oldPosition.y;
    }
    if (CheckBoxCollision(entity.position, oldPosition, entity.ColliderCenter(), entity.ColliderRadius(), cubeCenter, cubeScale, collisionOffset))
    {
        entity.position = originalPos;
        entity.position.x = oldPosition.x;
        entity.position.z = oldPosition.z;
    }
    if (CheckBoxCollision(entity.position, oldPosition, entity.ColliderCenter(), entity.ColliderRadius(), cubeCenter, cubeScale, collisionOffset))
    {
        entity.position = originalPos;
        entity.position.z = oldPosition.z;
        entity.position.y = oldPosition.y;
    }

    if (CheckBoxCollision(entity.position, oldPosition, entity.ColliderCenter(), entity.ColliderRadius(), cubeCenter, cubeScale, collisionOffset))
    {
        entity.position = originalPos;
        entity.position.x = oldPosition.x;
        entity.position.y = oldPosition.y;
        entity.position.z = oldPosition.z;
    }


    //entity.PhysicsCorrections.push_back(entity.position - originalPos);

    //entity.position = originalPos;
}

static void AdjustEntityCollision(Entity& entity, vec3& oldPosition, vec3& finalPosition, float collisionOffset = 0.0f)
{
    vec3 finalAdjustement;

    for (int axes = 0; axes < 3; axes++)
    {
        if (axes == 0)
        {
            entity.position.x = finalPosition.x;
            entity.position.y = oldPosition.y;
            entity.position.z = oldPosition.z;
        }
        else if (axes == 1)
        {
            entity.position.x = oldPosition.x;
            entity.position.y = finalPosition.y;
            entity.position.z = oldPosition.z;
        }
        else if (axes == 2)
        {
            entity.position.x = oldPosition.x;
            entity.position.y = oldPosition.y;
            entity.position.z = finalPosition.z;
        }

        for (int x = -ceil(entity.ColliderRadius().x) - 1; x <= ceil(entity.ColliderRadius().x) + 1; x++)
        {
            for (int y = -ceil(entity.ColliderRadius().y) - 1; y <= ceil(entity.ColliderRadius().y) + 1; y++)
            {
                for (int z = -ceil(entity.ColliderRadius().z) - 1; z <= ceil(entity.ColliderRadius().z) + 1; z++)
                {
                    if (helpBlock.checkCollidable(floor(entity.position.x + x), floor(entity.position.y + y), floor(entity.position.z + z), blocksMap, Blocks, Chunks))
                    {
                        Block checkBlock = helpBlock.GetBlock(floor(entity.position.x + x), floor(entity.position.y + y), floor(entity.position.z + z), blocksMap, Blocks, Chunks);

                        vec3 blockCenter = vec3
                        (
                            floor(entity.position.x + x) + 0.5f,
                            floor(entity.position.y + y) + 0.5f,
                            floor(entity.position.z + z) + 0.5f
                        );

                        vec3 blockScale = vec3
                        (
                            checkBlock.scale / 2.0f,
                            checkBlock.scale / 2.0f,
                            checkBlock.scale / 2.0f
                        );

                        CheckEntityCollision(entity, oldPosition, blockCenter, blockScale, collisionOffset);
                        //entity.CorrectCollision();

                        //checkBlocks.push_back(skybox.GetBlock(floor(entity.position.x + x), floor(entity.position.y + y), floor(entity.position.z + z), blocksMap, Blocks, Chunks));
                    }
                }
            }
        }

        if (axes == 0)
        {
            finalAdjustement.x = entity.position.x;
        }
        else if (axes == 1)
        {
            if (entity.position.y > finalPosition.y)
            {
                if (entity.falling)
                {
                    entity.Land();
                }

                entity.falling = false;
            }
            else if (entity.position.y < finalPosition.y)
            {
                if (entity.falling)
                {
                    entity.CeilingLand();
                }

                entity.falling = true;
            }
            else
            {
                entity.falling = true;
            }

            finalAdjustement.y = entity.position.y;
        }
        else if (axes == 2)
        {
            finalAdjustement.z = entity.position.z;
        }

        entity.position = finalPosition;
    }

    entity.position = finalAdjustement;



    if (entity.wasFalling && !entity.falling)
    {
        entity.PlayLandSound();
    }

    entity.wasFalling = entity.falling;
}

static Block RayCastBlock(vec3 origin, vec3 dir, float length = 100.0f)
{
    float precision = 0.5f;
    vec3 pos = origin;

    for (int i = 0; i < length / precision; i++)
    {
        if (isBlock(vec3(floor(pos.x), floor(pos.y), floor(pos.z))))
        {
            return helpBlock.GetBlock(floor(pos.x), floor(pos.y), floor(pos.z), blocksMap, Blocks, Chunks);
            break;
        }

        pos += normalize(dir) * precision;
    }
}

#endif //PHYSICS_H