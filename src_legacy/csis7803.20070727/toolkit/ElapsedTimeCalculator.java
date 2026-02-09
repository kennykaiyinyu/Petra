package toolkit;
import java.util.*;

public class ElapsedTimeCalculator {

	private static ElapsedTimeCalculator soleInstance;
	
	public static ElapsedTimeCalculator getInstance() {
		if (soleInstance == null)
			soleInstance = new ElapsedTimeCalculator();
		return soleInstance;
	}

	public int getDays(Calendar calendar1, Calendar calendar2) {
		
		Calendar c1, c2;
		c1 = (Calendar)calendar1.clone();
		c2 = (Calendar)calendar2.clone();
		c1.clear(Calendar.MILLISECOND);
		c1.clear(Calendar.SECOND);
		c1.clear(Calendar.MINUTE);
		c1.clear(Calendar.HOUR_OF_DAY);
		c2.clear(Calendar.MILLISECOND);
		c2.clear(Calendar.SECOND); 
		c2.clear(Calendar.MINUTE);
		c2.clear(Calendar.HOUR_OF_DAY);
		long time1 = c1.getTimeInMillis();
		long time2 = c2.getTimeInMillis();
		return (int) (Math.abs(time1 - time2)/86400000);
	}

	public int getMonths(Calendar calendar1, Calendar calendar2) {
		int elapsed = 0;
		Calendar c1, c2;
		if (calendar2.after(calendar1)) {
			c2 = (GregorianCalendar) calendar2.clone();
			c1 = (GregorianCalendar) calendar1.clone();
		} else {
			c2 = (GregorianCalendar) calendar1.clone();
			c1 = (GregorianCalendar) calendar2.clone();
		}
		c1.clear(Calendar.MILLISECOND);
		c1.clear(Calendar.SECOND);
		c1.clear(Calendar.MINUTE);
		c1.clear(Calendar.HOUR_OF_DAY);
		c1.clear(Calendar.DATE);
		c2.clear(Calendar.MILLISECOND);
		c2.clear(Calendar.SECOND);
		c2.clear(Calendar.MINUTE);
		c2.clear(Calendar.HOUR_OF_DAY);
		c2.clear(Calendar.DATE);
		while (c1.before(c2)) {
			c1.add(Calendar.MONTH, 1);
			elapsed++;
		}
		return elapsed;
	}
}
