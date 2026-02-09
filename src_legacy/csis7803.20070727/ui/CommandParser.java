
package ui;
import java.util.*;

// This program serves as an example to extract a line in the command file as an array of arguments.
public class CommandParser {
	
	private static TreeMap<String, Command> commandMap = new TreeMap<String, Command>();
	
	static {
		commandMap.put("echo", new EchoCommand());
		commandMap.put("importspec", new ImportCurveSpecCommand());
		commandMap.put("listoption", new ListOptionCommand());
		commandMap.put("priceoption", new PriceOptionCommand());
		commandMap.put("importdata", new ImportCurveDataCommand());
		commandMap.put("listinstruments", new ListInstrumentsCommand());
		commandMap.put("genyc", new GenYieldCurveCommand());
		commandMap.put("showyc", new ShowYieldCurveCommand());
		//commandMap.put("df", new GetDfOnDateCommand());
		commandMap.put("rate", new GetContinuousRateCommand());
		 commandMap.put("getforwardrate", new GetForwardRateCommand());
		commandMap.put("computevol", new ComputeVolatilityFromCallCommand());
		//commandMap.put("volfromcall", new SimpleComputeVolatilityFromCallCommand());
		commandMap.put("setrandomizer", new SetRandomizerCommand());
		commandMap.put("setinterpolator", new SetInterpolatorCommand());
		commandMap.put("addstock", new AddStockCommand());
		commandMap.put("stockinfo", new ShowStockInfoCommand());
		commandMap.put("today", new TodayCommand());
		commandMap.put("pricebackyc", new PriceBackCommand());
		// test
		commandMap.put("df", new GetDiscountFactorCommand());
		
		commandMap.put("bye", new Command(){
			public void execute(String args[]) {
				System.out.println("Bye!");
				System.exit(0);
			}
			public String getDescription() {
				return "Terminates the program";
			}
		});
		commandMap.put("help", new Command() {
			public void execute(String [] args) {
				Iterator<String> itr = commandMap.keySet().iterator();
				System.out.println("Available commands are: ");
				while (itr.hasNext()) {
					String commandName = itr.next();
					Command command = commandMap.get(commandName);
					if (command != null)
						System.out.println("\t"+commandName+":");
						System.out.println("\t   "+command.getDescription());
						System.out.println();
				}
			}
			public String getDescription() {
				return "Get a list of commands";
			}
		});
	}
		 
	
	public static void parseCommand(String command) {
		

		// Extract arguments of the input command into an array.
		StringTokenizer st = new StringTokenizer(command);
		if (!st.hasMoreTokens()) return;

		String commandType = st.nextToken().toLowerCase(); // this is the FIRST token!
		String firstChar = commandType.substring(0, 1);
		if (firstChar.equals("#")) // treat this line as a comment
			return;
		ArrayList<String> args = new ArrayList<String>(); 
 
		while (st.hasMoreTokens()) {
			String pre = st.nextToken(" ");

			if (pre.charAt(0) == '"') {
				if (pre.length() > 1 && pre.charAt(pre.length()-1) == '"') {
					args.add(pre.substring(1, pre.length()-1));
				} else {
					String in = st.nextToken("\"");
					args.add(pre.substring(1, pre.length()) + in);
					if (st.hasMoreTokens()) {
						st.nextToken(" ");
					} 
				}
			} else {
				args.add(pre.trim());
			}
		}

		
		String[] argsArray = new String[args.size()];
		for (int i=0; i<args.size(); i++) {
			argsArray[i] = (String) args.get(i);
		}
		
		Command c = commandMap.get(commandType);
		if (c == null)
			System.out.println(commandType + ": bad command");
		else 
		{
			try {
				c.execute(argsArray);
			} catch (CommandException e) {
				System.out.println(commandType + ": " + e.getMessage());
			}
		}
		
		
	}
	
	
}

