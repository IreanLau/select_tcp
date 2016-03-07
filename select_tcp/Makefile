.PHONY:all
all:select_client select_server

select_client:select_client.c
	gcc -o $@ $^ -g
select_server:select_server.c
	gcc -o $@ $^ -g

.PHONY:clean
clean:
	rm -rf select_client
	rm -rf select_server
