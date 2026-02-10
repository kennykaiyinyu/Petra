#include "GreekCore/Pricing/Parameters.h"

namespace GreekCore {

    // --- Constant Implementation ---
    ParametersConstant::ParametersConstant(double constant) 
        : m_constant(constant), m_constantSq(constant * constant) {}

    std::unique_ptr<ParametersInner> ParametersConstant::clone() const {
        return std::make_unique<ParametersConstant>(*this);
    }

    double ParametersConstant::integral(double time1, double time2) const {
        return (time2 - time1) * m_constant;
    }

    double ParametersConstant::integralSquare(double time1, double time2) const {
        return (time2 - time1) * m_constantSq;
    }

    // --- Bridge (Parameters) ---
    Parameters::Parameters(double constant) {
        m_inner = std::make_unique<ParametersConstant>(constant);
    }

    Parameters::Parameters(std::unique_ptr<ParametersInner>&& inner) 
        : m_inner(std::move(inner)) {}

    Parameters::Parameters(const Parameters& original) {
        if (original.m_inner)
            m_inner = original.m_inner->clone();
    }

    Parameters& Parameters::operator=(const Parameters& original) {
        if (this != &original) {
            if (original.m_inner)
                m_inner = original.m_inner->clone();
            else
                m_inner.reset();
        }
        return *this;
    }

    double Parameters::integral(double time1, double time2) const {
        return m_inner->integral(time1, time2);
    }

    double Parameters::integralSquare(double time1, double time2) const {
        return m_inner->integralSquare(time1, time2);
    }

    double Parameters::mean(double time1, double time2) const {
        double dt = time2 - time1;
        if (dt == 0) return 0.0; // Or instantaneous value?
        return integral(time1, time2) / dt;
    }

    double Parameters::rootMeanSquare(double time1, double time2) const {
        double dt = time2 - time1;
        if (dt == 0) return 0.0;
        return std::sqrt(integralSquare(time1, time2) / dt);
    }

}
