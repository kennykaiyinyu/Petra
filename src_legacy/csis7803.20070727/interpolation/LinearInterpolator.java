package interpolation;
import java.util.*;
import toolkit.ElapsedTimeCalculator;
import yieldcurve.core.NoRateAtTimeBeforeTodayException;
import yieldcurve.core.NoRateInTheFutureException;
import conventions.DayCountConvention;
public class LinearInterpolator implements Interpolator {
	
	private Calendar fix(Calendar c) {
		c.clear(Calendar.MILLISECOND);
		c.clear(Calendar.SECOND);
		c.clear(Calendar.MINUTE);
		c.clear(Calendar.HOUR);
		return c;
	}
	
	/**
	 * @deprecated
	 */
	public double getValueOn(Calendar c, SortedMap<Calendar, Double> map) throws NoRateInTheFutureException,NoRateAtTimeBeforeTodayException {
		fix(c);
		
		
		
		Set<Calendar> theKeySet = map.keySet();
		ArrayList<Calendar> theKeyList = new ArrayList<Calendar>(theKeySet);
		int position = Collections.binarySearch(theKeyList, c);
		
		if (position >= 0) {
			// the date is found
						
			Calendar theKey = theKeyList.get(position);
			double theValue = map.get(theKey);
			return theValue;
			
		} else {
			/*
			 * refer to java.util.Collections for the calculation of position
			 * the date is NOT found
			 * interpolate for the result!
			 */
			position = -1 - position; 
			
			if (position == 0) {
				// TODO: how can this case be handled???
				// System.out.println("TODO");
				// return -1000.0;
				throw new NoRateAtTimeBeforeTodayException("cannot interpolate this point at time less than 0");
			} else if (position < theKeyList.size()){
				// get the interpolated value
				Calendar time1 = theKeyList.get(position - 1);
				double rate1 = map.get(time1);
				Calendar time2 = theKeyList.get(position);
				double rate2 = map.get(time2);
				double x1 = 0;
				ElapsedTimeCalculator elapsed = ElapsedTimeCalculator.getInstance();
				double x2 = elapsed.getDays(time1, time2);
				double x = elapsed.getDays(time1, c);
				
				double y = (x - x1) * ( (rate2 - rate1) / (x2 - x1) ) + rate1;
				return y;
			} else {
				throw new NoRateInTheFutureException("out of bound!");
			}
		}
	
	}

	
	public double getValueOn(Calendar c, SortedMap<Calendar, Double> map, DayCountConvention dayCountConvention) throws NoRateInTheFutureException,NoRateAtTimeBeforeTodayException {
		fix(c);
		
		
		
		Set<Calendar> theKeySet = map.keySet();
		ArrayList<Calendar> theKeyList = new ArrayList<Calendar>(theKeySet);
		int position = Collections.binarySearch(theKeyList, c);
		
		if (position >= 0) {
			// the date is found
						
			Calendar theKey = theKeyList.get(position);
			double theValue = map.get(theKey);
			return theValue;
			
		} else {
			/*
			 * refer to java.util.Collections for the calculation of position
			 * the date is NOT found
			 * interpolate for the result!
			 */
			position = -1 - position; 
			
			if (position == 0) {
				// TODO: how can this case be handled???
				// System.out.println("TODO");
				// return -1000.0;
				throw new NoRateAtTimeBeforeTodayException("cannot interpolate this point at time less than 0");
			} else if (position < theKeyList.size()){
				// get the interpolated value
				Calendar time1 = theKeyList.get(position - 1);
				double rate1 = map.get(time1);
				Calendar time2 = theKeyList.get(position);
				double rate2 = map.get(time2);
				double x1 = 0;
				//ElapsedTimeCalculator elapsed = ElapsedTimeCalculator.getInstance();
				//double x2 = elapsed.getDays(time1, time2);
				double x2 = dayCountConvention.deltaTInBetween(time1, time2);
				//double x = elapsed.getDays(time1, c);
				double x = dayCountConvention.deltaTInBetween(time1, c);
				
				double y = (x - x1) * ( (rate2 - rate1) / (x2 - x1) ) + rate1;
				return y;
			} else {
				throw new NoRateInTheFutureException("out of bound!");
			}
		}
	
	}

	
	
}
