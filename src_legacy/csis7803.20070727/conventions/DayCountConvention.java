package conventions;

import java.util.Calendar;

public abstract class DayCountConvention {
	public abstract double deltaTInBetween(Calendar c1, Calendar c2);

	
}
