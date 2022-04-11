	component arm is
		port (
			buff_export               : out   std_logic_vector(7 downto 0);                     -- export
			card_select_export        : out   std_logic_vector(7 downto 0);                     -- export
			clk_clk                   : in    std_logic                     := 'X';             -- clk
			ending_info_export        : out   std_logic_vector(7 downto 0);                     -- export
			enemy_card_used_export    : out   std_logic_vector(7 downto 0);                     -- export
			enemy_card_visible_export : out   std_logic_vector(7 downto 0);                     -- export
			enemy_hp_export           : out   std_logic_vector(7 downto 0);                     -- export
			enemy_shield_export       : out   std_logic_vector(7 downto 0);                     -- export
			initial_screen_export     : out   std_logic_vector(7 downto 0);                     -- export
			keycode_export            : in    std_logic_vector(31 downto 0) := (others => 'X'); -- export
			keycode_reset_export      : out   std_logic_vector(31 downto 0);                    -- export
			memory_mem_a              : out   std_logic_vector(14 downto 0);                    -- mem_a
			memory_mem_ba             : out   std_logic_vector(2 downto 0);                     -- mem_ba
			memory_mem_ck             : out   std_logic;                                        -- mem_ck
			memory_mem_ck_n           : out   std_logic;                                        -- mem_ck_n
			memory_mem_cke            : out   std_logic;                                        -- mem_cke
			memory_mem_cs_n           : out   std_logic;                                        -- mem_cs_n
			memory_mem_ras_n          : out   std_logic;                                        -- mem_ras_n
			memory_mem_cas_n          : out   std_logic;                                        -- mem_cas_n
			memory_mem_we_n           : out   std_logic;                                        -- mem_we_n
			memory_mem_reset_n        : out   std_logic;                                        -- mem_reset_n
			memory_mem_dq             : inout std_logic_vector(31 downto 0) := (others => 'X'); -- mem_dq
			memory_mem_dqs            : inout std_logic_vector(3 downto 0)  := (others => 'X'); -- mem_dqs
			memory_mem_dqs_n          : inout std_logic_vector(3 downto 0)  := (others => 'X'); -- mem_dqs_n
			memory_mem_odt            : out   std_logic;                                        -- mem_odt
			memory_mem_dm             : out   std_logic_vector(3 downto 0);                     -- mem_dm
			memory_oct_rzqin          : in    std_logic                     := 'X';             -- oct_rzqin
			my_card_1_export          : out   std_logic_vector(7 downto 0);                     -- export
			my_card_2_export          : out   std_logic_vector(7 downto 0);                     -- export
			my_card_3_export          : out   std_logic_vector(7 downto 0);                     -- export
			my_card_used_export       : out   std_logic_vector(7 downto 0);                     -- export
			my_hp_export              : out   std_logic_vector(7 downto 0);                     -- export
			my_shield_export          : out   std_logic_vector(7 downto 0);                     -- export
			reset_reset_n             : in    std_logic                     := 'X';             -- reset_n
			round_export              : out   std_logic_vector(7 downto 0);                     -- export
			time_export               : out   std_logic_vector(7 downto 0);                     -- export
			ult_info_export           : out   std_logic_vector(7 downto 0);                     -- export
			show_instr_export         : out   std_logic_vector(7 downto 0)                      -- export
		);
	end component arm;

	u0 : component arm
		port map (
			buff_export               => CONNECTED_TO_buff_export,               --               buff.export
			card_select_export        => CONNECTED_TO_card_select_export,        --        card_select.export
			clk_clk                   => CONNECTED_TO_clk_clk,                   --                clk.clk
			ending_info_export        => CONNECTED_TO_ending_info_export,        --        ending_info.export
			enemy_card_used_export    => CONNECTED_TO_enemy_card_used_export,    --    enemy_card_used.export
			enemy_card_visible_export => CONNECTED_TO_enemy_card_visible_export, -- enemy_card_visible.export
			enemy_hp_export           => CONNECTED_TO_enemy_hp_export,           --           enemy_hp.export
			enemy_shield_export       => CONNECTED_TO_enemy_shield_export,       --       enemy_shield.export
			initial_screen_export     => CONNECTED_TO_initial_screen_export,     --     initial_screen.export
			keycode_export            => CONNECTED_TO_keycode_export,            --            keycode.export
			keycode_reset_export      => CONNECTED_TO_keycode_reset_export,      --      keycode_reset.export
			memory_mem_a              => CONNECTED_TO_memory_mem_a,              --             memory.mem_a
			memory_mem_ba             => CONNECTED_TO_memory_mem_ba,             --                   .mem_ba
			memory_mem_ck             => CONNECTED_TO_memory_mem_ck,             --                   .mem_ck
			memory_mem_ck_n           => CONNECTED_TO_memory_mem_ck_n,           --                   .mem_ck_n
			memory_mem_cke            => CONNECTED_TO_memory_mem_cke,            --                   .mem_cke
			memory_mem_cs_n           => CONNECTED_TO_memory_mem_cs_n,           --                   .mem_cs_n
			memory_mem_ras_n          => CONNECTED_TO_memory_mem_ras_n,          --                   .mem_ras_n
			memory_mem_cas_n          => CONNECTED_TO_memory_mem_cas_n,          --                   .mem_cas_n
			memory_mem_we_n           => CONNECTED_TO_memory_mem_we_n,           --                   .mem_we_n
			memory_mem_reset_n        => CONNECTED_TO_memory_mem_reset_n,        --                   .mem_reset_n
			memory_mem_dq             => CONNECTED_TO_memory_mem_dq,             --                   .mem_dq
			memory_mem_dqs            => CONNECTED_TO_memory_mem_dqs,            --                   .mem_dqs
			memory_mem_dqs_n          => CONNECTED_TO_memory_mem_dqs_n,          --                   .mem_dqs_n
			memory_mem_odt            => CONNECTED_TO_memory_mem_odt,            --                   .mem_odt
			memory_mem_dm             => CONNECTED_TO_memory_mem_dm,             --                   .mem_dm
			memory_oct_rzqin          => CONNECTED_TO_memory_oct_rzqin,          --                   .oct_rzqin
			my_card_1_export          => CONNECTED_TO_my_card_1_export,          --          my_card_1.export
			my_card_2_export          => CONNECTED_TO_my_card_2_export,          --          my_card_2.export
			my_card_3_export          => CONNECTED_TO_my_card_3_export,          --          my_card_3.export
			my_card_used_export       => CONNECTED_TO_my_card_used_export,       --       my_card_used.export
			my_hp_export              => CONNECTED_TO_my_hp_export,              --              my_hp.export
			my_shield_export          => CONNECTED_TO_my_shield_export,          --          my_shield.export
			reset_reset_n             => CONNECTED_TO_reset_reset_n,             --              reset.reset_n
			round_export              => CONNECTED_TO_round_export,              --              round.export
			time_export               => CONNECTED_TO_time_export,               --               time.export
			ult_info_export           => CONNECTED_TO_ult_info_export,           --           ult_info.export
			show_instr_export         => CONNECTED_TO_show_instr_export          --         show_instr.export
		);

