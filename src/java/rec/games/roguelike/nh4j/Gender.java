package rec.games.roguelike.nh4j;

import java.util.NoSuchElementException;

public enum Gender {
	
	MALE(0,   "male",   "he",  "him", "his", "himself", "Mal", 0x1000),
	FEMALE(1, "female", "she", "her", "her", "herself", "Fem", 0x2000),
	NEUTER(2, "neuter", "it",  "it",  "its", "itself",  "Ntr", 0x4000);
	
	private final int id;
	private final String adjective;
	private final String subjectivePronoun;
	private final String objectivePronoun;
	private final String possessivePronoun;
	private final String reflexivePronoun;
	private final String abbreviation;
	private final int mask;
	
	Gender(int id, String adjective, String subjectivePronoun, String objectivePronoun, String possesivePronoun, String reflexivePronoun, String abbreviation, int mask)
	{
		this.id = id;
		this.adjective = adjective;
		this.subjectivePronoun = subjectivePronoun;
		this.objectivePronoun = objectivePronoun;
		this.possessivePronoun = possesivePronoun;
		this.reflexivePronoun = reflexivePronoun;
		this.abbreviation = abbreviation;
		this.mask = mask;
	}
	
	public int id()
	{
		return this.id;
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

	public String reflexivePronoun()
	{
		return this.reflexivePronoun;
	}

	public String abbreviation()
	{
		return this.abbreviation;
	}

	public int mask()
	{
		return this.mask;
	}

	public static int adventurerGenders()
	{
		return 2; // don't support NEUTER;
	}
	
	public static Gender withID(int id)
	{
		for (Gender gender : values())
		{
			if (id == gender.id)
			{
				return gender;
			}
		}
		throw new NoSuchElementException();
	}

	public static String adjective(int genderID)
	{
		return Gender.withID(genderID).adjective();
	}
	
	public static String subjectivePronoun(int genderID)
	{
		return Gender.withID(genderID).subjectivePronoun();
	}

	public static String objectivePronoun(int genderID)
	{
		return Gender.withID(genderID).objectivePronoun();
	}

	public static String possessivePronoun(int genderID)
	{
		return Gender.withID(genderID).possessivePronoun();
	}

	public static String reflexivePronoun(int genderID)
	{
		return Gender.withID(genderID).reflexivePronoun();
	}

	public static String abbreviation(int genderID)
	{
		return Gender.withID(genderID).abbreviation();
	}

	public static int mask(int genderID)
	{
		return Gender.withID(genderID).mask();
	}
	
	public static final long MASK = 0xf000;

}
