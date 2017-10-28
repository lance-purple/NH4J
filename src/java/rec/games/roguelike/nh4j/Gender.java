package rec.games.roguelike.nh4j;

public enum Gender {
	
	MALE("male", "he", "him", "his", "Mal"),
	FEMALE("female", "she", "her", "her", "Fem"),
	NEUTER("neuter", "it", "it", "its", "Ntr");
	
	private final String adjective;
	private final String subjectivePronoun;
	private final String objectivePronoun;
	private final String possessivePronoun;
	private final String abbreviation;
	
	Gender(String adjective, String subjectivePronoun, String objectivePronoun, String possesivePronoun, String abbreviation)
	{
		this.adjective = adjective;
		this.subjectivePronoun = subjectivePronoun;
		this.objectivePronoun = objectivePronoun;
		this.possessivePronoun = possesivePronoun;
		this.abbreviation = abbreviation;
	}
	
	public String adjective()
	{
		return this.adjective;
	}

	public String subjectivePronoun()
	{
		return this.subjectivePronoun;
	}

	public String objectivePronoun()
	{
		return this.objectivePronoun;
	}

	public String possessivePronoun()
	{
		return this.possessivePronoun;
	}

	public String abbreviation()
	{
		return this.abbreviation;
	}
	
	public static String maleAdjective()
	{
		return MALE.adjective;
	}

	public static String femaleAdjective()
	{
		return FEMALE.adjective;
	}

}
