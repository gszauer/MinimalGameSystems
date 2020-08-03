#include "AnimationSerializer.h"
#include "AnimationInternal.h"
#include "AnimationState.h"
#include "AnimationData.h"

unsigned int Animation::Serializer::CountDigits(unsigned int n) {
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

unsigned int Animation::Serializer::StringLengthUInt(unsigned int v) {
    return CountDigits(v);
}

unsigned int Animation::Serializer::StringLengthInt(int v) {
    unsigned int length = 0;
    if (v < 0) {
        length = 1;
        v = -v;
    }
    unsigned int uv = (unsigned int)v;

    return CountDigits(uv) + length;;
}

unsigned int Animation::Serializer::StringLengthFloat(float v) {
    // Floating point numbers will always have 5 decimal places, hard coding that
    unsigned int length = 0;
    if (v < 0.0f) {
        length = 1;
        v = -v;
    }
    unsigned int uv = (unsigned int)v;
    length += CountDigits(uv) + 1 + 5;

    return length;
}

char* Animation::Serializer::WriteUInt(char* target, unsigned int v) {
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

char* Animation::Serializer::WriteInt(char* target, int v) {
    if (v < 0) {
        *target = '-';
        target += 1;
        v = -v;
    }
    unsigned int uv = (unsigned int)v;

    return WriteUInt(target, uv);
}

char* Animation::Serializer::WriteFloat(char* target, float v) {
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

char* Animation::Serializer::WriteNewLine(char* target) {
    *target = '\n';
    target += 1;
    return target;
}

const char* Animation::Serializer::IgnoreUntilNumber(const char* target) {
    char current = *target;

    while ((current < '0' || current > '9') && current != '-' && current != 0) {
        target += 1;
        current = *target;
    }

    return target;
}

const char* Animation::Serializer::ReadInt(const char* target, int& v) {
    target = IgnoreUntilNumber(target);
    bool negative = *target == '-';
    if (negative) {
        target += 1;
    }

    v = 0;
    char current = *target;
    while (current >= '0' && current <= '9') {
        v = (v * 10) + (current - '0');
        target += 1;
        current = *target;
    }

    if (negative) {
        v = -v;
    }

    return target;
}

const char* Animation::Serializer::ReadUInt(const char* target, unsigned int& v) {
    target = IgnoreUntilNumber(target);
    if (*target == '-') {
        target += 1;
    }

    v = 0;
    char current = *target;
    while (current >= '0' && current <= '9') {
        v = (v * 10) + (current - '0');
        target += 1;
        current = *target;
    }

    return target;
}

const char* Animation::Serializer::ReadFloat(const char* target, float& f) {
    target = IgnoreUntilNumber(target);
    bool negative = *target == '-';
    if (negative) {
        target += 1;
    }

    unsigned int whole = 0;
    target = ReadUInt(target, whole);
    float part = 0.0f;

    float shift[] = { 0.1f, 0.01f, 0.001f, 0.0001f, 0.00001f, 0.000001f };
    if (*target == '.') {
        target += 1;

        { // Start ReadUInt, but only 5 numbers.
            // First, make sure the next token is valid
            char current = *target;
            if (current >= '0' && current <= '9') {
                // Read up to 5 tokens
                for (unsigned int i = 0; i < 5; ++i) {
                    char current = *target;
                    // If token is valid, process it
                    if (current >= '0' && current <= '9') {
                        part += shift[i] * (current - '0');
                        target += 1; // increment target
                    }
                    else { // otherwise break
                        break;
                    }
                }
                // We read in 5 tokens, if there are more numbers, ignore them
                current = *target;
                while (current >= '0' && current <= '9') {
                    target += 1;
                    current = *target;
                }
            }
        } // End ReadUInt, but only 5 numbers.
    }

    f = (float)whole + part;
    if (negative) {
        f = -f;
    }

    return target;
}

void Animation::Serializer::SerializeData(char* output, const Data& data) {
    char* last = output;
    unsigned int written = 0;
    unsigned int delta = 0;
    
    unsigned int frameDataSize = data.FrameDataSize();
    const float* frameData = data.GetFrameData();

    unsigned int trackDataSize = data.TrackDataSize();
    const unsigned int* trackData = data.GetTrackData();

    const char* label = data.GetLabel();

    output = WriteUInt(output, frameDataSize);
    output = WriteNewLine(output);

    for (unsigned int i = 0; i < frameDataSize; ++i) {
        output = WriteFloat(output, frameData[i]);
    }
    output = WriteNewLine(output);

    output = WriteUInt(output, trackDataSize);
    output = WriteNewLine(output);
    for (unsigned int i = 0; i < trackDataSize; ++i) {
        output = WriteUInt(output, trackData[i]);
    }
    output = WriteNewLine(output);

    output = WriteFloat(output, data.GetStartTime());
    output = WriteFloat(output, data.GetEndtime());
    output = WriteNewLine(output);

    unsigned int labelLength = 0;
    if (label != 0) {
        const char* it = label;
        while (*it != '\0') {
            labelLength += 1;
            it += 1;
        }
    }
    output = WriteUInt(output, labelLength);

    for (unsigned int i = 0; i < labelLength; ++i) {
        *output = label[i];
        output += 1;
    }

    *output = '\0';
}

unsigned int Animation::Serializer::SerializedDataSize(const Data& data) {
    unsigned int size = 0;

    unsigned int frameDataSize = data.FrameDataSize();
    unsigned int trackDatasize = data.TrackDataSize();
    const float* frameData = data.GetFrameData();
    const unsigned int* trackData = data.GetTrackData();
    const char* label = data.GetLabel();

    size += StringLengthUInt(frameDataSize) + 1;
    size += 1;

    for (unsigned int i = 0; i < frameDataSize; ++i) {
        size += StringLengthFloat(frameData[i]) + 1;
    }
    size += 1;

    size += StringLengthUInt(trackDatasize) + 1;
    size += 1;
    for (unsigned int i = 0; i < trackDatasize; ++i) {
        size += StringLengthUInt(trackData[i]) + 1;
    }
    size += 1;

    size += StringLengthFloat(data.GetStartTime()) + 1;
    size += StringLengthFloat(data.GetEndtime()) + 1;
    size += 1;

    unsigned int labelLength = 0;
    if (label != 0) {
        const char* it = label;
        while (*it != '\0') {
            labelLength += 1;
            it += 1;
        }
    }
    size += StringLengthUInt(labelLength) + 1;

    for (unsigned int i = 0; i < labelLength; ++i) {
        size += 1;
    }

    size += 1;

    return size;
}

void Animation::Serializer::DeserializeData(Data& out, const char* input) {
    input = IgnoreUntilNumber(input);

    unsigned int frameDataSize = 0;
    input = ReadUInt(input, frameDataSize);
    float* frameData = (float*)Animation::Internal::Allocate(sizeof(float) * frameDataSize);
    for (unsigned int i = 0; i < frameDataSize; ++i) {
        input = ReadFloat(input, frameData[i]);
    }

    unsigned int trackDataSize = 0;
    input = ReadUInt(input, trackDataSize);
    unsigned int* trackData = (unsigned int*)Animation::Internal::Allocate(sizeof(unsigned int) * trackDataSize);
    for (unsigned int i = 0; i < trackDataSize; ++i) {
        input = ReadUInt(input, trackData[i]);
    }

    float startTime = 0.0f;
    float endTime = 0.0f;
    input = ReadFloat(input, startTime);
    input = ReadFloat(input, endTime);

    unsigned int labelLength = 0;
    input = ReadUInt(input, labelLength);
    char* label = (char*)Animation::Internal::Allocate(sizeof(char) * labelLength + 1);
    for (unsigned int i = 0; i < labelLength; ++i) {
        label[i] = *input;
        input += 1;
    }
    label[labelLength] = '\0';

    // Normalize vectors
    if (frameData != 0 && trackData != 0) {
        unsigned int trackStride = 4;
        // Loop trough all tracks
        for (unsigned int i = 0; i <trackDataSize; i += trackStride) {
            unsigned int component = trackData[i + 1];
            if (component == (unsigned int)Animation::Data::Component::Rotation) {
                unsigned int offset = trackData[i + 2];
                unsigned int size = trackData[i + 3];

                unsigned int frameDataStride = 13;

                for (int j = 0; j < size / frameDataStride; ++j) {
                    float* rot = &frameData[offset + (j * 13) + 1 + 4];

                    float rotLenSq = rot[0] * rot[0] + rot[1] * rot[1] + rot[2] * rot[2] + rot[3] * rot[3];
                    if (!Animation::Internal::FloatCompare(rotLenSq, 1.0f)) {
                        if (rotLenSq > 0.0f) {
                            float invRotLen = Animation::Internal::InvSqrt(rotLenSq);
                            rot[0] *= rotLenSq;
                            rot[1] *= rotLenSq;
                            rot[2] *= rotLenSq;
                            rot[3] *= rotLenSq;
                        }
                    }
                }
            }
        }
    }

    out.SetLabel(label);
    out.SetPointers(frameData, frameDataSize, trackData, trackDataSize);
    Animation::Internal::Free(label);
}

void Animation::Serializer::SerializeState(char* output, const State& state) {
    unsigned int size = state.Size();

    output = WriteUInt(output, size);

    for (unsigned int i = 0; i < size; ++i) {
        int parent = state.GetParent(i);
        output = WriteInt(output, parent);
    }
    output = WriteNewLine(output);

    for (unsigned int i = 0; i < size; ++i) {
        float pos[3], rot[4], scl[3];
        state.GetRelativePosition(i, pos);
        state.GetRelativeRotation(i, rot);
        state.GetRelativeScale(i, scl);

        output = WriteFloat(output, pos[0]);
        output = WriteFloat(output, pos[1]);
        output = WriteFloat(output, pos[2]);
        output = WriteFloat(output, rot[0]);
        output = WriteFloat(output, rot[1]);
        output = WriteFloat(output, rot[2]);
        output = WriteFloat(output, rot[3]);
        output = WriteFloat(output, scl[0]);
        output = WriteFloat(output, scl[1]);
        output = WriteFloat(output, scl[2]);
    }
    output = WriteNewLine(output);
    *output = '\0';
}

unsigned int Animation::Serializer::SerializedStateSize(const State& state) {
    unsigned int space = 1;
    unsigned int lineBreak = 1;
    unsigned int size = state.Size();

    unsigned int stringLength = StringLengthUInt(size) + space + lineBreak;

    for (unsigned int i = 0; i < size; ++i) {
        stringLength += StringLengthInt(state.GetParent(i)) + space;
    }
    stringLength += lineBreak;

    for (unsigned int i = 0; i < size; ++i) {
        float pos[3], rot[4], scl[3];
        state.GetRelativePosition(i, pos);
        state.GetRelativeRotation(i, rot);
        state.GetRelativeScale(i, scl);

        stringLength += StringLengthFloat(pos[0]) + space;
        stringLength += StringLengthFloat(pos[1]) + space;
        stringLength += StringLengthFloat(pos[2]) + space;
        stringLength += StringLengthFloat(rot[0]) + space;
        stringLength += StringLengthFloat(rot[1]) + space;
        stringLength += StringLengthFloat(rot[2]) + space;
        stringLength += StringLengthFloat(rot[3]) + space;
        stringLength += StringLengthFloat(scl[0]) + space;
        stringLength += StringLengthFloat(scl[1]) + space;
        stringLength += StringLengthFloat(scl[2]) + space;
    }
    stringLength += lineBreak;

    stringLength += 1; // null terminator

    return stringLength;
}

void Animation::Serializer::DeserializeState(State& out, const char* input) {
    unsigned int size = 0;
    input = ReadUInt(input, size);
    out.Resize(size);

    for (unsigned int i = 0; i < size; ++i) {
        int parent = 0;
        input = ReadInt(input, parent);
        out.SetParent(i, parent);
    }

    for (unsigned int i = 0; i < size; ++i) {
        float pos[3], rot[4], scl[3];

        input = ReadFloat(input, pos[0]);
        input = ReadFloat(input, pos[1]);
        input = ReadFloat(input, pos[2]);
        input = ReadFloat(input, rot[0]);
        input = ReadFloat(input, rot[1]);
        input = ReadFloat(input, rot[2]);
        input = ReadFloat(input, rot[3]);
        input = ReadFloat(input, scl[0]);
        input = ReadFloat(input, scl[1]);
        input = ReadFloat(input, scl[2]);

        float rotLenSq = rot[0] * rot[0] + rot[1] * rot[1] + rot[2] * rot[2] + rot[3] * rot[3];
        if (!Animation::Internal::FloatCompare(rotLenSq, 1.0f)) {
            if (rotLenSq > 0.0f) {
                float invRotLen = Animation::Internal::InvSqrt(rotLenSq);
                rot[0] *= rotLenSq;
                rot[1] *= rotLenSq;
                rot[2] *= rotLenSq;
                rot[3] *= rotLenSq;
            }
        }

        out.SetRelativePosition(i, pos);
        out.SetRelativeRotation(i, rot);
        out.SetRelativeScale(i, scl);
    }

    // Ignore null terminator
}