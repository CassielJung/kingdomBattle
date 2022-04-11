This folder contains all the source code for the hardware compoenent of the project. It contains two parts: A VGA output driver and a PS/2 keyboard input driver for DE1-SoC.

- canvas.sv takes all the parameters set by HPS and controls the output pixel colors according to the current pixel position and the parameters
- vga_controller.sv contains a counter for current pixel position that confroms to the standard 640*480 VGA specification
- draw_*.sv contains verilog code for drawing different components of the game UI
- *_mem are source files generated by Qartus to use the on-chip memory on DE1-SoC
- textures folder contains all the memory initialization files to initialize the on-chip memory with textures used for game UI. textures/png2mif.py is the python script used to generate these files
- ps2_kbd.sv contains the verilog code for PS/2 keyboard input driver. It reads the keycode and sends it to HPS

Note: The quartus project in this folder is for Quartus Prime 19.1