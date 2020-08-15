var gDownloadQueue = { };

var gProgram = null;
var gImports = {};
var gMemory = null;
var gExports = null;

const gUtf8Decoder = new TextDecoder("utf-8");
const gUtf8Encoder = new TextEncoder("utf-8");

const fileToLoad = "https://raw.githubusercontent.com/gszauer/MinimalGameSystems/master/Animation/Assets/char_vert.txt";

function StartToLoadFile(address) {
	if (gDownloadQueue[address] === undefined) {
		gDownloadQueue[address] = window.XMLHttpRequest ? new XMLHttpRequest() : new ActiveXObject("Microsoft.XMLHTTP");
		gDownloadQueue[address].IsFileDownloaded = false;
		gDownloadQueue[address].onreadystatechange = function() { 
			if (this.readyState == 4) {
				gDownloadQueue[address].IsFileDownloaded = true;
			} 
		};
		gDownloadQueue[address].open("GET", address);
		gDownloadQueue[address].send();
	}
}

function IsFileLoaded(address) {
	if (gDownloadQueue[address] !== undefined) {
		return gDownloadQueue[address].IsFileDownloaded? 1 : 0;
	}
	return 0;
}

function FileSize(address) {
	if (gDownloadQueue[address] !== undefined) {
		if (gDownloadQueue[address].responseText !== undefined) {
			return gDownloadQueue[address].responseText.length;
		}
	}
	return 0;
}

function FileContents(address) {
	if (gDownloadQueue[address] !== undefined) {
		if (gDownloadQueue[address].responseText !== undefined) {
			return gDownloadQueue[address].responseText;
		}
	}
	return "";
}

function JS_StartToLoadFile(address, stringLen) {
	let file = gUtf8Decoder.decode(gMemory.subarray(address, address+stringLen));
	StartToLoadFile(file);
}

function JS_IsFileLoaded(address, stringLen) {
	let file = gUtf8Decoder.decode(gMemory.subarray(address, address+stringLen));
	return IsFileLoaded(file);
}

function JS_FileSize(address, stringLen) {
	let file = gUtf8Decoder.decode(gMemory.subarray(address, address+stringLen));
	return FileSize(file);
}

function JS_FileCopyContents(address, addressLen, target) {
	let file = gUtf8Decoder.decode(gMemory.subarray(address, address+addressLen));
	let content = FileContents(file);

	let copyObject = gMemory.subarray(target, target+content.length)
	copyObject.set(gUtf8Encoder.encode(content));
}

function JS_JavascriptLog(message, stringLen) {
	let str = gMemory.subarray(message, message+stringLen);
	console.log(gUtf8Decoder.decode(str));
}

async function Initialize() {
	// Load WASM
	let request = await fetch('animation.wasm');
	let binary = await request.arrayBuffer();
	gImports.memory = new WebAssembly.Memory( 
		{'initial':32 * 4}  // 8MB
	);
	gImports['JS_StartToLoadFile'] = JS_StartToLoadFile;
	gImports['JS_IsFileLoaded'] = JS_IsFileLoaded;
	gImports['JS_FileSize'] = JS_FileSize;
	gImports['JS_FileCopyContents'] = JS_FileCopyContents;
	gImports['JS_JavascriptLog'] = JS_JavascriptLog;
	gMemory = new Uint8Array(gImports.memory.buffer);
	gProgram = await WebAssembly.instantiate( binary, { "env":gImports } );
	gExports = gProgram.instance.exports;

	// WASM is loaded fetch content and print result
	//console.log("JS: Fetching: " + fileToLoad);
	//StartToLoadFile(fileToLoad);

	// Do the same thing, but in C!
	gMemory[1029] = 'x';
	gExports["DownloadStart"]();


	// Update Window
	//window.setInterval(Loop, 16);
}

var printed = false;
function Loop() {
	// Print JS size
	if (!printed && IsFileLoaded(fileToLoad)) {
		let fileSize = FileSize(fileToLoad);
		let fileContent = FileContents(fileToLoad);
		console.log("JS: File is " + fileSize + " bytes.")
		console.log("JS: File content:\n" + fileContent);
		printed = true;
	}

	// Print C size
	gExports["DownloadLoop"]();
}

Initialize();