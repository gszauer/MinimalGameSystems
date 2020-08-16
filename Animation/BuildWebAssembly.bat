copy /Y "C:\Users\gszauer\Documents\Git\MinimalGameSystems\Animation\Code\WebAssembly\animation.js" "C:\Users\gszauer\Documents\Git\MinimalGameSystems\Animation\WebAssembly\animation.js"

clang^
	--target=wasm32^
	-fno-exceptions^
	-nostdlib^
	-Wl,--no-entry^
	-Wl,--export-all^
	-Wl,--allow-undefined-file=Code/WebAssembly/Animation.syms^
	-Wl,--import-memory^
	-Wl,-z,stack-size=4194304^
	-Wl,--export-dynamic^
	-o WebAssembly/animation.wasm^
	Code/AnimationBlend.cpp^
	Code/AnimationData.cpp^
	Code/WebAssembly/AnimationInternal.cpp^
	Code/AnimationSerializer.cpp^
	Code/AnimationBuilder.cpp^
	Code/AnimationState.cpp^
	Code/WebAssembly/math.cpp^
	Code/WebAssembly/main.cpp