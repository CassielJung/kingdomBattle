module draw_info
(
    input vga_clk,
    input [7:0] x_pos,
    input [4:0] y_pos,
    input [2:0] info_type,
    input [4:0] num,       // The number to be displayed following the text of an information
    output logic [7:0] out_data
);

    logic [7:0] text_width;
    always_comb begin
        case(info_type)
            3'b000: text_width = 8'd100;
            3'b001: text_width = 8'd72;
            3'b010: text_width = 8'd72;
            3'b011: text_width = 8'd44;
            default: text_width = 8'd93;
        endcase
    end

    logic [11:0] text_addr;
    assign text_addr = y_pos * text_width + x_pos;

    logic [3:0] num1, num0;
    always_comb begin
        if (num >= 5'd20) begin
            num1 = 4'd2;
            num0 = num - 5'd20;
        end
        else if (num >= 5'd10) begin
            num1 = 4'd1;
            num0 = num - 5'd10;
        end
        else begin
            num1 = 4'd0;
            num0 = num;
        end
    end

    logic [11:0] num_addr;
    always_comb begin
        if (x_pos >= text_width + 8'd17)
            num_addr = num0 * 9'd408 + y_pos * 5'd17 + x_pos - text_width - 8'd17;
        else
            num_addr = num1 * 9'd408 + y_pos * 5'd17 + x_pos - text_width;
    end

    logic [7:0] time_text_data, round_text_data, buff_text_data, hp_text_data, shield_text_data, nums_data;

    time_text_mem time_text_mem_inst(
        .address(text_addr[10:0]),
        .clock(vga_clk),
        .q(time_text_data)
    );

    round_text_mem round_text_mem_inst(
        .address(text_addr),
        .clock(vga_clk),
        .q(round_text_data)
    );

    buff_text_mem buff_text_mem_inst(
        .address(text_addr[10:0]),
        .clock(vga_clk),
        .q(buff_text_data)
    );

    hp_text_mem hp_text_mem_inst(
        .address(text_addr[10:0]),
        .clock(vga_clk),
        .q(hp_text_data)
    );

    shield_text_mem shield_text_mem_inst(
        .address(text_addr[10:0]),
        .clock(vga_clk),
        .q(shield_text_data)
    );

    text_nums_mem text_nums_mem_inst(
        .address(num_addr),
        .clock(vga_clk),
        .q(nums_data)
    );

    logic [7:0] x_delay1, x_delay2, y_delay1, y_delay2;
    always_ff @(posedge vga_clk) begin
        x_delay1 <= x_pos;
        x_delay2 <= x_delay1;
        y_delay1 <= y_pos;
        y_delay2 <= y_delay1;
    end

    always_comb begin
        if (x_delay2 >= text_width)
            out_data = nums_data;
        else
            case (info_type)
                3'b000: out_data = round_text_data;
                3'b001: out_data = time_text_data;
                3'b010: out_data = buff_text_data;
                3'b011: out_data = hp_text_data;
                default: out_data = shield_text_data;
            endcase
    end

endmodule