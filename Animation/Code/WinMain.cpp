#include "AnimationState.h"
#include "AnimationData.h"
#include "AnimationSkin.h"
#include "AnimationBlend.h"

#include <vector>
using std::vector;

struct Mesh {
	std::vector<float> vertices;
	std::vector<float> normals;
	std::vector<float> texCoords;
	std::vector<unsigned int> influences;
	std::vector<float> weights;
	std::vector<unsigned int> indices;
};

struct SkinnedMeshSampleState {
	// TODO: All of the global variables!
	// Maybe get rid of mesh struct and just store all of that in here
	// Probably make this into a class
};

void LoadDataFromFile(Animation::Data& data, const char* file);
void LoadMeshFromFile(Mesh& mesh, const char* file);
void LoadBindPoseFromFile(Animation::State& state, const char* file);
void DrawMesh(const float* model, const Mesh& mesh);

// Global application state (wrap into one big struct later!
Mesh gAnimatedMesh;
Mesh gOriginalMesh;

vector<Animation::Data> gAnimationClips;

Animation::State gAnimationStateA;
Animation::State gAnimationStateB;
Animation::State gAnimationBlend;
Animation::State gBindPose;
Animation::State gRestPose;

unsigned int gAnimationClipA;
unsigned int gAnimationClipB;

float gPlayTimeA;
float gPlayTimeB;
float gBlendFactor;

Animation::Skin::Descriptor<float, 3> gReadPositions;
Animation::Skin::Descriptor<float, 3> gReadNormals;
Animation::Skin::Descriptor<float, 3> gWritePositions;
Animation::Skin::Descriptor<float, 3> gWriteNormals;
Animation::Skin::Descriptor<unsigned int, 4> gInfluences;
Animation::Skin::Descriptor<float, 4> gWeights;

float* gAnimatedPalette; // TODO
float* gInvBindPalette; // TODO

// TODO: Blend logic is over complicated. Should be driven by some kind of easy to use waive. Maybe an ease in-out.

void Initialize() {
	LoadMeshFromFile(gOriginalMesh, "Assets/woman.mesh");
	gAnimatedMesh = gOriginalMesh;

	gReadPositions = Animation::Skin::Descriptor<float, 3>(&gOriginalMesh.vertices[0], gOriginalMesh.vertices.size());
	gWritePositions = Animation::Skin::Descriptor<float, 3>(&gAnimatedMesh.vertices[0], gAnimatedMesh.vertices.size());

	gReadNormals = Animation::Skin::Descriptor<float, 3>(&gOriginalMesh.normals[0], gOriginalMesh.normals.size());
	gWriteNormals = Animation::Skin::Descriptor<float, 3>(&gAnimatedMesh.normals[0], gAnimatedMesh.normals.size());

	gInfluences = Animation::Skin::Descriptor<unsigned int, 4>(&gOriginalMesh.influences[0], gOriginalMesh.influences.size());
	gWeights = Animation::Skin::Descriptor<float, 4>(&gOriginalMesh.weights[0], gOriginalMesh.weights.size());

	gAnimationClips.resize(5);
	LoadDataFromFile(gAnimationClips[0], "Assets/idle.anim");
	LoadDataFromFile(gAnimationClips[1], "Assets/run.anim");
	LoadDataFromFile(gAnimationClips[2], "Assets/walk.anim");

	LoadBindPoseFromFile(gBindPose, "Assets/bind.pose");
	LoadBindPoseFromFile(gRestPose, "Assets/rest.pose");
	gAnimationStateA = gRestPose;
	gAnimationStateB = gRestPose;
	gAnimationBlend = gRestPose;

	// Play clips 0 and 1. Set blend factor to 1 to avoid the Update function automatically
	// switching animations. 
	gAnimationClipA = 0;
	gAnimationClipB = 1;
	gBlendFactor = 1.0f;
}

// When blend factor is < 1, blending happens every frame. The blend factor is incremented
// every frame until the two animations are blended. When the blend factor reaches one, the
// update method will stop blending and only show the second animation. It will also stop
// sampling the first animation. The blend transition is 0.25 seconds long.
void Update(float deltaTime) {
	if (gBlendFactor < 1.0f) {

	}
	else {

	}

	Animation::Skin::Apply(gWritePositions, gReadPositions, 1.0f, gAnimatedPalette, gInvBindPalette, gInfluences, gWeights);
	Animation::Skin::Apply(gWriteNormals, gReadNormals, 0.0f, gAnimatedPalette, gInvBindPalette, gInfluences, gWeights);
}

// This function increments the current animation index by 1. When the animation index
// is incremented, the two animations blend over 0.25 seconds. This should result in a 
// smooth transition. All of the blending logic is in Update, this function just sets
// some global states, it doens't actually do much work.
void SwitchToNextAnimation() {
	gAnimationClipA = gAnimationClipB;

	gAnimationClipB = gAnimationClipA + 1;
	if (gAnimationClipB >= (unsigned int)gAnimationClips.size()) {
		gAnimationClipB = 0;
	}

	gPlayTimeA = gPlayTimeB;
	gPlayTimeB = 0;
	gBlendFactor = 0.0f;
}

// To minimize the amount of OpenGL code that needs to be called, i've moved a lot
// of uniforms to be hard coded in the shader. For example, the entire model / view
// / projection pipeline is hard coded in the vertex shader. Lighting is hard coded 
// too. The mesh is CPU skinned using the Skinning function declared in AnimationSkin.h
void Render(float aspect) {
	// DO setup shit

	float model[16] = { 0.0f };
	model[0] = model[5] = model[10] = model[15] = 1.0f;
	DrawMesh(model, gAnimatedMesh);

}

// Clean it all up!
void Shutdown() {
	// TODO
}

///////////////////////////////////////////////////////////////////////////////////////
// Everything below this line is just WinMain + OpenGL loader. Nothing too interesting.
///////////////////////////////////////////////////////////////////////////////////////

int main(int argc, char** argv) {
	return 0;
}