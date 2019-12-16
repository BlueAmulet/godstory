//-----------------------------------------------------------------------------
// PowerEngine
// Copyright (C) FairRain.com, Inc.
//-----------------------------------------------------------------------------

#ifndef _TALGORITHM_H_
#define _TALGORITHM_H_

//Includes

template <class Iterator, class Value>
Iterator find(Iterator first, Iterator last, Value value)
{
   while (first != last && *first != value)
      ++first;
   return first;
}


/// Exchanges the values of the two elements.
template <typename T> 
inline void _swap( T &left, T &right )
{
   T temp = right;
   right = left;
   left = temp;
}

#endif //_TALGORITHM_H_
