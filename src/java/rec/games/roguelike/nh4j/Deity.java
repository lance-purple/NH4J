package rec.games.roguelike.nh4j;

public enum Deity {

	AMATERASU_OMIKAMI(Gender.FEMALE, "Amaterasu Omikami"),
	ANHUR(Gender.MALE, "Anhur"),
	ANSHAR(Gender.MALE, "Anshar"),
	ANU(Gender.MALE, "Anu"),
	ATHENA(Gender.FEMALE, "Athena"),
	BLIND_IO(Gender.MALE, "Blind Io"),
	BRIGIT(Gender.FEMALE, "Brigit"),
	CAMAXTLI(Gender.MALE, "Camaxtli"),
	CHIH_SUNG_TZU(Gender.MALE, "Chih Sung-tzu"),
	CROM(Gender.MALE, "Crom"),
	HERMES(Gender.MALE, "Hermes"),
	HUAN_TI(Gender.MALE, "Huan Ti"),
	HUHETOTL(Gender.MALE, "Huhetotl"),
	ISHTAR(Gender.FEMALE, "Ishtar"),
	ISSEK(Gender.MALE, "Issek"),
	KOS(Gender.MALE, "Kos"),
	LOKI(Gender.MALE, "Loki"),
	LUGH(Gender.MALE, "Lugh"),
	MANANNAN_MAC_LIR(Gender.MALE, "Manannan Mac Lir"),
	MARS(Gender.MALE, "Mars"),
	MITRA(Gender.MALE, "Mitra"),
	MERCURY(Gender.MALE, "Mercury"),
	MOG(Gender.MALE, "Mog"),
    MOLOCH(Gender.MALE, "Moloch"),
	ODIN(Gender.MALE, "Odin"),
	OFFLER(Gender.MALE, "Offler"),
	POSEIDON(Gender.MALE, "Poseidon"),
	PTAH(Gender.MALE, "Ptah"),
	QUETZALCOATL(Gender.MALE, "Quetzalcoatl"),
	RAIJIN(Gender.MALE, "Raijin"),
	SET(Gender.MALE, "Set"),
	SHAN_LAI_CHING(Gender.MALE, "Shan Lai Ching"),
	SUSANOWO(Gender.MALE, "Susanowo"),
	THE_LADY(Gender.FEMALE, "The Lady"),
	THOTH(Gender.MALE, "Thoth"),
	TYR(Gender.MALE, "Tyr"),
	VENUS(Gender.FEMALE, "Venus");
	

	private final String deityName;
	private final Gender gender;
	
	private Deity(Gender gender, String name) {
		this.gender = gender;
		this.deityName = name;
	}

	public String deityName() {
		return deityName;
	}

	public boolean male() {
		return (Gender.MALE == gender);
	}

	public boolean female() {
		return (Gender.FEMALE == gender);
	}

	public static String title(Deity deity) {
		if (null != deity)
		{
			if (deity.female())
			{
				return "goddess";
			}
		}
		return "god";
	}
	
	public static Deity random() {
		int index = Randomizer.rn2(values().length);
		return values()[index];
	}

}
