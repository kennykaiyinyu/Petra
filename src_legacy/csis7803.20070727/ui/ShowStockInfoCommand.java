package ui;

import ui.options.Stock;


public class ShowStockInfoCommand implements Command {
	public String getDescription() {
		return "prints the information of stocks";
	}
	public void execute(String[] args) throws CommandException {
		if (args.length > 0) {
			throw new CommandException("Usage: stockinfo");
		}
		
		java.util.SortedMap<String, Stock> allStocks = Session.getInstance().getStocks();
		java.util.Iterator<String> itr = allStocks.keySet().iterator();
		if (allStocks.isEmpty()) {
			throw new CommandException("no stocks in the pricing system");
		}
		while (itr.hasNext()) {
			String name = itr.next();
			Stock stock = allStocks.get(name);
			System.out.println("Name: " + name);
			System.out.println("Stock price: " + stock.getCurrentPrice());
			System.out.println("Stock volatility: " + stock.getVolatility());
			System.out.println();
		}
			
	}

}
