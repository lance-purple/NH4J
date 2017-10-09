package rec.games.roguelike.nh4j;

public enum NHAttribute {

    STR(0),
    INT(1),
    WIS(2),
    DEX(3),
    CON(4),
    CHA(5);
	
	private int index;
	
	NHAttribute(int index) {
		this.index = index;
	}
	
	int index()
	{
		return index;
	}
	
	public static NHAttribute[] all() {
		return values();
	}

}
