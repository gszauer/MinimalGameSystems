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

Pose StateToPose(Animation::State& state) {
	Pose result;
	result.Resize(state.Size());

	for (unsigned int i = 0; i < state.Size(); ++i) {
		result.SetParent(i, state.GetParent(i));
		result.SetLocalTransform(i, Transform(
			state.GetRelativePosition(i),
			state.GetRelativeRotation(i),
			state.GetRelativeScale(i)
			));
	}

	return result;
}

Animation::Data ClipToData(Clip& clip) {
	Animation::Data result;

	unsigned int frameSize = 0;
	unsigned int trackSize = (unsigned int)clip.mTracks.size() * 3 * 4;

	unsigned int* trackData = new unsigned int[trackSize];

	for (unsigned int i = 0; i < trackSize; i += 3 * 4) {
		TransformTrack& track = clip.mTracks[i / 12];
		unsigned int joint = track.GetId();

		trackData[i + 0] = joint; // Position.id
		trackData[i + 1] = 1; // Position.component
		trackData[i + 2] = frameSize; // Position.offset
		trackData[i + 3] = track.mPosition.Size(); // Position.numFrames
		frameSize += 10 * trackData[i + 3];

		trackData[i + 4] = joint; // Rotation.id
		trackData[i + 5] = 2; // Rotation.component
		trackData[i + 6] = frameSize; // Rotation.offset
		trackData[i + 7] = track.mRotation.Size(); // Rotation.numFrames
		frameSize += 13 * trackData[i + 7];

		trackData[i + 8] = joint; // Scale.id
		trackData[i + 9] = 4; // Scale.component
		trackData[i + 10] = frameSize; // Scale.offset
		trackData[i + 11] = track.mScale.Size(); // Scale.numFrames
		frameSize += 10 * trackData[i + 11];
	}

	float* frameData = new float[frameSize];
	unsigned int index = 0;

	for (unsigned int i = 0; i < trackSize; i += 3 * 4) {
		VectorTrack& pos = clip.mTracks[i / 12].mPosition;
		for (unsigned int j = 0; j < pos.mFrames.size(); ++j) {
			frameData[index++] = pos.mFrames[j].mTime;

			frameData[index++] = pos.mFrames[j].mIn[0];
			frameData[index++] = pos.mFrames[j].mIn[1];
			frameData[index++] = pos.mFrames[j].mIn[2];

			frameData[index++] = pos.mFrames[j].mValue[0];
			frameData[index++] = pos.mFrames[j].mValue[1];
			frameData[index++] = pos.mFrames[j].mValue[2];

			frameData[index++] = pos.mFrames[j].mOut[0];
			frameData[index++] = pos.mFrames[j].mOut[1];
			frameData[index++] = pos.mFrames[j].mOut[2];
		}

		QuaternionTrack& rot = clip.mTracks[i / 12].mRotation;
		for (unsigned int j = 0; j < pos.mFrames.size(); ++j) {
			frameData[index++] = rot.mFrames[j].mTime;

			frameData[index++] = rot.mFrames[j].mIn[0];
			frameData[index++] = rot.mFrames[j].mIn[1];
			frameData[index++] = rot.mFrames[j].mIn[2];
			frameData[index++] = rot.mFrames[j].mIn[3];

			frameData[index++] = rot.mFrames[j].mValue[0];
			frameData[index++] = rot.mFrames[j].mValue[1];
			frameData[index++] = rot.mFrames[j].mValue[2];
			frameData[index++] = rot.mFrames[j].mValue[3];

			frameData[index++] = rot.mFrames[j].mOut[0];
			frameData[index++] = rot.mFrames[j].mOut[1];
			frameData[index++] = rot.mFrames[j].mOut[2];
			frameData[index++] = rot.mFrames[j].mOut[3];
		}

		VectorTrack& scl = clip.mTracks[i / 12].mScale;
		for (unsigned int j = 0; j < pos.mFrames.size(); ++j) {
			frameData[index++] = scl.mFrames[j].mTime;

			frameData[index++] = scl.mFrames[j].mIn[0];
			frameData[index++] = scl.mFrames[j].mIn[1];
			frameData[index++] = scl.mFrames[j].mIn[2];

			frameData[index++] = scl.mFrames[j].mValue[0];
			frameData[index++] = scl.mFrames[j].mValue[1];
			frameData[index++] = scl.mFrames[j].mValue[2];

			frameData[index++] = scl.mFrames[j].mOut[0];
			frameData[index++] = scl.mFrames[j].mOut[1];
			frameData[index++] = scl.mFrames[j].mOut[2];
		}
	}

	result.SetRawData(frameData, frameSize, trackData, trackSize);
	result.SetLabel(clip.GetName().c_str());

	delete[] trackData;
	delete[] frameData;

	return result;
}

