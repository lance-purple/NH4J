package rec.games.roguelike.nh4j;

public class EnergyAdvancement extends Advancement {

	public static final EnergyAdvancement NONE = EnergyAdvancement.of().init(0,0).low(0, 0).high(0, 0);

	@Override
	public EnergyAdvancement init(int constant, int variable) {
		super.init(constant, variable);
		return this;
	}

	@Override
	public EnergyAdvancement low(int constant, int variable) {
		super.low(constant, variable);
		return this;
	}

	@Override
	public EnergyAdvancement high(int constant, int variable) {
		super.high(constant, variable);
		return this;
	}

	public static EnergyAdvancement of() {
		return new EnergyAdvancement();
	}

}
