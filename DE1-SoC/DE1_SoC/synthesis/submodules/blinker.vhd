library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_unsigned.all;
entity blinker is
	port( clk:		in std_logic;
			reset:	in std_logic;
			
			switches:	in std_logic_vector(7 downto 0);
			buttons:		in std_logic_vector(3 downto 0);
			leds:			inout std_logic_vector(7 downto 0);
			
			address:		in std_logic;
			write:		in std_logic;
			writedata:	in std_logic_vector(7 downto 0);
			read: 		in std_logic;
			readdata:	out std_logic_vector(7 downto 0);
			irq:			buffer std_logic
			);
end entity;

architecture rtl of blinker is

-- Pulse shaper signals
signal sync_buttons: std_logic_vector(3 downto 0);
signal cur_value: 	std_logic_vector(3 downto 0);
signal last_value: 	std_logic_vector(3 downto 0);

-- Speed Control signals
signal speed:			std_logic_vector(3 downto 0);
signal delay:			std_logic_vector(3 downto 0);
signal faster: 		std_logic;
signal slower:			std_logic;

-- Blinker signals
signal count: 			std_logic_vector(23 downto 0);
signal position:		std_logic_vector(3 downto 0);
signal running: 		std_logic;
signal mode:			std_logic;
signal pause:			std_logic;
signal change_mode:	std_logic;

-- Interrupt controller signals
signal ena_irq:		std_logic;

begin
		-- Pulse shaper ----------------------------
		pulse_shaper:
		process(clk, reset)
		begin
				if reset = '1' then
					cur_value 	<= "1111";
					last_value 	<= "1111";
				elsif clk'event and clk = '1' then
					cur_value 	<= buttons;
					last_value	<= cur_value;
				end if;
		end process;
		
		sync_buttons <= buttons;
		
		-- speed control ---------------------------
		faster <= sync_buttons(0);
		slower <= sync_buttons(1);
		process(clk, reset)
		begin
				if (reset = '1') then
						speed <= x"8";
				elsif clk'event and clk = '1' then
						if(faster = '1' and speed /= 15) then
								speed <= speed + 1;
								
						elsif (slower = '1' and speed /= 1) then
								speed <= speed - 1;
						elsif write = '1' and address = '0' then
								speed <= writedata(3 downto 0); -- 0 behandeln
						end if;
				end if;
		end process;
		
		-- blinker --------------------------------
		change_mode <= sync_buttons(3);
		pause <= sync_buttons(2);
		
		-- leds manager
		process(position, switches, mode)
		begin
				if mode = '0' then
						case position is
								when "0000" => leds <= "00000001";
								when "0001" => leds <= "00000010";
								when "0010" => leds <= "00000100";
								when "0011" => leds <= "00001000";
								when "0100" => leds <= "00010000";
								when "0101" => leds <= "00100000";
								when "0110" => leds <= "01000000";
								when "0111" => leds <= "10000000";
								when "1000" => leds <= "01000000";
								when "1001" => leds <= "00100000";
								when "1010" => leds <= "00010000";
								when "1011" => leds <= "00001000";
								when "1100" => leds <= "00000100";
								when "1101" => leds <= "00000010";
								when others => leds <= "00000000";
						end case;
				else
						if position(0) = '0' then
								leds <= switches;
						else
								leds <= not switches;
						end if;
				end if;
		end process;
		
		-- Mode controller
		process(clk, reset)
		begin
				if reset = '1' then
						mode <= '1';
				elsif clk'event and clk = '1' then
						if change_mode = '1' then
								mode <= not mode;
								
						elsif write = '1' and address = '1' then
								mode <= writedata(0); -- ge??ndert von 1 auf 0
								
						end if;
				end if;
		end process;
		
		-- running controller
		process(clk, reset)
		begin
				if reset = '1' then
						running <= '1';
						
				elsif clk'event and clk = '1' then
						if pause = '1' then
								running <= not running;
								
						elsif write = '1' and address = '1' then
								running <= writedata(1);							
						end if;
				end if;
		end process;
		
		-- shift generation
		process(clk, reset)
		begin
				if (reset = '1') then
						count <= X"000000";
						position <= "0000";
						
				elsif clk'event and clk = '1' then
						if (running = '1') then
								if (count = 0) then
										count <= delay&X"00000";
										
										if (position = 13) then
												position <= "0000";												
										else
												position <= position + 1;												
										end if;
								else
										count <= count - 1;										
								end if;
						end if;
				end if;
		end process;
		
		delay <= 16 - speed;
		
		-- Avalon read control ---------------------------
		
		readdata <= speed&'0'&ena_irq&mode&running	when read = '1' and address = '1' else
						leds										when read = '1' and address = '0' else
						(others => '0');
						
		-- Avalon interrupt control ----------------------
		
		-- enable control
		process(clk, reset)
		begin
				if reset = '1' then
						ena_irq <= '0';
						
				elsif clk'event and clk = '1' then
						if write = '1' and address = '1' then
								ena_irq <= writedata(2);
						end if;
				end if;
		end process;
		
		-- interrupt generation control
		process(clk, reset)
		begin
				if reset = '1' then
						irq <= '0';
						
				elsif clk'event and clk = '1' then
						if (	(sync_buttons(0) = '1') or (sync_buttons(1) = '1') or
								(sync_buttons(2) = '1') or (sync_buttons(3) = '1'))
								and ena_irq = '1' then
									irq <= '1';
									
						elsif read = '1' and address = '1' then
									irq <= '0';
						end if;
				end if;
		end process;
		
end architecture;