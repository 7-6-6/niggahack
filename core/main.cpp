#include "../dependencies/common_includes.hpp"
#include "features/misc/misc.hpp"
#include "menu/config/config.hpp"
#include "features/misc/events.hpp"
#include "features/skinchanger/parser.hpp"


unsigned long __stdcall initial_thread(void* reserved) {




	while (!GetModuleHandleA("serverbrowser.dll"))
		Sleep(200);

	try {
		interfaces::initialize();
		hooks::initialize();
		render.setup_fonts();
		utilities::material_setup();
		config_system.run("nigga hack");
		events.setup();
		kit_parser.setup();
	}
	catch (const std::runtime_error & error) {
		MessageBoxA(NULL, error.what(), "nigga hack", MB_OK | MB_ICONERROR);
		FreeLibraryAndExitThread(reinterpret_cast<HMODULE>(reserved), 0);
		return 0ul;
	}


	while (!GetAsyncKeyState(VK_END))
		std::this_thread::sleep_for(std::chrono::milliseconds(50));

	hooks::shutdown();
	std::this_thread::sleep_for(std::chrono::milliseconds(100));


	FreeLibraryAndExitThread(reinterpret_cast<HMODULE>(reserved), 0);
	return 0ul;
}

bool __stdcall DllMain(void* instance, unsigned long reason_to_call, void* reserved) {
	if (reason_to_call == DLL_PROCESS_ATTACH) {
		CreateThread(0, 0, initial_thread, instance, 0, 0);
	}

	return true;
}