module vga_controller
(
    input vga_clk,
    input reset,
    output logic vga_hs,
    output logic vga_vs,
    output logic vga_blank,
    output logic [9:0] x_pos,
    output logic [9:0] y_pos
);

    always_ff @(posedge vga_clk or posedge reset) begin
        if (reset) begin
            x_pos <= 10'b0;
            y_pos <= 10'b0;
        end
        else if (x_pos >= 10'd799) begin
            x_pos <= 10'b0;
            if (y_pos >= 10'd524) begin
                y_pos <= 10'b0;
            end
            else begin
                y_pos <= y_pos + 10'b1;
            end
        end
        else begin
            x_pos <= x_pos + 10'b1;
        end
    end

    logic [2:0] vga_delay1, vga_delay2;

    always_ff @(posedge vga_clk) begin
        vga_delay1[2] <= ~((x_pos >= 10'd656) & (x_pos <= 10'd751));
        vga_delay1[1] <= ~((y_pos >= 10'd490) & (y_pos <= 10'd491));
        vga_delay1[0] <= ~((x_pos >= 10'd640) | (y_pos >= 10'd480));
        vga_delay2 <= vga_delay1;
        {vga_hs, vga_vs, vga_blank} <= vga_delay2;
    end  

endmodule