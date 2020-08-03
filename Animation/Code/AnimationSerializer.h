#ifndef _H_ANIMATIONSERIALIZER_
#define _H_ANIMATIONSERIALIZER_

namespace Animation {
    class Data;
    class State;

	namespace Serializer {
        unsigned int StringLengthUInt(unsigned int v);
        unsigned int StringLengthInt(int v);
        unsigned int StringLengthFloat(float v); // Will always have 5 decimals

        const char* ReadInt(const char* target, int& v);
        const char* ReadUInt(const char* target, unsigned int& v);
        const char* ReadFloat(const char* target, float& v);

        char* WriteUInt(char* target, unsigned int v);
        char* WriteInt(char* target, int v);
        char* WriteFloat(char* target, float v); // Will always have 5 decimals

        unsigned int CountDigits(unsigned int n);
        const char* IgnoreUntilNumber(const char* target);
        char* WriteNewLine(char* target);


        void SerializeData(char* out, const Data& in);
        void DeserializeData(Data& out, const char* in);
        unsigned int SerializedDataSize(const Data& in);

        void SerializeState(char* out, const State& in);
        void DeserializeState(State& out, const char* in);
        unsigned int SerializedStateSize(const State& in);
	}
}

#endif