Clip DataToClip(Animation::Data& data) {
	Clip result;

	// Read in track data
	unsigned int trackDataSize = data.TrackDataSize();
	const unsigned int* track = data.GetTrackData();
	std::vector<unsigned int> allJoints;
	allJoints.resize(trackDataSize / 4);
	for (unsigned int i = 0; i < trackDataSize; i += 4) {
		allJoints[i / 4] = track[i];
	}

	const float* frames = data.GetFrameData();
	
	// Remove duplicates
	auto end = allJoints.end();
	for (auto it = allJoints.begin(); it != end; ++it) {
		end = std::remove(it + 1, end, *it);
	}
	allJoints.erase(end, allJoints.end());

	// Set size, name and looping
	//result.Resize(allJoints.size());
	result.SetName(data.GetLabel());
	result.SetLooping(true);

	// Loop trough every track
	for (unsigned int i = 0; i < trackDataSize / 4; ++i) {
		unsigned int id = track[i * 4 + 0];
		unsigned int component = track[i * 4 + 1];
		unsigned int offset = track[i * 4 + 2];
		unsigned int size = track[i * 4 + 3];

		if (component == 1) {
			VectorTrack& targetTrack = result[id].GetPositionTrack();
			targetTrack.Resize(size);
			targetTrack.SetInterpolation(Interpolation::Linear); 

			for (unsigned int j = 0; j < size; ++j) { // For each frame
				const float* frame = &frames[offset + j * 10];
				//memcpy(&targetTrack[j], frame, sizeof(float) * 10);
				targetTrack[j].mTime = *frame++;
				targetTrack[j].mIn[0] = *frame++;
				targetTrack[j].mIn[1] = *frame++;
				targetTrack[j].mIn[2] = *frame++;
				targetTrack[j].mValue[0] = *frame++;
				targetTrack[j].mValue[1] = *frame++;
				targetTrack[j].mValue[2] = *frame++;
				targetTrack[j].mOut[0] = *frame++;
				targetTrack[j].mOut[1] = *frame++;
				targetTrack[j].mOut[2] = *frame++;
			}
		}
		else if (component == 2) {
			QuaternionTrack& targetTrack = result[id].GetRotationTrack();
			targetTrack.Resize(size);
			targetTrack.SetInterpolation(Interpolation::Linear); 

			for (unsigned int j = 0; j < size; ++j) { // For each frame
				const float* frame = &frames[offset + j * 13];
				//memcpy(&targetTrack[j], frame, sizeof(float) * 13);
				targetTrack[j].mTime = *frame++;
				targetTrack[j].mIn[0] = *frame++;
				targetTrack[j].mIn[1] = *frame++;
				targetTrack[j].mIn[2] = *frame++;
				targetTrack[j].mIn[3] = *frame++;
				targetTrack[j].mValue[0] = *frame++;
				targetTrack[j].mValue[1] = *frame++;
				targetTrack[j].mValue[2] = *frame++;
				targetTrack[j].mValue[3] = *frame++;
				targetTrack[j].mOut[0] = *frame++;
				targetTrack[j].mOut[1] = *frame++;
				targetTrack[j].mOut[2] = *frame++;
				targetTrack[j].mOut[3] = *frame++;
			}
		}
		else if (component == 4) {
			VectorTrack& targetTrack = result[id].GetScaleTrack();
			targetTrack.Resize(size);
			targetTrack.SetInterpolation(Interpolation::Linear); 

			for (unsigned int j = 0; j < size; ++j) { // For each frame
				const float* frame = &frames[offset + j * 10];
				//memcpy(&targetTrack[j], frame, sizeof(float) * 13);
				targetTrack[j].mTime = *frame++;
				targetTrack[j].mIn[0] = *frame++;
				targetTrack[j].mIn[1] = *frame++;
				targetTrack[j].mIn[2] = *frame++;
				targetTrack[j].mValue[0] = *frame++;
				targetTrack[j].mValue[1] = *frame++;
				targetTrack[j].mValue[2] = *frame++;
				targetTrack[j].mOut[0] = *frame++;
				targetTrack[j].mOut[1] = *frame++;
				targetTrack[j].mOut[2] = *frame++;
			}
		}
		else {
			std::cout << "Unexpected component: " << component << "\n";
		}
	}


	result.RecalculateDuration();
	return result;
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

	result += Animation::UIntStringLength(posSize) + space;
	result += newLine;
	for (unsigned int i = 0; i < posSize; ++i) {
		result += Animation::FloatStringLength(position[i].x) + space;
		result += Animation::FloatStringLength(position[i].y) + space;
		result += Animation::FloatStringLength(position[i].z) + space;
	}
	result += newLine;

	result += Animation::UIntStringLength(normSize) + space;
	result += newLine;
	for (unsigned int i = 0; i < normSize; ++i) {
		result += Animation::FloatStringLength(normal[i].x) + space;
		result += Animation::FloatStringLength(normal[i].y) + space;
		result += Animation::FloatStringLength(normal[i].z) + space;
	}
	result += newLine;

	result += Animation::UIntStringLength(texSize) + space;
	result += newLine;
	for (unsigned int i = 0; i < texSize; ++i) {
		result += Animation::FloatStringLength(texCoord[i].x) + space;
		result += Animation::FloatStringLength(texCoord[i].y) + space;
	}
	result += newLine;

	result += Animation::UIntStringLength(weightSize) + space;
	result += newLine;
	for (unsigned int i = 0; i < weightSize; ++i) {
		result += Animation::FloatStringLength(weights[i].x) + space;
		result += Animation::FloatStringLength(weights[i].y) + space;
		result += Animation::FloatStringLength(weights[i].z) + space;
		result += Animation::FloatStringLength(weights[i].w) + space;
	}
	result += newLine;

	result += Animation::UIntStringLength(infSize) + space;
	result += newLine;
	for (unsigned int i = 0; i < infSize; ++i) {
		result += Animation::UIntStringLength(influences[i].x) + space;
		result += Animation::UIntStringLength(influences[i].y) + space;
		result += Animation::UIntStringLength(influences[i].z) + space;
		result += Animation::UIntStringLength(influences[i].w) + space;
	}
	result += newLine;

	result += Animation::UIntStringLength(idxSize) + space;
	result += newLine;
	for (unsigned int i = 0; i < idxSize; ++i) {
		result += Animation::UIntStringLength(indices[i]) + space;
	}
	result += newLine;

	result += 1; // null term

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

	output = Animation::WriteUInt(output, posSize);
	output = Animation::WriteNewLine(output);
	for (unsigned int i = 0; i < posSize; ++i) {
		output = Animation::WriteFloat(output, position[i].x);
		output = Animation::WriteFloat(output, position[i].y);
		output = Animation::WriteFloat(output, position[i].z);
	}
	output = Animation::WriteNewLine(output);

	output = Animation::WriteUInt(output, normSize);
	output = Animation::WriteNewLine(output);
	for (unsigned int i = 0; i < normSize; ++i) {
		output = Animation::WriteFloat(output, normal[i].x);
		output = Animation::WriteFloat(output, normal[i].y);
		output = Animation::WriteFloat(output, normal[i].z);
	}
	output = Animation::WriteNewLine(output);

	output = Animation::WriteUInt(output, texSize);
	output = Animation::WriteNewLine(output);
	for (unsigned int i = 0; i < texSize; ++i) {
		output = Animation::WriteFloat(output, texCoord[i].x);
		output = Animation::WriteFloat(output, texCoord[i].y);
	}
	output = Animation::WriteNewLine(output);

	output = Animation::WriteUInt(output, weightSize);
	output = Animation::WriteNewLine(output);
	for (unsigned int i = 0; i < weightSize; ++i) {
		output = Animation::WriteFloat(output, weights[i].x);
		output = Animation::WriteFloat(output, weights[i].y);
		output = Animation::WriteFloat(output, weights[i].z);
		output = Animation::WriteFloat(output, weights[i].w);
	}
	output = Animation::WriteNewLine(output);

	output = Animation::WriteUInt(output, infSize);
	output = Animation::WriteNewLine(output);
	for (unsigned int i = 0; i < infSize; ++i) {
		output = Animation::WriteUInt(output, influences[i].x);
		output = Animation::WriteUInt(output, influences[i].y);
		output = Animation::WriteUInt(output, influences[i].z);
		output = Animation::WriteUInt(output, influences[i].w);
	}
	output = Animation::WriteNewLine(output);

	output = Animation::WriteUInt(output, idxSize);
	output = Animation::WriteNewLine(output);
	for (unsigned int i = 0; i < idxSize; ++i) {
		output = Animation::WriteUInt(output, indices[i]);
	}
	output = Animation::WriteNewLine(output);

	*output = '\0';
}

