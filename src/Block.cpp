#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <algorithm>
#include <fstream>
#include <string>
#include <sstream>

#include "Block.h"
#include "Utility.h"
#include "Texture.h"
#include "BlockSearch.h"

#include "vendor/stb_image/stb_image.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp> //value_ptr

using namespace std;


Block::Block(std::vector<std::string> &currentTextures, BlocksMap& blocksMap) : currentTextures(currentTextures), blocksMap(blocksMap)
{
   
}


Block::Block(int id, vec3 position, Color color, int scale, unsigned int shader, std::vector<std::string> &currentTextures, BlocksMap &blocksMap) :
    id(id), position(position), color(color), scale(scale), shader(shader), currentTextures(currentTextures), blocksMap(blocksMap),
    TexturePath(""), BottomTexturePath(""), TopTexturePath(""), FrontTexturePath(""), BackTexturePath(""), RightTexturePath(""), LeftTexturePath(""), IsSolidColor(false), initialized(true)
{
    #pragma region TIPOS DE BLOQUE

    if (!(id >= 14 && id <= 24))
    {
        switch (id)
        {
        case -1:
            IsSolidColor = true;
            TexturePath = "";
            break;
        case 0:
            TexturePath = "res/Textures/block/dirt.png";
            break;
        case 1:
            TexturePath = "res/Textures/block/stone.png";
            break;
        case 2:
            TexturePath = "res/Textures/block/grass_block_side.png";
            BottomTexturePath = "res/Textures/block/dirt.png";
            TopTexturePath = "res/Textures/block/grass_block_top_green.png";
            break;
        case 3:
            TexturePath = "res/Textures/block/sand.png";
            break;
        case 4:
            TexturePath = "res/Textures/block/gravel.png";
            break;
        case 5:
            TexturePath = "res/Textures/block/cobblestone.png";
            break;
        case 6:
            TexturePath = "res/Textures/block/bedrock.png";
            break;
        case 7:
            TexturePath = "res/Textures/block/oak_log.png";
            BottomTexturePath = "res/Textures/block/oak_log_top.png";
            TopTexturePath = "res/Textures/block/oak_log_top.png";
            break;
        case 8:
            TexturePath = "res/Textures/block/oak_leaves.png";
            //noTransparency = true;
            canOclude = false;
            break;
        case 9:
            TexturePath = "res/Textures/block/oak_planks.png";
            break;
        case 10:
            TexturePath = "res/Textures/block/diamond_ore.png";
            break;
        case 11:
            TexturePath = "res/Textures/block/obsidian.png";
            break;
        case 12:
            TexturePath = "res/Textures/block/glass.png";
            break;
        case 13:
            TexturePath = "res/Textures/block/grass.png";
            canOclude = false;
            twoQuads = true;
            collidable = false;
            break;
        case 25:
            TexturePath = "res/Textures/block/redstone_lamp_on.png";
            brightness = 16;
            brightDistance = 12;
            lightTint = vec4(2, 0.3, 0.3, 0);
            break;
        case 26:
            TexturePath = "res/Textures/block/glowstone.png";
            brightness = 16;
            brightDistance = 12;
            lightTint = vec4(1, 1, 0, 0);
            break;
        case 27:
            TexturePath = "res/Textures/block/sea_lantern_edit.png";
            brightness = 16;
            brightDistance = 12;
            lightTint = vec4(0, 0, 1, 0);
            break;

        default:
            TexturePath = "res/Textures/block/dirt.png";
            break;
        }
    }
    else
    {
        //Flowers
        switch (id)
        {
        case 14:
            TexturePath = "res/Textures/block/dandelion.png";
            canOclude = false;
            twoQuads = true;
            collidable = false;
            break;
        case 15:
            TexturePath = "res/Textures/block/poppy.png";
            canOclude = false;
            twoQuads = true;
            collidable = false;
            break;
        case 16:
            TexturePath = "res/Textures/block/blue_orchid.png";
            canOclude = false;
            twoQuads = true;
            collidable = false;
            break;
        case 17:
            TexturePath = "res/Textures/block/allium.png";
            canOclude = false;
            twoQuads = true;
            collidable = false;
            break;
        case 18:
            TexturePath = "res/Textures/block/azure_bluet.png";
            canOclude = false;
            twoQuads = true;
            collidable = false;
            break;
        case 19:
            TexturePath = "res/Textures/block/red_tulip.png";
            canOclude = false;
            twoQuads = true;
            collidable = false;
            break;
        case 20:
            TexturePath = "res/Textures/block/orange_tulip.png";
            canOclude = false;
            twoQuads = true;
            collidable = false;
            break;
        case 21:
            TexturePath = "res/Textures/block/white_tulip.png";
            canOclude = false;
            twoQuads = true;
            collidable = false;
            break;
        case 22:
            TexturePath = "res/Textures/block/pink_tulip.png";
            canOclude = false;
            twoQuads = true;
            collidable = false;
            break;
        case 23:
            TexturePath = "res/Textures/block/oxeye_daisy.png";
            canOclude = false;
            twoQuads = true;
            collidable = false;
            break;
        case 24:
            TexturePath = "res/Textures/block/cornflower.png";
            canOclude = false;
            twoQuads = true;
            collidable = false;
            break;
        default:
            TexturePath = "res/Textures/block/dandelion.png";
            canOclude = false;
            twoQuads = true;
            collidable = false;
            break;
        }
    }

    #pragma endregion
}

