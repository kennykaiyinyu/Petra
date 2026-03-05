#include "GreekCore/Numerics/Statistics.h"

namespace GreekCore {

    StatisticsMean::StatisticsMean() 
        : m_runningSum(0.0), m_runningSumSq(0.0), m_pathsDone(0) {}

    void StatisticsMean::dumpOneResult(double result) {
        m_pathsDone++;
        m_runningSum += result;
        m_runningSumSq += result * result;
    }

    std::vector<std::vector<double>> StatisticsMean::getResultsSoFar() const {
        std::vector<std::vector<double>> results(1);
        results[0].resize(2); // [Mean, StdErr]

        if (m_pathsDone == 0) return results;

        double mean = m_runningSum / m_pathsDone;
        results[0][0] = mean;

        double variance = (m_runningSumSq - mean * m_runningSum);
        if (m_pathsDone > 1) {
             variance /= (m_pathsDone - 1);
        } else {
             variance = 0.0;
        }

        if (variance < 0.0) variance = 0.0;
        
        double stdErr = std::sqrt(variance / m_pathsDone);
        results[0][1] = stdErr;

        return results;
    }

}
