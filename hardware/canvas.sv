module canvas
(
    input vga_clk,
    input [9:0] x_pos,
    input [9:0] y_pos,
    input [7:0] initial_screen,
    input [7:0] card_select,       
	input [7:0]	my_card_1,
	input [7:0]	my_card_2,         
	input [7:0]	my_card_3,
	input [7:0]	my_card_used,
	input [7:0]	enemy_card_used,
	input [7:0]	enemy_card_visible,
	input [7:0]	my_hp,
	input [7:0]	my_shield,
	input [7:0]	time_num,
	input [7:0]	round,
	input [7:0]	enemy_hp,
	input [7:0]	enemy_shield,
	input [7:0]	buff,
    input [7:0] ult_info,
    input [7:0] ending_info,
    input [7:0] show_instr,
    output logic [23:0] vga_out
);

    // Calculate the address for background texture
    logic [9:0] bg_addr;
    logic [23:0] bg_data;
    assign bg_addr = {y_pos[4:0], x_pos[4:0]};
    
    // Get the background color data of current pixel
    bg_tex_mem bg_mem_inst(
        .address(bg_addr),
        .clock(vga_clk),
        .q(bg_data)
    );

    // Draw buttons on the initial screen
    logic [23:0] button_data;
    logic [7:0] button_x_pos;
    logic [5:0] button_y_pos;
    logic button_type;
    assign button_x_pos = x_pos - 10'd243;
    assign button_y_pos = y_pos >= 10'd260 ? y_pos - 10'd260 : y_pos - 10'd180;
    assign button_type = y_pos >= 10'd260 ? 1'b1 : 1'b0;

    draw_buttons draw_buttons_inst(
        .vga_clk(vga_clk),
        .x_pos(button_x_pos),
        .y_pos(button_y_pos),
        .button_type(button_type),
        .out_data(button_data)
    );

    // Draw all the cards, including the cards of both players
    logic [23:0] card_data;
    logic [6:0] card_x_pos;
    logic [6:0] card_y_pos;
    always_comb begin
        if (x_pos >= 10'd453)
            card_x_pos = x_pos - 10'd453;
        else if (x_pos >= 10'd367)
            card_x_pos = x_pos - 10'd367;
        else if (x_pos >= 10'd294)
            card_x_pos = x_pos - 10'd294;
        else 
            card_x_pos = x_pos - 10'd221;
        if (y_pos >= 10'd395)
            card_y_pos = y_pos - 10'd395;
        else if (y_pos >= 10'd275)
            card_y_pos = y_pos - 10'd275;
        else if (y_pos >= 10'd138)
            card_y_pos = y_pos - 10'd138;
        else
            card_y_pos = y_pos - 10'd17;
    end

    logic [2:0] card_type;
    logic [3:0] card_num;
    logic card_visible;
    always_comb begin
        if (x_pos >= 10'd453) begin
            card_num = 4'b0;
            card_visible = 1'b1;
            if (y_pos >= 10'd395)
                card_type = ult_info[2:0] + 3'd3;
            else
                card_type = ult_info[5:3] + 3'd3;
        end
        else if (y_pos <= 10'd84) begin
            card_type = 2'b10;
            card_num = 4'd10;
            if (x_pos >= 10'd367)
                card_visible = enemy_card_visible[0];
            else if (x_pos >= 10'd294)
                card_visible = enemy_card_visible[1];
            else
                card_visible = enemy_card_visible[2];
        end
        else if (y_pos <= 10'd205) begin
            card_type = {1'b0, enemy_card_used[0]};
            card_num = enemy_card_used[4:1];
            card_visible = enemy_card_used[7];
        end
        else if (y_pos <= 10'd342) begin
            card_type = {1'b0, my_card_used[0]};
            card_num = my_card_used[4:1];
            card_visible = my_card_used[7];
        end
        else if (x_pos >= 10'd367) begin
            card_type = {1'b0, my_card_3[0]};
            card_num = my_card_3[4:1];
            card_visible = my_card_3[7];
        end
        else if (x_pos >= 10'd294) begin
            card_type = {1'b0, my_card_2[0]};
            card_num = my_card_2[4:1];
            card_visible = my_card_2[7];
        end
        else begin
            card_type = {1'b0, my_card_1[0]};
            card_num = my_card_1[4:1];
            card_visible = my_card_1[7];
        end
    end

    draw_card draw_card_inst(
        .vga_clk(vga_clk),
        .x_pos(card_x_pos),
        .y_pos(card_y_pos),
        .card_type(card_type),
        .card_num(card_num),
        .out_data(card_data)
    );

    // Draw informations, including HP, shield, round, and time
    logic [7:0] info_data;
    logic [7:0] info_x_pos;
    logic [4:0] info_y_pos;
    logic [2:0] info_type;
    logic [4:0] info_num;
    always_comb begin
        if (x_pos >= 10'd505) begin
            info_x_pos = x_pos - 10'd505;
            info_type = 3'b010;
        end
        else begin
            if (y_pos >= 10'd440 || y_pos <= 10'd40)
                info_type = 3'b011;
            else if (y_pos >= 10'd393 || y_pos <= 89)
                info_type = 3'b100;
            else if (y_pos >= 10'd247)
                info_type = 3'b001;
            else
                info_type = 3'b000;
            info_x_pos = x_pos - 10'd5;
        end
        if (y_pos >= 10'd440) begin
            info_y_pos = y_pos - 10'd440;
            info_num = my_hp;
        end
        else if (y_pos >= 10'd393) begin
            info_y_pos = y_pos - 10'd393;
            info_num = my_shield;
        end
        else if (y_pos >= 10'd247) begin
            info_y_pos = y_pos - 10'd247;
            info_num = time_num;
        end
        else if (y_pos >= 10'd207) begin
            info_y_pos = y_pos - 10'd207;
            info_num = round;
        end
        else if (y_pos >= 10'd66) begin
            info_y_pos = y_pos - 10'd66;
            info_num = enemy_shield;
        end
        else begin
            info_y_pos = y_pos - 10'd16;
            info_num = enemy_hp;
        end
    end

    draw_info draw_info_inst(
        .vga_clk(vga_clk),
        .x_pos(info_x_pos),
        .y_pos(info_y_pos),
        .info_type(info_type),
        .num(info_num),
        .out_data(info_data)
    );

    // Calculate the true output color from a grayscale value
    logic [23:0] converted_info_data;
    assign converted_info_data = info_data <= 8'h40 ? bg_data : {3{8'hff - info_data}};

    // Draw the current round buff
    logic [7:0] buff_data;
    logic [6:0] buff_x_pos;
    logic [6:0] buff_y_pos;
    assign buff_x_pos = x_pos - 10'd505;
    assign buff_y_pos = y_pos - 10'd275;

    draw_buff_desc draw_buff_desc_inst(
        .vga_clk(vga_clk),
        .x_pos(buff_x_pos),
        .y_pos(buff_y_pos),
        .buff_type(buff[1:0]),
        .out_data(buff_data)
    );

    logic [23:0] converted_buff_data;
    assign converted_buff_data = buff_data <= 8'h40 ? bg_data : {3{8'hff - buff_data}};

    // Draw the ending information (i.e. "You Win / You Lose" message)
    logic [23:0] ending_data;
    logic [8:0] ending_x_pos;
    logic [6:0] ending_y_pos;
    logic ending_type;
    assign ending_x_pos = x_pos - 10'd152;
    assign ending_y_pos = y_pos - 10'd191;
    assign ending_type = ending_info[1];

    draw_ending draw_ending_inst(
        .vga_clk(vga_clk),
        .x_pos(ending_x_pos),
        .y_pos(ending_y_pos),
        .ending_type(ending_type),
        .out_data(ending_data)
    );

    // Draw instructions to prompt for user input
    logic [7:0] instr_data;
    logic [7:0] instr_x_pos;
    logic [4:0] instr_y_pos;
    logic instr_type;
    assign instr_x_pos = x_pos <= 10'd440 ? x_pos - 10'd190 : x_pos - 10'd445;
    assign instr_y_pos = y_pos - 10'd350;
    assign instr_type = x_pos <= 10'd440 ? 1'b0 : 1'b1;

    draw_instr draw_instr_inst(
        .vga_clk(vga_clk),
        .x_pos(instr_x_pos),
        .y_pos(instr_y_pos),
        .instr_type(instr_type),
        .out_data(instr_data)
    );

    logic [23:0] converted_instr_data;
    assign converted_instr_data = instr_data <= 8'h40 ? bg_data : {3{8'hff - instr_data}};

    // Delay the position signal for two clock cycles to synchronize with on-chip memory
    logic [9:0] x_delay1, x_delay2, y_delay1, y_delay2, visible_delay1, visible_delay2;
    always_ff @(posedge vga_clk) begin
        x_delay1 <= x_pos;
        x_delay2 <= x_delay1;
        y_delay1 <= y_pos;
        y_delay2 <= y_delay1;
        visible_delay1 <= card_visible;
        visible_delay2 <= visible_delay1;
    end

    // Determine the output color
    logic [23:0] vga_out_reg;
    always_ff @(posedge vga_clk) begin
        if (~initial_screen[0]) begin
            if (x_delay2 >= 10'd243 && x_delay2 <= 10'd397 && 
                    ((y_delay2 >= 10'd180 && y_delay2 <= 10'd217) || (y_delay2 >= 10'd260 && y_delay2 <= 10'd297))) begin
                vga_out_reg <= button_data;
            end
            else if (x_delay2 >= 10'd238 && x_delay2 <= 10'd402 && 
                    ((y_delay2 >= 10'd175 && y_delay2 <= 10'd222 && ~initial_screen[1]) || 
                    (y_delay2 >= 10'd255 && y_delay2 <= 10'd302 && initial_screen[1]))) begin
                vga_out_reg <= 24'h37c837;
            end
            else begin
                vga_out_reg <= bg_data;
            end
        end
        else if (ending_info[0] && x_delay2 >= 10'd152 && x_delay2 <= 10'd487 && y_delay2 >= 10'd191 && y_delay2 <= 10'd288) begin
            vga_out_reg <= ending_data;
        end
        else begin
            if ((y_delay2 >= 10'd239 && y_delay2 <= 10'd241) || 
                    ((x_delay2 == 10'd200 || x_delay2 == 10'd201 || x_delay2 == 10'd439 || x_delay2 == 10'd440 || 
                    x_delay2 == 10'd519 || x_delay2 == 10'd520) && (y_delay2 >= 10'd380 || y_delay2 <=10'd100)) ||
                    ((y_delay2 == 10'd379 || y_delay2 == 10'd378 || y_delay2 == 10'd99 || y_delay2 == 10'd 100) && 
                    x_delay2 <= 10'd520) ||
                    ((y_delay2 == 10'd200 || y_delay2 == 10'd201 || y_delay2 == 10'd279 || y_delay2 == 10'd280) && x_delay2 <= 10'd160) ||
                    ((x_delay2 == 10'd160 || x_delay2 == 10'd161) && y_delay2 >= 10'd200 && y_delay2 <= 10'd280) ||
                    ((y_delay2 == 10'd320 || y_delay2 == 10'd321) && x_delay2 >= 10'd500) || 
                    ((x_delay2 == 10'd500 || x_delay2 == 10'd501) && y_delay2 >= 10'd241 && y_delay2 <= 320)||
                    ((y_delay2 == 10'd50 || y_delay2 == 10'd51 || y_delay2 == 10'd429 || y_delay2 == 10'd430) && x_delay2 <= 10'd200)) begin
                vga_out_reg <= 24'h3a2808;
            end
            else if (((((x_delay2 >= 10'd221 && x_delay2 <= 10'd274) || (x_delay2 >= 10'd294 && x_delay2 <= 10'd347) ||
                    (x_delay2 >= 10'd367 && x_delay2 <= 10'd420) || (x_delay2 >= 10'd453 && x_delay2 <= 10'd506)) && 
                    ((y_delay2 >= 10'd395 && y_delay2 <= 10'd462) || (y_delay2 >= 10'd17 && y_delay2 <= 10'd84))) || 
                    (x_delay2 >= 10'd294 && x_delay2 <= 10'd347 && ((y_delay2 >= 10'd275 && y_delay2 <= 10'd342) || 
                    (y_delay2 >= 10'd138 && y_delay2 <= 10'd205)))) && card_visible) begin
                vga_out_reg <= card_data;
            end
            else if ((x_delay2 >= 10'd5 && x_delay2 <= 10'd138 && y_delay2 >= 10'd207 && y_delay2 <= 10'd230) ||
                    (x_delay2 >= 10'd5 && x_delay2 <= 10'd110 && y_delay2 >= 10'd247 && y_delay2 <= 10'd270) ||
                    (x_delay2 >= 10'd505 && x_delay2 <= 10'd576 && y_delay2 >= 10'd247 && y_delay2 <= 10'd270) ||
                    (x_delay2 >= 10'd5 && x_delay2 <= 10'd82 && y_delay2 >= 10'd16 && y_delay2 <= 10'd39) ||
                    (x_delay2 >= 10'd5 && x_delay2 <= 10'd82 && y_delay2 >= 10'd440 && y_delay2 <= 10'd463) ||
                    (x_delay2 >= 10'd5 && x_delay2 <= 10'd131 && y_delay2 >= 10'd66 && y_delay2 <= 10'd89) ||
                    (x_delay2 >= 10'd5 && x_delay2 <= 10'd131 && y_delay2 >= 10'd393 && y_delay2 <= 10'd416)) begin
                vga_out_reg <= converted_info_data;
            end
            else if (x_delay2 >= 10'd505 && x_delay2 <= 10'd596 && y_delay2 >= 10'd275 && y_delay2 <= 10'd316) begin
                vga_out_reg <= converted_buff_data;
            end
            else if ((y_delay2 >= 10'd388 && y_delay2 <= 10'd469 &&
                    ((x_delay2 >= 10'd214 && x_delay2 <= 10'd281 && card_select[1:0] == 2'd0) || 
                    (x_delay2 >= 10'd287 && x_delay2 <= 10'd354 && card_select[1:0] == 2'd1) ||
                    (x_delay2 >= 10'd360 && x_delay2 <= 10'd427 && card_select[1:0] == 2'd2) || 
                    (x_delay2 >= 10'd446 && x_delay2 <= 10'd513 && ult_info[6]))) ||
                    (y_delay2 >= 10'd10 && y_delay2 <= 10'd91 && x_delay2 >= 10'd446 && x_delay2 <= 10'd513 && ult_info[7])) begin
                vga_out_reg <= 24'h37c837;
            end
            else if (y_delay2 >= 10'd350 && y_delay2 <= 10'd370 &&
                    ((x_delay2 >= 10'd190 && x_delay2 <= 10'd427 && show_instr[0]) || 
                    (x_delay2 >= 10'd445 && x_delay2 <= 10'd555 && show_instr[1]))) begin
                vga_out_reg <= converted_instr_data;
            end
            else begin
                vga_out_reg <= bg_data;
            end
        end
    end

    // Dimming some pixels as needed
    logic [7:0] vga_out_r, vga_out_g, vga_out_b;
    assign vga_out_r = vga_out_reg[23:16] <= 8'd40 ? 8'b0 : vga_out_reg[23:16] - 8'd40;
    assign vga_out_g = vga_out_reg[15:8] <= 8'd40 ? 8'b0 : vga_out_reg[15:8] - 8'd40;
    assign vga_out_b = vga_out_reg[7:0] <= 8'd40 ? 8'b0 : vga_out_reg[7:0] - 8'd40;

    always_comb begin
        if ((initial_screen[0] && (x_delay2 >= 10'd201 && x_delay2 <= 10'd520 && y_delay2 >= 10'd380 && ~show_instr[0]) ||
                (x_delay2 >= 10'd201 && x_delay2 <= 10'd520 && y_delay2 <= 10'd99 && show_instr[0])) || 
                (ending_info[0] && ~(x_delay2 >= 10'd153 && x_delay2 <= 10'd488 && y_delay2 >= 10'd191 && y_delay2 <= 10'd288)))
            vga_out = {vga_out_r, vga_out_g, vga_out_b};
        else
            vga_out = vga_out_reg;
    end

endmodule