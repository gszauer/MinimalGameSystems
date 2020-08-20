#ifndef _H_ANIMATION_BLEND_
#define _H_ANIMATION_BLEND_

namespace Animation {
	class State;

	// Lerp between two animations. Each joint is interpolated in local space.
	bool Blend(State& target, const State& a, const State& b, float t);
}

#endif