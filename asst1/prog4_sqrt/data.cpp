#include <algorithm>

// Generate random data
void initRandom(float *values, int N) {
    for (int i=0; i<N; i++)
    {
        // random input values
        values[i] = .001f + 2.998f * static_cast<float>(rand()) / RAND_MAX;
    }
}

// Generate data that gives high relative speedup
void initGood(float *values, int N) {
    for (int i=0; i<N; i++)
    {
        // Todo: Choose values
        values[i] = 2.999f;
    }
}

// Generate data that gives low relative speedup
void initBad(float *values, int N) {
    for (int i=0; i<N; i++)
    {
        values[i] = 1.0f;

        //
        // min speedup from ISPC
        //

        //if(i % 8 == 0) values[i] = 2.999f;  

        /*

            [sqrt serial]:          [483.695] ms
            [sqrt ispc]:            [518.981] ms
            [sqrt task ispc]:       [18.148] ms
                                            (0.93x speedup from ISPC)
                                            (26.65x speedup from task ISPC)
        */
    }

    //
    // min speedup from task ISPC
    //

    for(int i = 0; i < N / 64; i ++) {
        if(i % 8 == 0) values[i] = 2.999f;
    }

    /*
        [sqrt serial]:          [36.856] ms
        [sqrt ispc]:            [24.352] ms
        [sqrt task ispc]:       [11.724] ms
                                        (1.51x speedup from ISPC)
                                        (3.14x speedup from task ISPC)
    */
}

