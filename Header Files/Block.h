#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "Utility.h"
#include "Texture.h"
#include "BlockSearch.h"
#include <glm/glm.hpp>

using namespace glm;

//TupleHash se declara en Utility.h
// Define el tipo de mapa con función hash personalizada
typedef std::unordered_map<std::tuple<int, int, int>, std::array<int, 2>, TupleHash> BlocksMap;


// Forward declarations
class Block;
Block* GetBlock(vec3 pos);

class Chunk
{
public:

    unsigned int size;

    //Posición (x, y) del primer bloque del Chunk
    glm::vec2 position = glm::vec2(0, 0);

    //vector<vector> de Bloques que contiene el Chunk ordenados tal que [blockType][CreationOrder]
    vector<vector<Block>> blocks;

    //{x, y, z, light, dist, r, g, b}
    vector<vector<float>> BrightBlocks;

    //Cada posicion x y z se asocia a 2 index para el vector<vector<Block>> blocks, tal que [blockType][CreationOrder]
    BlocksMap blocksMap;

    Chunk(glm::vec2 position, unsigned int size, vector<vector<Block>> blocks) : position(position), size(size), blocks(blocks)
    {

    }
};


class Block
{
public:

    int lightLevels = 0;

    bool rewriteAllVariables = true;

    bool isSelected = false;

    bool isOcluded = false;

    #pragma region Create Texture Variables

    string texturePath;

    bool isSolidColor = false;

    string bottomTexturePath;
    string topTexturePath;
    string frontTexturePath;
    string backTexturePath;
    string rightTexturePath;
    string leftTexturePath;

    bool hasFrontFace = true;
    bool hasBackFace = true;
    bool hasTopFace = true;
    bool hasBottomFace = true;
    bool hasRightFace = true;
    bool hasLeftFace = true;

    bool diffBottom = false;
    bool diffTop = false;
    bool diffFront = false;
    bool diffBack = false;
    bool diffRight = false;
    bool diffLeft = false;

    #pragma endregion


    #pragma region Other Variables

	int id = 0;

    vec3 position = vec3(0, 0, 0);
    vec3 rotation = vec3(0, 0, 0);
	Color color = Color();
    bool noTransparency = false;

	int scale;

    float origin = 0.0f;

    bool isTreeBase = false;

    bool canOclude = true;
    bool twoQuads = false;


    bool collidable = true;

    #pragma region Buffers Data

    bool regenerateBuffers = true;
    // Crear un buffer para las coordenadas de vértices
    unsigned int vbo;
    // Crear un buffer para los índices del cubo
    unsigned int ibo;

    

    

    unsigned int topQuadVBO;
    unsigned int bottomQuadVBO;
    unsigned int frontQuadVBO;
    unsigned int backQuadVBO;
    unsigned int rightQuadVBO;
    unsigned int leftQuadVBO;

    #pragma endregion


    float cubeVertices[6 * 4 * 5] =
    {

    };

    unsigned int cubeIndices[36] =
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

    bool regenerateVertices = true;

	unsigned int shader;

	bool IsSolidColor;

	string TexturePath;
	string BottomTexturePath;
	string TopTexturePath;
	string FrontTexturePath;
	string BackTexturePath;
	string RightTexturePath;
	string LeftTexturePath;

	std::vector<std::string>& currentTextures;

    BlocksMap &blocksMap;

	bool initialized = false;

    #pragma endregion


    #pragma region Lighting

    int brightness = 0;
    float brightDistance = 12;
    float lightLevel = 16;
    bool coveredFromSun = false;
    bool hasShadow = true;
    vec4 lightTint = vec4(1, 1, 0.3f, 0);
    vec3 positiveLighting = vec3(0, 0, 0);
    vec3 negativeLighting = vec3(0, 0, 0);

    #pragma endregion



    Block(std::vector<std::string>& currentTextures, BlocksMap& blocksMap);
    Block(int id, std::vector<std::string>& currentTextures, BlocksMap& blocksMap) : id(id), currentTextures(currentTextures), blocksMap(blocksMap)
    {

    }

	Block(int id, vec3 position, Color color, int scale, unsigned int shader, std::vector<std::string>& currentTextures, BlocksMap &blocksMap);