void DeSerializeMesh(Mesh& mesh, const char* input) {
	std::vector<vec3>& position = mesh.GetPosition();
	std::vector<vec3>& normal = mesh.GetNormal();
	std::vector<vec2>& texCoord = mesh.GetTexCoord();
	std::vector<vec4>& weights = mesh.GetWeights();
	std::vector<ivec4>& influences = mesh.GetInfluences();
	std::vector<unsigned int>& indices = mesh.GetIndices();

	unsigned int posSize = 0;
	unsigned int normSize = 0;
	unsigned int texSize = 0;
	unsigned int weightSize = 0;
	unsigned int infSize = 0;
	unsigned int idxSize = 0;

	input = Animation::ReadUInt(input, posSize);
	position.resize(posSize);
	for (unsigned int i = 0; i < posSize; ++i) {
		input = Animation::ReadFloat(input, position[i].x);
		input = Animation::ReadFloat(input, position[i].y);
		input = Animation::ReadFloat(input, position[i].z);
	}

	input = Animation::ReadUInt(input, normSize);
	normal.resize(normSize);
	for (unsigned int i = 0; i < normSize; ++i) {
		input = Animation::ReadFloat(input, normal[i].x);
		input = Animation::ReadFloat(input, normal[i].y);
		input = Animation::ReadFloat(input, normal[i].z);
	}

	input = Animation::ReadUInt(input, texSize);
	texCoord.resize(texSize);
	for (unsigned int i = 0; i < texSize; ++i) {
		input = Animation::ReadFloat(input, texCoord[i].x);
		input = Animation::ReadFloat(input, texCoord[i].y);
	}

	input = Animation::ReadUInt(input, weightSize);
	weights.resize(weightSize);
	for (unsigned int i = 0; i < weightSize; ++i) {
		input = Animation::ReadFloat(input, weights[i].x);
		input = Animation::ReadFloat(input, weights[i].y);
		input = Animation::ReadFloat(input, weights[i].z);
		input = Animation::ReadFloat(input, weights[i].w);
	}

	input = Animation::ReadUInt(input, infSize);
	influences.resize(infSize);
	for (unsigned int i = 0; i < infSize; ++i) {
		unsigned int read = 0;
		input = Animation::ReadUInt(input, read);
		influences[i].x = (int)read;
		input = Animation::ReadUInt(input, read);
		influences[i].y = (int)read;
		input = Animation::ReadUInt(input, read);
		influences[i].z = (int)read;
		input = Animation::ReadUInt(input, read);
		influences[i].w = (int)read;
	}

	input = Animation::ReadUInt(input, idxSize);
	indices.resize(idxSize);
	for (unsigned int i = 0; i < idxSize; ++i) {
		input = Animation::ReadUInt(input, indices[i]);
	}

}
```