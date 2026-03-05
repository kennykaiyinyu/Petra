#ifndef GREEKCORE_BINOMIALTREE_H
#define GREEKCORE_BINOMIALTREE_H

#include <functional>

namespace GreekCore {

    enum class ExerciseType { European, American };

    /**
     * @brief A structure to hold the pricing results and Greek sensitivities from the Binomial Tree.
     */
    struct TreeResult {
        double price; ///< The fair value of the option.
        double delta; ///< Sensitivity to spot price ($\partial V / \partial S$).
        double gamma; ///< Sensitivity to delta ($\partial^2 V / \partial S^2$).
        double theta; ///< Sensitivity to time decay ($\partial V / \partial t$).
    };

    /**
     * @brief Cox-Ross-Rubinstein (CRR) Binomial Tree implementation.
     * 
     * Uses a recombining tree to price American and European options.
     * Calculates Greeks (Delta, Gamma, Theta) by examining the nodes at $t=0$ and $t=1$.
     * 
     * @tparam PayoffFunction Type of the payoff function (usually std::function<double(double)>).
     */
    class BinomialTreePricer {
    public:
        /**
         * @brief Prices an option using the CRR model.
         * 
         * @param S0 Initial spot price.
         * @param r Risk-free interest rate (annualized, continuously compounded).
         * @param sigma Volatility ($\sigma$).
         * @param T Time to maturity in years.
         * @param steps Number of time steps in the tree (N). Higher N -> better accuracy.
         * @param payoff Determining the payoff at maturity (or early exercise).
         * @param exercise Exercise style (European or American).
         * @return TreeResult containing price and major Greeks.
         */
        [[nodiscard]]
        static TreeResult price(double S0, double r, double sigma, double T, 
                              int steps, std::function<double(double)> payoff, ExerciseType exercise);
    };
}
#endif // GREEKCORE_BINOMIALTREE_H
