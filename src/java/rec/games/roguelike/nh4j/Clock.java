package rec.games.roguelike.nh4j;

public class Clock {

	private static long elapsedPlayingTimeSeconds = 0L;
	private static long epochSecondsOfMostRecentRestore = 0L;
	private static long epochSecondsOfGameEnd = 0L;

	
	public static long elapsedPlayingTimeSeconds() {
		return elapsedPlayingTimeSeconds;
	}
	
	public static void setElapsedPlayingTimeSeconds(long s) {
		elapsedPlayingTimeSeconds = s;
	}

	public static long epochSeconds()
	{
		return System.currentTimeMillis() / 1000L;
	}
	
	public static long epochSecondsOfMostRecentRestore()
	{
		return epochSecondsOfMostRecentRestore;
	}

	public static void setEpochSecondsOfMostRecentRestore(long epochSeconds)
	{
		epochSecondsOfMostRecentRestore = epochSeconds;
	}

	public static long epochSecondsOfGameEnd()
	{
		return epochSecondsOfGameEnd;
	}

	public static void setEpochSecondsOfGameEnd(long epochSeconds)
	{
		epochSecondsOfGameEnd = epochSeconds;
	}

}
