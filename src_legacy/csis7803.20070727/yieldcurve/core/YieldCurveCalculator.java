package yieldcurve.core;
import yieldcurve.instruments.*;

 
import interpolation.*;

import java.util.*;

import conventions.*;

/**
 * The yield curve calculator, upon different day count convention and business day convention,
 * takes in instruments, compounding frequency, start date and different interpolation method to generate
 * the yield curve.  
 * @author kykyu
 *
 */
public class YieldCurveCalculator {
	
	public YieldCurveCalculator
		(DayCountConvention dayCountConvention,
			BusinessDayConvention businessDayConvention,
			Instruments instruments,
			int compoundingFrequency,
			Interpolator interpolator,
			Calendar startDate)
	{
		this.dayCountConvention = dayCountConvention;
		this.businessDayConvention = businessDayConvention;
		this.instruments = instruments;
		this.compoundingFrequency = compoundingFrequency;
		this.interpolator = interpolator;
		this.startDate = startDate;
		//System.out.println("start date = " + startDate.getTime().toString() + "!!!!!!!!!!1");
	}
	
	private DayCountConvention dayCountConvention;
	private BusinessDayConvention businessDayConvention;
	private Instruments instruments;
	private int compoundingFrequency;
	private Interpolator interpolator;
	private Calendar startDate;
	
	
	private boolean modified = true; 
	
	private YieldCurve yieldCurve = null;
	private DiscountFactorCurve discountFactorCurve = null;
	
	public void putInstruments(Instruments instruments) {
		this.instruments = instruments;
	}
	
	
//	private Calendar fix(Calendar c) {
//		c.clear(Calendar.MILLISECOND);
//		c.clear(Calendar.SECOND);
//		c.clear(Calendar.MINUTE);
//		c.clear(Calendar.HOUR);
//		return c;
//	}

	/**
	 * Set the initial day. The modified flag is turned on.
	 * @param startDate
	 */
	public void setStartDate(Calendar startDate) {
		this.startDate = startDate;
		
		this.modified = true;
	}
	
	/**
	 * Set the day count convention. The modified flag is turned on.
	 * @param dayCountConvention
	 */
	public void setDayCountConvention(DayCountConvention dayCountConvention) {
		this.dayCountConvention = dayCountConvention;
		this.modified = true;
	}
	
	/**
	 * Set the Business Day Convention. The modified flag is turned on.
	 * @param businessDayConvention
	 */
	public void setBusinessDayConvention(BusinessDayConvention businessDayConvention) {
		this.businessDayConvention = businessDayConvention;
		this.modified = true;
	}
	
	/**
	 * Set the compounding frequency. The modified flag is turned on.
	 * @param cf
	 */
	public void setCompoundingFrequency(int cf) {
		this.compoundingFrequency = cf;
		this.modified = true;
	}
	
	/**
	 * Gets the interpolator instance
	 * @return
	 */
	public Interpolator getInterpolator( ) {
		return this.interpolator;
	}
	
	/**
	 * Sets the interpolator for calculation. The modified flag is turned on.
	 * @param interpolator
	 */
	public void setInterpolator( Interpolator interpolator ) {
		this.interpolator = interpolator;
		this.modified = true;
	}
	
	/**
	 * Get the yield curve. If the yield curve is not yet generated the calculator generates one and then return it.
	 * After calling this method, the 'modified' flag is turned off.
	 * @return
	 */
	public YieldCurve getZeroRateCurve() throws NoRateAtTimeBeforeTodayException{
		
		if (this.modified || this.yieldCurve == null) {
			this.yieldCurve = new YieldCurve();
			this.discountFactorCurve = new DiscountFactorCurve();
			generateYieldCurve(this.yieldCurve, this.discountFactorCurve);
		}
		this.modified = false;
		return yieldCurve;
	}
	
	/**
	 * Get the discount factor curve. If the discount factor curve is not yet generated the calculator generates one and then return it.
	 * After calling this method, the 'modified' flag is turned off.
	 * @return
	 */
	public DiscountFactorCurve getDiscountFactorCurve() throws NoRateAtTimeBeforeTodayException{
		if (this.discountFactorCurve == null||this.modified) {
			this.yieldCurve = new YieldCurve();
			this.discountFactorCurve = new DiscountFactorCurve();
			generateYieldCurve(this.yieldCurve, this.discountFactorCurve);
		}
		this.modified = false;
		return discountFactorCurve;		
	}
	
	
	
	
	// --------------- helper methods below ---------------------
	
