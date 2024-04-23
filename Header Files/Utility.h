#pragma once
#include <tuple>       // Para std::tuple
#include <functional>  // Para std::hash

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
};

// Función hash personalizada para std::tuple<int, int, int>
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