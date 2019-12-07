#include "misc.hpp"

c_misc misc;

void __stdcall c_misc::remove_smoke() noexcept {
	if (!config_system.item.remove_smoke || !config_system.item.visuals_enabled)
		return;

	if (!interfaces::engine->is_connected() && !interfaces::engine->is_in_game())
		return;

	static auto smoke_count = *reinterpret_cast<uint32_t **>(utilities::pattern_scan(GetModuleHandleA("client_panorama.dll"), "A3 ? ? ? ? 57 8B CB") + 1);

	static std::vector<const char*> smoke_materials = {
		"particle/vistasmokev1/vistasmokev1_fire",
		"particle/vistasmokev1/vistasmokev1_smokegrenade",
		"particle/vistasmokev1/vistasmokev1_emods",
		"particle/vistasmokev1/vistasmokev1_emods_impactdust"
	};

	for (auto material_name : smoke_materials) {
		i_material * smoke = interfaces::material_system->find_material(material_name, TEXTURE_GROUP_OTHER);
		smoke->increment_reference_count();
		smoke->set_material_var_flag(MATERIAL_VAR_WIREFRAME, true);

		*(int*)smoke_count = 0;
	}
}
static void CalcAngle(vec3_t src, vec3_t dst, vec3_t& angles)
{
	vec3_t delta = src - dst;
	double hyp = delta.Length2D();
	angles.y = atan(delta.y / delta.x) * 57.295779513082f;
	angles.x = atan(delta.z / hyp) * 57.295779513082f;
	if (delta.x >= 0.0)
		angles.y += 180.0f;
	angles.z = 0;
}

void c_misc::blockbot(c_usercmd* user_cmd) noexcept{

	if (!config_system.item.blockbot || !config_system.item.grief_enable)
		return;

	if (GetAsyncKeyState(config_system.item.blockbot_key)) {
		float bestdist = 150.f;
		int index = -1;

		auto local_player = reinterpret_cast<player_t*>(interfaces::entity_list->get_client_entity(interfaces::engine->get_local_player()));
		for (int i = 0; i < interfaces::globals->max_clients; i++)
		{
			auto entity = reinterpret_cast<player_t*>(interfaces::entity_list->get_client_entity(i));
			
			if (!entity)
				continue;

			if (!entity->is_alive() || entity->dormant() || entity == local_player)
				continue;

			float dist = local_player->origin().distance_to(entity->origin());

			if (dist < bestdist)
			{
				bestdist = dist;
				index = i;
			}
		}

		if (index == -1)
			return;

		static auto target = reinterpret_cast<player_t*>(interfaces::entity_list->get_client_entity(index));

		if (!target)
			return;
		
		vec3_t angles;
		CalcAngle(local_player->origin(), target->origin(), angles);
	
		angles.y -= local_player->eye_angles().y;
		angles.normalized();
		vec3_t nigga = local_player->origin() - target->origin();


		 if (angles.y < 0.0f)
			user_cmd->sidemove = 450.f;
		else if (angles.y > 0.0f)
			user_cmd->sidemove = -450.f;
	}
}

void __stdcall c_misc::remove_flash() noexcept {
	if (!config_system.item.reduce_flash || !config_system.item.visuals_enabled)
		return;

	if (!interfaces::engine->is_connected() && !interfaces::engine->is_in_game())
		return;

	auto local_player = reinterpret_cast<player_t*>(interfaces::entity_list->get_client_entity(interfaces::engine->get_local_player()));

	if (!local_player)
		return;

	if (local_player->flash_duration() > 0.0f)
		local_player->flash_duration() = 0.0f;
}

void __stdcall c_misc::rank_reveal() noexcept {
	if (!config_system.item.rank_reveal || !config_system.item.misc_enabled)
		return;

	if (GetAsyncKeyState(VK_TAB))
		interfaces::client->dispatch_user_message(cs_um_serverrankrevealall, 0, 0, nullptr);
}

void __stdcall c_misc::remove_scope() noexcept {
	if (!config_system.item.remove_scope || !config_system.item.visuals_enabled)
		return;

	if (!interfaces::engine->is_connected() && !interfaces::engine->is_in_game())
		return;

	auto local_player = reinterpret_cast<player_t*>(interfaces::entity_list->get_client_entity(interfaces::engine->get_local_player()));

	if (!local_player)
		return;

	if (local_player && !local_player->is_scoped())
		return;

	int w, h;
	interfaces::engine->get_screen_size(w, h);
	interfaces::surface->set_drawing_color(0, 0, 0, 255);
	interfaces::surface->draw_line(0, h / 2, w, h / 2);
	interfaces::surface->draw_line(w / 2, 0, w / 2, h);
}

void __stdcall c_misc::spectators() noexcept {
	if (!config_system.item.spectators_list || !config_system.item.misc_enabled)
		return;

	if (!interfaces::engine->is_connected() && !interfaces::engine->is_in_game())
		return;

	int spectator_index = 0;
	int width, height;
	interfaces::engine->get_screen_size(width, height);

	render.draw_text(35, 5, render.name_font, "spectators", true, color(255, 255, 255));
	for (int i = 0; i < interfaces::entity_list->get_highest_index(); i++) {
		auto local_player = reinterpret_cast<player_t*>(interfaces::entity_list->get_client_entity(interfaces::engine->get_local_player()));

		if (!local_player)
			return;

		auto entity = reinterpret_cast<player_t*>(interfaces::entity_list->get_client_entity(i));

		if (!entity)
			return;

		player_info_t info;

		if (entity && entity != local_player) {
			interfaces::engine->get_player_info(i, &info);
			if (!entity->is_alive() && !entity->dormant()) {
				auto target = entity->observer_target();

				if (!target)
					return;

				if (target) {
					auto spectator_target = interfaces::entity_list->get_client_entity_handle(target);
					if (spectator_target == local_player) {
						std::string player_name = info.name;
						std::transform(player_name.begin(), player_name.end(), player_name.begin(), ::tolower);
						player_info_t spectator_info;
						interfaces::engine->get_player_info(i, &spectator_info);
						render.draw_text(35, 5 + (10 * spectator_index), render.name_font, player_name.c_str(), true, color(255, 255, 255));
						spectator_index++;
					}
				}
			}
		}
	}
}

