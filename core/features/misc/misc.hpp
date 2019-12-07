#pragma once
#include "../../../dependencies/common_includes.hpp"

class c_misc {
public:
	void __stdcall remove_smoke() noexcept;
	void __stdcall remove_flash() noexcept;
	void __stdcall attack2spam(c_usercmd* user_cmd) noexcept;
	void __stdcall blockbot(c_usercmd* user_cmd) noexcept;
	void __stdcall rank_reveal() noexcept;
	void __stdcall doorspam(c_usercmd* user_cmd) noexcept;
	void __stdcall remove_scope() noexcept;
	void __stdcall niggatag() noexcept;
	void __stdcall spectators() noexcept;
	void __stdcall watermark() noexcept;
	void __stdcall clantag_spammer() noexcept;
	void __stdcall namespamidkmemes() noexcept;
	void __stdcall viewmodel_offset() noexcept;
	void __stdcall disable_post_processing() noexcept;
	void __stdcall recoil_crosshair() noexcept;
	void __stdcall force_crosshair() noexcept;
private:
};

extern c_misc misc;
