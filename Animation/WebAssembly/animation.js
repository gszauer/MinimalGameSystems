async function InitAnimation() {
	const { instance } = await WebAssembly.instantiateStreaming(
		fetch("./animation.wasm")
	);

	let numVerts = 12;
	let womanMesh = "";
	let bindState = "";
	let restState = "";
	let walkingData = "";
	let runningData = "";

	// TODO: Load data

	instance.exports.Initialize(numVerts);
	instance.exports.SetModelData(womanMesh);
	instance.exports.SetAnimationData(bindState, restState, walkingData, runningData);
	instance.exports.Update(0.0);

	return instance;
}

let Animation = InitAnimation();
Animation.exports.Update();