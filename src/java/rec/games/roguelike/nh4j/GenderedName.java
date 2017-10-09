package rec.games.roguelike.nh4j;

public class GenderedName {
	
	private static final int MALE = 0x1000;
	private static final int FEMALE = 0x2000;
			
	private final String maleVersion;
	private final String femaleVersion;
	private final boolean identical;

	private GenderedName(String maleVersion, String femaleVersion)
	{
		this.maleVersion = maleVersion;
		this.femaleVersion = femaleVersion;
		identical = (maleVersion.equals(femaleVersion));
	}
	
	private GenderedName(String genericVersion)
	{
		this.maleVersion = genericVersion;
		this.femaleVersion = genericVersion;
		identical = true;
	}
	
	public String asMale()
	{
		return maleVersion;
	}
	
	public String asFemale()
	{
		return femaleVersion;
	}
	
	public String as(int gender)
	{
		if (MALE == gender)
		{
			return asMale();
		}
		else if (FEMALE == gender)
		{
			return asFemale();
		}
		throw new IllegalArgumentException("Unknown gender " + gender);
	}
	
	public boolean identical()
	{
		return identical;
	}
	
	public static GenderedName of(String genericVersion)
	{
	    return new GenderedName(genericVersion);
	}

	public static GenderedName of(String maleVersion, String femaleVersion)
	{
	    return new GenderedName(maleVersion, femaleVersion);
	}
}
