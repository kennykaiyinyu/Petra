package interpolation;
import java.util.*;

import conventions.DayCountConvention;

import yieldcurve.core.*;

/**
 * This defines the interface which every interpolator class should have.
 * Every interpolator provides <code>getValueOn(Calendar theDate, SortedMap<Calendar, Double> map>)</code>.
 * The <code>map</code> is the map which maps dates to their discount factors from today; while
 * <code> theDate</code> is the date where the discount factor of which is of interest, so as to bean input
 * to the interpolator.   
 * @author kykyu
 *
 */

public interface Interpolator {
	//public double getValueOfYAt(double x, Point[] endPoints);
	/**
	 * @deprecated
	 */
	public double getValueOn(Calendar theDate, SortedMap<Calendar, Double> map) throws NoRateInTheFutureException,NoRateAtTimeBeforeTodayException;
	
	public double getValueOn(Calendar c, SortedMap<Calendar, Double> map, DayCountConvention dayCountConvention) throws NoRateInTheFutureException,NoRateAtTimeBeforeTodayException;
}
