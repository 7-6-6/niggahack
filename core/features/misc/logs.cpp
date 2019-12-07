#include "logs.hpp"

c_event_logs event_logs;

void c_event_logs::run() noexcept {
	if (logs.empty())
		return;
	if (!config_system.item.misc_enabled)
		return;
	auto last_y = 6;

	for (unsigned int i = 0; i < logs.size(); i++) {
		auto & log = logs.at(i);

		if (utilities::epoch_time() - log.log_time > 2700) {
			float factor = (log.log_time + 3100) - utilities::epoch_time();
			factor /= 1000;

			auto opacity = int(255 * factor);

			if (opacity < 2) {
				logs.erase(logs.begin() + i);
				continue;
			}

			//log.x += 1 * (factor * 1.25);
			log.y += 0.5 / (factor * 1.25);
		}

		const auto text = log.message.c_str();
		render.draw_text(log.x, last_y + log.y, render.name_font, text, false, color(255, 255, 255, 255));

		last_y += 14;
	}
}
void c_event_logs::bombplant(i_game_event* event) noexcept {
	if (!config_system.item.bombplant)
		return;

	auto userid = event->get_int("userid");

	if (!userid)
		return;

	auto engine_userid = interfaces::engine->get_player_for_user_id(userid);
	
	if (!engine_userid)
		return;

	player_info_t info;
	interfaces::engine->get_player_info(engine_userid, &info);

	
	std::string player_name = info.name;
	std::transform(player_name.begin(), player_name.end(), player_name.begin(), ::tolower);

	std::stringstream ss;
	ss << player_name << " started planting bomb ";

	utilities::console_warning("[nigga hack] ");
	interfaces::console->console_printf("%s started planting bomb.     \n", player_name.c_str());

	add(ss.str(), color(255, 255, 255, 255));
	
}
void c_event_logs::connect(i_game_event* event) noexcept {

	if (!config_system.item.connections)
		return;

	auto userid = event->get_int("userid");

	auto engine_userid = interfaces::engine->get_player_for_user_id(userid);

	player_info_t info;
	interfaces::engine->get_player_info(engine_userid, &info);


	std::string player_name = info.name;
	std::transform(player_name.begin(), player_name.end(), player_name.begin(), ::tolower);

	std::stringstream ss;
	ss << event->get_int("userid") << " connected. ";

	utilities::console_warning("[nigga hack] ");
	interfaces::console->console_printf("%s connected.     \n", event->get_int("userid"));

	add(ss.str(), color(255, 255, 255, 255));
}
void c_event_logs::disconnect(i_game_event* event) noexcept {

	if (!config_system.item.connections)
		return;

	auto userid = event->get_int("userid");

	if (!userid)
		return;

	auto engine_userid = interfaces::engine->get_player_for_user_id(userid);

	if (!engine_userid)
		return;

	player_info_t info;
	interfaces::engine->get_player_info(engine_userid, &info);


	std::string player_name = info.name;
	std::transform(player_name.begin(), player_name.end(), player_name.begin(), ::tolower);

	std::stringstream ss;
	ss << player_name << " disconnected´. ";

	utilities::console_warning("[nigga hack] ");
	interfaces::console->console_printf("%s disconnected.     \n", player_name.c_str());

	add(ss.str(), color(255, 255, 255, 255));
}
void c_event_logs::event_item_purchase(i_game_event* event) noexcept {
	if (!config_system.item.logs_player_bought)
		return;

	auto userid = event->get_int("userid");

	if (!userid)
		return;

	auto engine_userid = interfaces::engine->get_player_for_user_id(userid);

	if (!engine_userid)
		return;

	player_info_t info;
	interfaces::engine->get_player_info(engine_userid, &info);

	auto local_player = reinterpret_cast<player_t*>(interfaces::entity_list->get_client_entity(interfaces::engine->get_local_player()));
	auto entity = reinterpret_cast<player_t*>(interfaces::entity_list->get_client_entity(engine_userid));

	if (!local_player || !entity)
		return;

	if (entity->team() == local_player->team())
		return;

	std::string player_name = info.name;
	std::transform(player_name.begin(), player_name.end(), player_name.begin(), ::tolower);

	std::stringstream ss;
	ss << player_name << " purchased " << event->get_string("weapon");

	utilities::console_warning("[nigga hack] ");
	interfaces::console->console_printf("%s purchased %s.     \n", player_name.c_str(), event->get_string("weapon"));

	add(ss.str(), color(255, 255, 255, 255));

}

void c_event_logs::event_player_hurt(i_game_event* event) noexcept {
	if (!config_system.item.logs_player_hurt)
		return;

	if (!event)
		return;

	auto local_player = reinterpret_cast<player_t*>(interfaces::entity_list->get_client_entity(interfaces::engine->get_local_player()));

	if (!local_player)
		return;

	auto victim = interfaces::engine->get_player_for_user_id(event->get_int("userid"));

	if (!victim)
		return;

	auto attacker = interfaces::entity_list->get_client_entity(interfaces::engine->get_player_for_user_id(event->get_int("attacker")));

	if (!attacker)
		return;

	if (attacker == local_player) {
		auto entity = reinterpret_cast<player_t*>(interfaces::entity_list->get_client_entity(victim));
		if (!entity) {
			return;
		}

		player_info_t info;
		interfaces::engine->get_player_info(entity->index(), &info);

		auto hitbox = event->get_int("hitgroup");
		if (!hitbox)
			return;

		auto damage = event->get_int("dmg_health");
		if (!damage)
			return;

		auto health = event->get_int("health");
		if (!health && health != 0)
			return;

		auto hitgroup = utilities::hitgroup_name(hitbox);
		std::string player_name = info.name;
		std::transform(player_name.begin(), player_name.end(), player_name.begin(), ::tolower);

		std::stringstream ss;

		ss << "hit " << player_name.c_str() << " in the " << hitgroup << " for " << damage << " damage";
		ss << " (" << health << " health remaining).";

		utilities::console_warning("[nigga hack] ");
		interfaces::console->console_printf("hit %s in the %s for %d damage (%d health remaining). \n", player_name.c_str(), hitgroup, damage, health);
		event_logs.add(ss.str(), color(255, 255, 255, 255));

		if (config_system.item.extra_info) {
			utilities::console_warning("[nigga hack] ");
			interfaces::console->console_printf("moving: %s", entity->is_moving() ? "yes\n" : "no\n");
			utilities::console_warning("[nigga hack] ");
			interfaces::console->console_printf("in air: %s", entity->is_in_air() ? "yes\n" : "no\n");
			utilities::console_warning("[nigga hack] ");
			interfaces::console->console_printf("flashed: %s", entity->is_flashed() ? "yes \n" : "no \n");
			utilities::console_warning("[nigga hack] ");
			interfaces::console->console_printf("has c4: %s", entity->has_c4() ? "yes \n" : "no \n");
		}
	}
}

void c_event_logs::add(std::string text, color colors) noexcept {
	logs.push_front(loginfo_t(utilities::epoch_time(), text, colors));
}