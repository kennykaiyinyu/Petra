package ui;
import ui.options.*;
import java.util.*;
public class ListOptionCommand implements Command {

	public void execute(String[] args) throws CommandException{
		if (args.length > 0)
			throw new CommandException("shouldn't supply any arguments");
		Options options = Session.getInstance().getOptions();
		SortedMap<String, Option> optionsMap = options.getOptions();
		Iterator<String> itr = optionsMap.keySet().iterator();
		int i = 1;
		while (itr.hasNext()) {
			
			String key = itr.next();
			Option o = optionsMap.get(key);
			System.out.print("[" + i + "] ");
			System.out.println(o.getName());
			System.out.println("\t" + o.getDescription());
			System.out.println("\tSelling Price: " + o.getSellingPrice());
			if ( o instanceof StandardCall) {
				System.out.println("\tStrike Price: " + ((StandardCall)o).getStrikePrice());
			}
			System.out.println("\tMaturity: " + Session.getInstance().getSimpleDateFormat().format(o.getMaturityDate().getTime()));
			System.out.print("\tUnderlying Stock(s): ");
			if (o.getUnderlyingStockNames() != null)
				for (int j = 0; j < o.getUnderlyingStockNames().length; j++) {
					System.out.print(o.getUnderlyingStockNames()[j] + " ");
				}
			i++;
			System.out.println();
		}
		
	}
	public String getDescription() {
		return "List all the options stored in this pricing system";
	}

}
