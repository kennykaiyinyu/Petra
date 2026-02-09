package ui;
import java.util.*;

import montecarlo.StockPath;
import ui.options.Option;

public class StandardCall extends Option {

	private double strikePrice;
	private String underlyingStockName;
	private String nameOfCall;
	private String description = "";
	private Calendar maturityDate;
	private double sellingPrice = Double.NaN;
	
	
	public void setSellingPrice(double price) {
		this.sellingPrice = price;
	}
	public double getSellingPrice() {
		return this.sellingPrice;
	}
	
	public String getDescription() {
		return this.description;
	}
	
	public Calendar getMaturityDate() 
	{
		return (Calendar) this.maturityDate.clone();
	}

	public String getName() {
		return this.nameOfCall;
	}

	public double getPayoff(Map<String, StockPath> paths) {
		StockPath path = paths.get(underlyingStockName);
		double finalPrice = path.getFinalPrice();
		return Math.max(0, finalPrice - strikePrice);
	}

	public String[] getUnderlyingStockNames() {
		
		return new String[]{this.underlyingStockName};
	}
	
	public String getTheUnderlyingStockName() {
		return this.underlyingStockName;
	}
	public StandardCall(double strikePrice, double sellingPrice, String callName, String underlyingStock, String description, Calendar maturity) {
		this.strikePrice = strikePrice;
		this.sellingPrice = sellingPrice;
		this.underlyingStockName = underlyingStock;
		this.nameOfCall = callName;
		this.description = description;
		this.maturityDate = maturity;
	}
	
	public double getStrikePrice() {
		return this.strikePrice;
	}

}