extern vector<vector<Chunk>> Chunks;
extern vector<vector<Block>> Blocks;
extern vec3 skyLight;
extern int ChunksRenderDistance;
extern int ChunksSize;
extern float dayTime;
extern float dayDuration;

float LightSmoothFunction(float x)
{
    return (-pow(x, 2) + 1);
}


void Block::Create()
{
    #pragma region VARIABLES TEXTURAS

    if (rewriteAllVariables)
    {
        texturePath = TexturePath;

        isSolidColor = IsSolidColor;



        bottomTexturePath = BottomTexturePath;
        topTexturePath = TopTexturePath;
        frontTexturePath = FrontTexturePath;
        backTexturePath = BackTexturePath;
        rightTexturePath = RightTexturePath;
        leftTexturePath = LeftTexturePath;
        /*hasFrontFace = isSolidColor || !isOcluded(vec3((int)position.x, (int)position.y, (int)position.z + 1), GetChunk());
        hasBackFace = isSolidColor || !isOcluded(vec3((int)position.x, (int)position.y, (int)position.z - 1), GetChunk());
        hasTopFace = isSolidColor || !isOcluded(vec3((int)position.x, (int)position.y + 1, (int)position.z), GetChunk());
        hasBottomFace = isSolidColor || !isOcluded(vec3((int)position.x, (int)position.y - 1, (int)position.z), GetChunk());
        hasRightFace = isSolidColor || !isOcluded(vec3((int)position.x + 1, (int)position.y, (int)position.z), GetChunk());
        hasLeftFace = isSolidColor || !isOcluded(vec3((int)position.x - 1, (int)position.y, (int)position.z), GetChunk());
        */
        
        hasFrontFace = isSolidColor || !checkOclude((int)position.x, (int)position.y, (int)position.z + 1, blocksMap, Blocks, Chunks);
        hasBackFace = isSolidColor || !checkOclude((int)position.x, (int)position.y, (int)position.z - 1, blocksMap, Blocks, Chunks);
        hasTopFace = isSolidColor || !checkOclude((int)position.x, (int)position.y + 1, (int)position.z, blocksMap, Blocks, Chunks);
        hasBottomFace = isSolidColor || !checkOclude((int)position.x, (int)position.y - 1, (int)position.z, blocksMap, Blocks, Chunks);
        hasRightFace = isSolidColor || !checkOclude((int)position.x + 1, (int)position.y, (int)position.z, blocksMap, Blocks, Chunks);
        hasLeftFace = isSolidColor || !checkOclude((int)position.x - 1, (int)position.y, (int)position.z, blocksMap, Blocks, Chunks);
        

        diffBottom = (bottomTexturePath != "" && hasBottomFace);
        diffTop = (topTexturePath != "" && hasTopFace);
        diffFront = (frontTexturePath != "" && hasFrontFace);
        diffBack = (backTexturePath != "" && hasBackFace);
        diffRight = (rightTexturePath != "" && hasRightFace);
        diffLeft = (leftTexturePath != "" && hasLeftFace);

        rewriteAllVariables = false;
    }
    

    #pragma endregion
    
    if (hasFrontFace || hasBackFace || hasTopFace || hasBottomFace || hasRightFace || hasLeftFace)
    {
        
        #pragma region DIRECT LIGHTING

        // Definición de variables
        glm::vec3 _position = vec3(position.x, position.y, position.z);

        // Construcción de la matriz de modelo
        glm::mat4 modelMatrix = glm::mat4(1.0f); // Inicializa la matriz de modelo como la matriz identidad

        // Aplica la traslación
        modelMatrix = glm::translate(modelMatrix, _position);

        // Aplica la rotación (en orden YXZ)
        modelMatrix = glm::rotate(modelMatrix, rotation.z, glm::vec3(0.0f, 0.0f, 1.0f)); // Rotación alrededor del eje Z
        modelMatrix = glm::rotate(modelMatrix, rotation.y, glm::vec3(0.0f, 1.0f, 0.0f)); // Rotación alrededor del eje Y
        modelMatrix = glm::rotate(modelMatrix, rotation.x, glm::vec3(1.0f, 0.0f, 0.0f)); // Rotación alrededor del eje X

        // Aplica la escala
        modelMatrix = glm::scale(modelMatrix, vec3(scale, scale, scale));

        glUniformMatrix4fv(glGetUniformLocation(shader, "u_modelMatrix"), 1, GL_FALSE, glm::value_ptr(modelMatrix));


        glUniform3fv(glGetUniformLocation(shader, "u_skyLight"), 1, glm::value_ptr(skyLight));

        #pragma endregion


        #pragma region BLOCKS LIGHTING

        if (brightness == 0)
        {
            vector<Chunk*> chunksAround = GetChunksAroundBlock(this); //TODO: ALMACENAR ESTO EN UNA VARIABLE DE BLOCK PARA NO RECALCULARLO CADA FRAME
            vector<vec4> lightTintColors;

            for (int i = 0; i < chunksAround.size(); i++)
            {
                for (int b = 0; b < chunksAround[i]->BrightBlocks.size(); b++)
                {
                    vec3 blockPos = vec3(chunksAround[i]->BrightBlocks[b][0], chunksAround[i]->BrightBlocks[b][1], chunksAround[i]->BrightBlocks[b][2]);
                    float dist = length(blockPos - position);
                    float light = chunksAround[i]->BrightBlocks[b][3];
                    float maxDist = chunksAround[i]->BrightBlocks[b][4];
                    float distanceController = 1.5f;

                    if (dist <= maxDist)
                    {
                        bool hasReceivedLight = false;

                        if (blockPos.x >= position.x)
                        {
                            if (positiveLighting.x < light * LightSmoothFunction(dist / maxDist))
                            {
                                positiveLighting.x = light * LightSmoothFunction(dist / maxDist);
                                hasReceivedLight = true;
                            }
                        }
                        else
                        {
                            if (negativeLighting.x < light * LightSmoothFunction(dist / maxDist))
                            {
                                negativeLighting.x = light * LightSmoothFunction(dist / maxDist);
                                hasReceivedLight = true;
                            }
                        }

                        if (blockPos.y >= position.y)
                        {
                            if (positiveLighting.y < light * LightSmoothFunction(dist / maxDist))
                            {
                                positiveLighting.y = light * LightSmoothFunction(dist / maxDist);
                                hasReceivedLight = true;
                            }
                        }
                        else
                        {
                            if (negativeLighting.y < light * LightSmoothFunction(dist / maxDist))
                            {
                                negativeLighting.y = light * LightSmoothFunction(dist / maxDist);
                                hasReceivedLight = true;
                            }
                        }

                        if (blockPos.z >= position.z)
                        {
                            if (positiveLighting.z < light * LightSmoothFunction(dist / maxDist))
                            {
                                positiveLighting.z = light * LightSmoothFunction(dist / maxDist);
                                hasReceivedLight = true;
                            }
                        }
                        else
                        {
                            if (negativeLighting.z < light * LightSmoothFunction(dist / light * distanceController))
                            {
                                negativeLighting.z = light * LightSmoothFunction(dist / light * distanceController);
                                hasReceivedLight = true;
                            }
                        }

                        if (hasReceivedLight)
                        {
                            //lightTintColors.push_back(vec4(chunksAround[i]->BrightBlocks[b][5], chunksAround[i]->BrightBlocks[b][6], chunksAround[i]->BrightBlocks[b][7], 0));
                        }
                    }
                }
            }

            glUniform3fv(glGetUniformLocation(shader, "u_positiveLighting"), 1, glm::value_ptr(positiveLighting));
            glUniform3fv(glGetUniformLocation(shader, "u_negativeLighting"), 1, glm::value_ptr(negativeLighting));

            vec4 finalTintColor = vec4(0.0f, 0.0f, 0.0f, 0.0f);

            if (lightTintColors.size() > 0 && false)
            {
                for (int c = 0; c < lightTintColors.size(); c++)
                {
                    finalTintColor.x += lightTintColors[c].x;
                    finalTintColor.x /= 2.0f;
                    finalTintColor.y += lightTintColors[c].y;
                    finalTintColor.y /= 2.0f;
                    finalTintColor.z += lightTintColors[c].z;
                    finalTintColor.z /= 2.0f;
                }
            }
            else
            {
                finalTintColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);
            }
            
            //EL FINALTINT NO ESTÁ MULTIPLICADO EN EL SHADER

            //glUniform4fv(glGetUniformLocation(shader, "u_lightTint"), 1, glm::value_ptr(finalTintColor));


            positiveLighting = vec3(0, 0, 0);
            negativeLighting = vec3(0, 0, 0);
        }


        glUniform1f(glGetUniformLocation(shader, "u_dayTime"), dayTime);
        glUniform1f(glGetUniformLocation(shader, "u_dayDuration"), dayDuration);
        glUniform1f(glGetUniformLocation(shader, "u_isEmissive"), (brightness > 0));

        #pragma endregion


        #pragma region BLOCKS SELECTION

        glUniform1i(glGetUniformLocation(shader, "u_isSelected"), isSelected);

        isSelected = false;

        #pragma endregion


        if (!twoQuads)
        {
            unsigned int cubeIndices[] =
            {
                // Cara frontal
                0, 1, 2,
                2, 3, 0,
                // Cara trasera
                4, 5, 6,
                6, 7, 4,
                // Cara superior
                8, 9, 10,
                10, 11, 8,
                // Cara inferior
                12, 13, 14,
                14, 15, 12,
                // Cara derecha
                16, 17, 18,
                18, 19, 16,
                // Cara izquierda
                20, 21, 22,
                22, 23, 20
            };

            if (regenerateBuffers)
            {

                #pragma region VERTICES E INDICES       

                if (regenerateVertices)
                {
                    int vertexIndex = 0;

                    if (hasFrontFace && !diffFront)
                    {
                        float frontFace[]
                        {
                            // Cara frontal
                            position.x, position.y, position.z + scale,    0.0f, 0.0f,
                            position.x + scale, position.y, position.z + scale,    1.0f, 0.0f,
                            position.x + scale, position.y + scale, position.z + scale,    1.0f, 1.0f,
                            position.x, position.y + scale, position.z + scale,    0.0f, 1.0f
                        };

                        std::copy(std::begin(frontFace), std::end(frontFace), std::begin(cubeVertices) + vertexIndex);
                        vertexIndex += sizeof(frontFace) / sizeof(frontFace[0]);
                    }
                    if (hasBackFace && !diffBack)
                    {
                        float backFace[]
                        {
                            // Cara trasera
                           position.x, position.y, position.z,    1.0f, 0.0f,
                           position.x, position.y + scale, position.z,    1.0f, 1.0f,
                           position.x + scale, position.y + scale, position.z,    0.0f, 1.0f,
                           position.x + scale, position.y, position.z,    0.0f, 0.0f
                        };

                        std::copy(std::begin(backFace), std::end(backFace), std::begin(cubeVertices) + vertexIndex);
                        vertexIndex += sizeof(backFace) / sizeof(backFace[0]);
                    }


                    if (hasTopFace && !diffTop)
                    {
                        float topFace[]
                        {
                            // Cara superior
                            position.x, position.y + scale, position.z + scale,    0.0f, 0.0f,
                            position.x + scale, position.y + scale, position.z + scale,    1.0f, 0.0f,
                            position.x + scale, position.y + scale, position.z,    1.0f, 1.0f,
                            position.x, position.y + scale, position.z,    0.0f, 1.0f
                        };

                        std::copy(std::begin(topFace), std::end(topFace), std::begin(cubeVertices) + vertexIndex);
                        vertexIndex += sizeof(topFace) / sizeof(topFace[0]);
                    }
                    if (hasBottomFace && !diffBottom)
                    {
                        float bottomFace[]
                        {
                            // Cara inferior
                            position.x, position.y, position.z,    0.0f, 0.0f,
                            position.x + scale, position.y, position.z,    1.0f, 0.0f,
                            position.x + scale, position.y, position.z + scale,    1.0f, 1.0f,
                            position.x, position.y, position.z + scale,    0.0f, 1.0f
                        };

                        std::copy(std::begin(bottomFace), std::end(bottomFace), std::begin(cubeVertices) + vertexIndex);
                        vertexIndex += sizeof(bottomFace) / sizeof(bottomFace[0]);
                    }


                    if (hasRightFace && !diffRight)
                    {
                        float rightFace[]
                        {
                            // Cara derecha
                            position.x + scale, position.y, position.z + scale,    0.0f, 0.0f,
                            position.x + scale, position.y, position.z,    1.0f, 0.0f,
                            position.x + scale, position.y + scale, position.z,    1.0f, 1.0f,
                            position.x + scale, position.y + scale, position.z + scale,    0.0f, 1.0f
                        };

                        std::copy(std::begin(rightFace), std::end(rightFace), std::begin(cubeVertices) + vertexIndex);
                        vertexIndex += sizeof(rightFace) / sizeof(rightFace[0]);
                    }
                    if (hasLeftFace && !diffLeft)
                    {
                        float leftFace[]
                        {
                            // Cara izquierda
                            position.x, position.y, position.z,    0.0f, 0.0f,
                            position.x, position.y, position.z + scale,    1.0f, 0.0f,
                            position.x, position.y + scale, position.z + scale,    1.0f, 1.0f,
                            position.x, position.y + scale, position.z,    0.0f, 1.0f
                        };

                        std::copy(std::begin(leftFace), std::end(leftFace), std::begin(cubeVertices) + vertexIndex);
                        vertexIndex += sizeof(leftFace) / sizeof(leftFace[0]);
                    }

                    regenerateVertices = false;
                }



                #pragma endregion

            }


            #pragma region TEXTURAS DE CARAS DISTINTAS

            if (diffTop)
            {
                // Eliminar los vértices de la cara superior del cubo
                // Los vértices de la cara superior son los índices 8, 9, 10, 11
                // Se establecen las coordenadas de los vértices en (0, 0, 0)
                for (int i = 8; i <= 11; ++i) {
                    //cubeVertices[i * 5] = cubeVertices[i * 5 + 1] = cubeVertices[i * 5 + 2] = 0.0f;
                }

                float topQuadVertices[20] =
                {
                    position.x, position.y + scale, position.z + scale,    0.0f, 1.0f,
                    position.x + scale, position.y + scale, position.z + scale,    1.0f, 1.0f,
                    position.x + scale, position.y + scale, position.z,    1.0f, 0.0f,
                    position.x, position.y + scale, position.z,    0.0f, 0.0f
                };

                // Crear y configurar el buffer de vértices del quad
                if (regenerateBuffers)
                {
                    glGenBuffers(1, &topQuadVBO);
                }
                glBindBuffer(GL_ARRAY_BUFFER, topQuadVBO);
                if (true)
                {
                    glBufferData(GL_ARRAY_BUFFER, sizeof(topQuadVertices), topQuadVertices, GL_STATIC_DRAW);
                }

                // Configurar los atributos de vértices para el shader
                glEnableVertexAttribArray(0);
                glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
                glEnableVertexAttribArray(1);
                glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

                // Determinar el slot de textura para el shader
                int quadSlot = 1;
                for (int i = 0; i < currentTextures.size(); i++) {
                    if (currentTextures[i] == topTexturePath) {
                        quadSlot = i;
                        break;
                    }
                }

                // Configurar la textura en el shader
                glUniform1i(glGetUniformLocation(shader, "u_Texture"), quadSlot);
                glUniform4f(glGetUniformLocation(shader, "u_Color"), color.r, color.g, color.b, color.a);

                // Renderizar el quad
                glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

                // Limpiar el buffer de vértices del quad
                //glDeleteBuffers(1, &topQuadVBO);
            }

            if (diffBottom)
            {
                // Eliminar los vértices de la cara inferior del cubo
                // Los vértices de la cara inferior son los índices 12, 13, 14, 15
                // Se establecen las coordenadas de los vértices en (0, 0, 0)
                for (int i = 12; i <= 15; ++i) {
                    //cubeVertices[i * 5] = cubeVertices[i * 5 + 1] = cubeVertices[i * 5 + 2] = 0.0f;
                }

                float bottomQuadVertices[20] =
                {
                    position.x, position.y, position.z,    0.0f, 0.0f,
                    position.x + scale, position.y, position.z,    1.0f, 0.0f,
                    position.x + scale, position.y, position.z + scale,    1.0f, 1.0f,
                    position.x, position.y, position.z + scale,    0.0f, 1.0f
                };

                // Crear y configurar el buffer de vértices del quad
                if (regenerateBuffers)
                {
                    glGenBuffers(1, &bottomQuadVBO);
                }
                glBindBuffer(GL_ARRAY_BUFFER, bottomQuadVBO);
                if (true)
                {
                    glBufferData(GL_ARRAY_BUFFER, sizeof(bottomQuadVertices), bottomQuadVertices, GL_STATIC_DRAW);
                }

                // Configurar los atributos de vértices para el shader
                glEnableVertexAttribArray(0);
                glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
                glEnableVertexAttribArray(1);
                glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

                // Determinar el slot de textura para el shader
                int quadSlot = 1;
                for (int i = 0; i < currentTextures.size(); i++) {
                    if (currentTextures[i] == bottomTexturePath) {
                        quadSlot = i;
                        break;
                    }
                }

                // Configurar la textura en el shader
                glUniform1i(glGetUniformLocation(shader, "u_Texture"), quadSlot);
                glUniform4f(glGetUniformLocation(shader, "u_Color"), color.r, color.g, color.b, color.a);

                // Renderizar el quad
                glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

                // Limpiar el buffer de vértices del quad
                //glDeleteBuffers(1, &bottomQuadVBO);
            }


            if (diffFront)
            {
                // Eliminar los vértices de la cara frontal del cubo
                // Los vértices de la cara frontal son los índices 0, 1, 2, 3
                // Se establecen las coordenadas de los vértices en (0, 0, 0)
                for (int i = 0; i <= 3; ++i) {
                    //cubeVertices[i * 5] = cubeVertices[i * 5 + 1] = cubeVertices[i * 5 + 2] = 0.0f;
                }

                float frontQuadVertices[20] =
                {
                    position.x, position.y, position.z + scale,    0.0f, 0.0f,
                    position.x + scale, position.y, position.z + scale,    1.0f, 0.0f,
                    position.x + scale, position.y + scale, position.z + scale,    1.0f, 1.0f,
                    position.x, position.y + scale, position.z + scale,    0.0f, 1.0f
                };

                // Crear y configurar el buffer de vértices del quad
                if (regenerateBuffers)
                {
                    glGenBuffers(1, &frontQuadVBO);
                }
                glBindBuffer(GL_ARRAY_BUFFER, frontQuadVBO);
                if (true)
                {
                    glBufferData(GL_ARRAY_BUFFER, sizeof(frontQuadVertices), frontQuadVertices, GL_STATIC_DRAW);
                }

                // Configurar los atributos de vértices para el shader
                glEnableVertexAttribArray(0);
                glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
                glEnableVertexAttribArray(1);
                glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

                // Determinar el slot de textura para el shader
                int quadSlot = 1;
                for (int i = 0; i < currentTextures.size(); i++) {
                    if (currentTextures[i] == frontTexturePath) {
                        quadSlot = i;
                        break;
                    }
                }

                // Configurar la textura en el shader
                glUniform1i(glGetUniformLocation(shader, "u_Texture"), quadSlot);
                glUniform4f(glGetUniformLocation(shader, "u_Color"), color.r, color.g, color.b, color.a);

                // Renderizar el quad
                glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

                // Limpiar el buffer de vértices del quad
                //glDeleteBuffers(1, &frontQuadVBO);
            }

            if (diffBack)
            {
                // Eliminar los vértices de la cara trasera del cubo
                // Los vértices de la cara trasera son los índices 4, 5, 6, 7
                // Se establecen las coordenadas de los vértices en (0, 0, 0)
                for (int i = 4; i <= 7; ++i) {
                    //cubeVertices[i * 5] = cubeVertices[i * 5 + 1] = cubeVertices[i * 5 + 2] = 0.0f;
                }

                float backQuadVertices[20] =
                {
                    position.x, position.y, position.z,    1.0f, 0.0f,
                    position.x, position.y + scale, position.z,    1.0f, 1.0f,
                    position.x + scale, position.y + scale, position.z,    0.0f, 1.0f,
                    position.x + scale, position.y, position.z,    0.0f, 0.0f
                };

                // Crear y configurar el buffer de vértices del quad
                if (regenerateBuffers)
                {
                    glGenBuffers(1, &backQuadVBO);

                }
                glBindBuffer(GL_ARRAY_BUFFER, backQuadVBO);
                if (true)
                {
                    glBufferData(GL_ARRAY_BUFFER, sizeof(backQuadVertices), backQuadVertices, GL_STATIC_DRAW);
                }

                // Configurar los atributos de vértices para el shader
                glEnableVertexAttribArray(0);
                glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
                glEnableVertexAttribArray(1);
                glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

                // Determinar el slot de textura para el shader
                int quadSlot = 1;
                for (int i = 0; i < currentTextures.size(); i++) {
                    if (currentTextures[i] == backTexturePath) {
                        quadSlot = i;
                        break;
                    }
                }

                // Configurar la textura en el shader
                glUniform1i(glGetUniformLocation(shader, "u_Texture"), quadSlot);
                glUniform4f(glGetUniformLocation(shader, "u_Color"), color.r, color.g, color.b, color.a);

                // Renderizar el quad
                glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

                // Limpiar el buffer de vértices del quad
                //glDeleteBuffers(1, &backQuadVBO);
            }


            if (diffRight)
            {
                // Eliminar los vértices de la cara derecha del cubo
                // Los vértices de la cara derecha son los índices 16, 17, 18, 19
                // Se establecen las coordenadas de los vértices en (0, 0, 0)
                for (int i = 16; i <= 19; ++i) {
                    //cubeVertices[i * 5] = cubeVertices[i * 5 + 1] = cubeVertices[i * 5 + 2] = 0.0f;
                }

                float rightQuadVertices[20] =
                {
                    position.x + scale, position.y, position.z + scale,    0.0f, 0.0f,
                    position.x + scale, position.y, position.z,    1.0f, 0.0f,
                    position.x + scale, position.y + scale, position.z,    1.0f, 1.0f,
                    position.x + scale, position.y + scale, position.z + scale,    0.0f, 1.0f
                };

                // Crear y configurar el buffer de vértices del quad
                if (regenerateBuffers)
                {
                    glGenBuffers(1, &rightQuadVBO);
                }
                glBindBuffer(GL_ARRAY_BUFFER, rightQuadVBO);
                if (true)
                {
                    glBufferData(GL_ARRAY_BUFFER, sizeof(rightQuadVertices), rightQuadVertices, GL_STATIC_DRAW);
                }

                // Configurar los atributos de vértices para el shader
                glEnableVertexAttribArray(0);
                glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
                glEnableVertexAttribArray(1);
                glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

                // Determinar el slot de textura para el shader
                int quadSlot = 1;
                for (int i = 0; i < currentTextures.size(); i++) {
                    if (currentTextures[i] == rightTexturePath) {
                        quadSlot = i;
                        break;
                    }
                }

                // Configurar la textura en el shader
                glUniform1i(glGetUniformLocation(shader, "u_Texture"), quadSlot);
                glUniform4f(glGetUniformLocation(shader, "u_Color"), color.r, color.g, color.b, color.a);

                // Renderizar el quad
                glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

                // Limpiar el buffer de vértices del quad
                //glDeleteBuffers(1, &rightQuadVBO);
            }

            if (diffLeft)
            {
                // Eliminar los vértices de la cara izquierda del cubo
                // Los vértices de la cara izquierda son los índices 20, 21, 22, 23
                // Se establecen las coordenadas de los vértices en (0, 0, 0)
                for (int i = 20; i <= 23; ++i) {
                    //cubeVertices[i * 5] = cubeVertices[i * 5 + 1] = cubeVertices[i * 5 + 2] = 0.0f;
                }

                float leftQuadVertices[20] =
                {
                    position.x, position.y, position.z,    0.0f, 0.0f,
                    position.x, position.y, position.z + scale,    1.0f, 0.0f,
                    position.x, position.y + scale, position.z + scale,    1.0f, 1.0f,
                    position.x, position.y + scale, position.z,    0.0f, 1.0f
                };

                // Crear y configurar el buffer de vértices del quad
                if (regenerateBuffers)
                {
                    glGenBuffers(1, &leftQuadVBO);
                }
                glBindBuffer(GL_ARRAY_BUFFER, leftQuadVBO);
                if (true)
                {
                    glBufferData(GL_ARRAY_BUFFER, sizeof(leftQuadVertices), leftQuadVertices, GL_STATIC_DRAW);
                }

                // Configurar los atributos de vértices para el shader
                glEnableVertexAttribArray(0);
                glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
                glEnableVertexAttribArray(1);
                glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

                // Determinar el slot de textura para el shader
                int quadSlot = 1;
                for (int i = 0; i < currentTextures.size(); i++) {
                    if (currentTextures[i] == leftTexturePath) {
                        quadSlot = i;
                        break;
                    }
                }

                // Configurar la textura en el shader
                glUniform1i(glGetUniformLocation(shader, "u_Texture"), quadSlot);
                glUniform4f(glGetUniformLocation(shader, "u_Color"), color.r, color.g, color.b, color.a);

                // Renderizar el quad
                glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

                // Limpiar el buffer de vértices del quad
                //glDeleteBuffers(1, &leftQuadVBO);
            }

            #pragma endregion
        }
        else
        {
            unsigned int quadIndices[] =
            {
                0, 1, 2,
                2, 3, 0,
            };

            if (regenerateBuffers)
            {
                // Define los vértices del primer quad
                float quad1Vertices[] =
                {
                    position.x, position.y, position.z + scale - 0.5f,    0.0f, 0.0f,
                    position.x + scale, position.y, position.z + scale - 0.5f,    1.0f, 0.0f,
                    position.x + scale, position.y + scale, position.z + scale - 0.5f,    1.0f, 1.0f,
                    position.x, position.y + scale, position.z + scale - 0.5f,    0.0f, 1.0f
                };

                // Define los vértices del segundo quad
                float quad2Vertices[] =
                {
                    position.x + 0.5f, position.y, position.z,    0.0f, 0.0f,
                    position.x + 0.5f, position.y, position.z + scale,    1.0f, 0.0f,
                    position.x + 0.5f, position.y + scale, position.z + scale,    1.0f, 1.0f,
                    position.x + 0.5f, position.y + scale, position.z,    0.0f, 1.0f
                };

                // Copia los vértices de los quads al array de vértices
                std::copy(std::begin(quad1Vertices), std::end(quad1Vertices), std::begin(cubeVertices));
                std::copy(std::begin(quad2Vertices), std::end(quad2Vertices), std::begin(cubeVertices) + sizeof(quad1Vertices) / sizeof(float));

                // Asigna los índices de los quads al array de índices
                std::copy(std::begin(quadIndices), std::end(quadIndices), std::begin(cubeIndices));
            }
        }     
         

        #pragma region BUFFER

        if(regenerateBuffers)
        {
            glGenBuffers(1, &vbo);
        }

        glBindBuffer(GL_ARRAY_BUFFER, vbo);

        if (true)
        {
            glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);
        }
        
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);//el 5 antes era 3
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);//esta línea no estaba antes de las texturas

        if (regenerateBuffers)
        {
            glGenBuffers(1, &ibo);
        }

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);

        if (true)
        {
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cubeIndices), cubeIndices, GL_STATIC_DRAW);
        }

        regenerateBuffers = false;
         

        #pragma endregion
        
        
        #pragma region APLICAR TEXTURAS

        int  slot = 0;
    
        for (int i = 0; i < currentTextures.size(); i++)
        {
            if (currentTextures[i] == texturePath)
            {
                slot = i;
            
            }
        }
    
        glUniform1i(glGetUniformLocation(shader, "u_Texture"), slot);

        glUniform1i(glGetUniformLocation(shader, "u_isSolidColor"), isSolidColor);

        glUniform4f(glGetUniformLocation(shader, "u_Color"), color.r, color.g, color.b, color.a);

        glUniform1i(glGetUniformLocation(shader, "u_noTransparency"), noTransparency);


        #pragma endregion

    
        #pragma region RENDER FINAL

        Draw();
        //glDeleteBuffers(1, &vbo);
        //glDeleteBuffers(1, &ibo);

        #pragma endregion
    }
}

