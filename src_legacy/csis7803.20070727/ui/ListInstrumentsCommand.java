package ui;
import java.util.*;

import yieldcurve.instruments.Instrument;
import yieldcurve.instruments.RateNotAssignedException;
public class ListInstrumentsCommand implements Command {
	public String getDescription() {
		return "lists all instruments imported from the files for yield curve generation.";
	}
	public void execute(String[] args) throws CommandException {
		if (args.length > 0)
			throw new CommandException("no argument should be supplied");
		TreeMap<Integer, Instrument> treemap = Session.getInstance().getInstruments().getAllInstruments();
		Set<Integer> keySet = treemap.keySet();
		Iterator<Integer> itr = keySet.iterator();
		while (itr.hasNext()) {
			int i = itr.next();
			Instrument instrument = treemap.get(i);
			System.out.print(i + "\t");
			System.out.print(instrument.getType() + "\t" + instrument.getSubType() + "\t");
			try {
				double rate = instrument.getRate();
				System.out.println(rate);
			} catch (RateNotAssignedException e)
			{
				System.out.println("N/A");
			}
			
		}

	}

}
