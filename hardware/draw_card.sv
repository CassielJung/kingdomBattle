module draw_card
(
    input vga_clk,
    input [6:0] x_pos,
    input [6:0] y_pos,
    input [2:0] card_type,
    input [3:0] card_num,
    output logic [23:0] out_data
);

    logic [14:0] bg_addr;
    assign bg_addr = y_pos * 6'd54 + x_pos + 12'd3672 * card_type;

    logic [13:0] num_addr;
    assign num_addr = card_num * 10'd880 + y_pos * 5'd22 + (x_pos - 7'd16);

    logic [23:0] bg_data;
    logic [7:0] num_data;

    card_bg_mem card_bg_mem_inst(
        .address(bg_addr),
        .clock(vga_clk),
        .q(bg_data)
    );

    card_nums_mem card_nums_mem_inst(
        .address(num_addr),
        .clock(vga_clk),
        .q(num_data)
    );

    logic [6:0] x_delay1, x_delay2, y_delay1, y_delay2;
    always_ff @(posedge vga_clk) begin
        x_delay1 <= x_pos;
        x_delay2 <= x_delay1;
        y_delay1 <= y_pos;
        y_delay2 <= y_delay1;
    end

    // Combine the intensity of number and the color of card background to get final output color 
    logic [7:0] inter_r, inter_g, inter_b;
    logic [8:0] sum_r, sum_g, sum_b;
    assign sum_r = {1'b0, bg_data[23:16]} + {1'b0, num_data};
    assign sum_g = {1'b0, bg_data[15:8]} + {1'b0, num_data};
    assign sum_b = {1'b0, bg_data[7:0]} + {1'b0, num_data};
    assign inter_r = sum_r > 9'd255 ? 8'hff : sum_r[7:0];
    assign inter_g = sum_g > 9'd255 ? 8'hff : sum_g[7:0];
    assign inter_b = sum_b > 9'd255 ? 8'hff : sum_b[7:0];

    always_comb begin
        if (x_delay2 >= 7'd16 && x_delay2 <= 7'd37 && y_delay2 <= 7'd39 && num_data != 8'b0 && card_type <= 3'd2)
            out_data = {inter_r, inter_g, inter_b};
        else
            out_data = bg_data;
    end

endmodule