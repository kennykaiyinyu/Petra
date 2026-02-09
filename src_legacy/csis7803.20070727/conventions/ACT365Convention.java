package conventions;

import java.util.Calendar;

import toolkit.ElapsedTimeCalculator;


public class ACT365Convention extends DayCountConvention {

	private static ACT365Convention soleInstance;
	
	private ACT365Convention() {
	}
	
	public static ACT365Convention getInstance() {
		if (soleInstance == null)
			soleInstance = new ACT365Convention();
		return soleInstance;
	}
	
	
	public double deltaTInBetween(Calendar c1, Calendar c2) {
		ElapsedTimeCalculator et = ElapsedTimeCalculator.getInstance();
		int numberOfDays = et.getDays(c1, c2);
		return numberOfDays / 365.0;
	}

}
