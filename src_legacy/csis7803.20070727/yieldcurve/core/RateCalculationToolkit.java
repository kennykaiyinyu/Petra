package yieldcurve.core;
import java.util.*;

import conventions.*;



public class RateCalculationToolkit {
	
	private static Comparator<Calendar> dateComparator = new Comparator<Calendar>()  {
		public int compare(Calendar c1, Calendar c2) {
			if (c1 == null || c2 == null)
				throw new NullPointerException("paramters for a comparator cannot be null");
			int y1 = c1.get(Calendar.YEAR);
			int y2 = c2.get(Calendar.YEAR);
			int m1 = c1.get(Calendar.MONTH);
			int m2 = c2.get(Calendar.MONTH);
			int d1 = c1.get(Calendar.DATE);
			int d2 = c2.get(Calendar.DATE);
			if (y1 == y2 && m1 == m2 && d1 == d2) 
				return 0;
			else 
				return c1.compareTo(c2);
		}
	};
	
	/**
	 * 
	 * @param z the zero-coupon rate of return
	 * @param t length of time which the discount factor covers
	 * @param numberOfPaymentsInYear number Of Payments In Year  
	 * @return discount factor
	 */
	public static double zToDf(double z, double t, int cf) {
		return 1 / Math.pow(1 + z / cf, cf * t);
	}
	
	/**
	 * 
	 * @param discountFactor
	 * @param lengthOfTime
	 * @param numberOfPaymentsInYear
	 * @return
	 */ 
	public static double dfToZ(double discountFactor, double lengthOfTime, int cf) {
		return (1 / Math.pow(discountFactor, 1 / ( cf*lengthOfTime  )) - 1) * cf;
	}
	
	
// This shouldn't be used anymore because of inflexibility of the choice of interpolation
//
//	public static double interpolate(double x1, double y1, double x2, double y2, double x){
//		// TODO: the case would result infinity
//		// ToolKit.interpolate(2.0, 4.0, 2.0, 1.0, -1.0) = Infinity
//		return (y2 - y1) / (x2 - x1) * (x - x1) + y1;
//	}
	
	
	
//	public static void main(String [] args) {
//		
//		// to check the correctness of interpolation
//		for (int i = 0; i < 5; i++) {
//			
//			double x1 = Math.floor(Math.random() * 10) - 5.0;
//			double y1 = Math.floor(Math.random() * 10) - 5.0;
//			double x2 = Math.floor(Math.random() * 10) - 5.0;
//			double y2 = Math.floor(Math.random() * 10) - 5.0;
//			double x = Math.floor(Math.random() * 10) - 5.0;
//			double z = RateCalculationToolkit.interpolate(x1, y1, x2, y2, x);
//			System.out.print("ToolKit.interpolate(");
//			System.out.print(Math.floor(x1) +", ");
//			System.out.print(Math.floor(y1) +", ");
//			System.out.print(Math.floor(x2) +", ");
//			System.out.print(Math.floor(y2) +", ");
//			System.out.println(Math.floor(x) +") = " + z);
//		}
//		
//		
//	}
	
	 
	/**
	 * Returns the discount factor from today to the maturity of an FRA instrument
	 * @param dfAtStartingDate the discount factor from today to the starting day of the FRA intrument
	 * @param FRA_rate the rate at which the FRA instrument will be paying
	 * @param FRALength the time span from starting to ending dates of the FRA instrument. For example, 1x4 has a time span of ((3 months) / 365) 
	 * @return
	 */
	
	public static double getDiscountFactorFromFRA(double dfAtStartingDate, double FRA_rate, double FRA_length) {
		return (dfAtStartingDate / (1 + FRA_rate * FRA_length)); 
	}
	/**
	 * 
	 * @param depositRate
	 * @param depositLength
	 * @return
	 */
	public static double getDiscountFactorFromCash(double depositRate, double depositLength) {
		return getDiscountFactorFromFRA(1, depositRate, depositLength); 
	}
	
	
	public static double computeContinuousRate(double discountFactor, double timeToMaturity) {
		return - Math.log(discountFactor) / timeToMaturity;  
	}
	
	
	
