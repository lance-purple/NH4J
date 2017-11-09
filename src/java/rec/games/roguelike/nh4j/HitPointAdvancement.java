package rec.games.roguelike.nh4j;

public class HitPointAdvancement extends Advancement {

	@Override
	public HitPointAdvancement init(int constant, int variable) {
		super.init(constant, variable);
		return this;
	}

	@Override
	public HitPointAdvancement low(int constant, int variable) {
		super.low(constant, variable);
		return this;
	}

	@Override
	public HitPointAdvancement high(int constant, int variable) {
		super.high(constant, variable);
		return this;
	}

	public static HitPointAdvancement of() {
		return new HitPointAdvancement();
	}

}
