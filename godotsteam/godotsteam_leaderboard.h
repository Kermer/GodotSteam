
#ifndef GODOTSTEAM_LEADERBOARD_H
#define GODOTSTEAM_LEADERBOARD_H

#include <steam/steam_api.h>

#include "reference.h"
#include "dictionary.h" // contains array.h as well

#include "godotsteam.h"


class SteamLeaderboard: public Reference
{
public:
	enum {
		GLOBAL=0, GLOBAL_AROUND_USER=1, FRIENDS=2, USERS=3
		
	};
	SteamLeaderboard();
	SteamLeaderboard(SteamLeaderboard_t lHandle);
	~SteamLeaderboard();
	void update_handle(SteamLeaderboard_t lHandle);
	uint64 get_handle();
	
	String get_name();
	int get_total_entry_count();
	void query_entries(int rStart,int rEnd,int type=GLOBAL);
	void query_users_entries(Array usersId);
	Array get_entries();
	void upload_score(int score,bool keepBest=false);
	
	
	void store_entries( SteamLeaderboardEntries_t eHandle, int entryCount );
	
	CSteamID create_steamid( uint32 steamId, int accountType=-1 )
	{
		CSteamID cSteamID;
		if (accountType < 0 || accountType >= k_EAccountTypeMax )
			accountType = 1; // k_EAccountTypeIndividual
		cSteamID.Set( steamId, EUniverse(k_EUniversePublic), EAccountType(accountType) );
		return cSteamID;
	}
	

protected:
	static void _bind_methods();
	static SteamLeaderboard* singleton;

private:
	// ( typedef uint64 SteamLeaderboard_t )
	SteamLeaderboard_t leaderboard_handle;
	bool is_valid;
	Array leaderboard_entries;
	// STEAM_CALLBACK(SteamLeaderboard,_entries_loaded,LeaderboardScoresDownloaded_t );
	CCallResult<SteamLeaderboard, LeaderboardScoresDownloaded_t> callResultEntries;
		void _entries_loaded( LeaderboardScoresDownloaded_t *callData, bool bIOFailure );

	OBJ_TYPE(SteamLeaderboard, Reference);
	OBJ_CATEGORY("Steam");
};
#endif


