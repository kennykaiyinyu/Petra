package ui;

public interface Command {
	public void execute(String args[]) throws CommandException;
	public String getDescription() ;
}
