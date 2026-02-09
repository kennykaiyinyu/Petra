package yieldcurve.instruments;

import java.util.Calendar;


public class FRA extends Instrument {
	private TimeAmount startDay;
	private TimeAmount endDay;
	/**
	 * 
	 * @return
	 */
	public TimeAmount getStartDay() {
		return this.startDay;
	}
	
	public TimeAmount getEndDay() {
		return this.endDay;
	}
	
	public FRA(String subType) {
		super("FRA", subType);
		initialize();
	}
	
	private void initialize() {
		
		String subType = this.getSubType();
		subType.trim().toLowerCase();
	
		int index = subType.indexOf("x");
		int fromMonth = Integer.parseInt(subType.substring(0, index));
		this.startDay = new TimeAmount(Calendar.MONTH, fromMonth);
		int toMonth = Integer.parseInt(subType.substring(index +1, subType.length()));
		this.endDay = new TimeAmount(Calendar.MONTH, toMonth);
		
		
	}
	
}
