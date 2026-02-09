package montecarlo;

public class AverageOfUniformNumbers implements Randomizer {
	
	private java.util.Random random = new java.util.Random();

	public double rand() {
		double rand = 0;
		for (int i = 0; i < 12; i++)
			rand += random.nextDouble();
		rand -= 6.0;
		return rand;
	}
	
	

}
