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

        double mean = m_runningSum / m_pathsDone;
        results[0][0] = mean;

        double variance = (m_runningSumSq - mean * m_runningSum) / (m_pathsDone - 1);
        if (variance < 0.0) variance = 0.0;
        
        double stdErr = std::sqrt(variance / m_pathsDone);
        results[0][1] = stdErr;

        return results;
    }

    std::unique_ptr<StatisticsMC> StatisticsMean::clone() const {
        return std::make_unique<StatisticsMean>(*this);
    }

    // --- StatisticsConvergence Implementation ---

    StatisticsConvergence::StatisticsConvergence(const StatisticsMC& inner, const std::vector<unsigned long>& stoppingPoints)
        : m_inner(inner.clone()), 
          m_stoppingPoints(stoppingPoints), 
          m_pathsDone(0), 
          m_currentStoppingPointIndex(0) 
    {
    }

    StatisticsConvergence::StatisticsConvergence(const StatisticsConvergence& original)
        : m_inner(original.m_inner->clone()),
          m_resultsLog(original.m_resultsLog),
          m_stoppingPoints(original.m_stoppingPoints),
          m_pathsDone(original.m_pathsDone),
          m_currentStoppingPointIndex(original.m_currentStoppingPointIndex)
    {
    }

    void StatisticsConvergence::dumpOneResult(double result) {
        m_inner->dumpOneResult(result);
        m_pathsDone++;

        if (m_currentStoppingPointIndex < m_stoppingPoints.size() && 
            m_pathsDone == m_stoppingPoints[m_currentStoppingPointIndex]) {
            
            auto currentResults = m_inner->getResultsSoFar();
            // We store the results. Usually flattened, but here we just take the first row (Mean/Err)
            // Ideally we'd flatten the whole vector, but let's assume inner contributes one row.
            std::vector<double> entry;
            entry.push_back(static_cast<double>(m_pathsDone)); // Make it easy to read: [PathCount, Mean, Err]
            for (const auto& row : currentResults) {
                entry.insert(entry.end(), row.begin(), row.end());
            }

            m_resultsLog.push_back(entry);
            m_currentStoppingPointIndex++;
        }
    }

    std::vector<std::vector<double>> StatisticsConvergence::getResultsSoFar() const {
        std::vector<std::vector<double>> tmp(m_resultsLog);

        // Also append the current state if it wasn't a stopping point
        if (m_pathsDone > 0 && (m_resultsLog.empty() || m_resultsLog.back()[0] != m_pathsDone)) {
             auto currentResults = m_inner->getResultsSoFar();
             std::vector<double> entry;
             entry.push_back(static_cast<double>(m_pathsDone));
             for (const auto& row : currentResults) {
                 entry.insert(entry.end(), row.begin(), row.end());
             }
             tmp.push_back(entry);
        }
        return tmp;
    }

    std::unique_ptr<StatisticsMC> StatisticsConvergence::clone() const {
        return std::make_unique<StatisticsConvergence>(*this);
    }

}
