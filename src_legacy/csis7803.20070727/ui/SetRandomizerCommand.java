package ui;
import montecarlo.*;
public class SetRandomizerCommand implements Command {
	
	public String getDescription(){
		return "set the randomizer used in the pricing engine.";
	}
	
	public void execute(String args[]) throws CommandException
	{
		if (args.length == 0) {
			Randomizer r =Session.getInstance().getRandomizer(); 
			if ( r!= null)
				throw new CommandException("USAGE: randomizer arg\n where\n arg = 1: cart. box muller;\n 2: polar box muller;\n 3: average of uniformly random numbers");
			
		}
		
		if (args.length != 1) {
			throw new CommandException("usage: setrandomizer arg;\n where arg = 1: cart. box muller;\n 2: polar box muller;\n 3: average of uniformly random numbers");
		}
		Session session = Session.getInstance();
		String option = args[0];
		if ("1".equals(option)) {
			session.setRandomizer(Session.CARTESIAN_BOX_MULLER);
		} else if ("2".equals(option)) {
			session.setRandomizer(Session.POLAR_BOX_MULLER);
		} else if ("3".equals(option)) {
			session.setRandomizer(Session.AVERAGE_OF_UNIFORMLY_RANDOM_NUMBER);
		} else
			throw new CommandException("argument must be from 1 to 3");
		
		System.out.println("Selected");
	}
}
