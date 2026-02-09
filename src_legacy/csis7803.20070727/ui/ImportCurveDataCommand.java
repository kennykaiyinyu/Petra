package ui;
import java.io.*;

import yieldcurve.instruments.*;
public class ImportCurveDataCommand implements Command{
	public String getDescription() {
		return "imports curve data from a file";
	}
	public void execute(String [] args) throws CommandException{
		if (args.length != 1)
			throw new CommandException("Usage: importdata csv-file");
		Session session = Session.getInstance();
		Instruments instruments = session.getInstruments();
		if (instruments == null)
			throw new CommandException("You must use 'importspec' to import the curve specification first.");
		String line;
		String filename = args[0];
		try {
			
			BufferedReader reader = new BufferedReader(new FileReader(filename));
		
			while ( (line = reader.readLine()) != null ) {
				String [] tokens = line.split(",");
				int id;
				double rate;
				try {
					id = Integer.parseInt(tokens[0]);
					rate = Double.parseDouble(tokens[1]);
					rate = rate / 100.0;
				} catch (NumberFormatException e) {
					continue;
				}
				Instrument theInstrument = Session.getInstance().getInstruments().getInstrument(id);
				theInstrument.setRate(rate);
			}
		} catch (FileNotFoundException e) {
			throw new CommandException("File " + filename + " not found");
		} catch (IOException e) {
			e.printStackTrace();
		}
	}
}
