module draw_buff_desc
(
    input vga_clk,
    input [6:0] x_pos,
    input [6:0] y_pos,
    input [1:0] buff_type,
    output logic [23:0] out_data
);

    logic [13:0] addr;
    always_comb begin
        case (buff_type)
            2'b00: addr = y_pos * 7'd92 + x_pos;
            2'b01: addr = y_pos * 7'd92 + x_pos + 13'd3864;
            default: addr = y_pos * 7'd92 + x_pos + 13'd7728;
        endcase
    end
    
    buff_desc_mem buff_desc_mem_inst (
        .address(addr),
        .clock(vga_clk),
        .q(out_data),
    );

endmodule