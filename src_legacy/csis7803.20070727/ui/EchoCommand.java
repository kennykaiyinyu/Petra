package ui;

public class EchoCommand implements Command{
	public void execute(String args[]) throws CommandException{

		for (int i = 0; i < args.length; i++) {
			System.out.print(args[i] + " ");
		}
		System.out.println();
	}
	
	public String getDescription() {
		return "does echoing";
	}
}
