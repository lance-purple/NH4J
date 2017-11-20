package rec.games.roguelike.nh4j;

public class ATTRS {
	
    public static final int STR = 0;
    public static final int INT = 1;
    public static final int WIS = 2;
    public static final int DEX = 3;
    public static final int CON = 4;
    public static final int CHA = 5;
	
	int values[] = new int[6];
	
	public ATTRS() {
		for (int i = 0; i < values.length; i++) {
			values[i] = 0;
		}
	}

	public int get(int attrIndex) {
		return values[attrIndex];
	}

	public void set(int attrIndex, int value) {
		values[attrIndex] = value;
	}

	public void increase(int attrIndex, int delta) {
		values[attrIndex] += delta;
	}

	public void decrease(int attrIndex, int delta) {
		values[attrIndex] -= delta;
	}
	
	public ATTRS STR(int value)
	{
		set(STR, value);
		return this;
	}
	
	public ATTRS STR_18(int value)
	{
		set(STR, ATTRS.STR18(value));
		return this;
	}


	public ATTRS INT(int value)
	{
		set(INT, value);
		return this;
	}

	public ATTRS WIS(int value)
	{
		set(WIS, value);
		return this;
	}

	public ATTRS DEX(int value)
	{
		set(DEX, value);
		return this;
	}

	public ATTRS CON(int value)
	{
		set(CON, value);
		return this;
	}

	public ATTRS CHA(int value)
	{
		set(CHA, value);
		return this;
	}
	
	public static int STR18(int percentile)
	{
		return 18 + percentile;
	}

}
