To compile new_server:
	new_server.c is on eros.cs.txstate.edu
	open a window on eros.
	compile and run the server program first:
	to compile: gcc -O3 -g -std=c99 new_server.c -o new_server
	to run: ./new_server

To compile new_client:
	client_tcp.c is on zeus.cs.txstate.edu
	open a window on zeus.
	to compile: gcc -O3 -g -std=c99 new_client.c -o new_client
	to run: ./new_client