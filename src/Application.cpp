
#pragma region INCLUDE

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <chrono>
#include <vector>
#include <unordered_map>
#include <array>
#include <math.h>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp> 



#define _USE_MATH_DEFINES
#include <cmath>
#include <math.h>

//Audio
#include "Audio.h"


//Random
#include <stdlib.h>
#include <cstdlib>


#include "vendor/stb_image/stb_image.h"

#include "FastNoiseLite.h"

#include "Physics.h"
#include "Camera.h"
#include "Utility.h"
#include "Utility.h"
#include "Texture.h"
#include "BlockSearch.h"



using namespace std;

#pragma endregion


#pragma region CÓDIGOS DE ERROR

#define GLFW_DLL

//Detiene el código en caso de error
#define ASSERT(x) if (!(x)) __debugbreak();
#define GLCall(x) GLClearError(); x; ASSERT(GLLogCall(#x, __FILE__, __LINE__))

#pragma endregion


#pragma region OTROS

static void GLClearError()
{
    while (glGetError() != GL_NO_ERROR);
}

static bool GLLogCall(const char* function, const char* file, int line)
{
    while (GLenum error = glGetError())
    {
        std::cout << "Error de OpenGL (" << error << ") " << function <<
            " " << file << ": " << line << endl;
        return false;
    }

    return true;
}

struct ShaderProgramSource
{
    std::string VertexSource;
    std::string FragmentSource;
};

static ShaderProgramSource ParseShader(const string& filepath)
{
    std::ifstream stream(filepath);

    enum class ShaderType
    {
        NONE = -1, VERTEX = 0, FRAGMENT = 1
    };

    std::string line;
    std::stringstream ss[2];
    ShaderType type = ShaderType::NONE;

    while (getline(stream, line))
    {
        if (line.find("#shader") != std::string::npos)
        {
            if (line.find("vertex") != std::string::npos)
            {
                type = ShaderType::VERTEX;
            }
            else if (line.find("fragment") != std::string::npos)
            {
                type = ShaderType::FRAGMENT;
            }
        }
        else
        {
            ss[(int)type] << line << '\n';
        }
    }

    return { ss[0].str(), ss[1].str() };
}

static unsigned int CompileShader(unsigned int type, const string& source)
{
    unsigned int id = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(id, 1, &src, nullptr);
    glCompileShader(id);

    int result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);

    if (result == GL_FALSE)
    {
        int length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
        char* message = (char*)alloca(length * sizeof(char));
        glGetShaderInfoLog(id, length, &length, message);

        cout << "ERROR AL COMPILAR EL SHADER" << endl;
        cout << message << endl;

        glDeleteShader(id);
        return 0;
    }

    return id;
}

static int CreateShader(const string& vertexShader, const string& fragmentShader)
{
    unsigned int program = glCreateProgram();
    unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
    unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);
    glValidateProgram(program);

    glDeleteShader(vs);
    glDeleteShader(fs);

    return program;
}

double lastTime = 0;
int frameCount = 0;
int fps = 0;

void calculateFPS(double currentTime, int textureCalls = -1, int createCalls = -1, bool showDeltaTime = true) {
    double deltaTime = currentTime - lastTime;
    frameCount++;

    if (deltaTime >= 1.0) {
        fps = frameCount;
        frameCount = 0;
        lastTime = currentTime;
        std::cout << "FPS: " << fps << std::endl;

        if (textureCalls >= 0)
        {
            cout << "Texture Calls: " << textureCalls << endl;
        }

        if (createCalls >= 0)
        {
            cout << "Create() Calls: " << createCalls << endl;
        }

        if (showDeltaTime)
        {
            cout << "Delta Time: " << deltaTime << endl;
        }
    }
}

//Oclussion id
GLuint queryID;
#pragma endregion


#pragma region VARIABLES GLOBALES

// Define el tipo de mapa con función hash personalizada
typedef std::unordered_map<std::tuple<int, int, int>, std::array<int, 2>, TupleHash> BlocksMap;

BlocksMap blocksMap;
extern BlocksMap blocksMap;

std::vector<std::vector<Block>> Blocks;

extern vector<vector<Block>> Blocks;

vector<vector<Chunk>> Chunks;

extern vector<vector<Chunk>> Chunks;

std::vector<std::string> currentTextures;
extern std::vector<std::string> currentTextures;

float deltaTime = 0.0f;
extern float deltaTime;
std::chrono::steady_clock::time_point lastFrameTime;

float gravity = 9.8f;
extern float gravity;

int globalVolume = 100;
extern int globalVolume;

