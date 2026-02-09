package toolkit;

import java.util.Calendar;

public class CalendarFixer {
	 	
	public static Calendar fix(Calendar c) {
		c.clear(Calendar.MILLISECOND);
		c.clear(Calendar.SECOND);
		c.clear(Calendar.MINUTE);
		c.clear(Calendar.HOUR);
		return c;
	}
}
