package ui;

import java.util.Calendar;
import java.util.GregorianCalendar;
import java.text.*;

import conventions.ACT365Convention;
import conventions.DayCountConvention;


import montecarlo.VolatilityInCall;
import numericalmethods.FailToConvergeException;
import numericalmethods.NewtonMethod;
import ui.options.Stock;
import yieldcurve.core.*;
public class CopyOfComputeVolatilityFromCallCommand implements Command {
	public String getDescription() {
		return "compute the volatility of a stock by its put/call";
	}
	public void execute(String[] args) throws CommandException {

		if (args.length != 7) {
			throw new CommandException("Usage: volatility stockname <strike-pice> <call-selling price> <maturity date> <percision> <first trial> <max-trial> ");
		}
		if (Session.getInstance().getDiscountFactorCurve() == null) {
			throw new CommandException("df/yield curve is not generated");
		}
		java.util.SortedMap<String, Stock> allStocks = Session.getInstance().getStocks();
		String stockName = args[0];
		if (!allStocks.containsKey(stockName)) {
			throw new CommandException ("you must add this stock to the pricing engine first");
		}
		Stock theStock = allStocks.get(stockName);
		double currentStockPrice = theStock.getCurrentPrice(); 
		
		double strikePrice = Double.parseDouble(args[1]);
		double callSellingPrice = Double.parseDouble(args[2]);
		
		
		double precision = Double.parseDouble(args[4]);
		double firstTrial = Double.parseDouble(args[5]);
		int maxTrial = Integer.parseInt(args[6]);
		
		//BusinessDayConvention bdConvention = SimpleNextBusinessDayConvention.getInstance();
		DayCountConvention dcConvention = ACT365Convention.getInstance();
		Calendar today = Session.getInstance().getStartDay();
		
		SimpleDateFormat formatter = new SimpleDateFormat("yyyy-MM-dd");
		Calendar date = new GregorianCalendar();
		try {
			date.setTime( formatter.parse(args[3]) );
			DiscountFactorCurve dfCurve = Session.getInstance().getDiscountFactorCurve();
			//double theDf = Session.getInstance().getInterpolator().getValueOn(date, dfCurve);
			double theDf = Session.getInstance().getInterpolator().getValueOn(date, dfCurve, dcConvention);
			
			double deltaT = dcConvention.deltaTInBetween(today, date);
			double continuousRate = RateCalculationToolkit.computeContinuousRate(theDf, deltaT);
			NewtonMethod method = new numericalmethods.NewtonMethod();
			double vol = method.compute(new VolatilityInCall(currentStockPrice, strikePrice,callSellingPrice , continuousRate  ,deltaT), precision, firstTrial, maxTrial);
			theStock.setVolatility(vol);
			System.out.println("Volatility " + vol + " is assigned to the stock.");
			
		} catch (ParseException e) {
			throw new CommandException("cannot parse date ");
			
		} catch (FailToConvergeException e) {
			throw new CommandException("cannot converge");
		} catch (NoRateInTheFutureException e) {
			throw new CommandException("no rate in future");
		}catch (NoRateAtTimeBeforeTodayException e) {
			throw new CommandException("no rate before today");
		}
		
	}
}