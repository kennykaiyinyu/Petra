package ui;

import java.util.*;

import conventions.*;
import yieldcurve.core.*;
import yieldcurve.instruments.*;
public class PriceBackCommand implements Command {

	public void execute(String[] args) throws CommandException {
		try {
			DayCountConvention dCon = Session.getInstance().getDayCountConvention();
			BusinessDayConvention bCon = Session.getInstance().getBusinessDayConvention();
			if (Session.getInstance().getYieldCurve() == null)
				throw new CommandException("the yield curve is not yet generated");
			DiscountFactorCurve dfCurve = Session.getInstance().getDiscountFactorCurve();
			Instruments instruments = Session.getInstance().getInstruments();
			SortedMap<Integer, Instrument> map = instruments.getAllInstruments();
			Iterator<Map.Entry<Integer, Instrument>> itr = map.entrySet().iterator();
			while (itr.hasNext()) {
				Map.Entry<Integer, Instrument>entry = itr.next();
				Instrument instrument = entry.getValue();
				if (instrument instanceof Cash) {
					Cash cash = (Cash ) instrument;
					TimeAmount after = cash.getLengthOfDeposit();
					Calendar today = Session.getInstance().getStartDay();
					Calendar endDate = Session.getInstance().getBusinessDayConvention().getBusinessDayAfter(today, after.FIELD, after.AMOUNT);
					
					double deltaT = Session.getInstance().getDayCountConvention().deltaTInBetween(today, endDate);
					
					//double zeroRate = Session.getInstance().getInterpolator().getValueOn(endDate, Session.getInstance().getYieldCurve());
					double zeroRate = Session.getInstance().getInterpolator().getValueOn(endDate, Session.getInstance().getYieldCurve(), dCon);
					double df = RateCalculationToolkit.zToDf(zeroRate, deltaT, Session.getInstance().getCompoundingFrequency());
					double depositRate = (1/df - 1)/ deltaT;
					depositRate = Math.floor(0.5 + depositRate * 100000)/1000.0;
					System.out.println("Cash (" + cash.getSubType() + "), rate: " + depositRate + "%");
					
				} else if (instrument instanceof FRA) {
					FRA fra = (FRA)instrument;
					TimeAmount start = fra.getStartDay();
					TimeAmount end = fra.getEndDay();
					Calendar today = Session.getInstance().getStartDay();
					
					
					Calendar fraMaturity = bCon.getBusinessDayAfter(today, end.FIELD, end.AMOUNT);
					
					Calendar fraStart = bCon.getBusinessDayAfter(today, start.FIELD, start.AMOUNT);
					double deltaT1 = dCon.deltaTInBetween(fraStart, fraMaturity);
					//System.out.println("deltaT1 = " + deltaT1);
					double deltaT0 = dCon.deltaTInBetween(today, fraStart);
					//java.text.SimpleDateFormat formatter = new java.text.SimpleDateFormat();
					//System.out.println("FRA start:" + formatter.format(fraStart.getTime()));
					//System.out.println("FRA end:" + formatter.format(fraMaturity.getTime()));
					//System.out.println("FRA end:" + formatter.format(today.getTime()));
					
					//double df0 = RateCalculationToolkit.zToDf( Session.getInstance().getInterpolator().getValueOn(fraStart, Session.getInstance().getYieldCurve()), deltaT0, Session.getInstance().getCompoundingFrequency() );//dfCurve.get(fraStart);
					double df0 = RateCalculationToolkit.zToDf( Session.getInstance().getInterpolator().getValueOn(fraStart, Session.getInstance().getYieldCurve(), dCon), deltaT0, Session.getInstance().getCompoundingFrequency() );//dfCurve.get(fraStart);
					
					//df0 = Math.ceil(df0 * 100000.0) / 100000.0;
					double df = dfCurve.get(fraMaturity);
					//df = Math.ceil(df*100000.0)/100000.0;
					
				//	double fraRate = fra.getRate();
					//YieldCurve yc = Session.getInstance().getYieldCurve();
					//double cashRate = yc.get(fraStart);
					double fraRate = (  Math.floor(   (df0/df - 1) / deltaT1*1000000  +0.5)  ) / 10000.0;
					System.out.println("FRA (" + fra.getSubType() + "), rate: " + fraRate + "%");
				
			
				} else if (instrument instanceof Swap) {
					
					Swap swap = (Swap) instrument;
					TimeAmount endDay = swap.getEndDay();
					TimeAmount period = swap.getPeriod();
					Calendar today = (Calendar)Session.getInstance().getStartDay().clone();
					double denominator = 0.0;
					int count = 1;
					double deltaT_i;
					Calendar someDay = null;
					Calendar oldDay = today;
					Calendar maturity = Session.getInstance().getBusinessDayConvention().getBusinessDayAfter(today, endDay.FIELD, endDay.AMOUNT);
					double deltaT = 0;
					while (true) {
			
						 someDay = Session.getInstance().getBusinessDayConvention().getBusinessDayAfter(today, period.FIELD, period.AMOUNT * count);
						if (someDay.after(maturity))
							break;
						count++;
						deltaT_i = Session.getInstance().getDayCountConvention().deltaTInBetween(oldDay, someDay);
						deltaT += deltaT_i;
						
						//double zeroRate = Session.getInstance().getInterpolator().getValueOn(someDay, Session.getInstance().getYieldCurve());
						double zeroRate = Session.getInstance().getInterpolator().getValueOn(someDay, Session.getInstance().getYieldCurve(), dCon);
						double df_i = RateCalculationToolkit.zToDf(zeroRate, deltaT, Session.getInstance().getCompoundingFrequency());
						denominator += df_i * deltaT_i;
						oldDay = someDay;
		
					}
					double df_n = Session.getInstance().getDiscountFactorCurve().get(maturity);
					double swapRate = (1 - df_n) /  denominator;
					swapRate = Math.floor(swapRate * 100000 + 0.5) / 1000.0;
					System.out.println("Swap (" + swap.getSubType() + "), rate: " + swapRate + "%");
				}
				
			}	
		} catch(NoRateInTheFutureException e) {
			e.printStackTrace();
		}catch (NoRateAtTimeBeforeTodayException e) {
			throw new CommandException("no rate before today");
		}
	}

	public String getDescription() {
		return "verify the yield curve by generating back the rates of instruments";
	}

}
