package ui.options;
import java.util.*;
public class Options {
	
	// singleton
	private static Options soleInstance;
	public static Options getInstance() {
		if (soleInstance == null)
			soleInstance = new Options();
		return soleInstance;
	}
	
	
	private  SortedMap<String, Option> options;
	public SortedMap<String, Option> getOptions() {
		if (options == null)
			options = new TreeMap<String, Option>();
		return options;
	}
	
	// hide the default constructor
	private Options() { }
	
}
