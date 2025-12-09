music-dl: main.c curl_read.c dep.c play.c
	@gcc -O3 -o music-dl main.c dep.c curl_read.c play.c -l curl -l pthread -l mpv -Wall -Werror
msc_dl: mscdl.py
	nuitka mscdl.py --remove-output --standalone --onefile
