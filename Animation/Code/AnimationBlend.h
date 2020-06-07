#ifndef _H_ANIMATION_BLEND_
#define _H_ANIMATION_BLEND_

namespace Animation {
	class State;

	bool LinearBlend(State& target, const State& a, const State& b, float t);

	// TODO: UE4 blend
	// CalculateTargetVelocities
	// QuadraticBlend

	// TODO: Nearest Frame Blend
}

#endif