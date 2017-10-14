package rec.games.roguelike.nh4j;

public enum Pantheon {

    BABYLONIAN(Deity.ANU, Deity.ISHTAR, Deity.ANSHAR),
    CELTIC(Deity.LUGH, Deity.BRIGIT, Deity.MANANNAN_MAC_LIR),
    CENTRAL_AMERICAN(Deity.QUETZALCOATL, Deity.CAMAXTLI, Deity.HUHETOTL),
    CHINESE(Deity.SHAN_LAI_CHING, Deity.CHIH_SUNG_TZU, Deity.HUAN_TI),
    DISCWORLD(Deity.BLIND_IO, Deity.THE_LADY, Deity.OFFLER),
    EGYPTIAN(Deity.PTAH, Deity.THOTH, Deity.ANHUR),
    HYBORIAN(Deity.MITRA, Deity.CROM, Deity.SET),
    GREEK(Deity.ATHENA, Deity.HERMES, Deity.POSEIDON),
    JAPANESE(Deity.AMATERASU_OMIKAMI, Deity.RAIJIN, Deity.SUSANOWO),
    NEHWON(Deity.ISSEK, Deity.MOG, Deity.KOS),
    NORSE(Deity.TYR, Deity.ODIN, Deity.LOKI),
    ROMAN(Deity.MERCURY, Deity.VENUS, Deity.MARS);

	private final Deity lawfulDeity;
	private final Deity neutralDeity;
	private final Deity chaoticDeity;

	private Pantheon(Deity lawful, Deity neutral, Deity chaotic) {
		lawfulDeity = lawful;
		neutralDeity = neutral;
		chaoticDeity = chaotic;
	}

	public Deity lawfulDeity() {
		return lawfulDeity;
	}

	public Deity neutralDeity() {
		return neutralDeity;
	}

	public Deity chaoticDeity() {
		return chaoticDeity;
	}

	public static final int LAWFUL_DEITY = 1;
	public static final int NEUTRAL_DEITY = 0;
	public static final int CHAOTIC_DEITY = -1;
	public static final int AMORAL_DEITY = -128;
	
	public Deity deityWith(int alignment) {
		
		if (LAWFUL_DEITY == alignment) {
			return lawfulDeity;
		}
		else if (NEUTRAL_DEITY == alignment) {
			return neutralDeity;
		}
		else if (CHAOTIC_DEITY == alignment) {
			return chaoticDeity;
		}
		else if (AMORAL_DEITY == alignment) {
			return Deity.MOLOCH;
		}
		else {
			throw new IllegalArgumentException("Bad alignment " + alignment);
		}
	}
}