	/**
	 * Computes discount factor on some date in a given DiscountFactorCurve
	 * 
	 * NOTE: Business Day Convention is NOT considered
	 * 
	 * @param date the date of the required discount factor
	 * @param today
	 * @param dfCurve
	 * @param dayCountConvention
	 * @param interpolator
	 * @param compoundingFreq
	 * @return The discount factor
	 * @throws NoRateInTheFutureException
	 */
	public static double computeDiscountFactorOn(Calendar date, Calendar today, DiscountFactorCurve dfCurve, DayCountConvention dayCountConvention, interpolation.Interpolator interpolator, int compoundingFreq) throws NoRateInTheFutureException, NoRateAtTimeBeforeTodayException{
		toolkit.CalendarFixer.fix(date);
		toolkit.CalendarFixer.fix(today);
		
		if (dfCurve == null || date == null || interpolator == null)
			throw new IllegalArgumentException("dfCurve cannot be null");
		
		// put the keys in a Calendar arraylist
		ArrayList<Calendar> dateList = new ArrayList<Calendar>(dfCurve.keySet());
		
		// do a binary search for the date points
		int startIndex = Collections.binarySearch(dateList, date, dateComparator);
		
		// if the d.f. is found in the df curve, return it.
		if (startIndex >= 0) {
			return dfCurve.get( dateList.get(startIndex) );
		}
		
		// otherwise, convert into rates, interpolate in between, and convert the interpolated result back to a d.f.
		
		startIndex =  - startIndex - 2;
		if (startIndex < 0)
			throw new NoRateAtTimeBeforeTodayException();
		if (startIndex >= dateList.size() - 1)
			throw new yieldcurve.core.NoRateInTheFutureException();
		
		int endIndex = startIndex + 1;
		Calendar dateOnLeft = dateList.get(startIndex);
		double dfOnLeft = dfCurve.get( dateOnLeft );
		Calendar dateOnRight = dateList.get(endIndex);
		double dfOnRight = dfCurve.get( dateOnRight );
		//new interpolation.LinearInterpolator().getValueOn(date, dfCurve);
//		if ( businessDayConvention.isBusinessDay( dateOnLeft ) ) {
//			dateOnLeft = businessDayConvention.getNextBusinessDayOf( dateOnLeft );
//		}
		double timeFromTodayToLeftDate = dayCountConvention.deltaTInBetween(dateOnLeft, today);
		
		
		double leftZeroRate = dfToZ(dfOnLeft, timeFromTodayToLeftDate, compoundingFreq);
		
		// also, the right zero rate
//		if ( businessDayConvention.isBusinessDay( dateOnRight ) ) {
//			dateOnRight= businessDayConvention.getNextBusinessDayOf( dateOnRight );
//		}
		double timeFromTodayToRightDate = dayCountConvention.deltaTInBetween(dateOnRight, today);
		
		double rightZeroRate = dfToZ(dfOnRight, timeFromTodayToRightDate, compoundingFreq);
		
		// do interpolation
		TreeMap<Calendar, Double> zeroRateCurve  = new TreeMap<Calendar, Double>(); 
		zeroRateCurve.put(dateOnLeft, leftZeroRate);
		zeroRateCurve.put(dateOnRight, rightZeroRate);
		//double theZeroRateOfTheRequiredDate = interpolator.getValueOn(date, zeroRateCurve);
		double theZeroRateOfTheRequiredDate = interpolator.getValueOn(date, zeroRateCurve, dayCountConvention);
		
		double time = dayCountConvention.deltaTInBetween(today, date);	// the time from today to the required date, in ACT/365 
		return zToDf(theZeroRateOfTheRequiredDate, time, compoundingFreq);
		
	}
	
	public static double computeForwardRateIn(Calendar from, Calendar to, Calendar today, DiscountFactorCurve dfCurve, DayCountConvention dayCountConvention, interpolation.Interpolator interpolator, int compoundingFreq) throws NoRateInTheFutureException, NoRateAtTimeBeforeTodayException{
		double df0 = computeDiscountFactorOn(from, today, dfCurve, dayCountConvention, interpolator, compoundingFreq);
		double df1 = computeDiscountFactorOn(to, today, dfCurve, dayCountConvention, interpolator, compoundingFreq);
		double deltaT = dayCountConvention.deltaTInBetween(from, to);
		return (df0 - df1) / (deltaT * df1);
	}
}