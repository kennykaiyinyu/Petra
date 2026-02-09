package yieldcurve.instruments;

public class RateNotAssignedException extends Exception {
	
	static final long serialVersionUID = 4167315899151303722L;
	public RateNotAssignedException() {}
	public RateNotAssignedException(String message) {
		super (message);
	}
}