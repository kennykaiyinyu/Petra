package ui;
import java.io.*;
import java.util.*;

import yieldcurve.instruments.*;

public class ImportCurveSpecCommand implements Command{
	public String getDescription() {
		return "imports curve spec. from a file";
	}
	public void execute(String [] args) throws CommandException{
		if (args.length != 1)
			throw new CommandException("Usage: importspec csv-filename");
		
		String line;
		String filename = args[0];
		try {
			BufferedReader reader = new BufferedReader(new FileReader(filename));
		
			while ( (line = reader.readLine()) != null ) {
				String [] tokens = line.split(",");
				
				// parse the tokens
				String type = tokens[0];
				String subType = tokens[1];
				int id;
				try {
					 id = Integer.parseInt(tokens[2]);
				} catch (NumberFormatException e) {
					// skip this line
					continue;
				}
				Instrument instrument = null;
				if ( "SWAP".equals(type) )
					instrument = new Swap(subType, new TimeAmount(Calendar.MONTH, 3));
				else if ("FRA".equals(type))
					instrument = new FRA(subType) ;
				else if ("CASH".equals(type)) {
					instrument = new Cash(subType);
					
				} else {
					// skip this line
					continue;
				}
				
				// store up the new instrument 
				Instruments instrments = Session.getInstance().getInstruments();
				instrments.addInstrument(id, instrument);
			}
		}catch (FileNotFoundException e) {
				throw new CommandException("File " + filename + " not found");
		} catch (IOException e) {
			e.printStackTrace();
		}
	}
}
