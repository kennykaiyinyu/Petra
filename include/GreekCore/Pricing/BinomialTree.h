#ifndef GREEKCORE_BINOMIALTREE_H
#define GREEKCORE_BINOMIALTREE_H

#include <functional>

namespace GreekCore {

    enum class ExerciseType { European, American };

    struct TreeResult {
        double price;
        double delta;
        double gamma;
        double theta;
    };

    /**
     * @brief Cox-Ross-Rubinstein (CRR) Binomial Tree implementation.
     * Provides Price and Greeks (Delta, Gamma, Theta).
     */
    class BinomialTreePricer {
    public:
        [[nodiscard]]
        static TreeResult price(double S0, double r, double sigma, double T, 
                              int steps, std::function<double(double)> payoff, ExerciseType exercise);
    };
}
#endif // GREEKCORE_BINOMIALTREE_H
