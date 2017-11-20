package rec.games.roguelike.nh4j;

import java.util.HashMap;

public class AdventurerSpecies {

	private static final int HUMAN_ID = 0;
	private static final int ELF_ID = 1;
	private static final int DWARF_ID = 2;
	private static final int GNOME_ID = 3;
	private static final int ORC_ID = 4;

	private static HashMap<Integer, AdventurerSpecies> speciesByID = new HashMap<>();

	private final int speciesID;
	private String speciesNoun;
	private String speciesAdjective;
	private String speciesCollectiveNoun;
	private String fileCode;
	GenderedName individualName;
	
	private PM malePM;
	private PM femalePM;
	private PM mummyPM;
	private PM zombiePM;
	
	private long canStartAs;
	private long selfMask;
	private long friendshipMask = 0L;
	private long hostilityMask = 0L;

	private AdventurerSpecies(int speciesID) {
		this.speciesID = speciesID;
	}

	private AdventurerSpecies withSelf(long selfMask)
	{
		this.selfMask = selfMask;
		return this;
	}
	
	private AdventurerSpecies withNoun(String speciesNoun)
	{
		this.speciesNoun = speciesNoun;
		return this;
	}
	
	private AdventurerSpecies withAdjective(String speciesAdjective)
	{
		this.speciesAdjective = speciesAdjective;
		return this;
	}
	
	private AdventurerSpecies withCollectiveNoun(String speciesCollectiveNoun)
	{
		this.speciesCollectiveNoun = speciesCollectiveNoun;
		return this;
	}
	
	private AdventurerSpecies withFileCode(String fileCode)
	{
		this.fileCode = fileCode;
		return this;
	}
	
	private AdventurerSpecies withIndividualName(String maleVersion, String femaleVersion)
	{
		this.individualName = GenderedName.of(maleVersion, femaleVersion);
		return this;
	}
	
	private AdventurerSpecies withIndividualName(String genericVersion)
	{
		this.individualName = GenderedName.of(genericVersion);
		return this;
	}
	
	private AdventurerSpecies withPM(PM genericPM)
	{
		this.malePM = genericPM;
		this.femalePM = genericPM;
		return this;
	}

	private AdventurerSpecies withPM(PM malePM, PM femalePM)
	{
		this.malePM = malePM;
		this.femalePM = femalePM;
		return this;
	}
	
	private AdventurerSpecies withMummyPM(PM mummyPM)
	{
		this.mummyPM = mummyPM;
		return this;
	}
	
	private AdventurerSpecies withZombiePM(PM zombiePM)
	{
		this.zombiePM = zombiePM;
		return this;
	}
	
	private AdventurerSpecies canStartAs(long canStartAs)
	{
		this.canStartAs = canStartAs;
		return this;
	}
	
	private AdventurerSpecies friendship(long friendshipMask)
	{
		this.friendshipMask = friendshipMask;
		return this;
	}
	
	private AdventurerSpecies hostility(long hostilityMask)
	{
		this.hostilityMask = hostilityMask;
		return this;
	}
		
	private void add() {
		AdventurerSpecies.speciesByID.put(this.speciesID, this);
	}
	
	private static AdventurerSpecies withID(int roleID) {
		return new AdventurerSpecies(roleID);
	}

	private static AdventurerSpecies getSpecies(int speciesID) {
		if (speciesByID.isEmpty()) {
			initialize();
		}
		if (speciesByID.containsKey(speciesID)) {
			return speciesByID.get(speciesID);
		}
		return null;
	}

	public static int numberOfPlayableSpecies() {
		if (speciesByID.isEmpty()) {
			initialize();
		}
		return speciesByID.size();
	}

	public static long selfMask(int speciesID) {
		AdventurerSpecies species = getSpecies(speciesID);
		if (null != species)
		{
			return species.selfMask;
		}
		return 0L;				
	}

	public static String speciesNoun(int speciesID) {
		AdventurerSpecies species = getSpecies(speciesID);
		if (null != species)
		{
			return species.speciesNoun;
		}
		return "(unknown)";
	}
	
