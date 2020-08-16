#ifndef _H_VECTOR_
#define _H_VECTOR_

#include "../AnimationInternal.h"

#if !_WIN32
inline void * operator new (unsigned long, void * p)  {
	return p; 
}
#endif

template<typename T>
class Vector {
protected:
	T* mData;
	unsigned int mSize;
	unsigned int mCapacity;
public:
	inline Vector() {
		mData = 0;
		mSize = 0;
		mCapacity = 0;
	}

	inline Vector(const Vector& other) {
		mData = 0;
		mSize = 0;
		mCapacity = 0;
		*this = other;
	}

	inline Vector& operator=(const Vector& other) {
		if (this == &other) {
			return *this;
		}


		Reserve(other.mCapacity);
		mSize = other.mSize;
		for (unsigned int i = 0; i < mSize; ++i) {
			mData[i] = other.mData[i];			
		}

		return *this;
	}

	inline ~Vector() {
		Resize(0);
	}

	inline T& operator[](unsigned int index) {
		return mData[index];
	}

	const T& operator[](unsigned int index) const {
		return mData[index];
	}

	inline T& At(unsigned int index) {
		return mData[index];
	}

	const T& At(unsigned int index) const {
		return mData[index];
	}

	inline unsigned int Size() {
		return mSize;
	}

	inline unsigned int Capacity() {
		return mCapacity;
	}

	inline void Reserve(unsigned int newSize) {
		if (newSize > mCapacity) {
			T* newMemory = (T*)Animation::Internal::Allocate((sizeof(T)) * newSize);

			// Don't call constructors, the objects are just being moved
			for (unsigned int i = 0; i < mSize; ++i) {
				MemCpy(&newMemory[i], &mData[i], sizeof(T));
			}
			
			if (mData != 0) {
				// Don't call destructors, these objects don't know they are dying,
				// they have been moved, not really copied
				Animation::Internal::Free(mData);
			}

			mData = newMemory;
			mCapacity = newSize;
		}
	}

	inline void PushBack(const T& value) {
		if (mSize == mCapacity) {
			Reserve(mSize + mSize / 2 + 10);
		}

		mData[mSize++] = value;
	}

	inline void Resize(unsigned int newSize) {
		if (newSize == 0) { // Clear
			if (mData != 0) {
				for (unsigned int i = 0; i < mSize; ++i) {
					mData[i].~T();
				}
				Animation::Internal::Free(mData);
			}
			mData = 0;
			mSize = 0;
			mCapacity = 0;
			return;
		}
		else if (newSize <= mSize) { // Shrink
			for (unsigned int i = newSize; i < mSize; ++i) {
				mData[i].~T();
			}
			mSize = newSize;
		}
		else { // Grow
			unsigned int oldSize = mSize;
			Reserve(newSize);

			for (unsigned int i = oldSize; i < newSize; ++i) {
				new(&mData[i]) T();
			}

			mSize = newSize;
		}
	}
};

#endif