void __stdcall c_misc::watermark() noexcept {
	if (!config_system.item.watermark || !config_system.item.misc_enabled)
		return;

	int width, height;
	interfaces::engine->get_screen_size(width, height);

	static int fps, old_tick_count;

	if ((interfaces::globals->tick_count - old_tick_count) > 50) {
		fps = static_cast<int>(1.f / interfaces::globals->frame_time);
		old_tick_count = interfaces::globals->tick_count;
	}
	std::stringstream ss;

	auto net_channel = interfaces::engine->get_net_channel_info();
	auto local_player = reinterpret_cast<player_t*>(interfaces::entity_list->get_client_entity(interfaces::engine->get_local_player()));
	std::string incoming = local_player ? std::to_string(static_cast<int>(net_channel->get_latency(FLOW_INCOMING) * 1000)) : "0";
	std::string outgoing = local_player ? std::to_string(static_cast<int>(net_channel->get_latency(FLOW_OUTGOING) * 1000)) : "0";

	ss << "nigga hack";

	render.draw_filled_rect(width - 275, 4, 260, 20, color(33, 35, 47, 255));
	render.draw_outline(width - 275, 4, 260, 20, color(30, 30, 41, 255));
	render.draw_text(width - 270, 7, render.watermark_font, ss.str().c_str(), false, color(255, 255, 255, 255));
}

void __stdcall c_misc::clantag_spammer() noexcept {
	if (!config_system.item.clan_tag || !config_system.item.misc_enabled)
		return;

	static std::string tag = "nigga hack ";
	static float last_time = 0;

	if (interfaces::globals->cur_time > last_time) {
		std::rotate(std::begin(tag), std::next(std::begin(tag)), std::end(tag));
		utilities::apply_clan_tag(tag.c_str());

		last_time = interfaces::globals->cur_time + 0.9f;
	}

	if (fabs(last_time - interfaces::globals->cur_time) > 1.f)
		last_time = interfaces::globals->cur_time;
}
void __stdcall c_misc::niggatag() noexcept {
	if (!config_system.item.niggatag || !config_system.item.grief_enable)
		return;

	static std::string tag = "\x01\x0B\x07nigga\n ";
	utilities::apply_clan_tag(tag.c_str());

}
void __stdcall c_misc::doorspam(c_usercmd* user_cmd) noexcept {

	if (!config_system.item.doorspam_enabled || !config_system.item.grief_enable || !GetAsyncKeyState(config_system.item.doorspam_key))
		return;
	
	static bool spam = false;
	static float time = 0;
	if (interfaces::globals->cur_time > time) {

		user_cmd->buttons &= ~in_use;

		time = interfaces::globals->cur_time + 0.2f;
	}
	if (fabs(time - interfaces::globals->cur_time) > 0.3f)
		time = interfaces::globals->cur_time;

}
void __stdcall c_misc::viewmodel_offset() noexcept {
	if (!config_system.item.viewmodel_offset || !config_system.item.misc_enabled)
		return;

	interfaces::console->get_convar("viewmodel_offset_x")->set_value(config_system.item.viewmodel_x);
	interfaces::console->get_convar("viewmodel_offset_y")->set_value(config_system.item.viewmodel_y);
	interfaces::console->get_convar("viewmodel_offset_z")->set_value(config_system.item.viewmodel_z);
}
void __stdcall c_misc::namespamidkmemes() noexcept
{
	if (!config_system.item.name_spam || !config_system.item.grief_enable)
		return;

	static bool troll = false;

		if (troll)
		{
			utilities::change_name("NIGGA");

		}
		else
		{
			utilities::change_name("HACK");
		}

		troll = !troll;
	
}
void __stdcall c_misc::disable_post_processing() noexcept {
	if (!config_system.item.misc_enabled)
		return;

	static auto mat_postprocess_enable = interfaces::console->get_convar("mat_postprocess_enable");
	mat_postprocess_enable->set_value(config_system.item.disable_post_processing ? 0 : 1);
}

void __stdcall c_misc::recoil_crosshair() noexcept {
	if (!config_system.item.misc_enabled)
		return;

	static auto cl_crosshair_recoil = interfaces::console->get_convar("cl_crosshair_recoil");
	cl_crosshair_recoil->set_value(config_system.item.recoil_crosshair ? 1 : 0);
}

void __stdcall c_misc::force_crosshair() noexcept {
	if (!config_system.item.misc_enabled)
		return;

	auto local_player = reinterpret_cast<player_t*>(interfaces::entity_list->get_client_entity(interfaces::engine->get_local_player()));

	if (!local_player)
		return;

	static auto weapon_debug_spread_show = interfaces::console->get_convar("weapon_debug_spread_show");

	if (local_player && local_player->health() > 0) {
		weapon_debug_spread_show->set_value(local_player->is_scoped() || !config_system.item.force_crosshair ? 0 : 3);
	}
}
