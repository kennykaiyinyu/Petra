package ui;

import ui.options.Stock;
import java.text.*;

public class AddStockCommand implements Command {
	public String getDescription() {
		return "add a stock to the pricing engine.";
	}
	
	public void execute(String args[]) throws CommandException{
		if (args.length != 2) {
			throw new CommandException("Usage: addstock name price");
		}
		try {
			String name = args[0];
			
			double price = Double.parseDouble(args[1]); 
			java.util.SortedMap<String, Stock> allStocks = Session.getInstance().getStocks();
			if (!allStocks.containsKey(name)) {
				allStocks.put(name, new Stock(args[0], price));
				SimpleDateFormat formatter = new SimpleDateFormat("MMM dd, yyyy");
				String today = formatter.format(Session.getInstance().getStartDay().getTime());
				System.out.println("Stock " + name + " having price $" + price + " on date " + today + " is added.");
			}
			else 
				throw new CommandException("You cannot add a stock which has already been existing");
		} catch (NumberFormatException e) {
			throw new CommandException( e.getMessage());
		}
	}
}
