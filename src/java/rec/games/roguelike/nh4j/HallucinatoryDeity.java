package rec.games.roguelike.nh4j;


public enum HallucinatoryDeity {

	THE_FLYING_SPAGHETTI_MONSTER("the Flying Spaghetti Monster"), /* Church of the FSM */
	ERIS("Eris"),                           /* Discordianism */
    THE_MARTIANS("the Martians"),           /* every science fiction ever */
	XOM("Xom"),                             /* Crawl */
	ANDOR_DRAKON("AnDoR dRaKoN"),           /* ADOM */
	THE_CENTRAL_BANK_OF_YENDOR("the Central Bank of Yendor"),   /* economics */
	TOOTH_FAIRY("Tooth Fairy"),             /* real world(?) */
	OM("Om"),                               /* Discworld */
	YAWGMOTH("Yawgmoth"),                   /* Magic: the Gathering */
	MORGOTH("Morgoth"),                     /* LoTR */
	CTHULHU("Cthulhu"),                     /* Lovecraft */
	THE_ORI("the Ori"),                     /* Stargate */
	DESTINY("destiny"),                     /* why not? */
	YOUR_FRIEND_THE_COMPUTER("your Friend the Computer");     /* Paranoia */

	private final String deityName;
	
	private HallucinatoryDeity(String name) {
		this.deityName = name;
	}

	public String deityName() {
		return deityName;
	}


	
	public static HallucinatoryDeity random() {
		int index = Randomizer.rn2(values().length);
		return values()[index];
	}
	
	public static String randomName()
	{
	    switch (Randomizer.rn2(9))
	    {
	    case 0:
	    case 1:
	    case 2:
	    case 3:
	    case 4:
	    case 5: return Deity.random().deityName();
	    case 6:
	    case 7: return HallucinatoryDeity.random().deityName();
	    default: return Deity.MOLOCH.deityName();
	    }
	}

}
