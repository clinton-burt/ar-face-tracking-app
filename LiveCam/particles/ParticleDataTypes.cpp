#include "particles/ParticleDataTypes.h"

std::mt19937 SalmonRandom::generator = std::mt19937(time(0));

int SalmonRandom::RandomInt(int a, int b)
{
	std::uniform_int_distribution<> interval(a, b);
	return interval(generator);
}

double SalmonRandom::RandomReal(double a, double b)
{
	std::uniform_real_distribution<> interval(a, b);
	return interval(generator);
}