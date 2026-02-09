package yieldcurve.core;
/**
 * 
 */

/**
 * @author kennyyu
 *
 */
public class NoRateAtTimeBeforeTodayException extends Exception {

	static final long serialVersionUID = -9178309839210467782L;
	/**
	 * 
	 */
	public NoRateAtTimeBeforeTodayException() {
	}

	/**
	 * @param arg0
	 */
	public NoRateAtTimeBeforeTodayException(String arg0) {
		super(arg0);
	}

	/**
	 * @param arg0
	 */
	public NoRateAtTimeBeforeTodayException(Throwable arg0) {
		super(arg0);
	}

	/**
	 * @param arg0
	 * @param arg1
	 */
	public NoRateAtTimeBeforeTodayException(String arg0, Throwable arg1) {
		super(arg0, arg1);
	}

}
