#pragma once
#include "../../dependencies/common_includes.hpp"

extern IDirect3DStateBlock9* state_block;

enum esp_info_position {
	RIGHT,
	CENTER_DOWN,
	CENTER_UP
};

struct esp_info_s {
	esp_info_s(const std::string name, const color colors, const int position = RIGHT) {
		f_name = name; f_color = colors; f_position = position;
	}

	std::string f_name;
	int f_position;
	color f_color;
};

class c_menu {
public:

	void run_popup();
	void run();
	void run_visuals_preview();

	void __stdcall create_objects(IDirect3DDevice9* device) {
		if (hooks::window)
			ImGui_ImplDX9_CreateDeviceObjects();
	}

	void __stdcall invalidate_objects() {
		ImGui_ImplDX9_InvalidateDeviceObjects();
	}
	void __stdcall setup_resent(IDirect3DDevice9* device);


	void apply_fonts() {
		ImGui::CreateContext();

		font_main = ImGui::GetIO().Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\Tahoma.ttf", 18);
		font_menu = ImGui::GetIO().Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\Tahoma.ttf", 13);
	}

	void __stdcall end_present(IDirect3DDevice9* device) {
		if (opened) {
			if (ImGui::GetStyle().Alpha > 1.f)
				ImGui::GetStyle().Alpha = 1.f;
			else if (ImGui::GetStyle().Alpha != 1.f)
				ImGui::GetStyle().Alpha += 0.05f;
		}

		ImGui::Render();

		state_block->Apply();
		state_block->Release();
	}

	void __stdcall pre_render(IDirect3DDevice9* device) {
		D3DVIEWPORT9 d3d_viewport;
		device->GetViewport(&d3d_viewport);

		device->CreateStateBlock(D3DSBT_ALL, &state_block);
		state_block->Capture();

		device->SetVertexShader(nullptr);
		device->SetPixelShader(nullptr);
		device->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE);

		device->SetRenderState(D3DRS_LIGHTING, FALSE);
		device->SetRenderState(D3DRS_FOGENABLE, FALSE);
		device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
		device->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);

		device->SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE);
		device->SetRenderState(D3DRS_SCISSORTESTENABLE, TRUE);
		device->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
		device->SetRenderState(D3DRS_STENCILENABLE, FALSE);

		device->SetRenderState(D3DRS_MULTISAMPLEANTIALIAS, TRUE);
		device->SetRenderState(D3DRS_ANTIALIASEDLINEENABLE, TRUE);

		device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
		device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
		device->SetRenderState(D3DRS_SEPARATEALPHABLENDENABLE, TRUE);
		device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
		device->SetRenderState(D3DRS_SRCBLENDALPHA, D3DBLEND_INVDESTALPHA);
		device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
		device->SetRenderState(D3DRS_DESTBLENDALPHA, D3DBLEND_ONE);

		device->SetRenderState(D3DRS_SRGBWRITEENABLE, FALSE);
		device->SetRenderState(D3DRS_COLORWRITEENABLE, D3DCOLORWRITEENABLE_RED | D3DCOLORWRITEENABLE_GREEN | D3DCOLORWRITEENABLE_BLUE | D3DCOLORWRITEENABLE_ALPHA);
	}

	void __stdcall post_render() {
		ImGui_ImplDX9_NewFrame();
	}
	ImFont* font_main;
	ImFont* font_menu;
	ImFont* font_main_caps;
	bool opened = false;
private:
	ImDrawData _drawData;
	DWORD dwOld_D3DRS_COLORWRITEENABLE;
};

extern c_menu menu;