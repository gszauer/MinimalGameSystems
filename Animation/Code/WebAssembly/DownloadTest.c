void StartToLoadFile(const char* address);
int IsFileLoaded(const char* address);
int FileSize(const char* address);
void FileCopyContents(const char* address, char* target);
void JavascriptLog(const char* message);

char* WriteUInt(char* target, unsigned int v);
char* WriteFloat(char* target, float v);
unsigned int CountDigits(unsigned int n);

const char* gFileToDownload = 0;
int gIsLoaded = 0;

#define export __attribute__((visibility("default")))
#define import extern

extern unsigned char __heap_base;

unsigned int bump_pointer = (unsigned int) &__heap_base;

void* malloc(unsigned long n) {
  unsigned int r = bump_pointer;
  bump_pointer += n;
  return (void *)r;
}

void free(void* p) { }

export void DownloadStart() {
    gIsLoaded = 0;
    gFileToDownload = "https://raw.githubusercontent.com/gszauer/MinimalGameSystems/master/Animation/Assets/char_vert.txt";
	/*char printBuffer[512]; // should be dynamic maybe...

	char* print = printBuffer;
	const char* label = "C: Fetching: ";
	for (; (*print = *label); ++label, ++print); // StrCpy
	*print = '\n';
	print += 1;
	*print = '\0';*/
    JavascriptLog(gFileToDownload);
    JavascriptLog("test");
	//JavascriptLog(printBuffer);

	//StartToLoadFile(gFileToDownload);
}

export void DownloadLoop() {
    return;

	char printBuffer[1024];
	if (gIsLoaded == 0 && IsFileLoaded(gFileToDownload)) {
		gIsLoaded = 1;
		
		unsigned int fileSize = FileSize(gFileToDownload);
		char* print = printBuffer;
		const char* label = "C: File is ";
		for (; (*print = *label); ++label, ++print); // StrCpy
		print = WriteUInt(print, fileSize);
		label = "bytes.\n ";
		for (; (*print = *label); ++label, ++print); // StrCpy
		*print = '\0';
		JavascriptLog(printBuffer);

		print = printBuffer;
		label = "C: File content:\n";
		for (; (*print = *label); ++label, ++print); // StrCpy
		FileCopyContents(gFileToDownload, print); // Reads the file contents right into the print buffer
        print[fileSize] = '\0'; 
	}
}

unsigned int CountDigits(unsigned int n) {
    int length = 0;
    if (n == 0) {
        length = 1;
    }
    while (n != 0) {
        n = n / 10;
        ++length;
    }
    return length;
}

char* WriteFloat(char* target, float v) {
    if (v < 0) {
        *target = '-';
        target += 1;
        v = -v;
    }

    unsigned int whole = (unsigned int)v;
    target = WriteUInt(target, whole);

    // WriteUInt leaves a space at the end of the string
    // Go back one and replace it with the decimal
    target -= 1;
    *target = '.';
    target += 1;

    float part = v - (float)whole;

    for (unsigned int i = 0; i < 5; ++i) {
        part = part * 10.0f; // Move up one
        while (part >= 10.0f) { part -= 10.0f; }
        whole = (unsigned int)part; // Get Digit
        //if (whole <= 9) {
        *target = '0' + whole; // Since it's a single digit
        target += 1;
        //}
    }
    *target = ' ';
    target += 1;

    return target;
}

char* WriteUInt(char* target, unsigned int v) {
    if (v == 0) {
        *target = '0';
        target += 1;
    }
    else {
        unsigned int numDigits = CountDigits(v);
        char* write = target + numDigits - 1;

        while (v > 10) {
            unsigned int quotient = v / 10;
            unsigned int remainder = v % 10;

            *write = '0' + (char)remainder;
            write -= 1;

            v = quotient;
        }

        if (v == 10) {
            *write = '0';
            write -= 1;
            *write = '1';
        }
        else if (v != 0) {
            *write = '0' + (char)v;
        }
        target += numDigits;
    }

    *target = ' ';
    target += 1;

    return target;
}

import void JS_StartToLoadFile(int  address, int stringLen);
import int JS_IsFileLoaded(int  address, int stringLen);
import int JS_FileSize(int  address, int stringLen);
import void JS_FileCopyContents(int  address, int addressLen, int target);
import void JS_JavascriptLog(int  message, int stringLen);

int StrLen(const char* str) {
    const char* s;
    for (s = str; *s; ++s);
    return (int)(s - str);
}

void StartToLoadFile(const char* address) {
	JS_StartToLoadFile((int)address, StrLen(address));
}

int IsFileLoaded(const char* address) {
	return JS_IsFileLoaded((int)address, StrLen(address));
}

int FileSize(const char* address) {
	return JS_FileSize((int)address, StrLen(address));
}

void FileCopyContents(const char* address, char* target) {
	JS_FileCopyContents((int)address, StrLen(address), (int)target);
}

void JavascriptLog(const char* message) {
	JS_JavascriptLog((int)message, StrLen(message));
}