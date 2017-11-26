package rec.games.roguelike.nh4j;

public class GenderedName {

	private final String maleVersion;
	private final String femaleVersion;

	private GenderedName(String maleVersion, String femaleVersion) {
		this.maleVersion = maleVersion;
		this.femaleVersion = femaleVersion;
	}

	private GenderedName(String genericVersion) {
		this.maleVersion = genericVersion;
		this.femaleVersion = genericVersion;
	}

	public boolean hasMaleVersion() {
		return (null != maleVersion);
	}

	public String asMale() {
		return maleVersion;
	}

	public boolean hasFemaleVersion() {
		return (! femaleVersion.equals(maleVersion));
	}

	public String asFemale() {
		return femaleVersion;
	}

	public static GenderedName of(String genericVersion) {
		return new GenderedName(genericVersion);
	}

	public static GenderedName of(String maleVersion, String femaleVersion) {
		return new GenderedName(maleVersion, femaleVersion);
	}

}
