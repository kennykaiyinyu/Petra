package ui;
import interpolation.Interpolator;
import conventions.DayCountConvention;
import java.util.*;

import montecarlo.*;
import ui.options.*;
import yieldcurve.core.NoRateAtTimeBeforeTodayException;
import yieldcurve.core.NoRateInTheFutureException;
public class PriceOptionCommand implements Command {
	
	
	public String getDescription() {
		return "prices an option by Monte Carlo Simulations";
	}
	
	public void execute(String args[]) throws CommandException{
		System.out.println("=====================================================");
		doThing(args); 
	}
	
	public void doThing(String args[]) throws CommandException {
		try {
			if (args.length != 4 && args.length != 5)
				throw new CommandException("Usage: priceoption optionName numOfTimeSteps numOfSimulations verbosePeriod [antithetic]");
			
				int verbosePeriod = Integer.parseInt(args[3]);
				if (verbosePeriod <= 0)
					throw new NumberFormatException();
		
			
			String optionName = args[0];
			System.out.println("Option name: " + optionName);
			Session session = Session.getInstance();
			SortedMap<String, ui.options.Option> options = session.getOptions().getOptions();
			if (!options.containsKey(optionName)) 
				throw new CommandException("The option " + optionName + " is not in the pricing engine.");
			Option theOption = options.get(optionName);
			String [] stockNames = theOption.getUnderlyingStockNames();
			
			int numberOfTimeSteps = Integer.parseInt(args[1]);
			int numberOfSimulations = Integer.parseInt(args[2]);
			
			System.out.print("Underlying Stock: ");
			for (int i = 0; i < stockNames.length ; i++) {
				if (!session.getStocks().containsKey(stockNames[i]))
					throw new CommandException("One underlying stock " + stockNames[i] + " is not in the pricing engine.");
				else {
					System.out.print(stockNames[i]);
				}
			}
			System.out.println();
			
			if (args.length == 5 && "antithetic".toLowerCase().equals(args[4]) == true) {
				System.out.println("Using antithetic numbers.");
				doPricing(true, theOption,  numberOfTimeSteps, numberOfSimulations, verbosePeriod);
			} else {
				System.out.println("Not using antithetic numbers.");
				doPricing(false, theOption,  numberOfTimeSteps, numberOfSimulations, verbosePeriod);
			}
		} catch (NumberFormatException e) {
			throw new CommandException("you must provide a positive verbosePeriod - how frequent a statistical message is print");
		}
	} 
	
