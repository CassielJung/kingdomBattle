module draw_ending
(
    input vga_clk,
    input [8:0] x_pos,
    input [6:0] y_pos,
    input ending_type,
    output logic [23:0] out_data
);

    logic [14:0] addr;
    assign addr = ending_type ? ((y_pos >> 1) * 8'd168 + (x_pos >> 1) + 14'd8232) : ((y_pos >> 1) * 8'd168 + (x_pos >> 1));
    
    ending_mem ending_mem_inst (
        .address(addr),
        .clock(vga_clk),
        .q(out_data),
    );

endmodule