vector<Audio> Audios(100, Audio());
extern vector<Audio> Audios;

int currentAudioId = 0;
extern int currentAudioId;

Block helpBlock = Block(-1, vec3(-99.0f, -999.0f, -99.0f), Color(0, 0, 0, 0), 1, 0, currentTextures, blocksMap);
extern Block helpBlock;

vec3 skyLight = normalize(vec3(1, -1, 0));
extern vec3 skyLight;

int ChunksRenderDistance = 1;
extern int ChunksRenderDistance;

int ChunksSize = 20;
extern int ChunksSize;




void PlayAudio(std::string filePath, bool wholePath, int vol)
{
    for (int i = 0; i < Audios.size(); i++)
    {
        if (Audios[i].id == -1)
        {
            Audios[i] = Audio(filePath, wholePath, vol);
            Audios[i].Play();
            currentAudioId = i + 1;
            break;
        }
    }   
}

extern void PlayAudio(std::string filePath, bool wholePath, int vol);

#pragma endregion


#pragma region FUNCIONES

// Función para calcular el tiempo transcurrido desde el último frame
void calculateDeltaTime()
{
    // Obtener el tiempo actual
    auto currentFrameTime = std::chrono::steady_clock::now();

    // Calcular la diferencia de tiempo entre el último frame y el actual
    std::chrono::duration<float> duration = currentFrameTime - lastFrameTime;

    // Actualizar deltaTime
    deltaTime = duration.count();

    // Actualizar lastFrameTime para el próximo frame
    lastFrameTime = currentFrameTime;
}

float smoothFunc(float x, float p = -1.5f, float mid = 0.5f)
{
    return (1 - (1 / (1 + pow(exp(1.0), (p * (x - mid))))));
}

vector<vector<float>> generateF1TerrainBase(unsigned int res = 32, int whites = 50)
{
    vector<vector<float>> img(res, vector<float>(res, 0.5f));

    for (int i = 0; i < whites; i++)
    {
        int rndX = rand() % (res - 1);
        int rndY = rand() % (res - 1);

        img[rndX][rndY] = 1;
    }

    vector<vector<float>> imgSmooth(res, vector<float>(res, 0.5f));
    imgSmooth = img;

    #pragma region BlackPoints Smoothness

    //int i = 10;
    //int u = 5;

    //if(true)
    for (int i = 0; i < res; i++)
    {
        //if (true)
        for (int u = 0; u < res; u++)
        {
            if (img[i][u] == 0.5f)
            {
                imgSmooth[i][u] = -1;

                //Recorremos toda la imagen en busca del blanco más cercano
                for (int i2 = 0; i2 < res; i2++)
                {
                    for (int u2 = 0; u2 < res; u2++)
                    {
                        //Encontramos un punto blanco
                        if (img[i2][u2] == 1 && (i != i2 || u != u2))
                        {
                            //Calculamos la distancia
                            float d = distance(vec2(i, u), vec2(i2, u2));
                            float dist = smoothFunc(d, -0.5f, 10.0f);


                            if (dist > imgSmooth[i][u] || imgSmooth[i][u] == -1)
                            {
                                bool c = dist > imgSmooth[i][u];
                                bool d = imgSmooth[i][u] == -1;
                                imgSmooth[i][u] = dist;
                                float a = imgSmooth[i][u];
                                float b = imgSmooth[i][u];
                            }



                        }
                    }
                }
            }
        }
    }

    #pragma endregion


    return imgSmooth;
}

std::vector<std::vector<float>> generatePerlinTerrainBase(unsigned int res, unsigned int seed = 1234, float frec = 0.1f, float intensity = 1.5f, float blackAmount = 0.1f, float maxWhite = 1.0f)
{
    int width = res;
    int height = res;

    // Crear una instancia de FastNoiseLite
    FastNoiseLite noise;

    // Configurar el tipo de ruido (en este caso, Perlin)
    noise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);

    // Definir la semilla (opcional)
    noise.SetSeed(seed);

    // Definir la frecuencia del ruido (menor valor = mayor detalle)
    noise.SetFrequency(frec);

    // Generar el ruido Perlin
    std::vector<std::vector<float>> perlinNoise(width, std::vector<float>(height, 0.0f));
    for (int x = 0; x < width; ++x) {
        for (int y = 0; y < height; ++y) {
            float perlinValue = noise.GetNoise((float)x, (float)y) * intensity;
            float adjustedValue = std::max(0.0f, std::min(maxWhite, perlinValue + blackAmount));
            perlinNoise[x][y] = adjustedValue;
        }
    }

    return perlinNoise;
}

