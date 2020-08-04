# Sample Data

TODO: Write doc

```

Animation::State PoseToState(Pose& pose) {
	Animation::State result;
	result.Resize(pose.Size());

	for (unsigned int i = 0; i < pose.Size(); ++i) {
		result.SetParent(i, pose.GetParent(i));
		result.SetRelativePosition(i, pose.GetLocalTransform(i).position.v);
		result.SetRelativeRotation(i, pose.GetLocalTransform(i).rotation.v);
		result.SetRelativeScale(i, pose.GetLocalTransform(i).scale.v);
	}

	return result;
}

Animation::Builder::Track ConvertTrack(unsigned int joint, bool scale, VectorTrack& track) {
	Animation::Builder::Track result;

	result.SetJointID(joint);
	if (scale) {
		result.SetTarget(Animation::Data::Component::Scale);
	}
	else {
		result.SetTarget(Animation::Data::Component::Position);
	}

	result.Resize(track.Size());

	for (unsigned int frameIndex = 0; frameIndex < track.Size(); frameIndex++) {
		Animation::Builder::Frame& target = result[frameIndex];
		Frame<3>& source = track[frameIndex];

		target.time = source.mTime;

		for (int i = 0; i < 3; ++i) {
			target.in[i] = source.mIn[i];
			target.value[i] = source.mValue[i];
			target.out[i] = source.mOut[i];
		}
		target.in[3] = target.value[3] = target.out[3] = 0.0f;
	}

	if (track.GetInterpolation() == Interpolation::Constant) {
		for (unsigned int frameIndex = 0; frameIndex < track.Size(); frameIndex++) {
			Animation::Builder::Frame& target = result[frameIndex];

			for (int i = 0; i < 4; ++i) {
				target.in[i] = target.out[i] = Animation::Data::StepLimit + 1.0f;
			}
		}
	}
	else if (track.GetInterpolation() == Interpolation::Linear) {
		for (unsigned int frameIndex = 0; frameIndex < track.Size() - 1; frameIndex++) {
			Animation::Builder::Frame& thisFrame = result[frameIndex];
			Animation::Builder::Frame& nextFrame = result[frameIndex + 1];

			float frameDelta = nextFrame.time - thisFrame.time;
			for (int i = 0; i < 4; ++i) {
				float valueDelta = nextFrame.value[i] - thisFrame.value[i];
				float linearTangent = valueDelta / frameDelta;
				thisFrame.out[i] = nextFrame.in[i] = linearTangent;
			}
		}

		for (int i = 0; i < 4; ++i) {
			result[result.Size() - 1].out[i] = -result[0].out[i];
			result[0].in[i] = -result[result.Size() - 1].in[i];
		}
	}

	return result;
}

Animation::Builder::Track ConvertTrack(unsigned int joint, QuaternionTrack& track) {
	Animation::Builder::Track result;

	result.SetJointID(joint);
	result.SetTarget(Animation::Data::Component::Rotation);

	result.Resize(track.Size());

	for (unsigned int frameIndex = 0; frameIndex < track.Size(); frameIndex++) {
		Animation::Builder::Frame& target = result[frameIndex];
		Frame<4>& source = track[frameIndex];

		target.time = source.mTime;

		for (int i = 0; i < 4; ++i) {
			target.in[i] = source.mIn[i];
			target.value[i] = source.mValue[i];
			target.out[i] = source.mOut[i];
		}
	}

	if (track.GetInterpolation() == Interpolation::Constant) {
		for (unsigned int frameIndex = 0; frameIndex < track.Size(); frameIndex++) {
			Animation::Builder::Frame& target = result[frameIndex];

			for (int i = 0; i < 4; ++i) {
				target.in[i] = target.out[i] = Animation::Data::StepLimit + 1.0f;
			}
		}
	}
	else if (track.GetInterpolation() == Interpolation::Linear) {
		for (unsigned int frameIndex = 0; frameIndex < track.Size() - 1; frameIndex++) {
			Animation::Builder::Frame& thisFrame = result[frameIndex];
			Animation::Builder::Frame& nextFrame = result[frameIndex + 1];

			float frameDelta = nextFrame.time - thisFrame.time;
			for (int i = 0; i < 4; ++i) {
				float valueDelta = nextFrame.value[i] - thisFrame.value[i];
				float linearTangent = valueDelta / frameDelta;
				thisFrame.out[i] = nextFrame.in[i] = linearTangent;
			}
		}

		for (int i = 0; i < 4; ++i) {
			result[result.Size() - 1].out[i] = -result[0].out[i];
			result[0].in[i] = -result[result.Size() - 1].in[i];
		}
	}

	return result;
}

Animation::Data ClipToData(Clip& clip) {
	Animation::Builder::Clip result;

	std::string name = clip.GetName();
	result.SetName(name.c_str());

	unsigned int numTracks = 0;

	for (int trackIndex = 0; trackIndex < clip.Size(); ++trackIndex) {
		unsigned int joint = clip.GetIdAtIndex(trackIndex);
		TransformTrack& track = clip[joint];

		if (track.GetPositionTrack().Size() >= 2) {
			numTracks += 1;
		}
		if (track.GetRotationTrack().Size() >= 2) {
			numTracks += 1;
		}
		if (track.GetScaleTrack().Size() >= 2) {
			numTracks += 1;
		}
	}

	result.Reserve(numTracks);

	for (int trackIndex = 0; trackIndex < clip.Size(); ++trackIndex) {
		unsigned int joint = clip.GetIdAtIndex(trackIndex);
		TransformTrack& track = clip[joint];

		if (track.GetPositionTrack().Size() >= 2) {
			result.PushTrack(ConvertTrack(joint, false, track.GetPositionTrack()));
		}
		if (track.GetRotationTrack().Size() >= 2) {
			result.PushTrack(ConvertTrack(joint, track.GetRotationTrack()));
		}
		if (track.GetScaleTrack().Size() >= 2) {
			result.PushTrack(ConvertTrack(joint, true, track.GetScaleTrack()));
		}
	}

	return Animation::Builder::Convert(result);
}

unsigned int SerializedMeshStringSize(Mesh& mesh) {
	std::vector<vec3>& position = mesh.GetPosition();
	std::vector<vec3>& normal = mesh.GetNormal();
	std::vector<vec2>& texCoord = mesh.GetTexCoord();
	std::vector<vec4>& weights = mesh.GetWeights();
	std::vector<ivec4>& influences = mesh.GetInfluences();
	std::vector<unsigned int>& indices = mesh.GetIndices();

	unsigned int posSize = position.size();
	unsigned int normSize = normal.size();
	unsigned int texSize = texCoord.size();
	unsigned int weightSize = weights.size();
	unsigned int infSize = influences.size();
	unsigned int idxSize = indices.size();

	unsigned int result = 0;
	unsigned int space = 1;
	unsigned int newLine = 1;

	result += Animation::Serializer::StringLengthUInt(posSize) + space;
	result += newLine;
	for (unsigned int i = 0; i < posSize; ++i) {
		result += Animation::Serializer::StringLengthFloat(position[i].x) + space;
		result += Animation::Serializer::StringLengthFloat(position[i].y) + space;
		result += Animation::Serializer::StringLengthFloat(position[i].z) + space;
	}
	result += newLine;

	result += Animation::Serializer::StringLengthUInt(normSize) + space;
	result += newLine;
	for (unsigned int i = 0; i < normSize; ++i) {
		result += Animation::Serializer::StringLengthFloat(normal[i].x) + space;
		result += Animation::Serializer::StringLengthFloat(normal[i].y) + space;
		result += Animation::Serializer::StringLengthFloat(normal[i].z) + space;
	}
	result += newLine;

	result += Animation::Serializer::StringLengthUInt(texSize) + space;
	result += newLine;
	for (unsigned int i = 0; i < texSize; ++i) {
		result += Animation::Serializer::StringLengthFloat(texCoord[i].x) + space;
		result += Animation::Serializer::StringLengthFloat(texCoord[i].y) + space;
	}
	result += newLine;

	result += Animation::Serializer::StringLengthUInt(weightSize) + space;
	result += newLine;
	for (unsigned int i = 0; i < weightSize; ++i) {
		result += Animation::Serializer::StringLengthFloat(weights[i].x) + space;
		result += Animation::Serializer::StringLengthFloat(weights[i].y) + space;
		result += Animation::Serializer::StringLengthFloat(weights[i].z) + space;
		result += Animation::Serializer::StringLengthFloat(weights[i].w) + space;
	}
	result += newLine;

	result += Animation::Serializer::StringLengthUInt(infSize) + space;
	result += newLine;
	for (unsigned int i = 0; i < infSize; ++i) {
		result += Animation::Serializer::StringLengthUInt(influences[i].x) + space;
		result += Animation::Serializer::StringLengthUInt(influences[i].y) + space;
		result += Animation::Serializer::StringLengthUInt(influences[i].z) + space;
		result += Animation::Serializer::StringLengthUInt(influences[i].w) + space;
	}
	result += newLine;

	result += Animation::Serializer::StringLengthUInt(idxSize) + space;
	result += newLine;
	for (unsigned int i = 0; i < idxSize; ++i) {
		result += Animation::Serializer::StringLengthUInt(indices[i]) + space;
	}
	result += newLine;

	result += 1; // null term


	const char* str = "mIndices: mIndices.size(): mInfluences: mInfluences.size(): mWeights: mWeights.size(): mTexCoords: mTexCoords.size(): mNormals: mNormals.size(): mPositions: mPositions.size(): ";
	const char* s;
	for (s = str; *s; ++s);
	unsigned int strLen = (unsigned int)(s - str);
	result += strLen;

	return result;
}

void SerializeMesh(Mesh& mesh, char* output) {
	std::vector<vec3>& position = mesh.GetPosition();
	std::vector<vec3>& normal = mesh.GetNormal();
	std::vector<vec2>& texCoord = mesh.GetTexCoord();
	std::vector<vec4>& weights = mesh.GetWeights();
	std::vector<ivec4>& influences = mesh.GetInfluences();
	std::vector<unsigned int>& indices = mesh.GetIndices();

	unsigned int posSize = position.size();
	unsigned int normSize = normal.size();
	unsigned int texSize = texCoord.size();
	unsigned int weightSize = weights.size();
	unsigned int infSize = influences.size();
	unsigned int idxSize = indices.size();

	const char* message = "mPositions.size(): ";
	for (; *output = *message; ++message, ++output);
	output = Animation::Serializer::WriteUInt(output, posSize);
	output = Animation::Serializer::WriteNewLine(output);

	message = "mPositions: ";
	for (; *output = *message; ++message, ++output);
	for (unsigned int i = 0; i < posSize; ++i) {
		output = Animation::Serializer::WriteFloat(output, position[i].x);
		output = Animation::Serializer::WriteFloat(output, position[i].y);
		output = Animation::Serializer::WriteFloat(output, position[i].z);
	}
	output = Animation::Serializer::WriteNewLine(output);

	message = "mNormals.size(): ";
	for (; *output = *message; ++message, ++output);
	output = Animation::Serializer::WriteUInt(output, normSize);
	output = Animation::Serializer::WriteNewLine(output);

	message = "mNormals: ";
	for (; *output = *message; ++message, ++output);
	for (unsigned int i = 0; i < normSize; ++i) {
		output = Animation::Serializer::WriteFloat(output, normal[i].x);
		output = Animation::Serializer::WriteFloat(output, normal[i].y);
		output = Animation::Serializer::WriteFloat(output, normal[i].z);
	}
	output = Animation::Serializer::WriteNewLine(output);

	message = "mTexCoords.size(): ";
	for (; *output = *message; ++message, ++output);
	output = Animation::Serializer::WriteUInt(output, texSize);
	output = Animation::Serializer::WriteNewLine(output);

	message = "mTexCoords: ";
	for (; *output = *message; ++message, ++output);
	for (unsigned int i = 0; i < texSize; ++i) {
		output = Animation::Serializer::WriteFloat(output, texCoord[i].x);
		output = Animation::Serializer::WriteFloat(output, texCoord[i].y);
	}
	output = Animation::Serializer::WriteNewLine(output);

	message = "mWeights.size(): ";
	for (; *output = *message; ++message, ++output);
	output = Animation::Serializer::WriteUInt(output, weightSize);
	output = Animation::Serializer::WriteNewLine(output);

	message = "mWeights: ";
	for (; *output = *message; ++message, ++output);
	for (unsigned int i = 0; i < weightSize; ++i) {
		output = Animation::Serializer::WriteFloat(output, weights[i].x);
		output = Animation::Serializer::WriteFloat(output, weights[i].y);
		output = Animation::Serializer::WriteFloat(output, weights[i].z);
		output = Animation::Serializer::WriteFloat(output, weights[i].w);
	}
	output = Animation::Serializer::WriteNewLine(output);

	message = "mInfluences.size(): ";
	for (; *output = *message; ++message, ++output);
	output = Animation::Serializer::WriteUInt(output, infSize);
	output = Animation::Serializer::WriteNewLine(output);

	message = "mInfluences: ";
	for (; *output = *message; ++message, ++output);
	for (unsigned int i = 0; i < infSize; ++i) {
		output = Animation::Serializer::WriteUInt(output, influences[i].x);
		output = Animation::Serializer::WriteUInt(output, influences[i].y);
		output = Animation::Serializer::WriteUInt(output, influences[i].z);
		output = Animation::Serializer::WriteUInt(output, influences[i].w);
	}
	output = Animation::Serializer::WriteNewLine(output);

	message = "mIndices.size(): ";
	for (; *output = *message; ++message, ++output);
	output = Animation::Serializer::WriteUInt(output, idxSize);
	output = Animation::Serializer::WriteNewLine(output);
	
	message = "mIndices: ";
	for (; *output = *message; ++message, ++output);
	for (unsigned int i = 0; i < idxSize; ++i) {
		output = Animation::Serializer::WriteUInt(output, indices[i]);
	}
	output = Animation::Serializer::WriteNewLine(output);

	*output = '\0';
}


void Sample::Initialize() {
	cgltf_data* gltf = LoadGLTFFile("Assets/Woman.gltf");
	mMeshes = LoadMeshes(gltf);
	mSkeleton = LoadSkeleton(gltf);
	mClips = LoadAnimationClips(gltf);
	FreeGLTFFile(gltf);

	BoneMap bones = RearrangeSkeleton(mSkeleton);
	for (unsigned int i = 0, size = (unsigned int)mMeshes.size(); i < size; ++i) {
		RearrangeMesh(mMeshes[i], bones);
	}
	for (unsigned int i = 0, size = (unsigned int)mClips.size(); i < size; ++i) {
		RearrangeClip(mClips[i], bones);
	}

	Animation::State bindPose = PoseToState(mSkeleton.GetBindPose());
	Animation::State restPose = PoseToState(mSkeleton.GetRestPose());
	Animation::Data walkingClip = ClipToData(mClips[7]);
	Animation::Data runningClip = ClipToData(mClips[0]);

	std::ofstream out;

	char* outputBuffer = new char[Animation::Serializer::SerializedStateSize(bindPose)];
	Animation::Serializer::SerializeState(outputBuffer, bindPose);
	out.open("bindState.txt");
	out << outputBuffer;
	out.close();
	delete[] outputBuffer;

	outputBuffer = new char[Animation::Serializer::SerializedStateSize(restPose)];
	Animation::Serializer::SerializeState(outputBuffer, restPose);
	out.open("restState.txt");
	out << outputBuffer;
	out.close();
	delete[] outputBuffer;

	outputBuffer = new char[Animation::Serializer::SerializedDataSize(walkingClip)];
	Animation::Serializer::SerializeData(outputBuffer, walkingClip);
	out.open("walkingData.txt");
	out << outputBuffer;
	out.close();
	delete[] outputBuffer;

	outputBuffer = new char[Animation::Serializer::SerializedDataSize(runningClip)];
	Animation::Serializer::SerializeData(outputBuffer, runningClip);
	out.open("runningData.txt");
	out << outputBuffer;
	out.close();
	delete[] outputBuffer;

	outputBuffer = new char[SerializedMeshStringSize(mMeshes[0])];
	SerializeMesh(mMeshes[0], outputBuffer);
	out.open("womanMesh.txt");
	out << outputBuffer;
	out.close();
	delete[] outputBuffer;
```