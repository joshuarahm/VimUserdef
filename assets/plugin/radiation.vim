" Initialize the library
if ! exists('g:radiation_initialized')  || g:radiation_initialized==0
	call radiation#Radiation_Init()
	exe 'command! Radiate call radiation#Radiate()'
endif
