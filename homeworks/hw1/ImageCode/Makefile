######################################################################################################
# CSCI 5607: Fundamentals of Computer Graphics
# Adil Sadik
# sadik.adil@gmail.com
# Unite Student
# Fall 2017
# Homework 0
######################################################################################################

EXE=main.cpp
#GLAD=glad/glad.c

# Main target
all: build #run
# Lib framework 
#LIBS=-I/Library/Frameworks/SDL2.framework/Headers -F/Library/Frameworks -framework SDL2
# Clean
CLEAN=rm -f *.out *.o *.a
	
# Compile
build:
	g++ stb_image.h stb_image_write.h pixel.h pixel.cpp image.h image.cpp main.cpp 

# Run
run:
	./a.out

# Clean
clean:
	$(CLEAN)