    ~Block();

    void DefineOcluded();
	void Create();
	void Draw();

    void Recalculate()
    {
        regenerateBuffers = true;
        rewriteAllVariables = true;
        regenerateVertices = true;
    }

    /*
    static Block GetBlock(int x, int y, int z, BlocksMap& blocksMap, std::vector<std::vector<Block>>& Blocks, std::vector<std::vector<Chunk>>& Chunks)
    {
        for (int i = 0; i < Chunks.size(); i++)
        {
            for (int u = 0; u < Chunks[i].size(); u++)
            {
                if (x >= Chunks[i][u].position.x && x < Chunks[i][u].position.x + Chunks[i][u].size &&
                    z >= Chunks[i][u].position.y && z < Chunks[i][u].position.y + Chunks[i][u].size)
                {
                    auto iter = Chunks[i][u].blocksMap.find(std::make_tuple(x, y, z));

                    if (iter != Chunks[i][u].blocksMap.end())
                    {
                        //return Blocks[iter->second[0]][iter->second[1]];
                        return Chunks[i][u].blocks[iter->second[0]][iter->second[1]];
                    }
                    else
                    {
                        //std::cout << "ERROR: La clave <" << x << ", " << y << ", " << z << "> no se encontró en BlocksMap" << std::endl;
                        std::vector<std::string> emptyTextures;
                        return Block(-2, emptyTextures, Chunks[i][u].blocksMap);
                    }
                }
            }
        } 

        std::vector<std::string> emptyTextures;
        return Block(-2, emptyTextures, blocksMap);
    }
    */

    static void addBlock(Block block, BlocksMap& blocksMap, std::vector<std::vector<Block>>& Blocks, std::vector<std::vector<Chunk>>& Chunks)
    {
        while (Blocks.size() < block.id + 1)
        {
            //Blocks.push_back({});
        }

        //Blocks[block.id].push_back(block);

        for (int i = 0; i < Chunks.size(); i++)
        {
            for (int u = 0; u < Chunks[i].size(); u++)
            {
                if (block.position.x >= Chunks[i][u].position.x && block.position.x < Chunks[i][u].position.x + Chunks[i][u].size &&
                    block.position.z >= Chunks[i][u].position.y && block.position.z < Chunks[i][u].position.y + Chunks[i][u].size)
                {
                    while (Chunks[i][u].blocks.size() < block.id + 1)
                    {
                        Chunks[i][u].blocks.push_back({});
                    }

                    Chunks[i][u].blocks[block.id].push_back(block);
                    //Chunks[i][u].blocksMap[std::make_tuple((int)block.position.x, (int)block.position.y, (int)block.position.z)] = { {block.id, (int)Blocks[block.id].size() - 1} };
                    Chunks[i][u].blocksMap[std::make_tuple((int)block.position.x, (int)block.position.y, (int)block.position.z)] = { {block.id, (int)Chunks[i][u].blocks[block.id].size() - 1} };

                    if (block.brightness > 0)
                    {
                        vector<float> brightBlockData = { block.position.x, block.position.y, block.position.z, (float)block.brightness, block.brightDistance, block.lightTint.x, block.lightTint.y, block.lightTint.z};
                        Chunks[i][u].BrightBlocks.push_back(brightBlockData);
                    }
                }
            }
        }

        //blocksMap[std::make_tuple((int)block.position.x, (int)block.position.y, (int)block.position.z)] = { {block.id, (int)Blocks[block.id].size() - 1} };
    }

    /*static bool isBlock(int x, int y, int z, BlocksMap& blocksMap, std::vector<std::vector<Chunk>>& Chunks)
    {
        for (int i = 0; i < Chunks.size(); i++)
        {
            for (int u = 0; u < Chunks[i].size(); u++)
            {
                if (x >= Chunks[i][u].position.x && x < Chunks[i][u].position.x + Chunks[i][u].size &&
                    z >= Chunks[i][u].position.y && z < Chunks[i][u].position.y + Chunks[i][u].size)
                {
                    auto iter = Chunks[i][u].blocksMap.find(std::make_tuple(x, y, z));

                    return iter != Chunks[i][u].blocksMap.end();
                }
            }
        } 
    }*/

