package rec.games.roguelike.nh4j;

public class TributeNovel {

	public static final int MAXIMUM_PASSAGES = 30;

	public static boolean enabled;
	public static boolean stocked;
	public static boolean noticedByDeath;
	
	public static int id;
	public static int passages[] = new int[MAXIMUM_PASSAGES];
    public static int count;
	
	public static int maximumPassages()
	{
		return MAXIMUM_PASSAGES;
	}

	public static boolean enabled()
	{
		return enabled;
	}

	public static void setEnabled(boolean e)
	{
		enabled = e;
	}

	public static boolean stocked()
	{
		return stocked;
	}

	public static void setStocked(boolean s)
	{
		stocked = s;
	}

	public static boolean noticedByDeath()
	{
		return noticedByDeath;
	}

	public static void setNoticedByDeath(boolean n)
	{
		noticedByDeath = n;
	}

	public static int id()
	{
		return id;
	}

	public static void setId(int i)
	{
		id = i;
	}

	public static int passage(int index)
	{
		return passages[index];
	}

	public static void setPassage(int index, int value)
	{
		passages[index] = value;
	}

	public static int count()
	{
		return count;
	}

	public static void setCount(int c)
	{
		count = c;
	}

	public static void increaseCount(int c)
	{
		count += c;
	}

	public static void decreaseCount(int c)
	{
		count -= c;
	}

}