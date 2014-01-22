if ! exists('g:radiation_noisy')
	" tone down the warnings
	let g:radiation_noisy = 0
endif

" Initialize the library
if ! exists('g:radiation_initialized')  || g:radiation_initialized==0
	exe 'command! Radiate call radiation#Radiate()'
	exe 'command! RadiationDigest call radiation#Digest()'
    call radiation#Radiation_Init()
endif
