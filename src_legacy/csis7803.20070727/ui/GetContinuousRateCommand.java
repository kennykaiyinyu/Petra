package ui;
import yieldcurve.core.*;
import interpolation.*;
import java.util.*;
import java.text.*;
import conventions.DayCountConvention;
class GetContinuousRateCommand  implements Command{
	public String getDescription() {
		return "prints the continuous rate on a particular date";
	}
	public void execute(String args[]) throws CommandException {
		if (args.length != 1) {
			throw new CommandException("must supply the date as argument");
		}
		
		
		Session session = Session.getInstance();
		DayCountConvention dcConvention = session.getDayCountConvention();
		Interpolator interpolator = session.getInterpolator();
		//DiscountFactorCurve dfCurve = session.getDiscountFactorCurve();
		
		SimpleDateFormat formatter = new SimpleDateFormat("yyyy-MM-dd");
		Calendar theDay = Calendar.getInstance(); 
		try {
			theDay.setTime( formatter.parse(args[0]) );  
			YieldCurve yCurve = session.getYieldCurve();
			
			//double interest =  interpolator.getValueOn(theDay, yCurve) ;
			double interest =  interpolator.getValueOn(theDay, yCurve, dcConvention) ;
			Calendar startDay = session.getStartDay();
			double timeToMaturity = session.getDayCountConvention().deltaTInBetween(startDay, theDay);
			double df = RateCalculationToolkit.zToDf(interest, timeToMaturity, session.getCompoundingFrequency()); 
			System.out.print( "DF = " + df + " ==> ");
			
			System.out.println("Continuous rate = " + RateCalculationToolkit.computeContinuousRate(df, timeToMaturity));
		} catch (ParseException e){
			throw new CommandException(e.getMessage());
		} catch (NoRateInTheFutureException e) {
			throw new CommandException("no rate in the future");
		}catch (NoRateAtTimeBeforeTodayException e) {
			throw new CommandException("no rate before today");
		}
		
		
		
	}
	
	
}