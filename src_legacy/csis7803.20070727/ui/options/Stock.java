package ui.options;

public class Stock {
	private double currentPrice = Double.NaN;
	private double volatility = Double.NaN;
	private String name; 
	
	public Stock(String name, double currentPrice) {
		this.name = name;
		this.currentPrice = currentPrice;
	}
	
	
	public double getCurrentPrice() {
		return this.currentPrice;
	}
	
	public void setVolatility(double v) {
		this.volatility = v;
	}
	
	
	
	public double getVolatility() {
		return this.volatility; 
	}
	
	public String getName() {
		return name;
	}
}