static void genTree(int trunkId, int leavesId, vec3 pos, unsigned int shader, std::vector<std::string>& currentTextures, BlocksMap& blocksMap, std::vector<std::vector<Block>>& Blocks, std::vector<std::vector<Chunk>>& Chunks)
{
    unsigned int height = 2 + rand() % (3 - 1);

    for (int i = 0; i < height; i++)
    {
        Block block = Block(trunkId, vec3(pos.x, pos.y + 1.0f * i, pos.z), Color(), 1, shader, currentTextures, blocksMap);

        if (i == 0)
        {
            block.isTreeBase = true;
        }

        block.addBlock(block, blocksMap, Blocks, Chunks);
    }

    for (int i = -2; i < 3; i++)
    {
        for (int u = -2; u < 3; u++)
        {
            for (int v = 0; v < 2; v++)
            {
                if (!((i == -2 && u == -2) || (i == 2 && u == -2) || (i == -2 && u == 2) || (i == 2 && u == 2)))
                {
                    Block block = Block(leavesId, vec3(pos.x + 1.0f * i, pos.y + 1.0f * (height - 1) + 1.0f + 1.0f * v, pos.z + 1.0f * u), Color(0, 1.0f, 0, 1.0f), 1, shader, currentTextures, blocksMap);
                    block.addBlock(block, blocksMap, Blocks, Chunks);

                }
            }
        }
    }

    for (int i = -1; i < 2; i++)
    {
        for (int u = -1; u < 2; u++)
        {
            Block block = Block(leavesId, vec3(pos.x + 1.0f * i, pos.y + 1.0f * (height - 1) + 3.0f, pos.z + 1.0f * u), Color(0, 1.0f, 0, 1.0f), 1, shader, currentTextures, blocksMap);
            block.addBlock(block, blocksMap, Blocks, Chunks);
        }
    }

    for (int i = -1; i < 2; i++)
    {
        for (int u = -1; u < 2; u++)
        {
            if (!((i == -1 && u == -1) || (i == 1 && u == -1) || (i == -1 && u == 1) || (i == 1 && u == 1)))
            {
                Block block = Block(leavesId, vec3(pos.x + 1.0f * i, pos.y + 1.0f * (height - 1) + 4.0f, pos.z + 1.0f * u), Color(0, 1.0f, 0, 1.0f), 1, shader, currentTextures, blocksMap);
                block.addBlock(block, blocksMap, Blocks, Chunks);
            }
        }
    }
}

// Función para calcular un plano dado cuatro puntos
vector<float> planeFromPoints(const vec3& p1, const vec3& p2, const vec3& p3, const vec3& p4)
{
    // Vectores de dos puntos en el plano
    double v1x = p2.x - p1.x;
    double v1y = p2.y - p1.y;
    double v1z = p2.z - p1.z;

    double v2x = p3.x - p1.x;
    double v2y = p3.y - p1.y;
    double v2z = p3.z - p1.z;

    // Producto cruz para obtener el vector normal al plano
    double nx = v1y * v2z - v1z * v2y;
    double ny = v1z * v2x - v1x * v2z;
    double nz = v1x * v2y - v1y * v2x;

    // Ecuación del plano Ax + By + Cz + D = 0
    double A = nx;
    double B = ny;
    double C = nz;
    double D = -(nx * p1.x + ny * p1.y + nz * p1.z);

    vector<float> result;

    result.push_back(A);
    result.push_back(B);
    result.push_back(C);
    result.push_back(D);

    return result;
}

extern vector<float> planeFromPoints(const vec3& p1, const vec3& p2, const vec3& p3, const vec3& p4);

#pragma endregion

float gameTime = 0.0f;

