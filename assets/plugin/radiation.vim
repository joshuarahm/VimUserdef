" Initialize the library
if ! exists('g:radiation_initialized')  || g:radiation_initialized==0
	call radiation#Radiation_Init()
	exe 'command! Radiate call radiation#Radiate()'
endif

if ! exists('g:radiation_noisy')
	" tone down the warnings
	let g:radiation_noisy = 0
endif
