library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_unsigned.all;

entity displays_test is
port( clk: 			in 		std_logic;
		nRst: 		in			std_logic;
		disp_ena_n:	in 		std_logic_vector(5 downto 0);
		disp0:		buffer	std_logic_vector(6 downto 0);
		disp1:		buffer	std_logic_vector(6 downto 0);
		disp2:		buffer	std_logic_vector(6 downto 0);
		disp3:		buffer	std_logic_vector(6 downto 0);
		disp4:		buffer	std_logic_vector(6 downto 0);
		disp5:		buffer	std_logic_vector(6 downto 0)
		);
end entity;

architecture rtl of displays_test is
		signal counter: 		std_logic_vector(22 downto 0);
		signal segments:		std_logic_vector(6 downto 0);
		signal num_disp: 		std_logic_vector(2 downto 0);
		signal tic:				std_logic;
		constant tenth_sec:	natural := 4999999;	
begin
		process(clk, nRst)
		begin
			if nRst = '0' then 
				counter <= (others => '0');
			elsif clk'event and clk = '1' then
				if tic = '1' then 
					counter <= (others => '0');
				else 
					counter <= counter + 1;
				end if;
			end if;
		end process;
		
		tic <= '1' when counter = tenth_sec else '0';
		
		process(clk, nrst)
		begin
			if nRst = '0' then 
				segments <= "1111110";
			elsif clk'event and clk = '1' then 
				if tic = '1' then
					if segments = 0 then
						segments <= "1111110";
					else
						segments <= segments(5 downto 0)&'0';
					end if;
				end if;
			end if;
		end process;
		
		process(clk, nRst)
		begin
			if nRst = '0' then
				num_disp <= (others => '0');
			elsif clk'event and clk = '1' then
				if segments = 0 and tic = '1' then 
					if num_disp = 5 then
						num_disp <= (others => '0');
					else
						num_disp <= num_disp + 1;
					end if;
				end if;
			end if;
		end process;
		
		disp0 <= segments when disp_ena_n(0) = '0' and num_disp = 0 else (others => '0')
								when disp_ena_n(0) = '0' and num_disp > 0 else (others => '1');
								
		disp1 <= segments when disp_ena_n(1) = '0' and num_disp = 1 else (others => '0')
								when disp_ena_n(1) = '0' and num_disp > 1 else (others => '1');
								
		disp2 <= segments when disp_ena_n(2) = '0' and num_disp = 2 else (others => '0')
								when disp_ena_n(2) = '0' and num_disp > 2 else (others => '1');
								
		disp3 <= segments when disp_ena_n(3) = '0' and num_disp = 3 else (others => '0')
								when disp_ena_n(3) = '0' and num_disp > 3 else (others => '1');
	
		disp4 <= segments when disp_ena_n(4) = '0' and num_disp = 4 else (others => '0')
								when disp_ena_n(4) = '0' and num_disp > 4 else (others => '1');
							
		disp5 <= segments when disp_ena_n(5) = '0' and num_disp = 5 else (others => '0')
								when disp_ena_n(5) = '0' and num_disp > 5 else (others => '1');
								
end architecture;
		