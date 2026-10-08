*** Settings ***
Library   String
Library   SerialLibrary

*** Variables ***
${com}   	COM11
${baud} 	115200
${board}	nRF
${correct_time}      000105X
${incorrect_time}    006161X
${time_zero}		 000000X
${time_digit}      	 00005aX
${time_long}      	 0000591X

${correct_time_return}      65X
${time_len_error}      	 	-1X
${time_array_error}      	-2X
${time_value_error}      	-3X
${time_digit_error}      	 -5X
${time_zero_error}      	-6X


*** Test Cases ***
Connect Serial
	Log To Console  Connecting to ${board}
	Add Port  ${com}  baudrate=${baud}  encoding=ascii
	Port Should Be Open  ${com}
	Reset Input Buffer
	Reset Output Buffer


Correct Time Test
	Write Data   ${correct_time}   encoding=ascii 
	Log To Console   Send sequence ${correct_time}
	# vastaanotetaan merkkijono kunnes lopetusmerkki X (58) 
	${read} =   Read Until   terminator=58   encoding=ascii 
	# konsolille näkyviin vastaanotettu merkkijono
	Log To Console   Received ${read}
	# vertaillaan merkkijonoa
	Should Be Equal As Strings   ${read}    ${correct_time_return}
	Log To Console   Tested ${read} is same as ${correct_time_return}
	Reset Input Buffer
	Reset Output Buffer

Incorrect Time Test
	Write Data   ${incorrect_time}   encoding=ascii 
	Log To Console   Send sequence ${incorrect_time}
	# vastaanotetaan merkkijono kunnes lopetusmerkki X (58) 
	${read} =   Read Until   terminator=58   encoding=ascii 
	# konsolille näkyviin vastaanotettu merkkijono
	Log To Console   Received ${read}
	# vertaillaan merkkijonoa
	Should Be Equal As Strings   ${read}    ${time_value_error}
	Log To Console   Tested ${read} is same as ${time_value_error}
	Reset Input Buffer
	Reset Output Buffer

Time Zero Test
	Write Data   ${time_zero}   encoding=ascii 
	Log To Console   Send sequence ${time_zero}
	# vastaanotetaan merkkijono kunnes lopetusmerkki X (58) 
	${read} =   Read Until   terminator=58   encoding=ascii 
	# konsolille näkyviin vastaanotettu merkkijono
	Log To Console   Received ${read}
	# vertaillaan merkkijonoa
	Should Be Equal As Strings   ${read}    ${time_zero_error}
	Log To Console   Tested ${read} is same as ${time_zero_error}
	Reset Input Buffer
	Reset Output Buffer

Digit Test
	Write Data   ${time_digit}   encoding=ascii 
	Log To Console   Send sequence ${time_digit}
	# vastaanotetaan merkkijono kunnes lopetusmerkki X (58) 
	${read} =   Read Until   terminator=58   encoding=ascii 
	# konsolille näkyviin vastaanotettu merkkijono
	Log To Console   Received ${read}
	# vertaillaan merkkijonoa
	Should Be Equal As Strings   ${read}    ${time_digit_error}
	Log To Console   Tested ${read} is same as ${time_digit_error}
	Reset Input Buffer
	Reset Output Buffer

Time Long Test
	Write Data   ${time_long}   encoding=ascii 
	Log To Console   Send sequence ${time_long}
	# vastaanotetaan merkkijono kunnes lopetusmerkki X (58) 
	${read} =   Read Until   terminator=58   encoding=ascii 
	# konsolille näkyviin vastaanotettu merkkijono
	Log To Console   Received ${read}
	# vertaillaan merkkijonoa
	Should Be Equal As Strings   ${read}    ${time_len_error}
	Log To Console   Tested ${read} is same as ${time_len_error}
	Reset Input Buffer
	Reset Output Buffer

Disconnect Serial
	Log To Console  Disconnecting ${board}
	[TearDown]  Delete Port  ${com}


	
	
	

