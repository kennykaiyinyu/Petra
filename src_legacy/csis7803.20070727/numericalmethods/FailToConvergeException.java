package numericalmethods;

public class FailToConvergeException extends Exception{
	static final long serialVersionUID = 6317798137513359963L;
	public FailToConvergeException() {}
	public FailToConvergeException(String message) { super(message); }
}