package ui;
import java.util.*;
public class Test {
	public static void main(String args[]) {
		//double df0 = 0.9908596589813964;
		//double t0 = 0.2493150684931507;
		//double t1 = 0.25205479452054796;
		//double t = t0 + t1;
		//double rate = 0.041;
		
		
		
		double df0 = 0.9908596589813964;
		//double df1 = 0.9897714577028625;
		double df = 0.9807246090489778;
		
		
		//double df1 = 1/(1+rate*t1);
		//System.out.println(df1);
		//double df = 0.9807246090489778;//df0 * df1; 
		//System.out.println(df);
		Calendar today = Session.getInstance().getStartDay();
		Calendar start = Session.getInstance().getBusinessDayConvention().getBusinessDayAfter(today, Calendar.MONTH, 2);
		Calendar end = Session.getInstance().getBusinessDayConvention().getBusinessDayAfter(today, Calendar.MONTH, 5);
		double	t1 = 0.25205479452054796;
			System.out.println(t1);
		t1 = Session.getInstance().getDayCountConvention().deltaTInBetween(start, end);
				System.out.println(t1);
		double result = Math.ceil((df0/df - 1)/t1 * 100000)/1000.0;
		System.out.println(result);
		
		
	}
}
