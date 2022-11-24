#pragma once

#include <random>
class Random{
    private:
        static std::mt19937 s_RandomEngine ;
        static std::uniform_int_distribution<std::mt19937::result_type> s_Distribution;
    public:
        static void Init(){
            s_RandomEngine.seed(std::random_device()());
        }

        static int Int(int lower, int upper){
            s_Distribution = std::uniform_int_distribution<std::mt19937::result_type>(lower, upper);
            return s_Distribution(s_RandomEngine);
        }
        static float Float()
        {
            s_Distribution = std::uniform_int_distribution<std::mt19937::result_type>(0, std::numeric_limits<uint32_t>::max());
            return (float)s_Distribution(s_RandomEngine) / (float)std::numeric_limits<uint32_t>::max() - 0.5;
        }

        static float FloatPositive()
        {
            s_Distribution = std::uniform_int_distribution<std::mt19937::result_type>(0, std::numeric_limits<uint32_t>::max());
            return (float)s_Distribution(s_RandomEngine) / (float)std::numeric_limits<uint32_t>::max();
        }

};

