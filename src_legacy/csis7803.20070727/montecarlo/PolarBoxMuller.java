
package montecarlo;
import java.util.*;
public class PolarBoxMuller implements Randomizer {
	private Random randomizer = new Random();
	private double v = randomizer.nextDouble();
	private Double someNum;
	private double generateBoxMullerNumbers(double v) {
	
		double x, y;
	//	Random random = new Random();
		
		if (someNum != null) {
			double ret = someNum.doubleValue();
			someNum = null;
			return ret;
		}
		//x = Math.random();
		x = randomizer.nextDouble();
		//y = Math.random();
		y = randomizer.nextDouble();
			//x = random.nextDouble();
			//y = random.nextDouble();
	
		
		someNum = Math.sqrt(-2 * Math.log(x)) * Math.cos(2 * Math.PI * y);
		return Math.sqrt(-2 * Math.log(x)) * Math.sin(2 * Math.PI * y);
		
	} 
	
	
	

	
	
	public PolarBoxMuller() {
	}

	public double rand() {
		v = generateBoxMullerNumbers(v);
		return v;
	}
	
	// Test Driver
	public static void main(String args[]) {
		//ArrayList<Double> list = new ArrayList<Double>();
		Randomizer r = new PolarBoxMuller();
		//double sum = 0;
		long start = java.util.Calendar.getInstance().getTimeInMillis();
		for (int i=0 ; i < 1000000; i++) {
			 
			double n = r.rand();
			System.out.println(n);
//			list.add(n);
//			sum += n;
		}
		long end = java.util.Calendar.getInstance().getTimeInMillis();
		System.out.println((end - start)/1000000.0 + " milliSec");
		/*
		double mean = sum / (0.0+list.size()+0.0);
		System.out.println("mean = "+mean);
		Iterator<Double> itr = list.iterator();
		double sumOfSqDiff = 0;
		while (itr.hasNext()) {
			double next = itr.next();
			double sqDiff = next - mean;
			sqDiff *=sqDiff;
			sumOfSqDiff += sqDiff;
		}
		double sd = Math.sqrt(sumOfSqDiff/(0.0+list.size()));
		System.out.println("sd = " + sd);*/
	}
	
}