package ui;
import java.text.*;
public class TodayCommand implements Command {

	public void execute(String[] args) throws CommandException {
		SimpleDateFormat formatter = new SimpleDateFormat("MMM dd, yyyy");
		String today = formatter.format(Session.getInstance().getStartDay().getTime());
		System.out.println("Today is " + today);
	}

	public String getDescription() {
		return "See what is 'today' set in the pricing engine";
	}

}
