#ifndef GREEKCORE_STATISTICS_H
#define GREEKCORE_STATISTICS_H

#include <vector>
#include <memory>
#include <cmath>

namespace GreekCore {

    /**
     * @brief Statistics Gatherer Interface (Chapter 5)
     * 
     * Decouples the "Simulation" from the "Aggregation".
     * This allows us to add new metrics (VaR, Convergence Table) 
     * without touching the Monte Carlo engine.
     */
    class StatisticsMC {
    public:
        StatisticsMC() = default;
        virtual ~StatisticsMC() = default;

        // The engine calls this for every single path
        virtual void dumpOneResult(double result) = 0;

        // Return the matrix of results (e.g., [[Mean, Error]])
        virtual std::vector<std::vector<double>> getResultsSoFar() const = 0;

        // Support for cloning (Prototype Pattern)
        virtual std::unique_ptr<StatisticsMC> clone() const = 0;
    };

    /**
     * @brief Standard Mean and Standard Error Gatherer
     */
    class StatisticsMean : public StatisticsMC {
    private:
        double m_runningSum;
        double m_runningSumSq;
        unsigned long m_pathsDone;

    public:
        StatisticsMean();
        
        void dumpOneResult(double result) override;
        std::vector<std::vector<double>> getResultsSoFar() const override;
        std::unique_ptr<StatisticsMC> clone() const override;
    };

    /**
     * @brief Convergence Table Decorator (Joshi Chapter 5)
     * 
     * Wraps another gatherer and snapshots its results at specified stopping points.
     * Demonstrates the Decorator Pattern in action.
     */
    class StatisticsConvergence : public StatisticsMC {
    private:
        std::unique_ptr<StatisticsMC> m_inner;
        std::vector<std::vector<double>> m_resultsLog;
        std::vector<unsigned long> m_stoppingPoints;
        unsigned long m_pathsDone;
        unsigned long m_currentStoppingPointIndex;

    public:
        StatisticsConvergence(const StatisticsMC& inner, const std::vector<unsigned long>& stoppingPoints);
        StatisticsConvergence(const StatisticsConvergence& original); // Copy constructor for cloning

        void dumpOneResult(double result) override;
        std::vector<std::vector<double>> getResultsSoFar() const override;
        std::unique_ptr<StatisticsMC> clone() const override;
    };

}

#endif // GREEKCORE_STATISTICS_H
