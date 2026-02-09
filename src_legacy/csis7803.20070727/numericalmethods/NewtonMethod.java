package numericalmethods;

public class NewtonMethod implements NumericalMethod {
	
	private static NewtonMethod soleInstance;
	
	public static NewtonMethod getInstance() {
		if (soleInstance == null)
			soleInstance = new NewtonMethod();
		return soleInstance;
	}
	
	// instance
	public double compute(MathFunction function, double tolerance, double initialGuess, int maxNumberOfGuess) throws FailToConvergeException{
		try {
			double x = initialGuess;
			int count = 0;
			while (Math.abs(function.compute(x)) > tolerance && count < maxNumberOfGuess) {
				x = computeNextX(function, x);
				count++;
			}
			if (count < maxNumberOfGuess) {
				System.out.println("Number of iterations = " + count);
				//return Math.floor(x / tolerance + 0.5) * tolerance;
				return x;
			}
			else 
				throw new FailToConvergeException();
		} catch (UndefinedValueException e) {
			throw new FailToConvergeException(e.getMessage());
		}
	}
	
	private double computeNextX(MathFunction function, double x) throws UndefinedValueException{
		return x - function.compute(x) / function.computeFirstDerivative(x); 
	}
	
	/**
	 * Default compute() method, which makes precision at 0.00001 and iterations at most 200. Initial guess is x = 0.
	 */
	public double compute(MathFunction function) throws FailToConvergeException{
		return compute(function, 0.00000000001, 0, 200);
	}
	
	public static void main(String args[]) throws Exception {
		MathFunction function = new MathFunction() {
			public double compute(double x) {
		    	return x*x + 2*x - 2;
			}
			
			public double computeFirstDerivative(double x) {
		    	return 2*x + 2;
			}
		};
		
		System.out.println(getInstance().compute(function));
		
	}
}





