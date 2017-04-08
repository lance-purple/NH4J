package rec.games.roguelike.nh4j;

public class TributeNovel {

	public static final int MAXIMUM_PASSAGES = 30;

	public static int id;
	public static int passages[] = new int[MAXIMUM_PASSAGES];
    public static int count;
	
	public static int maximumPassages()
	{
		return MAXIMUM_PASSAGES;
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