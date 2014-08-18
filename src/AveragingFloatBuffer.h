#pragma once

/**
 * Created by John-Michael on 8/4/2014.
 */
template <int N>
class AveragingFloatBuffer {

public:
    constexpr static const float LENGTH_AS_FLOAT = static_cast<float>(N);
private:
    float mData[5] =  {0.f,0.f,0.f,0.f,0.f};
    int mInsertPointer = 0;
    float mSum = 0;
    float mAverage = 0;

public:
    void add(float val) {
        mSum -= mData[mInsertPointer];
        mData[mInsertPointer++] = val;
        mInsertPointer = mInsertPointer % N;
        mSum += val;
        mAverage = mSum / LENGTH_AS_FLOAT;

    };

    float average() {
        return mAverage;
    };
};
