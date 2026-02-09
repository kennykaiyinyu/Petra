package yieldcurve.core;
/**
 * 
 */

/**
 * @author kennyyu
 *
 */
public class NoRateInTheFutureException extends Exception {

	static final long serialVersionUID = 4383512479894474601L;

	/**
	 * 
	 */
	public NoRateInTheFutureException() {
	}

	/**
	 * @param message
	 */
	public NoRateInTheFutureException(String message) {
		super(message);
	}

	/**
	 * @param cause
	 */
	public NoRateInTheFutureException(Throwable cause) {
		super(cause);
	}

	/**
	 * @param message
	 * @param cause
	 */
	public NoRateInTheFutureException(String message, Throwable cause) {
		super(message, cause);
	}

}
