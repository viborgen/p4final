----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date:    09:28:03 05/07/2021 
-- Design Name: 
-- Module Name:    topmodul - Behavioral 
-- Project Name: 
-- Target Devices: 
-- Tool versions: 
-- Description: 
--
-- Dependencies: 
--
-- Revision: 
-- Revision 0.01 - File Created
-- Additional Comments: 
--
----------------------------------------------------------------------------------
library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.std_logic_unsigned.all;
use IEEE.NUMERIC_STD.ALL;

-- Uncomment the following library declaration if using
-- arithmetic functions with Signed or Unsigned values
--use IEEE.NUMERIC_STD.ALL;

-- Uncomment the following library declaration if instantiating
-- any Xilinx primitives in this code.
--library UNISIM;
--use UNISIM.VComponents.all;

entity topmodul is
    Port ( sram_data			: inout  STD_LOGIC_VECTOR (7 downto 0); 		-- Databus imellem, FSM og SRAM
			Di2					: in std_logic_vector(7 downto 0); 				-- Input Data
            led2 				: out  STD_LOGIC_VECTOR (7 downto 0); 			-- Bliver ikke brugt i denne kode
            sram_addr 		: inout  STD_LOGIC_VECTOR (12 downto 0); 		-- ADR signal til SRAM
			RWMEMLED2 		: out STD_LOGIC_VECTOR(1 downto 0); 			-- midlertidigt output der bruges til fejlfinding
            sram_Ce 			: inout  STD_LOGIC; 									-- Chip select signal til SRAM
            sram_WE 			: inout  STD_LOGIC; 									-- Write enable signal til SRAM
            sram_OE 			: inout  STD_LOGIC; 									-- output enable signal til SRAM
            clk 				: in  STD_LOGIC; 										-- klok signal
			mem2 				: in std_logic; 										-- memory operation fra arduino
			rpi					: in std_logic;								--rpi read/write signal
			vsync				: in std_logic;								--Kamera kontrol signal
			href				: in std_logic;								--Kamera kontrol signal
			pclk				: in std_logic;								--Kamera kontrol signal. Pixel clock
			TX					: out std_logic;							--Handshake signal til Pi
			RX					: IN std_logic;								--Handshake signal fra Pi
			reset				: IN std_logic;								--reset pin fra pi'en til at resette SRAM adresse
			test2				: out std_logic
			  );
end topmodul;

architecture Behavioral of topmodul is
component exram is
    Port ( sram_data			: inout  STD_LOGIC_VECTOR (7 downto 0); 		-- Databus imellem, FSM og SRAM
			Di					: in std_logic_vector(7 downto 0); 				-- Input Data
           	led 				: out  STD_LOGIC_VECTOR (7 downto 0); 			-- Bliver ikke brugt i denne kode
			RWMEMLED 			: out STD_LOGIC_VECTOR(1 downto 0); 			-- midlertidigt output der bruges til fejlfinding
           	sram_Ce 			: inout  STD_LOGIC; 									-- Chip select signal til SRAM
           	sram_WE 			: inout  STD_LOGIC; 									-- Write enable signal til SRAM
           	sram_OE 			: inout  STD_LOGIC; 									-- output enable signal til SRAM
           	clk 				: in  STD_LOGIC; 										-- klok signal
			rw 					: in std_logic;			  							-- rw signal fra arduino
			mem 				: in std_logic 										-- memory operation fra arduino
			  );
end component;

signal readWrite : std_logic;
signal cnt 	 : std_logic_vector(30 downto 0);
signal sclk	 : std_logic;

signal addr_buff_pclk: std_logic_vector(12 downto 0):="0000000000000";
signal addr_buff_rpi : std_logic_vector(12 downto 0):= "0000000000000"; 

signal test : std_logic_vector(1 downto 0);
signal flagB: std_logic:= '1';
signal flagA: std_logic:= '1';

begin
-- initialisere exram som komponent				
u1: exram port map(sram_data => sram_data, Di => Di2, led => led2, RWMEMLED => RWMEMLED2, sram_ce => sram_ce, sram_oe => sram_oe, sram_we => sram_we, clk=>clk, rw => readwrite, mem => mem2);

sclk <= cnt(13);



process (clk)
-- clk process, der driver sclk (slowclk) --
	begin 
		if (clk'event and clk = '1') then 
			cnt <= cnt + 1; 				-- til at lave slowclock. Incrementer. 
		end if;
end process;


process(clk)
-- clk process, der bestemmer om der skal skrives til SRAM chippen eller læses på baggrund af kontrol signaler fra enten kamera eller Pi -- 
begin
	if rising_edge(clk) then
		if vsync = '0' then 					--Hvis vsync går lav vides det et billede er i gang 
			if href = '1' then					--Hvis href er høj er kamera i gang med at scanne en række pixel
					readWrite <= '0';			-- SRAM chippen får signal om den skal gå i write mode
					sram_addr <= addr_buff_pclk;-- SRAM adressen bliver opdateret
			end if;
		elsif rpi = '1' then					-- rpi kontrol der bestemmer SRAM chippens skal gå i læse mode
			readwrite <= '1';					-- readWrite sender signal til komponenten SRAM den skal i læse mode
			sram_addr <= addr_buff_rpi;         -- opdatere sram adressen
			
		else
		readWrite <= '1';
		end if;
	end if;
end process;



--"0000001110111001001"

process(pclk)
-- adresse counter for write --
	begin
		if pclk'event and pclk = '1' then				-- Denne process bliver igang sat af en rising edge på kameraets PCLK
			
			if addr_buff_pclk = "1110111001001" then	-- max adresse for et billede af størrelsen 1271*6, når denne er nået resettes adressen
				addr_buff_pclk <= "0000000000000";		
			else	
				addr_buff_pclk <= addr_buff_pclk + 1;   -- adressen tæller en op for hver rising edge af PCLK
			end if;
		end if;
end process;



-- Den følge VHDL kode er handshakes mellem pi og FPGA og adresse counteren


process(clk)
	begin
			if rising_edge(clk) then	
				if  RX = '1' AND flagB = '1' then  			-- Raspberry pi'en sætter rx = '1' og siger den er klar til data
						if flagA = '1' then
						addr_buff_rpi <= addr_buff_rpi + 1;	-- incrementer adressen
						flagA <='0'; 						-- sætter flagA = '0' så den ikke tæller flere gange
						end if; 
						test <= "01";						-- Sætter FSM til at sætte TX = '1' for at svarer
				elsif RX = '0'  AND flagB = '0'then 		-- Efter pi modtager en fra TX sender den '0' til rx 
					test <= "10";							-- sætte FSM til case "10" og sætter TX = '1'
					flagA <= '1';							-- gør det muligt at går i if statementet der tæller adressen op
				end if;
				if reset = '1' then							-- resetter adressen når reset = '1'
					addr_buff_rpi <= "0000000000000";
				end if;
			end if;
end process;

process(clk) 
	begin
		if rising_edge(clk) then
			case test is
				when "01" =>
					flagB <= '0';
					TX <= '1';
				when "10" => 
					TX <= '0';
					flagB <= '1';
				when others => 
					test <= test;
			end case;
		end if; 
end process;		
	
end Behavioral;

