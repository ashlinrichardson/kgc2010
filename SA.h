#pragma once
/* template array with bounds checking inspired by Shane Ryan's SArray class	*/
#include <fstream>
#include <iostream>
#include <ostream>
#include <vector>
#include <algorithm>
#include <float.h>
#include <cmath>
#include <memory.h>
using namespace std;
using std::ostream;
using std::vector;

namespace _SA{

	template<class T>
	class SA{ 
	public:
		long int vsizei;
		long int vsizej;

		void init( long int _sizei, long int _sizej, long int _sizek){
			if(elements) free(elements);
			sizei = _sizei;
			sizej = _sizej;
			sizek = _sizek;

			long int size = sizei* ((sizej==0)?1:sizej) *  ((sizek==0)?1:sizek);
			mySize = size;
			elements=NULL;
			if(size > 0)
			{
				mySize=size;
				elements = NULL;
				elements= new T[size];
				if(!elements){
					printf("Error: SA.h: failed to allocate array.\n");
					exit(1);
				}
				memset( &(elements[0]), '\0', mySize*sizeof(T));
			}
		}

		void inline init(long int i, long int j){
			init(i,j,0);
		}

		void inline init(long int i){
			init(i,0,0);
		}

		SA(){
			elements=NULL;
			init(0,0,0);
		}

		SA(long int size){
			elements=NULL;
			init( size, 0, 0);
		}
	
		SA(long int sizei, long int sizej){
			elements=NULL;
			init( sizei, sizej, 0);
		}

		SA(long int sizei, long int sizej, long int sizek){
			elements=NULL;
			init( sizei, sizej, sizek);
		}


		~SA(){
			free(elements);
		}

		inline SA(SA<T> * other){
			if(!other){
				mySize=0;
				elements=NULL;
			}
			mySize = other->size();
			if(mySize==0)
			{
				elements=NULL;
				return;
			}
			else
			{
				elements= new T[mySize];
				for(register long int i=0; i<mySize; i++)
					elements[i]=(*other)[i];
			}
			sizei = other->sizei;
			sizej = other->sizej;
			sizek = other->sizek;
			
		}


    // return the number of elements
		inline long int size(){
			return mySize;
		}

    // return the number of elements
		inline long int length(){
			return mySize;
		}

		inline T & at(long int subscript){
			return elements[subscript];
		}

		inline T & at(long int indi, long  int indj){
			return elements[ (indi * sizej) + indj];
		}

		inline T & at(long int indi, long int indj, long int indk){
			if( sizei * sizej * sizek <1){
				printf("Error: must only use 3-d index with 3d array.\n");
				exit(1);
			}
			return elements[ (indi * sizej *sizek) + (indj*sizek) + indk];
		}

		inline void set(long int subscript, T * e){
			elements[subscript]=*e  ;
		}

	  inline T & operator[](long int subscript){
		  if(mySize == 0){
			  std::cerr << "\nError: SA.cpp: size()=0 Subscript " << subscript  << " out of range" << std::endl;
				exit( 1 );
		  }
		  if(subscript > mySize ){
			  std::cerr << "\nError: SA.cpp: Subscript " << subscript  << " out of range" << std::endl;
			  exit( 1 );
		  }
		  return elements[ subscript ]; 
	  }

	inline SA<T> & operator = (SA<T> &a ){
		if( a.size() != mySize){
			printf("SA.h: error: tried to equate arrays of different lengths.\n");
			exit(1);
		}
		register long int i=0;
		for(i=0; i<mySize; i++)
		{
			elements[i]=a[i];
		}
		return a;
	}
	
	inline T total(){
		T a = 0;
		for(long int i=0; i<mySize; i++){
			a += elements[i];
		}
		return a;
	}

	inline void sortf(){  // for float only
		float * adr =(float*) ((void*) (&elements[0]));
		sort( adr, adr+(mySize)); 
	}

	float percentBinBot(float _percent_){
		long int ind = (long int)(((float)(mySize-1))*(_percent_/100.));
		return (float)(elements[ind]);		
	}

	float percentBinTop(float _percent_){
		long int ind = (mySize-1) - (long int)(((float)(mySize-1))*(_percent_/100.));
		return (float)(elements[ind]);		
	}

	float max(){
		float m = FLT_MIN;
		for(register long int i=0; i<mySize; i++){
			if(!((isnan(elements[i])||(isinf(elements[i])))))
				if(elements[i]>m)
					m=elements[i];	
		}return m;
	}

	float max2(){
		return max();
	}

	float min(){
		float m = FLT_MAX; 
		for(register long int i=0; i<mySize; i++){
			if(!((isnan(elements[i])||isinf(elements[i]))))
				if(elements[i]<m)
					m=elements[i];	
		}return m;
	}
	
	public:
		long int mySize;
		long int sizei;
		long int sizej;
		long int sizek;
		T* elements;

	public:
		inline void clear(void){
			memset( elements, '\0', mySize*sizeof(T));
		}
	};

	template <class T>
	inline ostream &operator<<(ostream &output, SA<T> &out ){
		register long int i=0;
		for(i=0; i<(out.length()); i++){
			if(i!=(out.length()-1))
				output << out[i]<<",";
			else
				output<<out[i];
		}
		return output;

	}

	template <typename T>
	static inline void XORSwap(T&a, T&b){
		if(&a != &b){
			a^=b;
			b^=a;
			a^=b;
		}
	}

	template <typename T>
	static inline T _abs(T a){
		return (a<0)?(-a):(a);
	}

};

