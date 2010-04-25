
all:
	g++ -o seqc seqcircles.cpp -I/usr/include/opencv -lcv -lhighgui

clean:
	rm -f seqc
