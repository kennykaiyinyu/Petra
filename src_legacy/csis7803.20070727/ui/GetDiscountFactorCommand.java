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
public class GetDiscountFactorCommand implements Command {
	public void execute(String args[]) throws CommandException {
		if (args.length != 1) {
			throw new CommandException("must supply the date as argument");
		}
		
		Session session = Session.getInstance();
		Interpolator interpolator = session.getInterpolator();
		//yieldcurve.core.YieldCurve yCurve = session.getYieldCurve();
		
		SimpleDateFormat formatter = session.getSimpleDateFormat();
		Calendar theDay = Calendar.getInstance();
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
			//double t = Session.getInstance().getDayCountConvention().deltaTInBetween(Session.getInstance().getStartDay(), theDay);
			double df = yieldcurve.core.RateCalculationToolkit.computeDiscountFactorOn(theDay, today, dfCurve, dayCountConvention, interpolator, compoundingFreq);
			
			df = Math.ceil(df * 1000000.0) / 1000000.0;
			System.out.println(formatter.format(theDay.getTime()) + ","  + df);
			
//			System.out.println(df);
		} catch (ParseException e){
			throw new CommandException(e.getMessage());
		} catch (NoRateInTheFutureException e) {
			throw new CommandException("The date in the future is too far away to have discount factor");
		} catch (NoRateAtTimeBeforeTodayException e) {
			throw new CommandException("The date is to early to have discount factor");
		}
	}
	
	public String getDescription() {
		return "get the discount factor";
	}
}