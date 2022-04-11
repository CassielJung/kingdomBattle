module clk_generator 
(
    input logic in_clk,  //The input clock whose frequency is to be divided
    input logic [31:0] scale_count, //This value should be (input freq / output freq) / 2 - 1
                                    //For example, if input freq is 50MHz and desired output freq is 1MHz,
                                    //scale_count should be (50 / 1) / 2 - 1 = 24.
    output logic out_clk  //The output clock that has the desired frequency
);
    logic [31:0] curr_count = 32'h0;
    //Count the number of cycles of input clock that has passed. If it has reached the scale_count value,
    //reset the counter and reverse the output clock.
    always_ff @(posedge in_clk) 
    begin
        if (curr_count >= scale_count) 
        begin 
            curr_count <= 32'h0;
            out_clk <= ~out_clk;
        end 
        else 
        begin
            curr_count <= curr_count + 32'h1;
        end
    end
endmodule