Block::~Block()
{
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ibo);

    glDeleteBuffers(1, &topQuadVBO);
    glDeleteBuffers(1, &bottomQuadVBO);
    glDeleteBuffers(1, &frontQuadVBO);
    glDeleteBuffers(1, &backQuadVBO);
    glDeleteBuffers(1, &rightQuadVBO);
    glDeleteBuffers(1, &leftQuadVBO);
}

void Block::Draw()
{
    //Render
    if (twoQuads)
    {
        glDisable(GL_CULL_FACE);
    }
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, nullptr);
    if (twoQuads)
    {
        glEnable(GL_CULL_FACE);
    }
}

/*
Chunk* Block::GetChunk()
{
    for (int i = 0; i < Chunks.size(); i++)
    {
        for (int u = 0; u < Chunks[i].size(); u++)
        {
            if (position.x >= Chunks[i][u].position.x && position.z >= Chunks[i][u].position.y &&
                position.x < Chunks[i][u].position.x + ChunksSize && position.z < Chunks[i][u].position.y + ChunksSize)
            {
                return &Chunks[i][u];
            }
        }
    }

    return nullptr;
}


void Block::UpdateNeighbours()
{
    Chunk* actualChunk = GetChunk();

    if (actualChunk != nullptr)
    {
        for (int i = 0; i < 6; i++)
        {
            vec3 checkPos;

            if (i == 0)
            {
                checkPos = vec3(1, 0, 0);
            }
            else if (i == 1)
            {
                checkPos = vec3(-1, 0, 0);
            }
            else if (i == 2)
            {
                checkPos = vec3(0, 1, 0);
            }
            else if (i == 3)
            {
                checkPos = vec3(0, -1, 0);
            }
            else if (i == 4)
            {
                checkPos = vec3(0, 0, 1);
            }
            else if (i == 5)
            {
                checkPos = vec3(0, 0, -1);
            }

            Block* block = GetChunkBlock(actualChunk, position + checkPos);

            if (block != nullptr)
            {
                block->rewriteAllVariables = true;
                block->regenerateBuffers = true;
            }
        }
    }
    
}

void Block::Break()
{
    UpdateNeighbours();

    Chunk* chunk = GetChunk();

    if (chunk != nullptr)
    {
        for (int i = 0; i < chunk->blocks[id].size(); i++)
        {
            if (&chunk->blocks[id][i] == this)
            {
                cout << chunk->blocks[id].size() << endl;
                chunk->blocks[id].erase(chunk->blocks[id].begin() + i);
                cout << chunk->blocks[id].size() << endl;

                i++;
                break;
            }
        }
    }
    
}*/