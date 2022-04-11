
module arm (
	buff_export,
	card_select_export,
	clk_clk,
	ending_info_export,
	enemy_card_used_export,
	enemy_card_visible_export,
	enemy_hp_export,
	enemy_shield_export,
	initial_screen_export,
	keycode_export,
	keycode_reset_export,
	memory_mem_a,
	memory_mem_ba,
	memory_mem_ck,
	memory_mem_ck_n,
	memory_mem_cke,
	memory_mem_cs_n,
	memory_mem_ras_n,
	memory_mem_cas_n,
	memory_mem_we_n,
	memory_mem_reset_n,
	memory_mem_dq,
	memory_mem_dqs,
	memory_mem_dqs_n,
	memory_mem_odt,
	memory_mem_dm,
	memory_oct_rzqin,
	my_card_1_export,
	my_card_2_export,
	my_card_3_export,
	my_card_used_export,
	my_hp_export,
	my_shield_export,
	reset_reset_n,
	round_export,
	time_export,
	ult_info_export,
	show_instr_export);	

	output	[7:0]	buff_export;
	output	[7:0]	card_select_export;
	input		clk_clk;
	output	[7:0]	ending_info_export;
	output	[7:0]	enemy_card_used_export;
	output	[7:0]	enemy_card_visible_export;
	output	[7:0]	enemy_hp_export;
	output	[7:0]	enemy_shield_export;
	output	[7:0]	initial_screen_export;
	input	[31:0]	keycode_export;
	output	[31:0]	keycode_reset_export;
	output	[14:0]	memory_mem_a;
	output	[2:0]	memory_mem_ba;
	output		memory_mem_ck;
	output		memory_mem_ck_n;
	output		memory_mem_cke;
	output		memory_mem_cs_n;
	output		memory_mem_ras_n;
	output		memory_mem_cas_n;
	output		memory_mem_we_n;
	output		memory_mem_reset_n;
	inout	[31:0]	memory_mem_dq;
	inout	[3:0]	memory_mem_dqs;
	inout	[3:0]	memory_mem_dqs_n;
	output		memory_mem_odt;
	output	[3:0]	memory_mem_dm;
	input		memory_oct_rzqin;
	output	[7:0]	my_card_1_export;
	output	[7:0]	my_card_2_export;
	output	[7:0]	my_card_3_export;
	output	[7:0]	my_card_used_export;
	output	[7:0]	my_hp_export;
	output	[7:0]	my_shield_export;
	input		reset_reset_n;
	output	[7:0]	round_export;
	output	[7:0]	time_export;
	output	[7:0]	ult_info_export;
	output	[7:0]	show_instr_export;
endmodule
