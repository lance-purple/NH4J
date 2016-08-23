package rec.games.roguelike.nh4j;

import java.util.HashMap;

public class NHAttributeSet {
	
    public static final int STR = 0;
    public static final int INT = 1;
    public static final int WIS = 2;
    public static final int DEX = 3;
    public static final int CON = 4;
    public static final int CHA = 5;
	
	HashMap<NHAttribute, Integer> values = new HashMap<>();
	
	public NHAttributeSet() {
		for (NHAttribute attr : NHAttribute.all()) {
			values.put(attr, 0);
		}
	}
	
	public int get(int attrIndex) {
		NHAttribute attr = NHAttribute.all()[attrIndex];
		return values.get(attr);
	}

	public void set(int attrIndex, int value) {
		NHAttribute attr = NHAttribute.all()[attrIndex];
		values.put(attr, value);
	}

	public void increase(int attrIndex, int delta) {
		NHAttribute attr = NHAttribute.all()[attrIndex];
		values.put(attr, values.get(attr) + delta);
	}

	public void decrease(int attrIndex, int delta) {
		NHAttribute attr = NHAttribute.all()[attrIndex];
		values.put(attr, values.get(attr) - delta);
	}

}
