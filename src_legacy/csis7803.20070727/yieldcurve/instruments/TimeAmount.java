package yieldcurve.instruments;
import java.util.Calendar;
public class TimeAmount implements Comparable<TimeAmount> {
	public final int FIELD;
	public final int AMOUNT;
	private final static Calendar referencePoint = Calendar.getInstance();
	
	public TimeAmount(int field, int amount) {
		this.FIELD = field;
		this.AMOUNT = amount;
	}
	
	public int compareTo(TimeAmount timeAmount) {
		Calendar c1  = (Calendar)referencePoint.clone();
		Calendar c2  = (Calendar)referencePoint.clone();
		c1.add(this.FIELD, this.AMOUNT);
		c2.add(timeAmount.FIELD, timeAmount.AMOUNT);
		return c1.compareTo(c2);
	}
}
