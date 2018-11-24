/*
  ==============================================================================

    CircularBuffer.cpp
    Created: 24 Nov 2018 11:09:00am
    Author:  N

  ==============================================================================
*/

#include "CircularBuffer.h"

CircularBuffer::CircularBuffer() : read_index(), write_index(), nb_elmt(256)
{
	data = new IndexPos[nb_elmt];
}

CircularBuffer::CircularBuffer(unsigned int nb_elmts) : read_index(), write_index(), nb_elmt(256)
{
	data = new IndexPos[nb_elmt];
}

CircularBuffer::~CircularBuffer()
{
	delete [] data;
}


void CircularBuffer::write(const IndexPos & index)
{
	data[write_index].x = index.x;
	data[write_index].y = index.y;
	data[write_index].z = index.z;
	increaseCounter(write_index);
}

IndexPos & CircularBuffer::read()
{
	IndexPos res;
	res.x = data[read_index].x;
	res.y = data[read_index].y;
	res.z = data[read_index].z;
	increaseCounter(read_index);
	return res;
}

void CircularBuffer::increaseCounter(int counter)
{
	++counter;
	if (counter >= nb_elmt)
		counter = 0;
}