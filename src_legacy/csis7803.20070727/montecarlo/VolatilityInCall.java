package montecarlo;
import java.util.*;

import conventions.*;

import ui.*;
import numericalmethods.*;
public class VolatilityInCall implements MathFunction {
	
	private static final double SQRT_OF_2_PI =  Math.sqrt(2 * Math.PI);
	private static final double beta = 0.2316419;
		
	private static final double a1 = 0.319381530;
	private static final double a2 = -0.356563782;
	private static final double a3 = 1.781477937;
	private static final double a4 = -1.821255978;
	private static final double a5 = 1.330274429;
	
	
	
	private static double firstDerivateOfN_Wrt_x(double x) {
		return Math.exp(- x * x / 2) / SQRT_OF_2_PI;
	}
	
	private static double N_of_x(double x) throws UndefinedValueException{
		if (x == Double.NaN) {
			throw new numericalmethods.UndefinedValueException("argument is NaN");
		}
		if (x >= 0)
			return 1 - firstDerivateOfN_Wrt_x(x) * (taylerSeries(x));
		else 
			return 1 - N_of_x(-x);
	}
	
	private static double k(double x) {
		return 1 / (1 + beta * x);
	}
	
	private static double taylerSeries(double x) {
		double k = k(x), term = k;
		double returnValue = a1 * term;
		term *= k;
		returnValue += a2 * term;
		term *= k;
		returnValue += a3 * term;
		term *= k;
		returnValue += a4 * term;
		term *= k;
		returnValue += a5 * term;
		return returnValue;
	}
	
	
	
	// instance
	private double currentStockPrice;
	private double strikePrice;
	private double callSellingPrice;
	private double rateOfReturn;
	private double timeToMaturity;
	
	public VolatilityInCall(double currentStockPrice, double strikePrice, double callSellingPrice, double rateOfReturn, double timeToMaturity) {
		this.currentStockPrice = currentStockPrice;
		this.strikePrice = strikePrice;
		this.callSellingPrice = callSellingPrice;
		this.rateOfReturn = rateOfReturn;
		this.timeToMaturity = timeToMaturity;
	}
	
	public double compute(double sigma) throws UndefinedValueException  {
		double sigmaSqrtT = sigma * Math.sqrt(this.timeToMaturity);
		double d1 =
			(Math.log(this.currentStockPrice / this.strikePrice)
			+ (this.rateOfReturn + sigma * sigma / 2.0) * this.timeToMaturity) / (sigmaSqrtT);
		double n_of_d1 = N_of_x(d1);
		double d2 = d1 - sigmaSqrtT;
		double n_of_d2 = N_of_x(d2);
		return -this.callSellingPrice + this.currentStockPrice * n_of_d1 - this.strikePrice * Math.exp(- this.rateOfReturn * this.timeToMaturity) * n_of_d2;
	}
	
	public double computeFirstDerivative(double sigma) {
		double sigmaSqrtT = sigma * Math.sqrt(this.timeToMaturity);
		double d1 =
			(Math.log(this.currentStockPrice / this.strikePrice)
			+ (this.rateOfReturn + sigma * sigma / 2.0) * this.timeToMaturity) / (sigmaSqrtT);
		return this.currentStockPrice * Math.sqrt(this.timeToMaturity) * firstDerivateOfN_Wrt_x(d1);
	}
	
	public static void main(String args[]) throws FailToConvergeException {
		DayCountConvention dcConvention = ACT365Convention.getInstance();
		Calendar today = Session.getInstance().getStartDay();
		
		double deltaT = dcConvention.deltaTInBetween(today, new GregorianCalendar(2009, 1, 16));  
		NewtonMethod method = new numericalmethods.NewtonMethod();
		System.out.println(method.compute(new VolatilityInCall(29.12,27.5,7, 0.04173862261167498  ,deltaT), 0.00001, 1, 200));
	}
}

