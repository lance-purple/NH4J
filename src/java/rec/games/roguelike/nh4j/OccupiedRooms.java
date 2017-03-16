package rec.games.roguelike.nh4j;

public class OccupiedRooms {

	private static final int MAXIMUM_COUNT = 5;

	private static int currentlyOccupiedRoomIDs[]  = new int[MAXIMUM_COUNT];
	private static int previouslyOccupiedRoomIDs[] = new int[MAXIMUM_COUNT];
	private static int freshlyEnteredRoomIDs[]     = new int[MAXIMUM_COUNT];
	
	private static int currentlyOccupiedShopIDs[]  = new int[MAXIMUM_COUNT];
	private static int previouslyOccupiedShopIDs[] = new int[MAXIMUM_COUNT];
	private static int freshlyEnteredShopIDs[]     = new int[MAXIMUM_COUNT];
	private static int freshlyExitedShopIDs[]      = new int[MAXIMUM_COUNT];

	private static int emptyShopIDs[] = new int[MAXIMUM_COUNT];

	public static int maximumCount() {
		return MAXIMUM_COUNT;
	}

	public static int currentlyOccupiedRoomIDs(int index) {
		return currentlyOccupiedRoomIDs[index];
	}

	public static void setCurrentlyOccupiedRoomIDs(int index, int roomID) {
		currentlyOccupiedRoomIDs[index] = roomID;
	}

	public static int previouslyOccupiedRoomIDs(int index) {
		return previouslyOccupiedRoomIDs[index];
	}

	public static void setPreviouslyOccupiedRoomIDs(int index, int roomID) {
		previouslyOccupiedRoomIDs[index] = roomID;
	}

	public static int freshlyEnteredRoomIDs(int index) {
		return freshlyEnteredRoomIDs[index];
	}

	public static void setFreshlyEnteredRoomIDs(int index, int roomID) {
		freshlyEnteredRoomIDs[index] = roomID;
	}

	public static int currentlyOccupiedShopIDs(int index) {
		return currentlyOccupiedShopIDs[index];
	}

	public static void setCurrentlyOccupiedShopIDs(int index, int roomID) {
		currentlyOccupiedShopIDs[index] = roomID;
	}

	public static int previouslyOccupiedShopIDs(int index) {
		return previouslyOccupiedShopIDs[index];
	}

	public static void setPreviouslyOccupiedShopIDs(int index, int roomID) {
		previouslyOccupiedShopIDs[index] = roomID;
	}

	public static int freshlyEnteredShopIDs(int index) {
		return freshlyEnteredShopIDs[index];
	}

	public static void setFreshlyEnteredShopIDs(int index, int roomID) {
		freshlyEnteredShopIDs[index] = roomID;
	}

	public static int freshlyExitedShopIDs(int index) {
		return freshlyExitedShopIDs[index];
	}

	public static void setFreshlyExitedShopIDs(int index, int roomID) {
		freshlyExitedShopIDs[index] = roomID;
	}

	public static int emptyShopIDs(int index) {
		return emptyShopIDs[index];
	}

	public static void setEmptyShopIDs(int index, int roomID) {
		emptyShopIDs[index] = roomID;
	}

}
