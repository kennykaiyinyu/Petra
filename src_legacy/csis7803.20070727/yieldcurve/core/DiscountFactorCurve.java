/**
 * 
 */
package yieldcurve.core;

import java.util.Calendar;
import java.util.Collection;
import java.util.Comparator;
import java.util.Map;
import java.util.Set;
import java.util.SortedMap;
import java.util.TreeMap;

/**
 * @author kykyu
 *
 */
public class DiscountFactorCurve implements SortedMap<Calendar, Double> {

	private SortedMap<Calendar, Double> theMap;
	/**
	 * 
	 */
	public DiscountFactorCurve() {
		theMap = new TreeMap<Calendar, Double>();
	}
	
	public DiscountFactorCurve(SortedMap<Calendar, Double> theMap) {
		this.theMap = theMap;
	}

	/* (non-Javadoc)
	 * @see java.util.SortedMap#comparator()
	 */
	public Comparator<? super Calendar> comparator() {
		return theMap.comparator();
	}

	/* (non-Javadoc)
	 * @see java.util.SortedMap#firstKey()
	 */
	public Calendar firstKey() {
		return theMap.firstKey();
	}

	/* (non-Javadoc)
	 * @see java.util.SortedMap#headMap(java.lang.Object)
	 */
	public SortedMap<Calendar, Double> headMap(Calendar toKey) {
		return theMap.headMap(toKey);
	}

	/* (non-Javadoc)
	 * @see java.util.SortedMap#lastKey()
	 */
	public Calendar lastKey() {
		return theMap.lastKey();
	}

	/* (non-Javadoc)
	 * @see java.util.SortedMap#subMap(java.lang.Object, java.lang.Object)
	 */
	public SortedMap<Calendar, Double> subMap(Calendar fromKey, Calendar toKey) {
		return theMap.subMap(fromKey, toKey);
	}

	/* (non-Javadoc)
	 * @see java.util.SortedMap#tailMap(java.lang.Object)
	 */
	public SortedMap<Calendar, Double> tailMap(Calendar fromKey) {
		return this.theMap.tailMap(fromKey);
	}

	/* (non-Javadoc)
	 * @see java.util.Map#clear()
	 */
	public void clear() {
		theMap.clear();
	}

	/* (non-Javadoc)
	 * @see java.util.Map#containsKey(java.lang.Object)
	 */
	public boolean containsKey(Object key) {
		return this.theMap.containsKey(key);
	}

	/* (non-Javadoc)
	 * @see java.util.Map#containsValue(java.lang.Object)
	 */
	public boolean containsValue(Object value) {
		return this.theMap.containsValue(value);
	}

	/* (non-Javadoc)
	 * @see java.util.Map#entrySet()
	 */
	public Set<java.util.Map.Entry<Calendar, Double>> entrySet() {
		return this.theMap.entrySet();
	}

	/* (non-Javadoc)
	 * @see java.util.Map#get(java.lang.Object)
	 */
	public Double get(Object key) {
		return this.theMap.get(key);
	}

	/* (non-Javadoc)
	 * @see java.util.Map#isEmpty()
	 */
	public boolean isEmpty() {
		return this.theMap.isEmpty();
	}

	/* (non-Javadoc)
	 * @see java.util.Map#keySet()
	 */
	public Set<Calendar> keySet() {
		return this.theMap.keySet();
	}

	/* (non-Javadoc)
	 * @see java.util.Map#put(java.lang.Object, java.lang.Object)
	 */
	public Double put(Calendar key, Double value) {
		return theMap.put(key, value);
	}

	/* (non-Javadoc)
	 * @see java.util.Map#putAll(java.util.Map)
	 */
	public void putAll(Map<? extends Calendar, ? extends Double> t) {
		this.theMap.putAll(t);
	}

	/* (non-Javadoc)
	 * @see java.util.Map#remove(java.lang.Object)
	 */
	public Double remove(Object key) {
		return theMap.remove(key);
	}

	/* (non-Javadoc)
	 * @see java.util.Map#size()
	 */
	public int size() {
		return theMap.size();
	}

	/* (non-Javadoc)
	 * @see java.util.Map#values()
	 */
	public Collection<Double> values() {
		return theMap.values();
	}
	
	


}
