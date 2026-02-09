package ui;

import java.util.*;

import conventions.ACT365Convention;
import conventions.DayCountConvention;
import ui.options.*;


import montecarlo.VolatilityInCall;
import numericalmethods.FailToConvergeException;
import numericalmethods.NewtonMethod;
import ui.options.Stock;
import yieldcurve.core.*;

/**
 * @deprecated
 * @author kykyu
 *
 */
public class SimpleComputeVolatilityFromCallCommand implements Command {
	
	public String getDescription() {
		
		return "computes the volatility of a stock for its related call";
		
	}
	
	
	public void execute(String[] args) throws CommandException {

		if (args.length != 4) {
			throw new CommandException("Usage: volfromcall <callname> <percision> <first trial> <max-trial number>");
		}
		if (Session.getInstance().getDiscountFactorCurve() == null) {
			throw new CommandException("df/yield curve is not generated");
		}
		
		Session session = Session.getInstance();
		SortedMap<String, Option> options = session.getOptions().getOptions();
		String optionName = args[0];
		if (!options.containsKey(optionName)) {
			throw new CommandException ("Option " + optionName + " does not exist in the pricing engine.");
		}
		Option option = options.get(optionName);
		if (!(option instanceof StandardCall)) {
			throw new CommandException("Option " + optionName + " is not a standard call; volatility cannot be found by Black Scholes.");
		}
		java.util.SortedMap<String, Stock> allStocks = Session.getInstance().getStocks();
		
		
		StandardCall call = (StandardCall)option;
		
		String stockName = call.getTheUnderlyingStockName();//
		
		if (!allStocks.containsKey(stockName)) {
			throw new CommandException ("The underlying stock, " + stockName + " is not found in the pricing engine.");
		}
			
		Stock theStock = allStocks.get(stockName);
		double currentStockPrice = theStock.getCurrentPrice(); 
		
		double strikePrice = call.getStrikePrice();
		
		double callSellingPrice = call.getSellingPrice();
		
		double precision = Double.parseDouble(args[1]);
		double firstTrial = Double.parseDouble(args[2]);
		int maxTrial = Integer.parseInt(args[3]);
		
		DayCountConvention dcConvention = ACT365Convention.getInstance();
		Calendar today = Session.getInstance().getStartDay();
		
		//SimpleDateFormat formatter = new SimpleDateFormat("yyyy-MM-dd");
		Calendar date = new GregorianCalendar();
		
		try {
			date = (Calendar)call.getMaturityDate().clone();
			DiscountFactorCurve dfCurve = Session.getInstance().getDiscountFactorCurve();
			double theDf = Session.getInstance().getInterpolator().getValueOn(date, dfCurve);
			
			
			double deltaT = dcConvention.deltaTInBetween(today, date);
			double continuousRate = RateCalculationToolkit.computeContinuousRate(theDf, deltaT);
			NewtonMethod method = new numericalmethods.NewtonMethod();
			double vol = method.compute(new VolatilityInCall(currentStockPrice, strikePrice,callSellingPrice , continuousRate  ,deltaT), precision, firstTrial, maxTrial);
			theStock.setVolatility(vol);
			System.out.println("Volatility " + vol + " is assigned to the stock.");
	
		} catch (FailToConvergeException e) {
			throw new CommandException("cannot converge");
		} catch (NoRateInTheFutureException e) {
			throw new CommandException("no rate in future");
		}catch (NoRateAtTimeBeforeTodayException e) {
			throw new CommandException("no rate before today");
		}
	}
}
