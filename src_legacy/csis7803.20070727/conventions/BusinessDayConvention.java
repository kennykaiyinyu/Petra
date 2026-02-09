package conventions;
import java.util.Calendar;

public abstract class BusinessDayConvention {
	public abstract boolean isBusinessDay(Calendar theDay);
	public abstract Calendar getNextBusinessDayOf(Calendar theDay);
	public abstract int getNumberOfDaysBetween(Calendar day, Calendar anotherDay);
	public abstract Calendar getBusinessDayAfter(Calendar today, int field, int amount);
}
