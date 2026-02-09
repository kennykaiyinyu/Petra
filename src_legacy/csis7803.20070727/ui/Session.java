package ui;
import ui.options.*;
import interpolation.*;
import java.util.*;

import conventions.*;
import montecarlo.*;
import ui.options.Stock;
import yieldcurve.core.*;
import yieldcurve.instruments.Instruments;

public class Session {
	private static Session soleInstance;
	
	public static int POLAR_BOX_MULLER = 1;
	public static int CARTESIAN_BOX_MULLER = 2;
	public static int AVERAGE_OF_UNIFORMLY_RANDOM_NUMBER = 3;
	private static java.util.SortedMap<Integer, Randomizer> randomizerMap = new TreeMap<Integer, montecarlo.Randomizer>();
	static {
		randomizerMap.put(Session.POLAR_BOX_MULLER, new PolarBoxMuller());
		randomizerMap.put(Session.CARTESIAN_BOX_MULLER, new CartesianBoxMuller());
		randomizerMap.put(Session.AVERAGE_OF_UNIFORMLY_RANDOM_NUMBER, new AverageOfUniformNumbers());
	}
	
	public static Session getInstance() {
		if (soleInstance == null)
			soleInstance = new Session();
		return soleInstance;
	}
	
	public java.text.SimpleDateFormat getSimpleDateFormat() {
		return this.formatter;
	}
	
	// stocks
	SortedMap<String, Stock> stocks = new TreeMap<String, Stock>();

	// options
	Options options = Options.getInstance();
	public Options getOptions() {
		return options;
	}
	
	
	public SortedMap<String, Stock> getStocks() {
		return this.stocks;
	}
	
	private Session() {
		instruments = Instruments.getInstance();
		this.initializeSth();
	}
	private BusinessDayConvention businessDayConvention = SimpleNextBusinessDayConvention.getInstance();
	
	public  DayCountConvention getDayCountConvention() {
		return this.dayCountConvention;
	}
	public void setDayCountConvention(DayCountConvention dayCountConvention) {
		this.dayCountConvention = dayCountConvention;
	}
	
	
	
	public BusinessDayConvention getBusinessDayConvention() {
		return this.businessDayConvention;
	}
	
	public void setBusinessDayConvention(BusinessDayConvention convention) {
		this.businessDayConvention = convention;
	}
	
	private DayCountConvention dayCountConvention = ACT365Convention.getInstance();
	
	private Calendar startDate = fix(new GregorianCalendar());

	private java.text.SimpleDateFormat formatter = new java.text.SimpleDateFormat("MM/dd/yyyy");
	// fix the start date: if it is sat then shift it to next business day
	{
		 
		if (!this.businessDayConvention.isBusinessDay(startDate)) {
			System.out.print("Today is a holiday. Setting today to another business day... ");
			startDate = this.businessDayConvention.getNextBusinessDayOf(startDate); 
			System.out.println("DONE");
		}
		fix(startDate);
		System.out.println("The pricing engine has TODAY set to " + formatter.format(startDate.getTime()));
	}
	
	private Instruments instruments;
	
	public Calendar getStartDay() {
		return this.startDate;
	}
	
//	private void setStartDay(Calendar startDay) {
//		this.startDate = startDay;
//	}
	
	private Calendar fix(Calendar c) {
		c.set(Calendar.MILLISECOND, 0);
		c.set(Calendar.SECOND, 0);
		c.set(Calendar.MINUTE, 0);
		c.set(Calendar.HOUR, 0);
		return c;
	}
	
	private YieldCurve yieldCurve;
	private DiscountFactorCurve discountFactorCurve;
	
	public YieldCurve getYieldCurve() {
		return this.yieldCurve;
	}
	public void setYieldCurve(YieldCurve curve) {
		this.yieldCurve = curve;
	}
	public void setDiscountFactorCurve(DiscountFactorCurve dfCurve) {
		this.discountFactorCurve = dfCurve;
	}
	
	public DiscountFactorCurve getDiscountFactorCurve() {
		return this.discountFactorCurve;
	}
	
	
	public Instruments getInstruments() {
		return instruments;
	}
	
	private int compoundingFrequency = 2;
	
	public int getCompoundingFrequency() {
		return this.compoundingFrequency;
	}
	
	public void setCompoundingFrequency(int cf) {
		this.compoundingFrequency = cf;
	}
	
