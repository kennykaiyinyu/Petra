package ui.options;
import montecarlo.StockPath;

import java.util.*;
abstract  public class Option {
	abstract public double getPayoff(java.util.Map<String, StockPath> paths) ;
	abstract public String getName() ;
	abstract public String getDescription();
	abstract public String[] getUnderlyingStockNames();
	abstract public Calendar getMaturityDate( );
	abstract public double getSellingPrice();
	abstract public void setSellingPrice(double price);
}
