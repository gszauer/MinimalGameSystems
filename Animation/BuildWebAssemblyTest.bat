copy /Y "C:\Users\gszauer\Documents\Git\MinimalGameSystems\Animation\Code\WebAssembly\DownloadTest.js" "C:\Users\gszauer\Documents\Git\MinimalGameSystems\Animation\WebAssembly\animation.js"

REM 8388608 = [8 * 1024 * 1024]
REM 	

clang^
	--target=wasm32^
	-O3^
	-flto^
	-nostdlib^
	-Wl,--no-entry^
	-Wl,--export-all^
	-Wl,--allow-undefined-file=Code/WebAssembly/DownloadTest.syms^
	-Wl,--lto-O3^
	-Wl,-z,stack-size=8388608^
	-o WebAssembly/animation.wasm Code/WebAssembly/DownloadTest.c