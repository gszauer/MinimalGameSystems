#include "AnimationBlend.h"
#include "AnimationState.h"
#include "AnimationInternal.h"

bool Animation::Blend(State& target, const State& a, const State& b, float t) {
	if (a.Size() != b.Size() || target.Size() != a.Size()) {
		return false;
	}

	unsigned int numJoints = a.Size();
	float value_a[4] = { 0.0f };
	float value_b[4] = { 0.0f };
	float blend[4] = { 0.0f };

	for (unsigned int joint = 0; joint < numJoints; ++joint) {
		// Lerp between the local positions of both joints
		a.GetRelativePosition(joint, value_a);
		b.GetRelativePosition(joint, value_b);
		blend[0] = value_a[0] + (value_b[0] - value_a[0]) * t;
		blend[1] = value_a[1] + (value_b[1] - value_a[1]) * t;
		blend[2] = value_a[2] + (value_b[2] - value_a[2]) * t;
		target.SetRelativePosition(joint, blend);

		// Lerp between the local scales of both joints
		a.GetRelativeScale(joint, value_a);
		b.GetRelativeScale(joint, value_b);
		blend[0] = value_a[0] + (value_b[0] - value_a[0]) * t;
		blend[1] = value_a[1] + (value_b[1] - value_a[1]) * t;
		blend[2] = value_a[2] + (value_b[2] - value_a[2]) * t;
		target.SetRelativeScale(joint, blend);

		// NLERP (respecting neighborhood) between the local rotations of both joints
		a.GetRelativeRotation(joint, value_a);
		b.GetRelativeRotation(joint, value_b);
		float neighborhood = value_a[0] * value_b[0] + value_a[1] * value_b[1] + value_a[2] * value_b[2] + value_a[3] * value_b[3];
		if (neighborhood < 0.0f) {
			value_b[0] = -value_b[0];
			value_b[1] = -value_b[1];
			value_b[2] = -value_b[2];
			value_b[3] = -value_b[3];
		}

		blend[0] = value_a[0] + (value_b[0] - value_a[0]) * t;
		blend[1] = value_a[1] + (value_b[1] - value_a[1]) * t;
		blend[2] = value_a[2] + (value_b[2] - value_a[2]) * t;
		blend[3] = value_a[3] + (value_b[3] - value_a[3]) * t;
		float lenSquared = blend[0] * blend[0] + blend[1] * blend[1] + blend[2] * blend[2] + blend[3] * blend[3];
		if (!Animation::Internal::FloatCompare(lenSquared, 1.0f)) { // If length squared is not 1
			float inverseLength = Animation::Internal::InvSqrt(lenSquared);
			blend[0] *= inverseLength;
			blend[1] *= inverseLength;
			blend[2] *= inverseLength;
			blend[3] *= inverseLength;
		}
		target.SetRelativeRotation(joint, blend);
	}

	return true;
}