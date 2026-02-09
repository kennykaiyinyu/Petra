package ui;
import java.io.*;
public class MainClass {
	public static void main(String args[]) throws IOException {
		
		// initialization of a new session.
		
		Session.getInstance();
		
		BufferedReader reader = new BufferedReader(new InputStreamReader(System.in));
		String commandLine;
		System.out.print("[PricingEngine]$ ");
		while ((commandLine = reader.readLine())!= null) {
			CommandParser.parseCommand(commandLine);
			System.out.print("[PricingEngine]$ ");
		}
	}
}
