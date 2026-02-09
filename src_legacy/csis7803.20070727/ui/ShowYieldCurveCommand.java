package ui;
import yieldcurve.core.*;

import java.io.*;
import java.util.*;
import java.text.*;
public class ShowYieldCurveCommand implements Command{
	public String getDescription() {
		return "generate the yield curve to the standard output or to a file";
	}
	public void execute(String [] args) throws CommandException{
		String filename = "";
		try {
			if (args.length > 1)
				throw new CommandException("Usage: showyc [output-file]");
			YieldCurve yc = Session.getInstance().getYieldCurve();
			if (yc == null)
				throw new CommandException("yield curve is not yet generated");
			
			DiscountFactorCurve dfCurve = Session.getInstance().getDiscountFactorCurve();
			
			Iterator<Calendar> itr = yc.keySet().iterator();
			 
			SimpleDateFormat formatter = Session.getInstance().getSimpleDateFormat();
			PrintStream out;
			if (args.length == 1) {
				filename = args[0];
				out = new PrintStream(new FileOutputStream(args[0]));
				while (itr.hasNext()) {
					Calendar day = itr.next();
					//double rate = yc.get(day);
					double df = dfCurve.get(day);
					df = Math.ceil(df * 1000000.0) / 1000000.0;
					out.println(formatter.format(day.getTime()) + "," + df);
					
				}
				out.close();
			}
			else {
				out = System.out;
				while (itr.hasNext()) {
					Calendar day = itr.next();
					//double rate = yc.get(day);
					double df = dfCurve.get(day);
					df = Math.ceil(df * 1000000.0) / 1000000.0;
					out.println(formatter.format(day.getTime()) + ","  + df);
					
				}
			}
			
			
		} catch (FileNotFoundException e) {
			throw new CommandException("Fail to output to " + filename + ": " + e.getMessage());
		}
	}
}
