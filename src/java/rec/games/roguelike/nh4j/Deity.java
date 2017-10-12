package rec.games.roguelike.nh4j;

public class Deity {

	public static final Deity AMATERASU_OMIKAMI = male("Amaterasu Omikami");
	public static final Deity ANHUR = male("Anhur");
	public static final Deity ANSHAR = male("Anshar");
	public static final Deity ANU = male("Anu");
	public static final Deity ATHENA = female("Athena");
	public static final Deity BLIND_IO = male("Blind Io");
	public static final Deity BRIGIT = female("Brigit");
	public static final Deity CAMAXTLI = male("Camaxtli");
	public static final Deity CHIH_SUNG_TZU = male("Chih Sung-tzu");
	public static final Deity CROM = male("Crom");
	public static final Deity HERMES = male("Hermes");
	public static final Deity HUAN_TI = male("Huan Ti");
	public static final Deity HUHETOTL = male("Huhetotl");
	public static final Deity ISHTAR = female("Ishtar");
	public static final Deity ISSEK = male("Issek");
	public static final Deity KOS = male("Kos");
	public static final Deity LOKI = male("Loki");
	public static final Deity LUGH = male("Lugh");
	public static final Deity MANANNAN_MAC_LIR = male("Manannan Mac Lir");
	public static final Deity MARS = male("Mars");
	public static final Deity MITRA = male("Mitra");
	public static final Deity MERCURY = male("Mercury");
	public static final Deity MOG = male("Mog");
	public static final Deity MOLOCH = male("Moloch");
	public static final Deity ODIN = male("Odin");
	public static final Deity OFFLER = male("Offler");
	public static final Deity POSEIDON = male("Poseidon");
	public static final Deity PTAH = male("Ptah");
	public static final Deity QUETZALCOATL = male("Quetzalcoatl");
	public static final Deity RAIJIN = male("Raijin");
	public static final Deity SET = male("Set");
	public static final Deity SHAN_LAI_CHING = male("Shan Lai Ching");
	public static final Deity SUSANOWO = male("Susanowo");
	public static final Deity THE_LADY = female("The Lady");
	public static final Deity THOTH = male("Thoth");
	public static final Deity TYR = male("Tyr");
	public static final Deity VENUS = female("Venus");
	

	private static final char FEMALE = 'f';
	private static final char MALE = 'm';

	private final String name;
	private final char gender;

	private Deity(String name, char gender) {
		this.name = name;
		this.gender = gender;
	}

	public String name() {
		return name;
	}

	public boolean male() {
		return (MALE == gender);
	}

	public boolean female() {
		return (FEMALE == gender);
	}

	public static Deity male(String name) {
		return new Deity(name, MALE);
	}

	public static Deity female(String name) {
		return new Deity(name, FEMALE);
	}

}
