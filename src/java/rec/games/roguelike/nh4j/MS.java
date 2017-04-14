package rec.games.roguelike.nh4j;
    
public enum MS {

    SILENT(0),    /* makes no sound */
    BARK(1),      /* if full moon, may howl */
    MEW(2),       /* mews or hisses */
    ROAR(3),      /* roars */
    GROWL(4),     /* growls */
    SQEEK(5),     /* squeaks, as a rodent */
    SQAWK(6),     /* squawks, as a bird */
    HISS(7),      /* hisses */
    BUZZ(8),      /* buzzes (killer bee) */
    GRUNT(9),
    ORC(9),       /* grunts (or speaks own language) */
    NEIGH(10),    /* neighs, as an equine */
    WAIL(11),     /* wails, as a tortured soul */
    GURGLE(12),   /* gurgles, as liquid or through saliva */
    BURBLE(13),   /* burbles (jabberwock) */
    ANIMAL(13),   /* up to here are animal noises */
    SHRIEK(15),   /* wakes up others */
    BONES(16),    /* rattles bones (skeleton) */
    LAUGH(17),    /* grins, smiles, giggles, and laughs */
    MUMBLE(18),   /* says something or other */
    IMITATE(19),  /* imitates others (leocrotta) */
    HUMANOID(20), /* generic traveling companion */
    ARREST(21),   /* "Stop in the name of the law!" (Kops) */
    SOLDIER(22),  /* army and watchmen expressions */
    GUARD(23),    /* "Please drop that gold and follow me." */
    DJINNI(24),   /* "Thank you for freeing me!" */
    NURSE(25),    /* "Take off your shirt, please." */
    SEDUCE(26),   /* "Hello, sailor." (Nymphs) */
    VAMPIRE(27),  /* vampiric seduction, Vlad's exclamations */
    BRIBE(28),    /* asks for money, or berates you */
    CUSS(29),     /* berates (demons) or intimidates (Wiz) */
    RIDER(30),    /* astral level special monsters */
    LEADER(31),   /* your class leader */
    NEMESIS(32),  /* your nemesis */
    GUARDIAN(33), /* your leader's guards */
    SELL(34),     /* demand payment, complain about shoplifters */
    ORACLE(35),   /* do a consultation */
    PRIEST(36),   /* ask for contribution; do cleansing */
    SPELL(37),    /* spellcaster not matching any of the above */
    WERE(38),     /* lycanthrope in human form */
    BOAST(39);    /* giants */

	int id;
	
	MS(int id)
	{
		this.id = id;
	}
	
	public int id()
	{
		return id;
	}

}
