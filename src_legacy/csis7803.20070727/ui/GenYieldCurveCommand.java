package ui;

import yieldcurve.core.*;


public class GenYieldCurveCommand implements Command {

	public String getDescription() {
		return "generate the yield curve. The curve spec and curve data must be imported first";
	}
	public void execute(String[] args) throws CommandException {
		Session session = Session.getInstance();
		
		yieldcurve.core.YieldCurveCalculator calculator = new yieldcurve.core.YieldCurveCalculator
			(
				session.getDayCountConvention(),
				session.getBusinessDayConvention(),
				session.getInstruments(),
				session.getCompoundingFrequency(),
				session.getInterpolator(),
				session.getStartDay()
			);
		try {
			YieldCurve yc = calculator.getZeroRateCurve();
			DiscountFactorCurve dfCurve = calculator.getDiscountFactorCurve();
		
		
		
			Session.getInstance().setYieldCurve(yc);
			Session.getInstance().setDiscountFactorCurve(dfCurve);
			System.out.println("Yield Curve generated. Type 'showyc' or 'showyc [output-file]' to view the result.");
		} catch (NoRateAtTimeBeforeTodayException e) {
			throw new CommandException("no rate before today exception");
		}
		
	}
	
	
	
	

}
