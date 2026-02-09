package numericalmethods;

public interface NumericalMethod {
	public double compute(MathFunction function) throws FailToConvergeException;
}
