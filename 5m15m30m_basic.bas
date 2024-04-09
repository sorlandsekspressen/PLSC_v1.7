'BASIC converted from file:
'C:\Users\tho2303\OneDrive - Vestfold og Telemark fylkeskommune\Hansen electronics and education\PLSC MKR\Kode_picaxe\Picaxekode for stromstyring med 5min valg.xml
'Converted  2023-09-19 at 09:11:19

symbol i = w0
symbol sleepLoop = w1
symbol sleepTime = w2

main:
	do
  	if pinC.1 = 1 then
    	let sleepLoop = 5
  	else
    	if pinC.2 = 1 then
      	let sleepLoop = 30
    	else
      	let sleepLoop = 15
    	endif
  	endif
  	let sleepTime = 60000
  ;Skrur p? MKR
  	low C.0
  	pause 5000
  ;Vent til MKR er ferdig og setter pin C.2 til LOW
  	do
  		pause 10
  	loop until pinC.3 = 0
  ;Skrur MKR av
  	high C.0
  ;Sleep loop
  	for i = sleepLoop to 0 step -1
    	pause sleepTime
  	next i
	loop
	stop
