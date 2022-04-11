module ps2_kbd
(
    input clk,
    input ps2_clk,
    input ps2_data,
    input keycode_reset,
    output logic keycode_valid,
    output logic [23:0] keycode_out
);

    logic [7:0] counter;
    logic counter_reset;

    // Synchronize the PS/2 signals to avoid metastability
    logic ps2_clk_sync, ps2_clk_sync1, ps2_data_sync, ps2_data_sync1;
    always_ff @(posedge clk) begin
        ps2_clk_sync1 <= ps2_clk;
        ps2_clk_sync <= ps2_clk_sync1;
        ps2_data_sync1 <= ps2_data;
        ps2_data_sync <= ps2_data_sync1;
    end

    always_ff @(negedge ps2_clk_sync or posedge counter_reset) begin
        if (counter_reset)
            counter <= 8'b0;
        else
            counter <= counter + 8'b1;
    end

    logic [23:0] keycode;
    always_ff @ (negedge ps2_clk_sync) begin
        case (counter)
            8'd1	: keycode[0] <= ps2_data_sync;
            8'd2	: keycode[1] <= ps2_data_sync;
            8'd3	: keycode[2] <= ps2_data_sync;
            8'd4	: keycode[3] <= ps2_data_sync;
            8'd5	: keycode[4] <= ps2_data_sync;
            8'd6	: keycode[5] <= ps2_data_sync;
            8'd7	: keycode[6] <= ps2_data_sync;
            8'd8	: keycode[7] <= ps2_data_sync;
            
            8'd12	: keycode[8] <= ps2_data_sync;
            8'd13	: keycode[9] <= ps2_data_sync;
            8'd14	: keycode[10] <= ps2_data_sync;
            8'd15	: keycode[11] <= ps2_data_sync;
            8'd16	: keycode[12] <= ps2_data_sync;
            8'd17	: keycode[13] <= ps2_data_sync;
            8'd18	: keycode[14] <= ps2_data_sync;
            8'd19	: keycode[15] <= ps2_data_sync;

            8'd23	: keycode[16] <= ps2_data_sync;
            8'd24	: keycode[17] <= ps2_data_sync;
            8'd25	: keycode[18] <= ps2_data_sync;
            8'd26	: keycode[19] <= ps2_data_sync;
            8'd27	: keycode[20] <= ps2_data_sync;
            8'd28	: keycode[21] <= ps2_data_sync;
            8'd29	: keycode[22] <= ps2_data_sync;
            8'd30	: keycode[23] <= ps2_data_sync;
        endcase
    end

    logic data_ready, data_ready_br, data_ready_ext, data_ready_extbr;
    assign data_ready = (counter == 8'd11) & (keycode[7:0] != 8'hf0) & (keycode[7:0] != 8'he0);
    assign data_ready_br = (counter == 8'd22) & (keycode[7:0] == 8'hf0);
    assign data_ready_ext = (counter == 8'd22) & (keycode[7:0] == 8'he0) & (keycode[15:8] != 8'hf0);
    assign data_ready_extbr = counter >= 8'd33;

    always_ff @(posedge clk) begin
        if (data_ready | data_ready_br | data_ready_ext | data_ready_extbr) begin
            counter_reset <= 1'b1;
        end
        else if (counter_reset) begin
            counter_reset <= 1'b0;
        end
    end

    // Reset only when HPS has already read the keycode, so we don't get the same keycode multiple times
    always_ff @(posedge clk or posedge keycode_reset) begin
        if (keycode_reset) begin
            keycode_valid <= 1'b0;
        end
        else if (data_ready) begin
            keycode_out <= {16'b0, keycode[7:0]};
            keycode_valid <= 1'b1;
        end
        else if (data_ready_ext) begin
            keycode_out <= {8'b0, keycode[7:0], keycode[15:8]};
            keycode_valid <= 1'b1;
        end
    end
endmodule