	public static String speciesAdjective(int speciesID) {
		AdventurerSpecies species = getSpecies(speciesID);
		if (null != species)
		{
			return species.speciesAdjective;
		}
		return "(unknown)";
	}

	public static String speciesFileCode(int speciesID) {
		AdventurerSpecies species = getSpecies(speciesID);
		if (null != species)
		{
			return species.fileCode;
		}
		return "(unknown)";
	}

	public static boolean individualNameHasMaleVersion(int speciesID) {
		AdventurerSpecies species = getSpecies(speciesID);
		if ((null != species) && (null != species.individualName))
		{
			return species.individualName.hasMaleVersion();
		}
		return false;
	}
	
	public static String individualNameAsMale(int speciesID) {
		AdventurerSpecies species = getSpecies(speciesID);
		if ((null != species) && (null != species.individualName))
		{
			return species.individualName.asMale();
		}
		return "(unknown)";
	}
	
	public static boolean individualNameHasFemaleVersion(int speciesID) {
		AdventurerSpecies species = getSpecies(speciesID);
		if ((null != species) && (null != species.individualName))
		{
			return species.individualName.hasFemaleVersion();
		}
		return false;
	}

	public static String individualNameAsFemale(int speciesID) {
		AdventurerSpecies species = getSpecies(speciesID);
		if ((null != species) && (null != species.individualName))
		{
			return species.individualName.asFemale();
		}
		return "(unknown)";
	}
	
	public static boolean pmidHasFemaleVersion(int speciesID) {
		return PM.UNKNOWN.id() != pmidAsFemale(speciesID);
	}
	
	public static int pmidAsFemale(int speciesID) {
		AdventurerSpecies species = getSpecies(speciesID);
		if ((null != species) && (null != species.femalePM))
		{
			return species.femalePM.id();
		}
		return PM.UNKNOWN.id();
	}

	public static boolean pmidHasMaleVersion(int speciesID) {
		return PM.UNKNOWN.id() != pmidAsMale(speciesID);
	}
	
	public static int pmidAsMale(int speciesID) {
		AdventurerSpecies species = getSpecies(speciesID);
		if ((null != species) && (null != species.malePM))
		{
			return species.malePM.id();
		}
		return PM.UNKNOWN.id();
	}

	public static boolean pmidHasMummyVersion(int speciesID) {
		return PM.UNKNOWN.id() != pmidAsMummy(speciesID);
	}
	
	public static int pmidAsMummy(int speciesID) {
		AdventurerSpecies species = getSpecies(speciesID);
		if ((null != species) && (null != species.mummyPM))
		{
			return species.mummyPM.id();
		}
		return PM.UNKNOWN.id();
	}

	public static boolean pmidHasZombieVersion(int speciesID) {
		return PM.UNKNOWN.id() != pmidAsZombie(speciesID);
	}
	
	public static int pmidAsZombie(int speciesID) {
		AdventurerSpecies species = getSpecies(speciesID);
		if ((null != species) && (null != species.zombiePM))
		{
			return species.zombiePM.id();
		}
		return PM.UNKNOWN.id();
	}

	public static long startingMask(int speciesID) {
		AdventurerSpecies species = getSpecies(speciesID);
		if (null != species)
		{
			return species.canStartAs | species.selfMask;
		}
		return 0L;				
	}

	public static long startingAlignmentMask(int speciesID) {
        return startingMask(speciesID) & AM.MASK;
	}

	public static long startingGenderMask(int speciesID) {
        return startingMask(speciesID) & Gender.MASK;
	}

	public static boolean canStartAsChaotic(int speciesID) {
		return (startingAlignmentMask(speciesID) & AM.CHAOTIC) != 0L;
	}

	public static boolean canStartAsNeutral(int speciesID) {
		return (startingAlignmentMask(speciesID) & AM.NEUTRAL) != 0L;
	}

