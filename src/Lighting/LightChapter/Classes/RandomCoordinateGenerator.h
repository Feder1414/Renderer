//
// Created by USUARIO on 1/27/2026.
//

#ifndef GRAFICOS_RANDOMCOORDINATEGENERATOR_H
#define GRAFICOS_RANDOMCOORDINATEGENERATOR_H
#include <random>

template <typename RandomEngine, typename Distribution>
class RandomCoordinateGenerator
{
public:
    RandomCoordinateGenerator(float minVal, float maxVal)
    {
        m_randomEngine = RandomEngine(rd());
        SetRange(minVal, maxVal);
    };

    void SetRange(float minVal, float maxVal)
    {
        if (minVal > maxVal)
        {
            m_minVal = maxVal;
            m_maxVal = minVal;
            SetRandDistribution();
            return;
        }
        m_minVal = minVal;
        m_maxVal = maxVal;
        SetRandDistribution();
    };

    void SetRandDistribution()
    {
        m_distribution = Distribution(m_minVal, m_maxVal);
    };

    typename Distribution::result_type GetRandom()
    {
        return m_distribution(m_randomEngine);
    };

private:
    std::random_device rd;
    RandomEngine m_randomEngine;
    Distribution m_distribution;


    float m_minVal;
    float m_maxVal;
};


#endif //GRAFICOS_RANDOMCOORDINATEGENERATOR_H