	private void generateYieldCurve(YieldCurve yieldCurve, DiscountFactorCurve theDfCurve) throws NoRateAtTimeBeforeTodayException{
		
		
		// rate of today equals 0
		yieldCurve.put(this.startDate, 0.0);
		theDfCurve.put(this.startDate, 1.0);
		
		SortedMap<Integer, Instrument> map = instruments.getAllInstruments();
		ArrayList<Cash> cashs = new ArrayList<Cash>();	// CASH
		ArrayList<FRA> fras = new ArrayList<FRA>();		// Forward Rate Agreements
		ArrayList<Swap> swaps = new ArrayList<Swap>();	// Swaps
		Collection<Instrument> all = map.values();
		Iterator<Instrument> itr = all.iterator();
		while (itr.hasNext()) {
			Instrument i = itr.next();
			if (i instanceof Cash) {
				cashs.add((Cash)i);
			} else if (i instanceof FRA) {
				fras.add((FRA)i);
			} else if (i instanceof Swap) {
				swaps.add((Swap)i);
			}
		}
		
		// for each CASH, add the zero rates found
		Iterator<Cash> cashItr = cashs.iterator();
		while (cashItr.hasNext()) {
			Cash cash = (Cash) cashItr.next();
			try {
				this.addZeroRateFromCash(yieldCurve, theDfCurve, cash);
			} catch (RateNotAssignedException e) {
				continue;
			}
		}
		
		// for each FRA, add the zero rates found
		Iterator<FRA> fraItr = fras.iterator();
		while (fraItr.hasNext()) {
			FRA fra = (FRA) fraItr.next();
			try {
				this.addZeroRateFromFRA(yieldCurve, theDfCurve, fra);
			} catch (RateNotAssignedException e) {
				continue;
			} catch (NoRateInTheFutureException e) {
				e.printStackTrace();
			} catch (NoRateAtTimeBeforeTodayException e) {
				e.printStackTrace();
			}
		}
		
		// compute the swap curve part
		this.addZeroRatesFromSwaps(yieldCurve, theDfCurve, swaps, 4);
		

	}
	
	
	
	
	
	
	private void addZeroRatesFromSwaps(YieldCurve theYieldCurve, DiscountFactorCurve dfCurve, List<Swap> swaps, int annualFrequency) throws NoRateAtTimeBeforeTodayException  
		 {
		
		SortedMap<Calendar, Double> swapRates = getMapOfSwapRates(swaps);
		Object[] dates = swapRates.keySet().toArray();
		Calendar lastDay = (Calendar)dates[dates.length - 1];

		//System.out.println(lastDay.getTime().toString());
		Calendar startDay = (Calendar)this.startDate.clone();
		
		//int daysPassed = 0; 
		
		double s = 0;
		int count = 1;
		double theAccumulatedDeltaT = 0;
		Calendar theDay = (Calendar)startDay.clone();
		while (true) {
			
				Calendar oldDay = theDay;
				//theDay = (Calendar)theDay.clone();
				//theDay.add(Calendar.MONTH, 12 / annualFrequency);
				theDay = this.businessDayConvention.getBusinessDayAfter(startDay, Calendar.MONTH, count++ * 12 / annualFrequency);
				
				if (theDay.after(lastDay))
				{
					//System.out.println("THE DAY ---> " + theDay.getTime());
					//System.out.println("last DAY ---> " + lastDay.getTime());
					//break;
				}
				
				//System.out.println("THE DAY ---> " + theDay.getTime());
				 
				double deltaT = this.dayCountConvention.deltaTInBetween(oldDay, theDay);
				theAccumulatedDeltaT += deltaT;
				double df;
				try {
					//df = RateCalculationToolkit.zToDf(getRateOnDate(theYieldCurve, theDay), theAccumulatedDeltaT, this.compoundingFrequency);
					df = RateCalculationToolkit.zToDf(getRateOnDate(theYieldCurve, theDay, this.dayCountConvention), theAccumulatedDeltaT, this.compoundingFrequency);
					
					//System.out.println(theDay.getTime() + ": df = " + df);
					theYieldCurve.put(theDay, RateCalculationToolkit.dfToZ(df, theAccumulatedDeltaT, 2));
					dfCurve.put(theDay, df);
				}
				catch(NoRateInTheFutureException e) {
					try {
						//double theSwapRate = theSwapCurve.getSwapRateOnDate(daysPassed);
						double theSwapRate = getRateOnDate(swapRates, theDay, this.dayCountConvention);
					//	System.out.println(theDay.getTime()+ ": swapRate = " + theSwapRate);
						df = (1 - theSwapRate * s)/(1 + theSwapRate * deltaT);
						//System.out.println(theDay.getTime()+ ": df = " + df);	
	//					 add zero rates to the point graph now!
						theYieldCurve.put(theDay, RateCalculationToolkit.dfToZ(df, theAccumulatedDeltaT, 2));
						dfCurve.put(theDay, df);
						
					} catch(NoRateInTheFutureException e2) {
						// the points in the future cannot be found
						break;
					}
				}
				
				// next iteration for this swap
				//j++;
				s = s + deltaT * df;
				
			}
			//yearsPassed += theSwap.getNumberOfYears();
		
	}
	
	
	
	
	
	
	
	
	
	
	