	private Interpolator interpolator = new LinearInterpolator();
	public void setInterpolator(Interpolator interpolator) {
		this.interpolator = interpolator;
	}
	
	public Interpolator getInterpolator() {
		return this.interpolator;
	}
	
	private Randomizer randomizer = new PolarBoxMuller();
	public Randomizer getRandomizer() {
		return this.randomizer;
	}
	
	public void setRandomizer(int num) {
		this.randomizer = randomizerMap.get(num);
	}
	

	
	
	// JUST FOR THIS ASSIGNMENT ONLY
	
	public void initializeSth() {
		stocks.put("P", new Stock("P", 29.12));
		System.out.println("For assignment purpose, stock P, today's price at $29.12 is added to the pricing engine");
		
		
		Calendar maturity = Calendar.getInstance();
		maturity.set(2009, 0, 16, 0, 0, 0); // Jan 16, 2009
		maturity.clear(Calendar.MILLISECOND);
		StandardCall call = new StandardCall(27.5, 7.0, "BenchmarkOption", "P", "The option for finding the volatility in the programming assignmnet", (Calendar)maturity.clone());
		
		// the $7.00 benchmark option
		options.getOptions().put("BenchmarkOption", call);
		
	
		
		options.getOptions().put("OptionA", new Option() {
			private Calendar maturity = Calendar.getInstance();
			private double sellingPrice = Double.NaN; 
				
			public double getSellingPrice() {
				return this.sellingPrice;
				
			}
			public void setSellingPrice(double price) {
				this.sellingPrice = price;
			}
			
			{
				maturity.set(2009, 0, 16, 0, 0, 0);
				maturity.clear(Calendar.MILLISECOND);
			}
			
			public Calendar getMaturityDate() {
				return (Calendar) this.maturity.clone();
			}
			
			public  String getName() {
				return "OptionA";
			}
			public String[] getUnderlyingStockNames() {
				return new String[]{"P"};
			}
			
			public String getDescription() {
				return "if S < 20 then 0; otherwise if 20 <= S <= 35 then 0.5(S - 20); otherwise 3(S - 35)";
			}
			public double getPayoff(Map<String, montecarlo.StockPath> paths) {
				
				double finalPrice =paths.get("P").getFinalPrice();
				if (finalPrice < 20)
					return 0;
				if (finalPrice >=20 && finalPrice <=35)
					return 0.5*(finalPrice - 20);
				else 
					return 3*(finalPrice - 35);
					
			}
		});
		
		options.getOptions().put("OptionB", new Option() {
			
			private double sellingPrice = Double.NaN; 
			
			public double getSellingPrice() {
				return this.sellingPrice;
				
			}
			public void setSellingPrice(double price) {
				this.sellingPrice = price;
			}
			
			private Calendar maturity = Calendar.getInstance();
			
			{
				maturity.set(2009, 0, 16, 0, 0, 0);
				maturity.clear(Calendar.MILLISECOND);
			}
			
			public Calendar getMaturityDate() {
				return (Calendar) this.maturity.clone(); 
			}
			
			
	
			public  String getName() {
				return "OptionB";
			}
			public String getDescription() {
				return "option with payoff varying with Smax and Smin";
			}
			public String[] getUnderlyingStockNames() {
				return new String[]{"P"};
			}
			public double getPayoff(Map<String, montecarlo.StockPath> paths) {
				double[] stockPrices = paths.get("P").getStockPrices();
				
				double min = Double.MAX_VALUE;
				double max = Double.MIN_VALUE;
				for (int i = 0; i < stockPrices.length; i++) {
					if (stockPrices[i] > max)
						max = stockPrices[i];
					if (stockPrices[i] < min)
						min = stockPrices[i];
				}
				/*
				 * If (Smax-Smin) >= 20, then 2*(Smax-Smin)
				 * If 20>(Smax-Smin) >= 10, then (Smax-Smin)
				 * If 10>(Smax-Smin)>=5, then 0.5*(Smax-Smin)
				 * If (Smax-Smin)<5, then 0
				 */
				if (max - min >= 20)
					return 2*(max - min);
				else if (20 > max - min && max -min >= 10)
					return (max - min);
				else if (10 > max - min && max - min >= 5)
					return 0.5 * max - min;
				else 
					return 0;
			}
		});
	}
}
