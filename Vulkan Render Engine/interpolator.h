#ifndef INTERPOLATOR_H
#define INTERPOLATOR_H

#include <vector>
#include <glm.hpp>

typedef struct Interpolate_Info
{
	std::vector<glm::vec4> current_state;
	std::vector<glm::vec4> desired_state;
	float duration;
}Interpolate_Info;

class Interpolator
{
public:
	Interpolator();
	~Interpolator();

	bool interpolate_linear(Interpolate_Info*,float);
};

#endif // !INTERPOLATOR_H
