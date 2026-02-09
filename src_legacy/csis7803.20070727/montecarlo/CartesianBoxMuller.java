package montecarlo;
public class CartesianBoxMuller implements Randomizer {
	
	private double v = Math.random();
	private Double someNum = null;
	private double generateBoxMullerNumbers(double v) {
	
		if (someNum != null) {
			double ret = someNum;
			someNum = null;
			return ret;
		}
 		double r, x, y;
	//	Random random = new Random();
		while (true) {
			
			x = Math.random() * 2 - 1;
			y = Math.random() * 2 - 1;
			//x = random.nextDouble();
			//y = random.nextDouble();

			
			r = x*x + y*y;
			if (r < 1 && r >= 0) 
				break;
		}
		
		someNum = y * Math.sqrt(-2 * Math.log(r) / r);;
		return x * Math.sqrt(-2 * Math.log(r) / r);
	} 
	
	
	
	
	
	
	public CartesianBoxMuller() {
	}

	public double rand() {
		v = generateBoxMullerNumbers(v);
		return v;
	}
	
}