	private void addZeroRateFromCash(YieldCurve yieldCurve, DiscountFactorCurve dfCurve, Cash cash) throws RateNotAssignedException {
		double rate = cash.getRate();
		Calendar startDay = this.startDate;

		TimeAmount ta = cash.getLengthOfDeposit();
		Calendar endDay = this.businessDayConvention.getBusinessDayAfter(startDay, ta.FIELD, ta.AMOUNT)	;
		
		double time = dayCountConvention.deltaTInBetween(startDay, endDay);
		double df = 1 / (1 + rate * time);
		
		dfCurve.put(endDay, df);
		yieldCurve.setZeroRateOnDay(endDay, RateCalculationToolkit.dfToZ(df, time, this.compoundingFrequency));
	}
	
	private void addZeroRateFromFRA(YieldCurve yc, DiscountFactorCurve dfCurve , FRA theFRA) throws RateNotAssignedException, NoRateInTheFutureException , NoRateAtTimeBeforeTodayException{
			TimeAmount timeToStart = theFRA.getStartDay();
			TimeAmount timeToEnd = theFRA.getEndDay();

			//Calendar fraStartDate = (Calendar)this.startDate.clone();
			//fraStartDate.add(timeToStart.FIELD, timeToStart.AMOUNT);
			Calendar fraStartDate = this.businessDayConvention.getBusinessDayAfter(this.startDate, timeToStart.FIELD, timeToStart.AMOUNT);
			
			
		//System.out.println("frastart=" + ui.Session.getInstance().getSimpleDateFormt().format(fraStartDate.getTime()));
			//Calendar fraEndDate = (Calendar)this.startDate.clone();
			//fraEndDate.add(timeToEnd.FIELD, timeToEnd.AMOUNT);
			Calendar fraEndDate = this.businessDayConvention.getBusinessDayAfter(this.startDate, timeToEnd.FIELD, timeToEnd.AMOUNT);
		//System.out.println("FRA Start Date = " + fraStartDate.getTime());
		//System.out.println("FRA End Date = " + fraEndDate.getTime());
		//System.out.println("fraend=" + ui.Session.getInstance().getSimpleDateFormt().format(fraEndDate.getTime()));
			double deltaT1 = this.dayCountConvention.deltaTInBetween(fraStartDate, fraEndDate);
		//System.out.println("deltaT1 = " + deltaT1);	
			double delatT = this.dayCountConvention.deltaTInBetween(this.startDate, fraEndDate);
		//System.out.println("deltaT = " + delatT);	
			double deltaT0 = this.dayCountConvention.deltaTInBetween(this.startDate, fraStartDate);
			//System.out.println("deltaT0 = " + deltaT0);	
			double fraRate = theFRA.getRate(); // FRA rate
		//System.out.println("FRA Rate = " + fraRate);	
			double df0 = RateCalculationToolkit.zToDf( this.getRateOnDate(yc, fraStartDate, this.dayCountConvention), deltaT0, this.compoundingFrequency );
		//System.out.println("df0 = " + df0);
			double df1 = 1/(1 + fraRate * deltaT1);
		//System.out.println("df1 = " + df1);
			double df = df1 * df0;
			//	this.addDfFromNow(endDay, discountFactorFromNow); 	// added on Apr 6
		//System.out.println("df = " + df);
			yc.put(fraEndDate, RateCalculationToolkit.dfToZ(df, delatT,2));
			dfCurve.put(fraEndDate, df);
		}
	

	
	
/**
 * 
 * @param map
 * @param date
 * @param dayCountConvention
 * @return
 * @throws NoRateInTheFutureException
 * @throws NoRateAtTimeBeforeTodayException
 */
	private double getRateOnDate(SortedMap<Calendar, Double> map, Calendar date, DayCountConvention dayCountConvention) throws NoRateInTheFutureException, NoRateAtTimeBeforeTodayException {
		if (map.containsKey(date)) 
			return map.get(date);
		else {
			//return interpolator.getValueOn(date, map);
			return interpolator.getValueOn(date, map, dayCountConvention);			
			
		}
	}
	
	
	
	private SortedMap<Calendar, Double> getMapOfSwapRates(List<Swap> listOfSwaps) {
		SortedMap<Calendar, Double> map = new TreeMap<Calendar, Double>();
		map.put(this.startDate, 0.0);
		Iterator<Swap> itr = listOfSwaps.iterator();
		while (itr.hasNext()) {
			try {
				Swap swap = itr.next();
				
				double swapRate = swap.getRate();
				TimeAmount timeToMaturity = swap.getEndDay();
				//Calendar endDate = (Calendar)this.startDate.clone();
				//endDate.add(timeToMaturity.FIELD, timeToMaturity.AMOUNT);
				Calendar endDate = this.businessDayConvention.getBusinessDayAfter(startDate, timeToMaturity.FIELD, timeToMaturity.AMOUNT);
				map.put(endDate, swapRate);
				
			} catch (RateNotAssignedException e) {
				// ignore this swap
				continue;
			}
		}
		return map;
	}
}