	public static boolean canStartAsLawful(int speciesID) {
		return (startingAlignmentMask(speciesID) & AM.LAWFUL) != 0L;
	}
	
	public static long friendshipMask(int speciesID) {
		AdventurerSpecies species = getSpecies(speciesID);
		if (null != species)
		{
			return species.friendshipMask;
		}
		return 0L;
	}

	public static long hostilityMask(int speciesID) {
		AdventurerSpecies species = getSpecies(speciesID);
		if (null != species)
		{
			return species.hostilityMask;
		}
		return 0L;
	}

	public static final int MASK = 0x0ff8;

	private static void initialize() {

		AdventurerSpecies.withID(HUMAN_ID)
		    .withSelf(M2.HUMAN)
		    .withNoun("human")
		    .withAdjective("human")
		    .withCollectiveNoun("humanity")
			.withFileCode("Hum")
			.withIndividualName("man", "woman")
			.withPM(PM.HUMAN)
			.withMummyPM(PM.HUMAN_MUMMY)
			.withZombiePM(PM.HUMAN_ZOMBIE)
			.canStartAs(Gender.MALE.mask() | Gender.FEMALE.mask() |
				    AM.LAWFUL | AM.NEUTRAL | AM.CHAOTIC)
			.hostility(M2.GNOME | M2.ORC)
			.add();

		AdventurerSpecies.withID(ELF_ID)
		.withSelf(M2.ELF)
	    .withNoun("elf")
	    .withAdjective("elven")
	    .withCollectiveNoun("elvenkind")
		.withFileCode("Elf")
		.withIndividualName("elf")
		.withPM(PM.ELF)
		.withMummyPM(PM.ELF_MUMMY)
		.withZombiePM(PM.ELF_ZOMBIE)
		.canStartAs(Gender.MALE.mask() | Gender.FEMALE.mask() |
			    AM.CHAOTIC)
		.friendship(M2.ELF)
		.hostility(M2.ORC)
		.add();

		AdventurerSpecies.withID(DWARF_ID)
		.withSelf(M2.DWARF)
	    .withNoun("dwarf")
	    .withAdjective("dwarven")
	    .withCollectiveNoun("dwarvenkind")
		.withFileCode("Dwa")
		.withIndividualName("dwarf")
		.withPM(PM.DWARF)
		.withMummyPM(PM.DWARF_MUMMY)
		.withZombiePM(PM.DWARF_ZOMBIE)
		.canStartAs(Gender.MALE.mask() | Gender.FEMALE.mask() |
			    AM.LAWFUL)
		.friendship(M2.DWARF | M2.GNOME)
		.hostility(M2.ORC)
		.add();
		
		AdventurerSpecies.withID(GNOME_ID)
		.withSelf(M2.GNOME)
	    .withNoun("gnome")
	    .withAdjective("gnomish")
	    .withCollectiveNoun("gnomehood")
		.withFileCode("Gno")
		.withIndividualName("gnome")
		.withPM(PM.GNOME)
		.withMummyPM(PM.GNOME_MUMMY)
		.withZombiePM(PM.GNOME_ZOMBIE)
		.canStartAs(Gender.MALE.mask() | Gender.FEMALE.mask() |
			    AM.NEUTRAL)
		.friendship(M2.DWARF | M2.GNOME)
		.hostility(M2.HUMAN)
		.add();

		AdventurerSpecies.withID(ORC_ID)
		.withSelf(M2.ORC)
	    .withNoun("orc")
	    .withAdjective("orcish")
	    .withCollectiveNoun("orcdom")
		.withFileCode("Orc")
		.withIndividualName("orc")
		.withPM(PM.ORC)
		.withMummyPM(PM.ORC_MUMMY)
		.withZombiePM(PM.ORC_ZOMBIE)
		.canStartAs(Gender.MALE.mask() | Gender.FEMALE.mask() |
			    AM.CHAOTIC)
		.hostility(M2.HUMAN | M2.ELF | M2.DWARF)
		.add();
	}
}
