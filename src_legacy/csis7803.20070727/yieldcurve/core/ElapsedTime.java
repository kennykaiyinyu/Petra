package yieldcurve.core;
import java.util.*;

/**
 * 
 * @author kykyu
 * @deprecated
 */
public class ElapsedTime {


	/**
	 * @deprecated
	 * @param calendar1
	 * @param calendar2
	 * @return
	 */
	public static int getDays(Calendar calendar1, Calendar calendar2) {
		int elapsed = 0;
		Calendar c1, c2;
		if (calendar2.after(calendar1)) {
			c2 = (Calendar) calendar2.clone();
			c1 = (Calendar) calendar1.clone();
		} else {
			c2 = (Calendar) calendar1.clone();
			c1 = (Calendar) calendar2.clone();
		}
		c1.clear(Calendar.MILLISECOND);
		c1.clear(Calendar.SECOND);
		c1.clear(Calendar.MINUTE);
		c1.clear(Calendar.HOUR_OF_DAY);
		c2.clear(Calendar.MILLISECOND);
		c2.clear(Calendar.SECOND);
		c2.clear(Calendar.MINUTE);
		c2.clear(Calendar.HOUR_OF_DAY);
		while (c1.before(c2)) {
			c1.add(Calendar.DATE, 1);
			elapsed++;
		}
		return elapsed;
	}

	/**
	 * @deprecated
	 * @param calendar1
	 * @param calendar2
	 * @return
	 */
	public static int getMonths(Calendar calendar1, Calendar calendar2) {
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
