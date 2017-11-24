package rec.games.roguelike.nh4j;

public class SpellcastingPenalties {

	private int basePenalty;
    private int healingPenalty;
	
	public static SpellcastingPenalties of() {
		return new SpellcastingPenalties();
	}

	public SpellcastingPenalties base(int penalty) {
		this.basePenalty = penalty;
		return this;
	}

	public SpellcastingPenalties healing(int penalty) {
		this.healingPenalty = penalty;
		return this;
	}
	
	public int base() {
		return this.basePenalty;
	}
	
	public int healing() {
		return this.healingPenalty;
	}

}
