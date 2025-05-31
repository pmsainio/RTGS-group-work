#include <vector>

class SequenceStrategy {

    private:
        float minInteronset;
        float maxInteronset;
        float density;

    public:
        SequenceStrategy(float minIO, float maxIO)
            : minInteronset(minIO), maxInteronset(maxIO) {}

        float frandom() {
            return static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
        }

        float nextDuration() {
            return minInteronset + frandom() * (maxInteronset - minInteronset);
        }

        float nextInteronset() {
            return minInteronset + frandom() * (maxInteronset - minInteronset);
        }
};
