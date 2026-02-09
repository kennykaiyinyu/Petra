package numericalmethods;

/** 
 * thrown when x is put as an argument of f(x) but f(x) is undefined 
 * @author kykyu
 *
 */

public class UndefinedValueException extends Exception {
	static final long serialVersionUID = -7854989551615071535L;
	
	public UndefinedValueException() {}
	public UndefinedValueException(String message) {
		super(message);
	}
}
