package conventions;
import java.util.*;
import toolkit.ElapsedTimeCalculator;

//import java.util.Date;
public class SimpleNextBusinessDayConvention extends BusinessDayConvention {
	
	private static SimpleNextBusinessDayConvention soleInstance = null;
	
	public static SimpleNextBusinessDayConvention getInstance() {
		if (soleInstance == null)
			soleInstance = new SimpleNextBusinessDayConvention();
		return soleInstance;
	}
	
	
	public int getNumberOfDaysBetween(Calendar c1, Calendar c2) {
		ElapsedTimeCalculator elapsedTime = ElapsedTimeCalculator.getInstance();
		return elapsedTime.getDays(c1, c2);
	}
	
	private SimpleNextBusinessDayConvention() { }
	
	public  boolean isBusinessDay(Calendar theDay) {
		int weekDay = theDay.get(Calendar.DAY_OF_WEEK);
		if (weekDay == Calendar.SATURDAY || weekDay == Calendar.SUNDAY) {
			return false;
		}
		else return true;
	}
	
	public Calendar getNextBusinessDayOf(Calendar thisDay) {
		Calendar copyOfThisDay = (Calendar) thisDay.clone();
		int weekDay = copyOfThisDay.get(Calendar.DAY_OF_WEEK);
		if (weekDay == Calendar.SATURDAY)
			copyOfThisDay.add(Calendar.DATE, 2);
		else 
			copyOfThisDay.add(Calendar.DATE, 1);
		return copyOfThisDay;
	}
	
	public Calendar getBusinessDayAfter(Calendar from, int field, int amount) {
		Calendar to = (Calendar) from.clone();
		to.add(field, amount);
		
		if ( isBusinessDay(to) )
			return to;
		else 
			return getNextBusinessDayOf(to);
		
		//return to;
	}
	
	
	
}