    static bool checkOclude(int x, int y, int z, BlocksMap& blocksMap, std::vector<std::vector<Block>>& Blocks, std::vector<std::vector<Chunk>>& Chunks)
    {
        Block* block = GetBlock(vec3(x, y, z));

        return (block != nullptr && block->id >= 0 && block->canOclude);
    }

    static bool checkCollidable(int x, int y, int z, BlocksMap& blocksMap, std::vector<std::vector<Block>>& Blocks, std::vector<std::vector<Chunk>>& Chunks)
    {
        Block* block = GetBlock(vec3(x, y, z));

        return (block != nullptr && block->id >= 0 && block->collidable);
        
    }
    
    /*
    void UpdateNeighbours();
    Chunk* GetChunk();
    void Break();

    Block& operator=(const Block& other)
    {
        if (this != &other) // Evitar la autoasignación
        {
            // Copiar los miembros booleanos
            this->rewriteAllVariables = other.rewriteAllVariables;
            this->isSelected = other.isSelected;
            this->isSolidColor = other.isSolidColor;
            this->hasFrontFace = other.hasFrontFace;
            this->hasBackFace = other.hasBackFace;
            this->hasTopFace = other.hasTopFace;
            this->hasBottomFace = other.hasBottomFace;
            this->hasRightFace = other.hasRightFace;
            this->hasLeftFace = other.hasLeftFace;
            this->diffBottom = other.diffBottom;
            this->diffTop = other.diffTop;
            this->diffFront = other.diffFront;
            this->diffBack = other.diffBack;
            this->diffRight = other.diffRight;
            this->diffLeft = other.diffLeft;
            this->canOclude = other.canOclude;
            this->twoQuads = other.twoQuads;
            this->collidable = other.collidable;
            this->regenerateBuffers = other.regenerateBuffers;
            this->hasVertices = other.hasVertices;
            this->isTreeBase = other.isTreeBase;
            this->noTransparency = other.noTransparency;
            this->initialized = other.initialized;
            this->IsSolidColor = other.IsSolidColor;

            // Copiar los miembros de cadena
            this->texturePath = other.texturePath;
            this->bottomTexturePath = other.bottomTexturePath;
            this->topTexturePath = other.topTexturePath;
            this->frontTexturePath = other.frontTexturePath;
            this->backTexturePath = other.backTexturePath;
            this->rightTexturePath = other.rightTexturePath;
            this->leftTexturePath = other.leftTexturePath;
            this->TexturePath = other.TexturePath;
            this->BottomTexturePath = other.BottomTexturePath;
            this->TopTexturePath = other.TopTexturePath;
            this->FrontTexturePath = other.FrontTexturePath;
            this->BackTexturePath = other.BackTexturePath;
            this->RightTexturePath = other.RightTexturePath;
            this->LeftTexturePath = other.LeftTexturePath;

            // Copiar los miembros de entero
            this->id = other.id;
            this->scale = other.scale;
            this->brightness = other.brightness;

            // Copiar los miembros de punto flotante
            this->origin = other.origin;

            // Copiar los miembros de vec3
            this->position = other.position;
            this->rotation = other.rotation;
            this->color = other.color;
            this->positiveLighting = other.positiveLighting;
            this->negativeLighting = other.negativeLighting;

            // Copiar los miembros de vector
            this->currentTextures = other.currentTextures;

            // Copiar el mapa de bloques
            this->blocksMap = other.blocksMap;

            // Copiar los shaders y los buffers
            this->shader = other.shader;
            this->vbo = other.vbo;
            this->ibo = other.ibo;
            this->topQuadVBO = other.topQuadVBO;
            this->bottomQuadVBO = other.bottomQuadVBO;
            this->frontQuadVBO = other.frontQuadVBO;
            this->backQuadVBO = other.backQuadVBO;
            this->rightQuadVBO = other.rightQuadVBO;
            this->leftQuadVBO = other.leftQuadVBO;

            // Copiar los datos de vértices e índices
            std::memcpy(this->cubeVertices, other.cubeVertices, sizeof(other.cubeVertices));
            std::memcpy(this->cubeIndices, other.cubeIndices, sizeof(other.cubeIndices));
        }
        return *this;
    }
    */

};

