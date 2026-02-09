/**
 * 
 */
package forwardvolcurve;

/**
 * @author Kenny Yu
 */
public class ForwardVolatilityCurveCalculator {
	private static ForwardVolatilityCurveCalculator soleInstance;
	private ForwardVolatilityCurveCalculator() {
	}
	
	public ForwardVolatilityCurveCalculator getInstance() {
		if (soleInstance != null) {
			soleInstance = new ForwardVolatilityCurveCalculator(); 
		}
		return soleInstance;
	}
	
	// TODO
	public ForwardVolatilities doCurveBootstrapping() {
		return null;
	}
}
