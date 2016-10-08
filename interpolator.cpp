#include "interpolator.h"

Interpolator::Interpolator()
{
	return;
}

Interpolator::~Interpolator()
{
	return;
}

//returns "Done" conditional
bool Interpolator::interpolate_linear(Interpolate_Info* info, float dt)
{
	if (info->current_state.size() != info->desired_state.size())
	{
		return true;
	}

	int dimm = info->current_state.size()*4;
	float* current = &info->current_state[0].x;
	float* desired = &info->desired_state[0].x;

	float duration = info->duration;

	for (int i = 0; i < dimm; i++)
	{
		current[i] += dt*(desired[i] - current[i]) / duration;
	}

	duration -= dt;

	info->duration = duration;

	if ( duration < 0.0f)
	{
		return true;
	}

	return false;
}