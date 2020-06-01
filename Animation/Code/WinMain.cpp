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

void LoadDataFromFile(Animation::Data& data, const char* file);
void LoadMeshFromFile(Mesh& mesh, const char* file);
void LoadBindPoseFromFile(Animation::State& state, const char* file);

// Global application state
Mesh gCharacterMesh;
vector<Animation::Data> gAnimationClips;
Animation::State gPoseA, gPoseB, gBlendedPose, gBindPose;
unsigned int gAnimationA, gAnimationB;
float gPlayTimeA, gPlayTimeB, gBlendFactor;

void Initialize() {
	LoadMeshFromFile(gCharacterMesh, "Assets/woman.anim");
	// Set pose A, B and Blend

	gAnimationClips.resize(5);
	LoadDataFromFile(gAnimationClips[0], "Assets/idle.anim");
	LoadDataFromFile(gAnimationClips[1], "Assets/run.anim");
	LoadDataFromFile(gAnimationClips[2], "Assets/walk.anim");

	LoadBindPoseFromFile(gBindPose, "Assets/woman.pose");
	gPoseA = gBindPose;
	gPoseB = gBindPose;
	gBlendedPose = gBindPose;

	// Play clips 0 and 1. Set blend factor to 1 to avoid the Update function automatically
	// switching animations. 
	gAnimationA = 0;
	gAnimationB = 1;
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
}

// This function increments the current animation index by 1. When the animation index
// is incremented, the two animations blend over 0.25 seconds. This should result in a 
// smooth transition. All of the blending logic is in Update, this function just sets
// some global states, it doens't actually do much work.
void SwitchToNextAnimation() {
	gAnimationA = gAnimationB;

	gAnimationB = gAnimationA + 1;
	if (gAnimationB >= (unsigned int)gAnimationClips.size()) {
		gAnimationB = 0;
	}

	gPlayTimeA = gPlayTimeB;
	gPlayTimeB = 0;
	gBlendFactor = 0.0f;
}

// To minimize the amount of OpenGL code that needs to be called, i've moved a lot
// of uniforms to be hard coded in the shader. For example, the entire model / view
// / projection pipeline is hard coded in the vertex shader. Lighting is hard coded 
// too. The mesh is CPU skinned using the Skinning function declared in AnimationSkin.h
void Render();

// Clean it all up!
void Shutdown();

///////////////////////////////////////////////////////////////////////////////////////
// Everything below this line is just WinMain + OpenGL loader. Nothing too interesting.
///////////////////////////////////////////////////////////////////////////////////////

int main(int argc, char** argv) {
	return 0;
}