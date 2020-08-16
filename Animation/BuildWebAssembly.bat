if exist "WebAssembly" del /Y "WebAssembly"
md "WebAssembly"

copy /Y "Code\WebAssembly\WebAssembly.js"	"WebAssembly\WebAssembly.js"
copy /Y "Code\WebAssembly\math.js"			"WebAssembly\math.js"
copy /Y "Code\WebAssembly\graphics.js"		"WebAssembly\graphics.js"
copy /Y "Code\WebAssembly\animation.js"		"WebAssembly\animation.js"
copy /Y "Code\WebAssembly\sample.js"		"WebAssembly\sample.js"
copy /Y "Code\WebAssembly\index.html"		"WebAssembly\index.html"
copy /Y "Assets\webgl_vert.txt"				"WebAssembly\webgl_vert.txt"
copy /Y "Assets\webgl_frag.txt"				"WebAssembly\webgl_frag.txt"
copy /Y "Assets\Woman.png"					"WebAssembly\Woman.png"
copy /Y "Assets\webgl_mesh.txt"				"WebAssembly\webgl_mesh.txt"
copy /Y "Assets\webgl_skeleton.txt"			"WebAssembly\webgl_skeleton.txt"
copy /Y "Assets\webgl_walking.txt"			"WebAssembly\webgl_walking.txt"
copy /Y "Assets\webgl_running.txt"			"WebAssembly\webgl_running.txt"

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
	Code/AnimationSkin.cpp^
	Code/AnimationState.cpp^
	Code/WebAssembly/math.cpp^
	Code/WebAssembly/main.cpp

