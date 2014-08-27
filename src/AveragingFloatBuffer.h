#pragma once

/**
 * Created by John-Michael on 8/4/2014.
 */
template <uint16_t N>
class AveragingFloatBuffer {

public:
    constexpr static const float LENGTH_AS_FLOAT = static_cast<float>(N);
    static const uint16_t LENGTH = N;
private:
    float * mData;
    int mInsertPointer = 0;
    float mSum = 0;
    float mAverage = 0;

public:
    AveragingFloatBuffer() {
        mData = new float[N];
        for(uint16_t k=0;k<N;++k) {
            mData[k] = 0;
        }
    }
    ~AveragingFloatBuffer() {
        delete[] mData;
    }
    void add(float val) {
        mSum -= mData[mInsertPointer];
        mData[mInsertPointer++] = val;
        mInsertPointer = mInsertPointer % LENGTH;
        mSum += val;
        mAverage = mSum / LENGTH_AS_FLOAT;

    };
    float average() const {
        return mAverage;
    } ;
    float const* getData() const {
        return mData;
    };
};
