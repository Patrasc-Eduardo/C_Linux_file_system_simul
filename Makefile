build:
	gcc simul.c -g -o simul

clean:
	rm simul

checker:
	python3 checker.py