int main(void)
{
    //4k:   3840  2160
    int width = 640 * 3;
    int heigth = 480 * 2;
    int startFramesToWait = 10;

    const int TARGET_FPS = 60;
    const double FRAME_TIME = 1.0 / TARGET_FPS;

    #pragma region VENANA Y OTROS

        GLFWwindow* window;

        /* Initialize the library */
        if (!glfwInit())
            return -1;

        /* Create a windowed mode window and its OpenGL context */
        window = glfwCreateWindow(width, heigth, "My Minecraft", NULL, NULL);
        if (!window)
        {
            glfwTerminate();
            return -1;
        }


        //Oclussion Culling
        if (GLEW_ARB_occlusion_query) {
            //glGenQueries(1, &queryID);
        }

        //Desactivar VSync
        //glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_FALSE); // Deshabilitar el doble búfer
        //glfwWindowHint(GLFW_REFRESH_RATE, GLFW_DONT_CARE); // Establecer la tasa de refresco a "no importa"


        /* Make the window's context current */
        glfwMakeContextCurrent(window);

        //ESTABILIZAR FRAMES
        glfwSwapInterval(1);

        //Inicia glewInit(), una "extensión" de OpenGL para graficos modernos
        if (glewInit() != GLEW_OK)
        {
            cout << "ERROR EN EL glewInit()" << endl;
        }

    #pragma endregion
    
    #pragma region SHADERS
        //SHADERS
        ShaderProgramSource source = ParseShader("res/shaders/Basic.shader");
        unsigned int shader = CreateShader(source.VertexSource, source.FragmentSource);
        glUseProgram(shader);
    #pragma endregion

    #pragma region VARIABLES CAMARA

        unsigned int viewDistance = 25;
        unsigned int heightViewDistance = 25;

        Camera camera(width, heigth, vec3(5.0f, 4.0f, 5.0f));
        double lastMouseX = 0;
        double lastMouseY = 0;
        double camOffsetX = 0;
        double camOffsetY = 0;

        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);

        //float footStepsTimer = (camera.entity.running ? camera.entity.footStepsRunFrec : camera.entity.footStepsWalkFrec);

    #pragma endregion

    #pragma region BACKFACE CULLING

        //Establece el orden de renderizado
        glEnable(GL_DEPTH_TEST);

        //Añadimos backface culling, no renderizar caras internas
        glEnable(GL_CULL_FACE); // Habilita el culling de caras
        //glCullFace(GL_BACK); // Indica que se deben eliminar las caras traseras

    #pragma endregion
    
    #pragma region VECTOR DE TEXTURAS CARGADAS

        GLint maxTextureUnits;
        glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &maxTextureUnits);

        cout << "maxTextureUnits: " << maxTextureUnits << endl;

        

        if (maxTextureUnits > 30)
        {
            maxTextureUnits = 30;
        }

        for (int i = 0; i < maxTextureUnits; i++)
        {
            currentTextures.push_back("");
        }

    #pragma endregion

    //Music
    PlayAudio("res/Music/music/game/calm2.wav", true, 50);

    //SkyBox
    int skyScale = 100;
    Block skybox = Block(-1, vec3(0,0,0), Color(171.0f / 255.0f, 197.0f / 255.0f, 1.0f, 1.0f), skyScale, shader, currentTextures, blocksMap);


    #pragma region TERRENO
        
        int groundScale = 75;//25

        int ChunksAmount = groundScale / ChunksSize + 1;

        for (int i = 0; i < ChunksAmount; i++)
        {
            Chunks.push_back({});

            for (int u = 0; u < ChunksAmount; u++)
            {
                Chunks[i].push_back(Chunk(vec2(i * ChunksSize, u * ChunksSize), ChunksSize, {}));
            }
        }

        
        vec2 dirtLayers = vec2(1, 3);
        int minHeight = -10;
        int groundHeight = 10;

        unsigned int blocksAmount = 0;

        vector<vector<int>> TerrainBlocksId(groundScale, vector<int>(groundScale));

        vector<vector<Block>> TerrainBlocks;
        bool ViewHeightMapMode = false;
        
        //<vector<float>> img = generateF1TerrainBase(groundScale, groundScale*20/45);
        vector<vector<float>> img = generatePerlinTerrainBase(groundScale, rand() % (9999), 0.05f, 0.3f, 0.2f, 0.8f);


        for (int i = 0; i < groundScale; i++)
        {
            cout << "Generando el Terreno: " << i * 100 / groundScale << "%" << endl;

            for (int u = 0; u < groundScale; u++)
            {
                int id = 2;

                float div = 1;

                Color color = Color();

                if (ViewHeightMapMode)
                {
                    id = -1;
                    color = Color(img[i][u] / div, img[i][u] / div, img[i][u] / div, 1);
                }


                Block block = Block(id, vec3(1.0f * i - 3.0f, -1.0f, 1.0f * u - 6.0f), color, 1, shader, currentTextures, blocksMap);

                if (!ViewHeightMapMode)
                {
                    block.position.y = floor(img[i][u] * 8.5f) - 10;
                }


                while (TerrainBlocks.size() <= i)
                {
                    TerrainBlocks.push_back({});
                }

                TerrainBlocks[i].push_back(block);

                if (!ViewHeightMapMode)
                {
                    block.addBlock(block, blocksMap, Blocks, Chunks);
                }
                else
                {
                    block.Create();
                }

                int dirtBelow = (rand() % (int)(dirtLayers.y - 1)) + dirtLayers.x;

                for (int t = 0; t < dirtBelow; t++)
                {
                    Block blockBelow = Block(0, vec3(1.0f * i - 3.0f, block.position.y - 1.0f - 1.0f * t, 1.0f * u - 6.0f), color, 1, shader, currentTextures, blocksMap);
                    blockBelow.addBlock(blockBelow, blocksMap, Blocks, Chunks);
                }

                int stoneLayers = block.position.y - 1.0f - 1.0f * (dirtBelow - 1);
                while (stoneLayers > minHeight)
                {
                    int id = 1;

                    if (stoneLayers == minHeight + 1)
                    {
                        id = 6;
                    }

                    Block blockBelow = Block(id, vec3(1.0f * i - 3.0f, stoneLayers - 1.0f, 1.0f * u - 6.0f), color, 1, shader, currentTextures, blocksMap);
                    blockBelow.addBlock(blockBelow, blocksMap, Blocks, Chunks);
                    stoneLayers--;
                }
            }
        }
       

        Block blockX = Block(1, vec3(1.0f, 0.0f, 0.0f), Color(1.0f, 0.0f, 0.0f, 1.0f), 1, shader, currentTextures, blocksMap);
        Block blockY = Block(13, vec3(0.0f, 1.0f, 0.0f), Color(0.0f, 1.0f, 0.0f, 1.0f), 1, shader, currentTextures, blocksMap);
        Block blockZ = Block(3, vec3(0.0f, 0.0f, 1.0f), Color(0.0f, 0.0f, 1.0f, 1.0f), 1, shader, currentTextures, blocksMap);
        Block blockZ2 = Block(3, vec3(0.0f, 0.0f, 5.0f), Color(0.0f, 0.0f, 1.0f, 1.0f), 1, shader, currentTextures, blocksMap);
        Block blockZ3 = Block(3, vec3(0.0f, 0.0f, 9.0f), Color(0.0f, 0.0f, 1.0f, 1.0f), 1, shader, currentTextures, blocksMap);
        Block blockZ4 = Block(3, vec3(0.0f, 0.0f, 10.0f), Color(0.0f, 0.0f, 1.0f, 1.0f), 1, shader, currentTextures, blocksMap);

        blockX.addBlock(blockX, blocksMap, Blocks, Chunks);
        blockY.addBlock(blockY, blocksMap, Blocks, Chunks);
        blockZ.addBlock(blockZ, blocksMap, Blocks, Chunks);
        blockZ2.addBlock(blockZ2, blocksMap, Blocks, Chunks);
        blockZ3.addBlock(blockZ3, blocksMap, Blocks, Chunks);
        blockZ4.addBlock(blockZ4, blocksMap, Blocks, Chunks);
        

    #pragma endregion

    #pragma region ÁRBOLES

        float intensity = 0.05f;
        vector<vector<float>> treesNoise = generatePerlinTerrainBase(groundScale, rand() % (9999), intensity, 0.3f, 0.2f, 0.8f);

        int treesToGen = (groundScale * groundScale) / 35;

        for (int i = 0; i < groundScale; i++)
        {
            cout << "Generando Arboles: " << i * 100 / groundScale << "%" << endl;
            for (int u = 0; u < groundScale; u++)
            {
                float minDist = 6 + rand() % (4 - 1);
                bool canGenerate = true;
                int yPos = floor(img[i][u] * 8.5f) - 10;

                //Revisar el chunk actual
                for (int e = 0; e < Chunks.size(); e++)
                {
                    for (int j = 0; j < Chunks[e].size(); j++)
                    {
                        //Revisar los chunks contiguos
                        for (int k = -1; k <= 1; k++)
                        {
                            for (int l = -1; l <= 1; l++)
                            {
                                if (i + k >= Chunks[e][j].position.x && i + k < Chunks[e][j].position.x + Chunks[e][j].size &&
                                    u + l >= Chunks[e][j].position.y && u + l < Chunks[e][j].position.y + Chunks[e][j].size)
                                {
                                    //Buscar un bloque de base de árbol

                                    for (int m = 0; m < Chunks[e][j].blocks.size(); m++)
                                    {
                                        for (int n = 0; n < Chunks[e][j].blocks[m].size(); n++)
                                        {
                                            if (Chunks[e][j].blocks[m][n].isTreeBase)
                                            {
                                                if (distance(vec3(i, yPos, u), vec3(Chunks[e][j].blocks[m][n].position.x, Chunks[e][j].blocks[m][n].position.y, Chunks[e][j].blocks[m][n].position.z)) < minDist)
                                                {
                                                    canGenerate = false;
                                                    break;
                                                    break;
                                                    break;
                                                    break;
                                                    break;
                                                    break;
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }

                if (canGenerate)
                {
                    float chances = ((float)(rand() % 101) / 100.0f / 2) * 3 * ((treesNoise[i][u]) / 3) / intensity;

                    if (chances > 2.3f)
                    {
                        genTree(7, 8, vec3(i, yPos, u), shader, currentTextures, blocksMap, Blocks, Chunks);
                        treesToGen--;

                        if (treesToGen <= 0)
                        {
                            i = groundScale;
                            u = groundScale;
                        }
                    }
                }
            }
        }

        //genTree(7, 8, vec3(22, 0, 22), shader, currentTextures, blocksMap, Blocks, Chunks);

    #pragma endregion

    #pragma region HIERBA

        intensity = 0.05f;  
        vector<vector<float>> grassNoise = generatePerlinTerrainBase(groundScale, rand() % (9999), intensity, 0.3f, 0.2f, 0.8f);

        for (int i = 0; i < groundScale; i++)
        {
            //cout << "Generando Hierba: " << i * 100 / groundScale << "%" << endl;
            
            for (int u = 0; u < groundScale; u++)
            {
                int yPos = floor(img[i][u] * 8.5f) - 10;
                float chances = ((float)(rand() % 101) / 100.0f / 2) * 3 * ((grassNoise[i][u]) / 3) / intensity;

                if (chances > 1.4f)
                {
                    if (helpBlock.isBlock(i, yPos, u, blocksMap, Chunks))
                    {
                        Block grass = Block(13, vec3(i, yPos+1, u), Color(0.3f, 1, 0.3f, 1), 1, shader, currentTextures, blocksMap);
                        grass.addBlock(grass, blocksMap, Blocks, Chunks);
                    }
                }
            }
        }

        cout << "Hierba Generada!" << endl;

    #pragma endregion

    #pragma region FLORES   

        intensity = 0.05f;

        for (int i = 0; i < groundScale; i++)
        {
            //cout << "Generando  Flores: " << i * 100 / groundScale << "%" << endl;

            for (int u = 0; u < groundScale; u++)
            {
                int yPos = floor(img[i][u] * 8.5f) - 10;
                float chances = ((float)(rand() % 101) / 100.0f / 2) * 3 * ((grassNoise[i][u]) / 3) / intensity;

                if (chances > 1.8f)
                {
                    if (helpBlock.isBlock(i, yPos, u, blocksMap, Chunks))
                    {
                        Block flower = Block(14 + (rand() % (10 + 1)), vec3(i, yPos + 1, u), Color(), 1, shader, currentTextures, blocksMap);
                        flower.addBlock(flower, blocksMap, Blocks, Chunks);
                    }
                }
            }
        }

        cout << "Flores Generadas!" << endl;

    #pragma endregion

        bool candoonce = true;

    
    while (!glfwWindowShouldClose(window))
    {
        gameTime += deltaTime;

        auto startTime = std::chrono::steady_clock::now();     

        #pragma region INICIO DEL LOOP
        
        calculateDeltaTime();
        
        /* Render here */
        GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
        
        // Activar el modo de wireframe
        //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

        // Desactivar el modo de wireframe
        //glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        sf::Listener::setGlobalVolume(globalVolume);

        #pragma endregion

        #pragma region SKYBOX

        glCullFace(GL_FRONT); // Indica que se deben eliminar las caras frontales

        skybox.position = vec3(camera.entity.position.x - skyScale / 2, camera.entity.position.y - skyScale / 2, camera.entity.position.z - skyScale / 2);
        skybox.Recalculate();
        skybox.Create();

        if (ViewHeightMapMode)
        {
            for (int i = 0; i < TerrainBlocks.size(); i++)
            {
                for (int u = 0; u < TerrainBlocks[i].size(); u++)
                {
                    TerrainBlocks[i][u].Create();
                }
            }
        }
        

        glCullFace(GL_BACK); // Indica que se deben eliminar las caras traseras

        //Directional Light
        if (false)
        {
            float angularSpeed = glm::radians(45.0f); // Velocidad de rotación (en radianes por segundo)
            float angle = deltaTime * angularSpeed; // Ángulo de rotación en radianes

            // Definir la dirección de la diagonal
            glm::vec3 diagonalDirection = glm::normalize(glm::vec3(1.0f, 1.0f, 1.0f));

            // Rotar el vector alrededor de la diagonal
            skyLight = glm::rotate(skyLight, angle, diagonalDirection);
        }
        

        #pragma endregion

        #pragma region CAMERA

        if (startFramesToWait <= 0)
        {
            //GetCursorPos
            glfwGetCursorPos(window, &xpos, &ypos);

            camOffsetX += lastMouseX - xpos;
            camOffsetY += lastMouseY - ypos;
            
            vec3 OldCameraPos = camera.entity.position;

            //Move
            camera.Inputs(window, lastMouseX, lastMouseY, camOffsetX, camOffsetY);

            //Gravity
            if (!camera.fly)
            {
                camera.entity.UpdateGravity();
            }

            vec3 finalCamPos = camera.entity.position;

            //Collisions
            AdjustEntityCollision(camera.entity, OldCameraPos, finalCamPos, 0.0f);

            //Footsteps
            camera.entity.UpdateFootSteps(OldCameraPos);

            //GetCursorPos
            glfwGetCursorPos(window, &xpos, &ypos);
            lastMouseX = xpos;
            lastMouseY = ypos;

            //Matrix
            camera.Matrix(70, 0.1f, 100.0f, shader, "u_camMatrix");
        }
        else
        {
            startFramesToWait--;
        }

        #pragma endregion

        #pragma region SELECT BLOCKS

        vec3 selectedBlock;
        bool breakSelectedBlock = false;

        selectedBlock = RayCastBlock(camera.entity.position, camera.Orientation, 10.0f).position;

        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
        {
            breakSelectedBlock = true;
        }

        #pragma endregion


        if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS)
        {
            Block posBlock = Block(25, vec3(floor(camera.entity.position.x + 2), floor(camera.entity.position.y), floor(camera.entity.position.z)), Color(), 1, shader, currentTextures, blocksMap);
            posBlock.addBlock(posBlock, blocksMap, Blocks, Chunks);
        }

//---------------------------RENDER---------------------------------------
       

        #pragma region RENDER BLOQUES Y TEXTURAS
        int dbg = 0;

        int textureCalls = 0;
        int createCalls = 0;

        for (int c = 0; c < Chunks.size(); c++)
        {
            for (int e = 0; e < Chunks[c].size(); e++)
            {
                bool renderChunk = false;

                for (int r = -ChunksRenderDistance; r <= ChunksRenderDistance; r++)
                {
                    for (int r2 = -ChunksRenderDistance; r2 <= ChunksRenderDistance; r2++)
                    {
                        if (camera.entity.position.x + ChunksSize * r > Chunks[c][e].position.x && camera.entity.position.x + ChunksSize * r2 < Chunks[c][e].position.x + Chunks[c][e].size &&
                            camera.entity.position.z + ChunksSize * r > Chunks[c][e].position.y && camera.entity.position.z + ChunksSize * r2 < Chunks[c][e].position.y + Chunks[c][e].size)
                        {
                            renderChunk = true;
                        }
                    }
                }

                if (renderChunk)
                {
                    //cout << "Chunk Camara: " << c << "  " << e << endl;

                    for (int i = 0; i < Chunks[c][e].blocks.size(); i++)
                    {
                        if (Chunks[c][e].blocks[i].size() > 0)
                        {
                            int slot = -1;
                            int slotTop = -1;
                            int slotBottom = -1;

                            //Este for se puede optimizar un poco para ganar un par de fps
                            for (int z = 0; z < currentTextures.size(); z++)
                            {
                                if (Chunks[c][e].blocks[i][0].TexturePath != "")
                                {
                                    if (currentTextures[z] == Chunks[c][e].blocks[i][0].TexturePath && Chunks[c][e].blocks[i][0].TexturePath != "")
                                    {
                                        slot = z;
                                    }

                                    if (currentTextures[z] == Chunks[c][e].blocks[i][0].TopTexturePath && Chunks[c][e].blocks[i][0].TopTexturePath != "")
                                    {
                                        slotTop = z;
                                    }

                                    if (currentTextures[z] == Chunks[c][e].blocks[i][0].BottomTexturePath && Chunks[c][e].blocks[i][0].BottomTexturePath != "")
                                    {
                                        slotBottom = z;
                                    }
                                }

                            }

                            for (int z = 0; z < currentTextures.size(); z++)
                            {
                                if (currentTextures[z] == "")
                                {
                                    if (slot == -1)
                                    {
                                        slot = z;
                                    }
                                    else if (slotTop == -1 && z != slot)
                                    {
                                        slotTop = z;
                                    }
                                    else if (slotBottom == -1 && z != slot && z != slotTop)
                                    {
                                        slotBottom = z;
                                    }
                                }
                            }

                            if (slot == -1) { slot = 0; }
                            if (slotTop == -1) { slotTop = 1; }
                            if (slotBottom == -1) { slotBottom = 2; }

                            

                            
                            if (Chunks[c][e].blocks[i][0].TexturePath != currentTextures[slot])
                            {
                                Texture texture(Chunks[c][e].blocks[i][0].TexturePath, slot, true);
                                texture.Bind(slot, true);

                                currentTextures[slot] = Chunks[c][e].blocks[i][0].TexturePath; textureCalls++;
                            }

                            if (Chunks[c][e].blocks[i][0].TopTexturePath != "" && Chunks[c][e].blocks[i][0].TopTexturePath != currentTextures[slotTop])
                            {
                                Texture texture2(Chunks[c][e].blocks[i][0].TopTexturePath, slotTop, true);
                                texture2.Bind(slotTop, true);

                                currentTextures[slotTop] = Chunks[c][e].blocks[i][0].TopTexturePath; textureCalls++;
                            }

                            if (Chunks[c][e].blocks[i][0].BottomTexturePath != "" && Chunks[c][e].blocks[i][0].BottomTexturePath != currentTextures[slotBottom])
                            {
                                Texture texture3(Chunks[c][e].blocks[i][0].BottomTexturePath, slotBottom, true);
                                texture3.Bind(slotBottom, true);

                                currentTextures[slotBottom] = Chunks[c][e].blocks[i][0].BottomTexturePath; textureCalls++;
                            }

                            
                            for (int u = 0; u < Chunks[c][e].blocks[i].size(); u++)
                            {
                                if (abs(camera.entity.position.y - Chunks[c][e].blocks[i][u].position.y) <= heightViewDistance)
                                {
                                    bool closeEnough = length(camera.entity.position - vec3(Chunks[c][e].blocks[i][u].position.x, Chunks[c][e].blocks[i][u].position.y, Chunks[c][e].blocks[i][u].position.z)) <= viewDistance;

                                    if (closeEnough && (length(camera.entity.position - vec3(Chunks[c][e].blocks[i][u].position.x, Chunks[c][e].blocks[i][u].position.y, Chunks[c][e].blocks[i][u].position.z)) <= 3 ||
                                        dot(vec3(Chunks[c][e].blocks[i][u].position.x, Chunks[c][e].blocks[i][u].position.y, Chunks[c][e].blocks[i][u].position.z) - camera.entity.position, camera.Orientation) > 0))
                                    {
                                        bool render = true;
                                        if (Chunks[c][e].blocks[i][u].position == selectedBlock)
                                        {
                                            Chunks[c][e].blocks[i][u].isSelected = true;

                                            if (breakSelectedBlock && candoonce)
                                            {
                                                //Chunks[c][e].blocks[i][u].Break();
                                                //Chunks[c][e].blocks[i][u].UpdateNeighbours();
                                                //Chunks[c][e].blocks[i].erase(Chunks[c][e].blocks[i].begin() + u);
                                                render = false;
                                                candoonce = false;
                                                selectedBlock = vec3(-999, -999, -999);
                                            }
                                        }

                                        if (render)
                                        {
                                            Chunks[c][e].blocks[i][u].Create();
                                            createCalls++;
                                        }   
                                    }
                                }   
                            }
                        }
                    }
                }
            } 

            
        }
        
        

        #pragma endregion  




//-------------------------FIN RENDER-------------------------------------
        

        #pragma region FIN DEL RENDER

        if (true)
        {
            //currentTextures.clear();

            if (maxTextureUnits > 30)
            {
                maxTextureUnits = 30;
            }

            for (int i = currentTextures.size(); i < maxTextureUnits; i++)
            {
                currentTextures.push_back("");
            }

            calculateFPS(glfwGetTime(), textureCalls, createCalls, true);
        }
        
        textureCalls = 0;
        createCalls = 0;

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();

        // Calcular el tiempo que tomó el fotograma actual
        auto endTime = std::chrono::steady_clock::now();
        auto elapsedTime = std::chrono::duration_cast<std::chrono::duration<double>>(endTime - startTime).count();

        // Si el tiempo del fotograma es menor al tiempo objetivo por fotograma, esperar
        if (elapsedTime < FRAME_TIME) {
            //std::this_thread::sleep_for(std::chrono::duration<double>(FRAME_TIME - elapsedTime));
        }

        #pragma endregion 
    }

    glDeleteProgram(shader);

    glfwTerminate();

    return 0;
}