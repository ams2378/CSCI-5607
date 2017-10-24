######################################################################################################
# CSCI 5607: Fundamentals of Computer Graphics
# Adil Sadik
# sadik.adil@gmail.com
# Unite Student
# Fall 2017
# Homework 0
######################################################################################################

EXE=raytracer.cpp
GLAD=glad/glad.c

#CPPFLAGS = -DLIGHT_OFF

# Main target
all: build run
# Lib framework 
LIBS=-I/Library/Frameworks/SDL2.framework/Headers -F/Library/Frameworks -framework SDL2
# Clean
CLEAN=rm -f *.out *.o *.a *.gch*
	
# Compile
build:
	g++ vec3.h material.h material.cpp object.h sphere.h sphere.cpp triangle.h triangle.cpp light.h light.cpp datastructure.h fileparser.h fileparser.cpp stb_image_write.h stb_image.h pixel.h pixel.cpp image.h image.cpp $(EXE) 

# Run
run:
	./a.out

# Clean
clean:
	$(CLEAN)