/*
static Block* GetChunkBlock(Chunk* chunk, vec3 pos)
{
    if (chunk != nullptr)
    {
        for (int i = 0; i < chunk->blocks.size(); i++)
        {
            for (int u = 0; u < chunk->blocks[i].size(); u++)
            {
                if (chunk->blocks[i][u].position == pos)
                {
                    return &chunk->blocks[i][u];
                }
            }
        }
    }

    return nullptr;
}

static bool isOcluded(vec3 pos, Chunk* chunk)
{
    Block* block = GetChunkBlock(chunk, pos);
    return (block != nullptr && !block->canOclude);
}
*/

extern vector<vector<Chunk>> Chunks;
extern int ChunksSize;
extern BlocksMap blocksMap;

static Chunk* GetChunk(vec2 pos)
{
    return &Chunks[pos.x][pos.y];
}

static Chunk* GetChunkOfBlock(Block* block)
{
    return &Chunks[floor(block->position.x / ChunksSize) * ChunksSize][floor(block->position.y / ChunksSize) * ChunksSize];
}

static vector<Chunk*> GetChunksAroundBlock(Block* block)
{
    vector<Chunk*> ret;

    for (int i = 0; i < 9; i++)
    {
        vec2 offset = vec2(0, 0);

        if (i == 0)
        {
            offset = vec2(0, 0);
        }
        else if (i == 1)
        {
            offset = vec2(1, 0);
        }
        else if (i == 2)
        {
            offset = vec2(0, 1);
        }
        else if (i == 3)
        {
            offset = vec2(1, 1);
        }
        else if (i == 4)
        {
            offset = vec2(-1, 0);
        }
        else if (i == 5)
        {
            offset = vec2(0, -1);
        }
        else if (i == 6)
        {
            offset = vec2(-1, -1);
        }
        else if (i == 7)
        {
            offset = vec2(-1, 1);
        }
        else if (i == 8)
        {
            offset = vec2(1, -1);
        }

        int ChunkX = floor(block->position.x / ChunksSize) + offset.x;
        int ChunkY = floor(block->position.z / ChunksSize) + offset.y;

        if (Chunks.size() > ChunkX)
        {
            if (Chunks[ChunkX].size() > ChunkY)
            {
                ret.push_back(&Chunks[ChunkX][ChunkY]);
            }
        }
    }

    return ret;
}

static bool isBlock(vec3 pos)
{
    int x = pos.x;
    int y = pos.y;
    int z = pos.z;

    for (int i = 0; i < Chunks.size(); i++)
    {
        for (int u = 0; u < Chunks[i].size(); u++)
        {
            if (x >= Chunks[i][u].position.x && x < Chunks[i][u].position.x + Chunks[i][u].size &&
                z >= Chunks[i][u].position.y && z < Chunks[i][u].position.y + Chunks[i][u].size)
            {
                auto iter = Chunks[i][u].blocksMap.find(std::make_tuple(x, y, z));

                if (iter != Chunks[i][u].blocksMap.end())
                {
                    return true;
                }
                else
                {
                    return false;
                }
            }
        }
    }

    return false;
}

static Block* GetBlock(vec3 pos)
{
    for (int i = 0; i < Chunks.size(); i++)
    {
        for (int u = 0; u < Chunks[i].size(); u++)
        {
            if (pos.x >= Chunks[i][u].position.x && pos.x < Chunks[i][u].position.x + Chunks[i][u].size &&
                pos.z >= Chunks[i][u].position.y && pos.z < Chunks[i][u].position.y + Chunks[i][u].size)
            {
                auto iter = Chunks[i][u].blocksMap.find(std::make_tuple(pos.x, pos.y, pos.z));

                if (iter != Chunks[i][u].blocksMap.end())
                {
                    //return Blocks[iter->second[0]][iter->second[1]];
                    return &Chunks[i][u].blocks[iter->second[0]][iter->second[1]];
                }
                else
                {
                    //std::cout << "ERROR: La clave <" << x << ", " << y << ", " << z << "> no se encontró en BlocksMap" << std::endl;
                    //std::vector<std::string> emptyTextures;
                    return nullptr;
                }
            }
        }
    }

    //std::vector<std::string> emptyTextures;
    return nullptr;
}
