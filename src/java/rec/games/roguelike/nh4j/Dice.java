package rec.games.roguelike.nh4j;

public enum Dice {
	
	D0(0),
	D1(1),
	D2(2),
	D3(3),
	D4(4),
	D5(5),
	D6(6),
	D7(7),
	D8(8),
	D10(10),
	D12(12),
	D20(20),
	D25(25),
	D70(70);
	
	int sides;
	
	Dice(int sides)
	{
		this.sides = sides;
	}
	
	public int sides()
	{
		return sides;
	}

}
