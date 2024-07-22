#include "../World.h"
#include "../LoginServer.h"

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <sstream>

extern ZoneList zone_list;
extern World world;
extern LoginServer loginserver;
extern sint32 numclients;

void World::Web_worldhandle_status(const http::request<http::string_body>& req, http::response<http::string_body>& res) {
    res.set(http::field::content_type, "application/json");
	boost::property_tree::ptree pt;

    pt.put("web_status", "online");
	bool world_online = world.world_loaded;
    pt.put("world_status", world.world_loaded ? "online" : "offline");
    pt.put("world_uptime", (getCurrentTimestamp() - world.world_uptime));
	auto [days, hours, minutes, seconds] = convertTimestampDuration((getCurrentTimestamp() - world.world_uptime));
	std::string uptime_str("Days: " + std::to_string(days) + ", " + "Hours: " + std::to_string(hours) + ", " + "Minutes: " + std::to_string(minutes) + ", " + "Seconds: " + std::to_string(seconds));
    pt.put("world_uptime_string", uptime_str);
    pt.put("login_connected", loginserver.Connected() ? "connected" : "disconnected");
    pt.put("player_count", zone_list.GetZonesPlayersCount());
    pt.put("client_count", numclients);
    pt.put("zones_connected", zone_list.Count());
    pt.put("world_reloading", world.IsReloadingSubsystems() ? "yes" : "no");

    std::ostringstream oss;
    boost::property_tree::write_json(oss, pt);
    std::string json = oss.str();
    res.body() = json;
    res.prepare_payload();
}

void World::Web_worldhandle_clients(const http::request<http::string_body>& req, http::response<http::string_body>& res) {
	zone_list.PopulateClientList(res);
}

void ZoneList::PopulateClientList(http::response<http::string_body>& res) {
	res.set(http::field::content_type, "application/json");
	boost::property_tree::ptree maintree;

    std::ostringstream oss;

	MClientList.lock();
	map<string,Client*>::iterator itr;
	for(itr = client_map.begin(); itr != client_map.end(); itr++){
		if(itr->second){
			Client* cur = (Client*)itr->second;
			boost::property_tree::ptree pt;
			pt.put("character_id", cur->GetCharacterID());
			pt.put("character_name", cur->GetPlayer() ? cur->GetPlayer()->GetName() : "N/A");
			pt.put("class1", cur->GetPlayer() ? cur->GetPlayer()->GetInfoStruct()->get_class1() : 0);
			pt.put("class2", cur->GetPlayer() ? cur->GetPlayer()->GetInfoStruct()->get_class2() : 0);
			pt.put("class3", cur->GetPlayer() ? cur->GetPlayer()->GetInfoStruct()->get_class3() : 0);
			pt.put("tradeskill_class1", cur->GetPlayer() ? cur->GetPlayer()->GetInfoStruct()->get_tradeskill_class1() : 0);
			pt.put("tradeskill_class2", cur->GetPlayer() ? cur->GetPlayer()->GetInfoStruct()->get_tradeskill_class2() : 0);
			pt.put("tradeskill_class3", cur->GetPlayer() ? cur->GetPlayer()->GetInfoStruct()->get_tradeskill_class3() : 0);
			pt.put("race", cur->GetPlayer() ? cur->GetPlayer()->GetInfoStruct()->get_race() : 0);
			pt.put("level", cur->GetPlayer() ? cur->GetPlayer()->GetInfoStruct()->get_level() : 0);
			pt.put("effective_level", cur->GetPlayer() ? cur->GetPlayer()->GetInfoStruct()->get_effective_level() : 0);
			pt.put("tradeskill_level", cur->GetPlayer() ? cur->GetPlayer()->GetInfoStruct()->get_tradeskill_level() : 0);
			pt.put("account_age", cur->GetPlayer() ? cur->GetPlayer()->GetInfoStruct()->get_account_age_base() : 0);
			pt.put("account_id", cur->GetAccountID());
			pt.put("version", cur->GetVersion());
			pt.put("is_zoning", (cur->IsZoning() || !cur->IsReadyForUpdates()));
			
			bool linkdead = cur->GetPlayer() ? (((cur->GetPlayer()->GetActivityStatus() & ACTIVITY_STATUS_LINKDEAD) > 0)) : false;
			pt.put("is_linkdead", linkdead);
			pt.put("in_zone", cur->IsReadyForUpdates());
			pt.put("zonename", (cur->GetPlayer() && cur->GetPlayer()->GetZone()) ? cur->GetPlayer()->GetZone()->GetZoneName() : "N/A");
			maintree.add_child("Client", pt);
		}
	}
	MClientList.unlock();
    
    boost::property_tree::write_json(oss, maintree);
    std::string json = oss.str();
    res.body() = json;
    res.prepare_payload();
}

