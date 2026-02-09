package montecarlo;

public class MonteCarloSimulator {
	
	/**
	 * 
	 * @param S
	 * @param driftRate
	 * @param deltaT
	 * @param risk
	 * @param randomizer
	 * @param numberOfIterations
	 * @return a map from time (counting from 0 to T  in years) to Stock Prices
	 */
	public static StockPath generatePath(double initPrice, double driftRate, double deltaT, double risk, Randomizer randomizer, int numberOfTimeSteps){
		double t = 0;
		
		double[] prices = new double[numberOfTimeSteps];
		double[] epsilons = new double[numberOfTimeSteps];;
		
				
		double epsilon;
		double price = initPrice;
//		double antitheticPrice = initPrice;
		double muDeltaT = driftRate * deltaT;
		double sqrtDeltaT = Math.sqrt(deltaT);
		double riskSqrtDeltaT = sqrtDeltaT * risk;
		for (int i = 0; i < numberOfTimeSteps; i++) {
			//System.out.println(i + "," + S);
			t += deltaT;
			epsilon = randomizer.rand();
			
			price += price * (muDeltaT + riskSqrtDeltaT * epsilon);
			prices[i] = price;
			//System.out.print(prices[i] + "\t");
			//S += S * (driftRate * deltaT);
			//System.out.println("rand =" + rand);
			//System.out.println(S);
			epsilons[i] = epsilon;
			
		}
		
		return new StockPath(initPrice, epsilons, prices);
	}
	 
	public static StockPath generateAntitheticPath(final double initPrice, final double driftRate, final double deltaT, final double risk, final double[] determinedEpsilons, final int numberOfTimeSteps){
		double t = 0;
		
		double[] prices = new double[numberOfTimeSteps];
		double[] epsilons = new double[numberOfTimeSteps];;
		double muDeltaT = driftRate * deltaT;
		double sqrtDeltaT = Math.sqrt(deltaT);
		double riskSqrtDeltaT = sqrtDeltaT * risk;
		double epsilon;
		double price = initPrice;
		for (int i = 0; i < numberOfTimeSteps; i++) {
			//System.out.println(i + "," + S);
			t += deltaT;
			epsilon = - determinedEpsilons[i];
			
			
			double deltaS = price * (muDeltaT + riskSqrtDeltaT * epsilon);
			price += deltaS;
			//S += S * (driftRate * deltaT);
			//System.out.println("rand =" + rand);
			//System.out.println(S);
			epsilons[i] = epsilon;
			prices[i] = price;
		}
		
		return new StockPath(initPrice, epsilons, prices);
	}
	
	
	
/*
	public static void main(String args[]) {
		if (args.length != 6) {
			System.out.println("Usage: java initPrice expectedReturn deltaT risk numberOfIterations time");
			System.exit(1);
		}
		long timeAllowed = Long.parseLong(args[5]);
		double initPrice= Double.parseDouble(args[0]);
		System.out.println("Init Stock Price = " + initPrice);
		double expectedReturn = Double.parseDouble(args[1]);
		System.out.println("ExpectedReturn = " + expectedReturn);
		double deltaT = Double.parseDouble(args[2]);
		System.out.println("Delta T = " + deltaT);
		double risk = Double.parseDouble(args[3]);
		System.out.println("Risk = " + risk);
		int numberOfTimeSteps = Integer.parseInt(args[4]);
		
		System.out.println("# of time steps = " + numberOfTimeSteps);
		
		System.out.println("total length = " + (deltaT * numberOfTimeSteps));
		//10
		System.out.println("Start pricing... the call price in the future should be [" + 7 * Math.exp((deltaT * numberOfTimeSteps)*expectedReturn) + "]");
	
		approximatePayOut((long)Math.pow(10,15),initPrice, expectedReturn, deltaT, risk, numberOfTimeSteps, timeAllowed);
	
		
	}
	*/
	
	/*
	private static void printMeanAndVar(List<Double> list) {
		double sum = 0;
		for (int i = 0; i < list.size(); i++) {
			sum += list.get(i);
		}
		double mean = sum / (0.0+list.size());
		double sumOfSquares = 0.0;
		for (int i = 0; i < list.size(); i++) {
			double squareDiff = (list.get(i) - mean);
			squareDiff *= squareDiff;
			sumOfSquares += squareDiff;
		}
		double sd = Math.sqrt(sumOfSquares / (0.0 + list.size()));
		System.out.println("Count = " + list.size());
		System.out.println("Mean = " + mean);
		System.out.println("SD = " + sd);
		
	}*/
	/*
	private static long approximatePayOut(long loops, double initPrice, double expectedReturn, double deltaT, double risk, int numberOfTimeSteps, long timeAllowed) {
		double sum = 0;
		Randomizer rand = new AverageOfUniformNumbers();
		long i = 0;
		ArrayList<Double> list = new ArrayList<Double>();
		long start= Calendar.getInstance().getTimeInMillis();
		double[] epsilons = null;
		try {
			
			 
			System.out.println("StartTime = " + start);
			System.out.println(loops + " loops");
			
			StockPath path = null;
			for (i = 0; i < loops; i++) {
				
					path = generatePath(initPrice, expectedReturn, deltaT, risk,  rand, numberOfTimeSteps, true );
				
			//System.out.println(trial);
				
				double finalPrice = path.getFinalPrice(); 
				
				double payout = Math.max(0, finalPrice - 27.5);
				payout /= Math.exp(expectedReturn * deltaT * numberOfTimeSteps);
				list.add(payout);
					//System.out.println("stock =" + finalPrice +"; payout=" + payout);
				//list.add(payout);
				sum += payout;
				if ((i+1) % 100 == 0 && i>0) {
					System.out.println("--------------------checkpoint--------------");
					System.out.println("COUNT = " + (i+1)  );
					double avg = sum / (i+1);
					System.out.println("Average Payoff discounted (call price) = " + avg);
					long now = Calendar.getInstance().getTimeInMillis();
					System.out.println("time used in " + (i+1) + " iterations =  " + (now-start));
					printMeanAndVar(list);
					//start = now;
					
					System.out.println("------------end-----------------------------");
				}
			}
		} catch (java.lang.OutOfMemoryError e) {
			e.printStackTrace();
		}
		long end = Calendar.getInstance().getTimeInMillis();
		System.out.println("EndTime = " + end);
		//printMeanAndVar(list);
		System.out.println("COUNT = " + loops);
		double avg = sum / loops;
		System.out.println("Average Payoff at maturity = " + avg);
		System.out.println("Discounted Price (Call Price) = " + avg / Math.exp(expectedReturn * deltaT * numberOfTimeSteps));
		System.out.println("endtime - starttime =  " + (end-start));
		System.out.println("----------");
		return end-start;
	}
	*/
}