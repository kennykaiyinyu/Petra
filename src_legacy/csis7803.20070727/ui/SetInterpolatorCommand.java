package ui;

public class SetInterpolatorCommand implements Command {
	public String getDescription() {
		return "set the interpolator used by the pricing engine.";
	}
	public void execute(String[] args) throws CommandException {
		throw new CommandException("Currently, only linear interpolator can be selected.");
	}

}