	private void doPricing(boolean usingAntitheticPath, Option option, int numberOfTimeSteps, int numberOfSimulations, int verbosePeriod) throws CommandException {
		
		Session session = Session.getInstance();
		DayCountConvention dCon = session.getDayCountConvention();
		Calendar maturity = option.getMaturityDate();
		yieldcurve.core.YieldCurve yCurve = session.getYieldCurve();
		if (yCurve == null) {
			throw new ui.CommandException("Yield Curve is not generated");
		}

		
		
		double timeToMaturity = session.getDayCountConvention().deltaTInBetween(session.getStartDay(), maturity);
		System.out.println("Time to maturity (in years): "+ timeToMaturity);
		Interpolator interpolator = session.getInterpolator();
		System.out.println("CSV,numOfTimeStep,size,mean,sd,optionname,isAntithetic,Randomizer,speed(in 1-1000th millisec)");
		double df = 0;
		try {
			//System.out.println("mat" + maturity.getTime());
			//double interestRate = interpolator.getValueOn(maturity, yCurve);
			double interestRate = interpolator.getValueOn(maturity, yCurve, dCon);
			df = yieldcurve.core.RateCalculationToolkit.zToDf(interestRate, timeToMaturity, session.getCompoundingFrequency());

			
			double driftRate = yieldcurve.core.RateCalculationToolkit.computeContinuousRate(df, timeToMaturity);
			System.out.println("Drift Rate = " + driftRate);
			// for each stock
			String[] stockNames = option.getUnderlyingStockNames();
			montecarlo.Randomizer randomizer = session.getRandomizer();
			double deltaT = timeToMaturity / (0.0 + numberOfTimeSteps);
			
			SortedMap<String, Stock> allStock = Session.getInstance().getStocks();
			StockPath path = null;
		//	double[] epsilons = null;
			Map<String, StockPath> stockAndPaths = new TreeMap<String, StockPath>();
			Map<String, StockPath> stockAndAntitheticPaths =null;
			if (usingAntitheticPath)
			{
				stockAndAntitheticPaths = new TreeMap<String, StockPath>();
			}
			double sum = 0;
			double sumOfSquares = 0;
			int count = 0;
			double [] epsilons ;
			double dividedBy = Math.exp(driftRate * deltaT * numberOfTimeSteps);
			long start = Calendar.getInstance().getTime().getTime();
			for (int j = 0; j < numberOfSimulations; j++) {
				
				// clear all
				for (int i = 0; i < stockNames.length;i++) {
					Stock stock = allStock.get(stockNames[i]);
					double currentPrice = stock.getCurrentPrice();
					double risk = stock.getVolatility();
					//System.out.println("Volatility of Stock " + stockNames[i] + " = " + risk); 
					//if (j % 2 == 0) {
					path = montecarlo.MonteCarloSimulator.generatePath(currentPrice, driftRate, deltaT, risk, randomizer, numberOfTimeSteps);
					epsilons = path.getEpsilons();
					//}
					//else
					//	path = mc.MonteCarloSimulation.generateAntitheticPath(currentPrice, driftRate, deltaT, risk, epsilons, numberOfTimeSteps);
					
					stockAndPaths.put(stockNames[i], path); // parameters for the payoff function
					if (usingAntitheticPath) {
						StockPath antitheticPath = montecarlo.MonteCarloSimulator.generateAntitheticPath(currentPrice, driftRate, deltaT, risk, epsilons, numberOfTimeSteps);
						stockAndAntitheticPaths.put(stockNames[i], antitheticPath);
					}
						
				}
				
				
				double payoff = option.getPayoff(stockAndPaths);
				if (usingAntitheticPath) {
					payoff += option.getPayoff(stockAndAntitheticPaths);
					payoff /= 2.0;
				}
				double discountedPayoff = payoff / dividedBy;
				sum += discountedPayoff;
				sumOfSquares += discountedPayoff * discountedPayoff;
				count ++;
				
				if ((j+1) % verbosePeriod == 0) {
					long now = Calendar.getInstance().getTime().getTime();
					double speed = (now - start) / (0.0+verbosePeriod);
					printSummary(j+1, sum, sumOfSquares, dividedBy, option.getName(), usingAntitheticPath, randomizer, speed,numberOfTimeSteps );
					start = now;
					
				}
				
				
				stockAndPaths.clear();
			
			}
			//printSummary(count,sum, sumOfSquares, dividedBy, option.getName(), usingAntitheticPath, randomizer);
		} catch (NoRateInTheFutureException e) {
			throw new CommandException("no rate in the future");
			//e.printStackTrace();
		}catch (NoRateAtTimeBeforeTodayException e) {
			throw new CommandException("no rate before today");
		}
		
	}
	
	private void printSummary(int size, double sum, double sumOfSquares, double dividedBy, String optionName, boolean isAntithetic, Randomizer ran, double speed, int numOfTimeSteps) {
		//System.out.println("-------------------------------------------------------");
		double mean = sum / size;
		double stDev = Math.sqrt(sumOfSquares / size - mean * mean);
		java.io.PrintStream out = System.out;
		
		//out.println("Sample size = " + size);
		//out.println("Discounted Price (Call Price)        = " + discountedPrice);
		//out.println("Mean of sample payoffs               = " + mean);
		//out.println("Standard Deviation of sample payoffs = " + stDev);
		out.println("CSV," + numOfTimeSteps + ","+ size +","+ mean + "," + stDev +"," + optionName + "," + isAntithetic +"," + ran.getClass().getName() +"," + speed);
	}
	
	
}
