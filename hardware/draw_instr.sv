module draw_instr
(
    input vga_clk,
    input [7:0] x_pos,
    input [4:0] y_pos,
    input instr_type,
    output logic [7:0] out_data
);

    logic [13:0] addr;
    assign addr = instr_type ? (y_pos * 8'd237 + x_pos + 13'd4977) : (y_pos * 8'd237 + x_pos);
      
    instr_mem instr_mem_inst (
        .address(addr),
        .clock(vga_clk),
        .q(out_data),
    );

endmodule