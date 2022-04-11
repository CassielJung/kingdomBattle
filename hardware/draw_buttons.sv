module draw_buttons
(
    input vga_clk,
    input [7:0] x_pos,
    input [5:0] y_pos,
    input button_type,
    output logic [23:0] out_data
);

    logic [13:0] addr;
    assign addr = button_type ? (y_pos * 8'd155 + x_pos + 13'd5890) : (y_pos * 8'd155 + x_pos);
    
    buttons_mem buttons_mem_inst (
        .address(addr),
        .clock(vga_clk),
        .q(out_data),
    );

endmodule