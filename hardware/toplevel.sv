module toplevel
(
    input CLOCK_50,
    input KEY[0],

    input PS2_CLK,   // PS/2 pins
    input PS2_DAT,

    output logic VGA_CLK,      // VGA pins
    output logic VGA_BLANK_N,
    output logic VGA_SYNC_N,
    output logic VGA_HS,
    output logic VGA_VS,
    output logic [7:0] VGA_R,
    output logic [7:0] VGA_G,
    output logic [7:0] VGA_B,

    output [14:0] HPS_DDR3_ADDR,  // HPS pins
    output [2:0]  HPS_DDR3_BA,
    output HPS_DDR3_CAS_N,
    output HPS_DDR3_CKE,
    output HPS_DDR3_CK_N,
    output HPS_DDR3_CK_P,
    output HPS_DDR3_CS_N,
    output [3:0] HPS_DDR3_DM,
    inout [31:0] HPS_DDR3_DQ,
    inout [3:0] HPS_DDR3_DQS_N,
    inout [3:0]	HPS_DDR3_DQS_P,
    output HPS_DDR3_ODT,
    output HPS_DDR3_RAS_N,
    output HPS_DDR3_RESET_N,
    input HPS_DDR3_RZQ,
    output HPS_DDR3_WE_N
);

    logic vga_clk;
    logic [9:0] x_pos, y_pos;
    logic [23:0] vga_out;
    assign VGA_CLK = vga_clk;
    assign VGA_SYNC_N = 1'b0;
    assign VGA_R = VGA_BLANK_N ? vga_out[23:16] : 8'b0;
    assign VGA_G = VGA_BLANK_N ? vga_out[15:8] : 8'b0;
    assign VGA_B = VGA_BLANK_N ? vga_out[7:0] : 8'b0;

    // Generate 25MHz clock for VGA
    clk_generator gen_vga_clk(
        .in_clk(CLOCK_50), 
        .scale_count(0), 
        .out_clk(vga_clk)
    );
    
    vga_controller vga_ctrl(
        .vga_clk(vga_clk),
        .reset(~KEY[0]),
        .vga_hs(VGA_HS),
        .vga_vs(VGA_VS),
        .vga_blank(VGA_BLANK_N),
        .x_pos(x_pos),
        .y_pos(y_pos)
    );

    // Hardware timer for the time displayed on the screen
    logic [6:0] curr_time, set_timer;
    logic [31:0] counter;
    logic start_timer;
    always_ff @(posedge CLOCK_50) begin
        if (start_timer) 
            curr_time <= set_timer;
        else if (counter >= 32'd50000000 && curr_time != 7'b0)
            curr_time <= curr_time - 7'b1;
        else if (counter >= 32'd50000000)
            curr_time <= 7'b0;
    end

    always_ff @(posedge CLOCK_50) begin
        if (counter >= 32'd50000000)
            counter <= 32'd0;
        else
            counter <= counter + 32'b1;
    end
    
    // Core for driving the VGA output
    canvas canvas_inst(
        .vga_clk(vga_clk),
        .x_pos(x_pos),
        .y_pos(y_pos),
        .vga_out(vga_out),
        .initial_screen     (initial_screen),
        .card_select        (card_select),       
		.my_card_1          (my_card_1),         
		.my_card_2          (my_card_2),         
		.my_card_3          (my_card_3),       
		.my_card_used       (my_card_used),      
		.enemy_card_used    (enemy_card_used),  
		.enemy_card_visible (enemy_card_visible), 
		.my_hp              (my_hp),            
		.my_shield          (my_shield),         
		.time_num           (curr_time),         
		.round              (round),        
		.enemy_hp           (enemy_hp),        
		.enemy_shield       (enemy_shield),    
		.buff               (buff),
        .ult_info           (ult_info),
        .ending_info        (ending_info),
        .show_instr         (show_instr)
    );

    // Connect all the VGA controlling registers to HPS
    logic keycode_reset, keycode_valid;
    logic [23:0] keycode;
    logic [7:0] initial_screen, card_select, my_card_1, my_card_2, my_card_3, my_card_used, enemy_card_used, show_instr;
    logic [7:0] enemy_card_visible, my_hp, my_shield, time_num, round, enemy_hp, enemy_shield, buff, ult_info, ending_info;
    arm arm_inst (
		.clk_clk            (CLOCK_50),   
		.reset_reset_n      (KEY[0]),  
		.memory_mem_a			(HPS_DDR3_ADDR),
        .memory_mem_ba			(HPS_DDR3_BA),
        .memory_mem_ck			(HPS_DDR3_CK_P),
        .memory_mem_ck_n		(HPS_DDR3_CK_N),
        .memory_mem_cke		(HPS_DDR3_CKE),
        .memory_mem_cs_n		(HPS_DDR3_CS_N),
        .memory_mem_ras_n		(HPS_DDR3_RAS_N),
        .memory_mem_cas_n		(HPS_DDR3_CAS_N),
        .memory_mem_we_n		(HPS_DDR3_WE_N),
        .memory_mem_reset_n	(HPS_DDR3_RESET_N),
        .memory_mem_dq			(HPS_DDR3_DQ),
        .memory_mem_dqs		(HPS_DDR3_DQS_P),
        .memory_mem_dqs_n		(HPS_DDR3_DQS_N),
        .memory_mem_odt		(HPS_DDR3_ODT),
        .memory_mem_dm			(HPS_DDR3_DM),
        .memory_oct_rzqin		(HPS_DDR3_RZQ),
        .keycode_export     ({keycode_valid, keycode}),
        .keycode_reset_export (keycode_reset),
        .initial_screen_export     (initial_screen),
        .card_select_export        (card_select),  
		.my_card_1_export          (my_card_1),   
		.my_card_2_export          (my_card_2),   
		.my_card_3_export          (my_card_3),     
		.my_card_used_export       (my_card_used),  
		.enemy_card_used_export    (enemy_card_used),   
		.enemy_card_visible_export (enemy_card_visible), 
		.my_hp_export              (my_hp),           
		.my_shield_export          (my_shield),   
		.time_export               ({start_timer, set_timer}),          
		.round_export              (round),     
		.enemy_hp_export           (enemy_hp),    
		.enemy_shield_export       (enemy_shield),   
		.buff_export               (buff),
        .ult_info_export           (ult_info),
        .ending_info_export        (ending_info),
        .show_instr_export         (show_instr) 
	);

    // PS/2 keyboard module
    ps2_kbd kbd_inst(
        .clk(CLOCK_50),
        .ps2_clk(PS2_CLK),
        .ps2_data(PS2_DAT),
        .keycode_reset(keycode_reset),
        .keycode_valid(keycode_valid),
        .keycode_out(keycode)
    );
endmodule