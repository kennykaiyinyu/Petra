package yieldcurve.instruments;

public  abstract class Instrument {
	
	
	private double rate = -100;
	private final String type;
	private final String subType;
	
	public void setRate(double rate)  {

		this.rate = rate;
	}
	
	public Instrument(String type, String subtype) {
		this.type = type;
		this.subType =subtype;
	}
	
	public double getRate() throws RateNotAssignedException {
		if (rate <0)
			throw new RateNotAssignedException();
		return this.rate;
	}
	
	public String getType () {
		return type;
	}
	
	public String getSubType() {
		return subType;
	}
	
	
	
}
