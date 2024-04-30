#pragma once
#include <iostream>
#include <chrono>


class Timer
{
private:

	std::chrono::steady_clock::time_point startTime;


public:

	float speed = 1;

	void StartTimer()
	{
		startTime = std::chrono::steady_clock::now();
	}

	void ResetTimer()
	{
		StartTimer();
	}

	float time()
	{
		auto currentTime = std::chrono::steady_clock::now();
		std::chrono::duration<float> elapsedTime = currentTime - startTime;
		return elapsedTime.count() / speed;
	}

	Timer(bool autoStart = true)
	{
		StartTimer();
	}
};