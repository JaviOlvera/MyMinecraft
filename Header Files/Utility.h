#pragma once
#include <tuple>       // Para std::tuple
#include <functional>  // Para std::hash
#include <glm/glm.hpp>

/*class Vector3
{
public:
	float x = 0;
	float y = 0;
	float z = 0;

	Vector3(float x, float y, float z);
};*/

class Color
{
public:
	float r = 1.0f;
	float g = 1.0f;
	float b = 1.0f;
	float a = 1.0f;

	Color(float r, float g, float b, float a);
	Color() : r(1.0f), g(1.0f), b(1.0f), a(1.0f)
	{

	};
	Color(glm::vec4 color) : r(color.x), g(color.y), b(color.z), a(color.w)
	{

	};
	Color(glm::vec3 color, float w) : r(color.x), g(color.y), b(color.z), a(w)
	{

	};

	glm::vec3 toVec3()
	{
		return glm::vec3(r, g, b);
	}

	glm::vec4 toVec4()
	{
		return glm::vec4(r, g, b, a);
	}
};

// Funci�n hash personalizada para std::tuple<int, int, int>
struct TupleHash
{
    std::size_t operator()(const std::tuple<int, int, int>& t) const
    {
        auto hash1 = std::hash<int>{}(std::get<0>(t));
        auto hash2 = std::hash<int>{}(std::get<1>(t));
        auto hash3 = std::hash<int>{}(std::get<2>(t));
        return hash1 ^ hash2 ^ hash3;
    }
};