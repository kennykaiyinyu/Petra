package ui;
import yieldcurve.core.*;
import interpolation.*;
import java.util.*;
import java.text.*;
import conventions.DayCountConvention;
/**
 * @deprecated
 * @author kykyu
 *
 */
class GetDfOnDateCommand  implements Command{
	public String getDescription() {
		return "print the discount factor on some particular date";
	}
	public void execute(String args[]) throws CommandException {
		if (args.length != 1) {
			throw new CommandException("must supply the date as argument");
		}
		
		Session session = Session.getInstance();
		DayCountConvention dcConvention = session.getDayCountConvention();
		Interpolator interpolator = session.getInterpolator();
		yieldcurve.core.YieldCurve yCurve = session.getYieldCurve();
		
		SimpleDateFormat formatter = session.getSimpleDateFormat();
		Calendar theDay = Calendar.getInstance(); 
		try {
			theDay.setTime( formatter.parse(args[0]) );  
			theDay.set(Calendar.MILLISECOND, 0);
			theDay.set(Calendar.SECOND, 0);
			theDay.set(Calendar.HOUR, 0);
			theDay.set(Calendar.MINUTE, 0);
			double t = Session.getInstance().getDayCountConvention().deltaTInBetween(Session.getInstance().getStartDay(), theDay);
			//double df = yieldcurve.core.RateCalculationToolkit.zToDf(interpolator.getValueOn(theDay, yCurve), t, Session.getInstance().getCompoundingFrequency());
			double df = yieldcurve.core.RateCalculationToolkit.zToDf(interpolator.getValueOn(theDay, yCurve, dcConvention), t, Session.getInstance().getCompoundingFrequency());
			System.out.println(df);
		} catch (ParseException e){
			throw new CommandException(e.getMessage());
		} catch (NoRateInTheFutureException e) {
			throw new CommandException("no rate in the future");
		}catch (NoRateAtTimeBeforeTodayException e) {
			throw new CommandException("no rate before today");
		}
		
		
		
	}
	
	
}