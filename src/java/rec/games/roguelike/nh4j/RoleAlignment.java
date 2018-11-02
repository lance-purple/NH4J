package rec.games.roguelike.nh4j;

public enum RoleAlignment {
    
	LAWFUL(1, "law", "lawful", "Law", 0x4),
	NEUTRAL(0, "balance", "neutral", "Neu", 0x2),
	CHAOTIC(-1, "chaos", "chaotic", "Cha", 0x1),
	EVIL(-128, "evil", "unaligned", "Una", 0x0);

	private final int type;
	private final String noun;
	private final String adjective;
	private final String fileCode;
	private final int mask;
	
	RoleAlignment(int type, String noun, String adjective, String fileCode, int mask)
	{
		this.type = type;
		this.noun = noun;
		this.adjective = adjective;
		this.fileCode = fileCode;
		this.mask = mask;
	}

	private static RoleAlignment roleAlignment(int index)
	{
		return values()[index];
	}	
	
	public static int numberOfRoleAlignments()
	{
		return values().length;
	}
	

	public static int type(int type)
	{
		return roleAlignment(type).type;
	}

	public static String noun(int index)
	{
		return roleAlignment(index).noun;
	}
	
	public static String adjective(int type)
	{
		return roleAlignment(type).adjective;
	}
	
	public static String fileCode(int type)
	{
		return roleAlignment(type).fileCode;
	}

	public static int mask(int type)
	{
		return roleAlignment(type).mask;
	}
}
