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
CLEAN=rm -f *.out *.o *.a
	
# Compile
build:
	g++ vector.h vector.cpp datastructure.h datastructure.cpp fileparser.h fileparser.cpp stb_image_write.h stb_image.h pixel.h pixel.cpp image.h image.cpp $(EXE) #$(GLAD) $(LIBS)

# Run
run:
	./a.out

# Clean
clean:
	$(CLEAN)