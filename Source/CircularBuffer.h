/*
  ==============================================================================

    CircularBuffer.h
    Created: 24 Nov 2018 11:09:13am
    Author:  N

  ==============================================================================
*/

#pragma once

#include <iostream>

typedef struct IndexPos
{
	int x;
	int y;
	int z;
};


class CircularBuffer
{
private:
	IndexPos * data;
	unsigned int read_index;
	unsigned int write_index;
	unsigned int nb_elmt;

public:
	CircularBuffer();
	CircularBuffer(unsigned int nb_elmts = 256);
	~CircularBuffer();
	void write(const IndexPos & );
	IndexPos & read();

	void increaseCounter(int counter);

};