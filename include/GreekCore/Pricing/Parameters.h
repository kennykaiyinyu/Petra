#ifndef GREEKCORE_PARAMETERS_H
#define GREEKCORE_PARAMETERS_H

#include <memory>
#include <vector>
#include <cmath>

namespace GreekCore {

    // Abstract Implementation Class (The "Hidden" Interface)
    class ParametersInner {
    public:
        virtual ~ParametersInner() = default;
        virtual std::unique_ptr<ParametersInner> clone() const = 0;
        virtual double integral(double time1, double time2) const = 0;
        virtual double integralSquare(double time1, double time2) const = 0;
    };

    // Concrete Implementation: Constant Parameter
    class ParametersConstant : public ParametersInner {
        double m_constant;
        double m_constantSq;
    public:
        explicit ParametersConstant(double constant);
        std::unique_ptr<ParametersInner> clone() const override;
        double integral(double time1, double time2) const override;
        double integralSquare(double time1, double time2) const override;
    };

    // The Bridge Class (Value Semantics)
    // Users use THIS class. They pass it by value.
    class Parameters {
        std::unique_ptr<ParametersInner> m_inner;
    public:
        // Constructor for Constant (Implicit conversion allows Parameters p = 0.05)
        Parameters(double constant); 

        // Constructor for taking ownership of a custom inner implementation
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
