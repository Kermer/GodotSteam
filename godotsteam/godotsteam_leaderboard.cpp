
#include "godotsteam_leaderboard.h"


SteamLeaderboard::SteamLeaderboard()
{
	is_valid = false;
}

SteamLeaderboard::SteamLeaderboard(SteamLeaderboard_t lHandle)
{
	update_handle(lHandle);
}

void SteamLeaderboard::update_handle(SteamLeaderboard_t lHandle)
{
	leaderboard_handle = (uint64)lHandle;
	is_valid = false;
	if ( leaderboard_handle > 0 && SteamUserStats() != NULL )
	{
		is_valid = true;
	}
}

uint64 SteamLeaderboard::get_handle()
{
	return leaderboard_handle;
}










String SteamLeaderboard::get_name()
{
	if ( SteamUserStats() == NULL ) { return ""; }
	return SteamUserStats()->GetLeaderboardName( leaderboard_handle );
}

int SteamLeaderboard::get_total_entry_count()
{
	if ( SteamUserStats() == NULL ) { return -1; }
	return SteamUserStats()->GetLeaderboardEntryCount( leaderboard_handle );
}

void SteamLeaderboard::query_entries(int rStart,int rEnd,int type)
{
	if ( SteamUserStats() == NULL ) { return; }
	SteamAPICall_t apiCall = SteamUserStats()->DownloadLeaderboardEntries( leaderboard_handle, ELeaderboardDataRequest(type), rStart, rEnd);
	callResultEntries.Set(apiCall, this, &SteamLeaderboard::_entries_loaded);
}

void SteamLeaderboard::query_users_entries(Array usersId)
{
	if ( SteamUserStats() == NULL ) { return; }
	int users_count = usersId.size();
	if ( users_count == 0 ) { return; }
	CSteamID *pUsers = new CSteamID[users_count];
	for(int i=0;i<users_count;i++)
	{
		CSteamID user = create_steamid( usersId[i] );
		pUsers[i] = user;
	}
	SteamAPICall_t apiCall = SteamUserStats()->DownloadLeaderboardEntriesForUsers( leaderboard_handle, pUsers, users_count );
	callResultEntries.Set(apiCall, this, &SteamLeaderboard::_entries_loaded);
	delete[] pUsers;
}

Array SteamLeaderboard::get_entries()
{
	return leaderboard_entries;
}

void SteamLeaderboard::upload_score(int score,bool keepBest)
{
	if ( SteamUserStats() == NULL ) { return; }
	int method = 0;
	if (keepBest)
		method = 1;
	else
		method = 2;
	SteamUserStats()->UploadLeaderboardScore( leaderboard_handle, ELeaderboardUploadScoreMethod(method), (int32)score, NULL, 0 );
}








void SteamLeaderboard::_entries_loaded( LeaderboardScoresDownloaded_t *callData, bool bIOFailure )
{
	if ( callData->m_hSteamLeaderboard != leaderboard_handle ) { return; } // not this leaderboard
	store_entries( callData->m_hSteamLeaderboardEntries, callData->m_cEntryCount );
	emit_signal("entries_loaded");
}

void SteamLeaderboard::store_entries( SteamLeaderboardEntries_t eHandle, int entryCount )
{
	if ( SteamUserStats() == NULL ) { return; }
	leaderboard_entries.clear();
	for(int i=0;i<entryCount;i++)
	{
		LeaderboardEntry_t *entry=NULL;
		SteamUserStats()->GetDownloadedLeaderboardEntry( eHandle, i, entry, NULL, 0 );
		Dictionary entryDict;
		entryDict["score"] = entry->m_nScore;
		entryDict["steam_id"] = entry->m_steamIDUser.GetAccountID();
		entryDict["global_rank"] = entry->m_nGlobalRank;
		leaderboard_entries.append(entryDict);
	}
}









void SteamLeaderboard::_bind_methods()
{

	ObjectTypeDB::bind_method("get_name",&SteamLeaderboard::get_name);
	ObjectTypeDB::bind_method("get_total_entry_count",&SteamLeaderboard::get_total_entry_count);
	ObjectTypeDB::bind_method(_MD("query_entries","range_start","range_end","type"),&SteamLeaderboard::query_entries,DEFVAL(GLOBAL));
	ObjectTypeDB::bind_method(_MD("query_users_entries","users_id"),&SteamLeaderboard::query_users_entries);
	ObjectTypeDB::bind_method("get_entries",&SteamLeaderboard::get_entries);
	ObjectTypeDB::bind_method(_MD("upload_score","score","keep_best"),&SteamLeaderboard::upload_score,DEFVAL(true));
	
	ADD_SIGNAL(MethodInfo("entries_loaded"));
	
	// BIND_CONSTANT(ERR_NO_CLIENT);
}

SteamLeaderboard::~SteamLeaderboard()
{
	
}
