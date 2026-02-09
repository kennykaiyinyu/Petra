package yieldcurve.instruments;
import java.util.Calendar;

public class Cash extends Instrument {
	private TimeAmount lengthOfDeposit;
	/**
	 * 
	 * @return
	 */
	public TimeAmount getLengthOfDeposit() {
		return this.lengthOfDeposit;
	}
	public Cash(String subType) {
		super("CASH", subType);
		initialize();
	}
	
	private void initialize() {
		
		String subType = this.getSubType();
		subType.trim();
		if ("ON".equals(this.getSubType())) {
			this.lengthOfDeposit = new TimeAmount(Calendar.DATE, 1);
			return;
		}
		char unit = subType.charAt(subType.length() - 1);

		if (unit == 'N') {	// the unit is in number of nights
			int amount = Integer.parseInt(subType.substring(0, subType.length() - 1));
			this.lengthOfDeposit = new TimeAmount(Calendar.DATE, amount);
			return;
		}
		if (unit == 'W') {
			
			int amount = Integer.parseInt(subType.substring(0, subType.length() - 1));
			this.lengthOfDeposit = new TimeAmount(Calendar.WEEK_OF_YEAR, amount);
			return;
		}
		if (unit == 'M') {
			int amount = Integer.parseInt(subType.substring(0, subType.length() - 1));
			this.lengthOfDeposit = new TimeAmount(Calendar.MONTH, amount);
			return;
		}
		
		throw new IllegalArgumentException("no such subtype - " + subType);
	}
	
}
