#ifndef GREEKCORE_PARAMETERS_H
#define GREEKCORE_PARAMETERS_H

#include <memory>
#include <vector>
#include <cmath>

namespace GreekCore {

    /**
     * @brief Abstract base class for the Parameter implementation (Bridge Pattern).
     * 
     * Hides the actual parameter implementation (constant, piecewise constant, stochastic)
     * from the client code.
     */
    class ParametersInner {
    public:
        virtual ~ParametersInner() = default;
        virtual std::unique_ptr<ParametersInner> clone() const = 0;
        virtual double integral(double time1, double time2) const = 0;
        virtual double integralSquare(double time1, double time2) const = 0;
    };

    /**
     * @brief Concrete implementation for constant parameters.
     * 
     * Represents a parameter that does not change over time (e.g., constant volatility).
     */
    class ParametersConstant : public ParametersInner {
        double m_constant;
        double m_constantSq;
    public:
        explicit ParametersConstant(double constant);
        std::unique_ptr<ParametersInner> clone() const override;
        double integral(double time1, double time2) const override;
        double integralSquare(double time1, double time2) const override;
    };

    /**
     * @brief The Bridge Class for financial parameters.
     * 
     * Provides value semantics to the client while holding a pointer to the implementation.
     * This allows for easy swapping of parameter types (constant vs term structure)
     * without changing the client code (e.g., Monte Carlo engine).
     * 
     * Users pass this class by value. It manages the lifecycle of the inner implementation.
     */
    class Parameters {
        std::unique_ptr<ParametersInner> m_inner;
    public:
        /**
         * @brief Constructs a constant parameter.
         * Implicit conversion is allowed so users can pass `0.05` where a `Parameters` object is expected.
         */
        Parameters(double constant); 

        /**
         * @brief Constructs a parameter from a custom implementation.
         * Takes ownership of the provided inner implementation.
         */
        explicit Parameters(std::unique_ptr<ParametersInner>&& inner);
        
        // Rule of 5 (for deep copying the inner pointer)
        Parameters(const Parameters& original);
        Parameters& operator=(const Parameters& original);
        Parameters(Parameters&& original) = default;
        Parameters& operator=(Parameters&& original) = default;
        ~Parameters() = default;

        // The Interface
        double integral(double time1, double time2) const;
        double integralSquare(double time1, double time2) const;
        double mean(double time1, double time2) const;
        double rootMeanSquare(double time1, double time2) const;
    };

}
#endif // GREEKCORE_PARAMETERS_H
