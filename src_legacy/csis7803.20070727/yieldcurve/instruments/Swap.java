package yieldcurve.instruments;

import java.util.Calendar;


public class Swap extends Instrument {
	private TimeAmount period;
	private TimeAmount endDay;
	/**
	 * 
	 * @return
	 */
	public TimeAmount getPeriod() {
		return this.period;
	}
	
	public TimeAmount getEndDay() {
		return this.endDay;
	}
	
	public Swap(String subType, TimeAmount period) {
		super("SWAP", subType);
		initialize();
		
		// default period in the problem spec: 3 months
		this.period = period;
	}
	
	private void initialize() {
		
		String subType = this.getSubType();
		subType.trim();
		int index = subType.indexOf("Y");
		int numberOfYears = Integer.parseInt(subType.substring(0, index));
		this.endDay = new TimeAmount(Calendar.YEAR, numberOfYears);
		
		
	}
	
}
