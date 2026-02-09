package yieldcurve.instruments;
import java.util.*;


public class Instruments {
	private static Instruments soleInstance;
	public static Instruments getInstance() {
		if (soleInstance == null)
			soleInstance = new Instruments();
		return soleInstance;
	}
	
	private Instruments() { }
	
	private TreeMap<Integer, Instrument> allInstruments = new TreeMap<Integer, Instrument>();
	
	public void addInstrument(int id , Instrument instrument) {
		allInstruments.put(id, instrument);
	}
	
	public Instrument getInstrument(int id) {
		return this.allInstruments.get(id);
	}
	
	public TreeMap<Integer, Instrument> getAllInstruments() {
		return this.allInstruments;
	}
}

