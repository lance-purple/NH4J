package rec.games.roguelike.nh4j;

public abstract class Advancement {

	int initialConstant;
	int initialVariable;

	int lowLevelConstant;
	int lowLevelVariable;

	int highLevelConstant;
	int highLevelVariable;
	
	public Advancement init(int constant, int variable)
	{
		this.initialConstant = constant;
		this.initialVariable = variable;
		return this;
	}
	
	public Advancement low(int constant, int variable)
	{
		this.lowLevelConstant = constant;
		this.lowLevelVariable = variable;
		return this;
	}
	
	public Advancement high(int constant, int variable)
	{
		this.highLevelConstant = constant;
		this.highLevelVariable = variable;
		return this;
	}
	
	
	public int forLevel(int level, int cutoffLevel)
	{
		if (0 == level)
		{
			return initialConstant + Randomizer.rndIfNonzero(initialVariable);
		} else if (level < cutoffLevel) {
			return lowLevelConstant + Randomizer.rndIfNonzero(lowLevelVariable);
		} else {
			return highLevelConstant + Randomizer.rndIfNonzero(highLevelVariable);
		}
	}

}
