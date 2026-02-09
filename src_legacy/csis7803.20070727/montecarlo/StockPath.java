package montecarlo;
public class StockPath {
	private double[] stockPrices;
	private double initialPrice;
	private int numOfTimeSteps;
	private double[] epsilons;
	
	public StockPath(double initialPrice, double[] epsilons, double[] stockPrices) {
		this.numOfTimeSteps = stockPrices.length;
		this.initialPrice = initialPrice;
		this.epsilons = epsilons;
		this.stockPrices = stockPrices;
	}
	

	public double[] getEpsilons() {
		return this.epsilons;
	}
	
	public double[] getStockPrices() {
		return this.stockPrices;
	}
	
	public double getInitialPrice( ) {
		return this.initialPrice;
	}
	
	public double getFinalPrice() {
		return this.stockPrices[stockPrices.length - 1];
	}
	
	public double getNumberOfTimeSteps() {
		return this.numOfTimeSteps;
	}
}
