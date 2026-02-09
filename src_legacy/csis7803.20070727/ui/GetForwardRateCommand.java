/**
 * 
 */
package ui;
import interpolation.Interpolator;

import java.text.ParseException;
import java.text.SimpleDateFormat;
import java.util.Calendar;
import conventions.*;
import yieldcurve.core.*;

/**
 * @author kykyu
 *
 */
public class GetForwardRateCommand implements Command {
	public void execute(String args[]) throws CommandException {
		if (args.length != 2) {
			throw new CommandException("Usage: getforwardrate startDate endDate");
		}
		
		Session session = Session.getInstance();
		Interpolator interpolator = session.getInterpolator();
		//yieldcurve.core.YieldCurve yCurve = session.getYieldCurve();
		
		SimpleDateFormat formatter = session.getSimpleDateFormat();
		Calendar theDay = Calendar.getInstance();
		Calendar theDay2 = Calendar.getInstance();
		Calendar today = session.getStartDay();
		DiscountFactorCurve dfCurve = session.getDiscountFactorCurve();
		DayCountConvention dayCountConvention = session.getDayCountConvention();
		int compoundingFreq = session.getCompoundingFrequency();
		
		try {
			theDay.setTime( formatter.parse(args[0]) );  
			theDay.set(Calendar.MILLISECOND, 0);
			theDay.set(Calendar.SECOND, 0);
			theDay.set(Calendar.HOUR, 0);
			theDay.set(Calendar.MINUTE, 0);
			theDay2.setTime( formatter.parse(args[1]) );  
			theDay2.set(Calendar.MILLISECOND, 0);
			theDay2.set(Calendar.SECOND, 0);
			theDay2.set(Calendar.HOUR, 0);
			theDay2.set(Calendar.MINUTE, 0);
			//double t = Session.getInstance().getDayCountConvention().deltaTInBetween(Session.getInstance().getStartDay(), theDay);
			//double df = yieldcurve.core.RateCalculationToolkit.computeDiscountFactorOn(theDay, today, dfCurve, dayCountConvention, interpolator, compoundingFreq);
			
			double forwardRate = yieldcurve.core.RateCalculationToolkit.computeForwardRateIn(theDay, theDay2, today, dfCurve, dayCountConvention, interpolator, compoundingFreq); 
			forwardRate = Math.floor(forwardRate * 1000000.0 + 0.5) / 10000.0;
			System.out.println(formatter.format(theDay.getTime()) + " to " + formatter.format(theDay2.getTime()) + " : " + forwardRate + "%");
			
			//double fraRate = (  Math.floor(   (df0/df - 1) / deltaT1*1000000  +0.5)  ) / 10000.0;
			//System.out.println("FRA (" + fra.getSubType() + "), rate: " + fraRate + "%");
			
			
//			System.out.println(df);
		} catch (ParseException e){
			throw new CommandException(e.getMessage());
		} catch (NoRateInTheFutureException e) {
			throw new CommandException("The date in the future is too far away to have forward");
		} catch (NoRateAtTimeBeforeTodayException e) {
			throw new CommandException("The date is to early to have forward rate");
		}
	}
	
	public String getDescription() {
		return "get the forward rate";
	}
}