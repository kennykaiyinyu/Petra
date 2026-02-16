#ifndef GREEKCORE_STATISTICS_H
#define GREEKCORE_STATISTICS_H

/**
 * @cite Based on "Statistics Gatherer" Pattern - Chapter 5.
 *       Joshi, M. S. (2008). "C++ Design Patterns and Derivatives Pricing".
 *
 * Refactored to use C++20 Concepts and Templates instead of Runtime Polymorphism.
 */

#include <vector>
#include <memory>
#include <cmath>
#include <mutex>
#include <concepts>

namespace GreekCore {

    /**
     * @brief Concept for a Statistics Gatherer
     * 
     * Types satisfying this concept can be used in the Monte Carlo engine.
     * This replaces the abstract base class 'StatisticsMC'.
     */
    template<typename T>
    concept StatisticsGatherer = requires(T t, double result) {
        { t.dumpOneResult(result) } -> std::same_as<void>;
        { t.getResultsSoFar() } -> std::same_as<std::vector<std::vector<double>>>;
    };

    /**
     * @brief Standard Mean and Standard Error Gatherer
     * 
     * Collects running sum and sum of squares to compute Mean and Standard Error.
     * Compatible with StatisticsGatherer concept.
     */
    class StatisticsMean {
    private:
        double m_runningSum;
        double m_runningSumSq;
        unsigned long m_pathsDone;

    public:
        StatisticsMean();
        
        void dumpOneResult(double result);
        std::vector<std::vector<double>> getResultsSoFar() const;
    };


    /**
     * @brief Convergence Table Decorator (Joshi Chapter 5)
     * 
     * Wraps another gatherer and snapshots its results at specified stopping points.
     * Demonstrates compile-time Decorator Pattern using Templates.
     */
    template<StatisticsGatherer InnerGatherer>
    class StatisticsConvergence {
    private:
        InnerGatherer m_inner; // Value semantics!
        std::vector<std::vector<double>> m_resultsLog;
        std::vector<unsigned long> m_stoppingPoints;
        unsigned long m_pathsDone;
        unsigned long m_currentStoppingPointIndex;

    public:
        StatisticsConvergence(const InnerGatherer& inner, const std::vector<unsigned long>& stoppingPoints)
            : m_inner(inner),
              m_stoppingPoints(stoppingPoints),
              m_pathsDone(0),
              m_currentStoppingPointIndex(0) 
        {}

        void dumpOneResult(double result) {
            m_inner.dumpOneResult(result);
            m_pathsDone++;

            if (m_currentStoppingPointIndex < m_stoppingPoints.size() && 
                m_pathsDone == m_stoppingPoints[m_currentStoppingPointIndex]) {
                
                auto currentResults = m_inner.getResultsSoFar();
                
                // Store [PathCount, Mean, Err, ...]
                std::vector<double> entry;
                entry.push_back(static_cast<double>(m_pathsDone));
                for (const auto& row : currentResults) {
                    entry.insert(entry.end(), row.begin(), row.end());
                }

                m_resultsLog.push_back(entry);
                m_currentStoppingPointIndex++;
            }
        }

        std::vector<std::vector<double>> getResultsSoFar() const {
            std::vector<std::vector<double>> tmp(m_resultsLog);

            // Also append the current state if it wasn't a stopping point
            if (m_pathsDone > 0 && (m_resultsLog.empty() || m_resultsLog.back()[0] != m_pathsDone)) {
                 auto currentResults = m_inner.getResultsSoFar();
                 std::vector<double> entry;
                 entry.push_back(static_cast<double>(m_pathsDone));
                 for (const auto& row : currentResults) {
                     entry.insert(entry.end(), row.begin(), row.end());
                 }
                 tmp.push_back(entry);
            }
            return tmp;
        }

        // Helper to access inner (e.g., for testing or further inspection)
        const InnerGatherer& getInner() const { return m_inner; }
    };

    /**
     * @brief Thread-Safe Decorator (Mutex Protected)
     *
     * Wraps any StatisticsGatherer with a mutex to ensure thread safety
     * when shared across threads.
     */
    template<StatisticsGatherer InnerGatherer>
    class StatisticsThreadSafe {
    private:
        InnerGatherer m_inner; // Value semantics/Exclusive Ownership
        mutable std::mutex m_mutex;

    public:
        // Construct by wrapping an existing gatherer (copy-construct)
        explicit StatisticsThreadSafe(const InnerGatherer& inner)
            : m_inner(inner) {}

        // Construct by moving
        explicit StatisticsThreadSafe(InnerGatherer&& inner)
            : m_inner(std::move(inner)) {}

        // Move constructor (mutex cannot be moved, start fresh)
        StatisticsThreadSafe(StatisticsThreadSafe&& other) noexcept
            : m_inner(std::move(other.m_inner)) {
        }
        
        // Deleted Copy Constructor (std::mutex is not copyable)
        StatisticsThreadSafe(const StatisticsThreadSafe&) = delete;
        StatisticsThreadSafe& operator=(const StatisticsThreadSafe&) = delete;

        void dumpOneResult(double result) {
            std::lock_guard<std::mutex> lock(m_mutex);
            m_inner.dumpOneResult(result);
        }

        std::vector<std::vector<double>> getResultsSoFar() const {
            std::lock_guard<std::mutex> lock(m_mutex);
            return m_inner.getResultsSoFar();
        }
    };

}

#endif // GREEKCORE_STATISTICS_H
