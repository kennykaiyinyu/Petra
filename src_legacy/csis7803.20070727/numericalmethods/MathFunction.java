package numericalmethods;

public interface MathFunction {
	public double compute(double x0) throws UndefinedValueException;
	public double computeFirstDerivative(double x0) throws UndefinedValueException;
}
 