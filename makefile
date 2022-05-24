all : base_opengl 

tp4 : ex1_1 ex1_2 ex2_1 ex2_2 ex3 clear

clear : 
	rm *.o


kohonen : kohonen.c
	gcc kohonen.c -lm -o kohonen

base_opengl: base_opengl.o ppm.o 
	gcc -Wall ppm.o base_opengl.o -o base_opengl -lGL -lGLU -lglut -lm 




ex1_1: ex1_1.o ppm.o 
	gcc -Wall ppm.o ex1_1.o -o ex1_1 -lGL -lGLU -lglut -lm 

ex1_1.o : ex1_1.c base_opengl.h
	gcc -Wall -c ex1_1.c -o ex1_1.o



ex1_2: ex1_2.o ppm.o 
	gcc -Wall ppm.o ex1_2.o -o ex1_2 -lGL -lGLU -lglut -lm 

ex1_2.o : ex1_2.c base_opengl.h
	gcc -Wall -c ex1_2.c -o ex1_2.o



ex2_1: ex2_1.o ppm.o 
	gcc -Wall ppm.o ex2_1.o -o ex_2_1 -lGL -lGLU -lglut -lm 

ex2_1.o : ex2_1.c base_opengl.h
	gcc -Wall -c ex2_1.c -o ex2_1.o 


ex2_2: ex2_2.o ppm.o 
	gcc -Wall ppm.o ex2_2.o -o ex2_2 -lGL -lGLU -lglut -lm 

ex2_2.o : ex2_2.c base_opengl.h
	gcc -Wall -c ex2_2.c -o ex2_2.o

ex3: ex3.o ppm.o 
	gcc -Wall ppm.o ex3.o -o ex3 -lGL -lGLU -lglut -lm 

ex3.o : ex3.c base_opengl.h
	gcc -Wall -c ex3.c -o ex3.o

ppm.o : ppm.c ppm.h
	gcc -Wall -c ppm.c -o ppm.o

base_opengl.o : base_opengl.c base_opengl.h
	gcc -Wall -c base_opengl.c -o base_opengl.o



base_opengl.h :
	touch base_opengl.h

ppm.h : 
	touch ppm.h

clean :
	rm -f base_opengl *.o

