#pragma once
//#define BOOST_ENABLE_ASSERT_HANDLER

/*****************************************************************************/
/**

Auxillary Array operations for OpenModelica.


\date     September, 1st, 2010
\author

*/
/*****************************************************************************
Copyright (c) 2010, OSMC
*****************************************************************************/


/**
Assertion function
*/
//void boost::assertion_failed(char const * expr, char const * function,
//                             char const * file, long line);


/*index type for multi array, first shape, second indeces*/
typedef std::vector<std::vector<size_t> > idx_type;
typedef std::pair<vector<size_t>,idx_type >  spec_type;


size_t getNextIndex(vector<size_t> idx,size_t k);

/**
Operation class which performs the array operation *,/
*/
template< typename T, class F >
struct Operation
{
  Operation( F op ): _op(op) {}
  T &operator()( T &x, const T &y ) const
  {
      x = _op( y );
    return x;
  }
  F _op;
};

/**
Operation class which performs the array operation +,-
*/
template< typename T, class F >
struct Operation2
{
  Operation2( F op ): _op(op) {}
  T &operator()( T &x, const T &y, const T &z ) const
  { x = _op( y, z ); return x; }
  F _op;
};


/**
Helper function for multiply_array,divide_array copies array a used as return value and performs operation
*/
template < typename T, size_t NumDims, class F >
boost::multi_array< T, NumDims > op_cp_array( boost::multi_array_ref< T, NumDims > a, F f )
{
  boost::multi_array< T, NumDims > retVal(a);
  Operation< T, F > opis( f );
  return array_operation( retVal, a, opis );
}
/**
Helper function for subtract_array,add_array, copies array a used as return value and performs operation
*/
template<
  typename T,size_t NumDims, class F
 >
boost::multi_array< T, NumDims > op_cp_array( boost::multi_array_ref< T, NumDims > a, boost::multi_array_ref< T, NumDims > b, F f )
{
  boost::multi_array< T, NumDims > retVal( a );
  return array_operation( retVal, a, b, Operation2< T, F >( f ) );
}

/**
Multiplies an array with a scalar value (a type as template parameter)
*/
template < typename T, size_t NumDims >
boost::multi_array< T, NumDims > multiply_array( boost::multi_array_ref< T, NumDims > a,  const T &b )
{
  return  op_cp_array<T>( a, std::bind2nd( std::multiplies< T >(), b ) );
};


/**
Divides an array with a scalar value (a type as template parameter)
*/
template < typename T, size_t NumDims >
boost::multi_array< T, NumDims > divide_array( boost::multi_array_ref< T, NumDims > a,  const T &b )
{
    return  op_cp_array<T>( a, std::bind2nd( std::divides< T >(), b ) );
};

/**
Subtracts two arrays (a,b type as template parameter)
*/
template < typename T, size_t dims >
boost::multi_array< T, dims > subtract_array( boost::multi_array_ref< T, dims > a ,  boost::multi_array_ref< T, dims > b  )
{
    return op_cp_array< T >( a, b, std::minus< T >() );
};

/**
Adds two arrays (a,b type as template parameter)
*/
template < typename T, size_t dims >
boost::multi_array< T, dims > add_array( boost::multi_array_ref< T, dims > a ,  boost::multi_array_ref< T, dims > b  )
{
  return op_cp_array< T >( a, b, std::plus< T >() );
};

/**
scalar product of two arrays (a,b type as template parameter)
*/
template < typename T >
T dot_array( boost::multi_array_ref< T, 1 > a ,  boost::multi_array_ref< T, 1 > b  )
{
  T tmp = 0;
  typename boost::multi_array< T, 1 >::iterator j = b.begin();
  for ( typename boost::multi_array< T, 1 >::iterator i = a.begin();  i != a.end(); i++, j++ )
    tmp += (*i) * (*j);

  return tmp;
};

/**
cross product of two arrays (a,b type as template parameter)
*/
template < typename T >
boost::multi_array< T, 1 > cross_array( boost::multi_array_ref< T, 1 > a ,  boost::multi_array_ref< T, 1 > b  )
{
  typename boost::multi_array<T, 1> res(boost::extents[3]);
  res.reindex(1);
  res[1] = (a[2] * b[3]) - (a[3] * b[2]);
  res[2] = (a[3] * b[1]) - (a[1] * b[3]);
  res[3] = (a[1] * b[2]) - (a[2] * b[1]);
  return res;
};

/**
- array (a)
*/
template < typename T, size_t dims >
void usub_array(boost::multi_array_ref< T, dims > &a)
{
  for ( typename boost::multi_array< T, dims >::iterator i = a.begin();  i != a.end(); i++)
    (*i) = -(*i);
}

/**
Applies array operation F (*,/) on array
*/

template< typename T, size_t dims, class F >
boost::multi_array< T, dims >  array_operation( boost::multi_array_ref< T, dims > a,  boost::multi_array_ref< T, dims > b, F& op )
{
  typename boost::multi_array_ref< T, dims >::iterator j = b.begin();
  for ( typename boost::multi_array< T, dims >::iterator i = a.begin();
        i != a.end(); i++, j++ )
        array_operation( *i, *j, op );
  return a;
}


/**
Applies array operation F  (*,/) on one dimensional array
*/
template<
  typename T, class F
>

boost::multi_array< T, 1 > array_operation( boost::multi_array_ref< T, 1 > a, boost::multi_array_ref< T, 1 > b, F& op )
{
  typename boost::multi_array_ref< T, 1 >::iterator j = b.begin();
  for ( typename boost::multi_array< T, 1 >::iterator i = a.begin();
        i != a.end(); i++, j++ )
    op( *i, *j );
  return a;
}

/**
Applies array operation F  (*,/) on  sub array a[i]
*/
template<typename T, size_t NumDims, class F >

boost::detail::multi_array::sub_array< T, NumDims > array_operation( boost::detail::multi_array::sub_array< T, NumDims > a,  boost::detail::multi_array::sub_array< T, NumDims > b, F op )
{
  typename boost::detail::multi_array::sub_array< T, NumDims >::iterator j = b.begin();
  for ( typename boost::detail::multi_array::sub_array< T, NumDims >::iterator i = a.begin();
        i != a.end(); i++, j++ )
    array_operation( *i, *j, op );
  return a;
}

/**
Applies array operation F  (*,/) on one dimensial sub array a[i]
*/
template< typename T, class F >
boost::detail::multi_array::sub_array< T, 1 > array_operation( boost::detail::multi_array::sub_array< T, 1 > a, boost::detail::multi_array::sub_array< T, 1 > b,  F op )
{
  typename boost::detail::multi_array::sub_array< T, 1 >::iterator j = b.begin();
  for ( typename boost::detail::multi_array::sub_array< T, 1 >::iterator i = a.begin();
        i != a.end(); i++, j++ )
    op( *i, *j );
  return a;
}


/**
Applies array operation F (+,-) on  on dimensional  subarray
*/

template<
  typename T, class F >
boost::detail::multi_array::sub_array< T, 1 > array_operation( boost::detail::multi_array::sub_array< T, 1 > a,  boost::detail::multi_array::sub_array< T, 1 > b, boost::detail::multi_array::sub_array< T, 1 > c, F& op )
{
  typename boost::detail::multi_array::sub_array< T, 1 >::iterator j = b.begin();
  typename boost::detail::multi_array::sub_array< T, 1 >::iterator k = c.begin();
  for ( typename boost::detail::multi_array::sub_array< T, 1 >::iterator i = a.begin();
        i != a.end(); i++, j++, k++ )
   op( *i, *j, *k );
  return a;
}


/**
Applies array operation F (+,-) on array
*/

template<
  typename T, class F >
boost::multi_array< T, 1 > array_operation( boost::multi_array< T, 1 > &a,  boost::multi_array_ref< T, 1 > b,  boost::multi_array_ref< T, 1 > c, F op )
{
 typename boost::multi_array_ref< T, 1 >::iterator j = b.begin();
 typename boost::multi_array_ref< T, 1 >::iterator k = c.begin();
  for ( typename boost::multi_array< T, 1 >::iterator i = a.begin();
        i != a.end(); i++, j++, k++ )
    op( *i, *j, *k );
  return a;
}

/**
Applies array operation F (+,-) on subarray
*/

template<
  typename T, size_t dims, class F >
boost::detail::multi_array::sub_array< T, dims > array_operation( boost::detail::multi_array::sub_array< T, dims > a, boost::detail::multi_array::sub_array< T, dims > b, boost::detail::multi_array::sub_array< T, dims > c,  F op )
{
  typename boost::detail::multi_array::sub_array< T, dims >::iterator j = b.begin();
  typename boost::detail::multi_array::sub_array< T, dims >::iterator k = c.begin();
  for ( typename boost::detail::multi_array::sub_array< T, dims >::iterator i = a.begin();
        i != a.end(); i++, j++, k++ )    array_operation( *i, *j, *k, op );
  return a;
}
/**
Applies array operation F (+,-) on array
*/

template<
  typename T, size_t dims, class F >
boost::multi_array< T, dims > array_operation( boost::multi_array_ref< T, dims > a,  boost::multi_array_ref< T, dims > b,  boost::multi_array_ref< T, dims > c, F op )
{
  typename boost::multi_array_ref< T, dims >::iterator j = b.begin();
  typename boost::multi_array_ref< T, dims >::iterator k = c.begin();
  for (typename boost::multi_array< T, dims >::iterator i = a.begin();
        i != a.end(); i++, j++, k++ )
    array_operation( *i, *j, *k, op );
  return a;
}



/**
Copies a array
*/
template<
  typename T, size_t dims,
  template< typename, size_t > class MultiArray
>
boost::multi_array< T, dims > empty_clone
( MultiArray< T, dims > x )
{
  boost::array< size_t, dims > shape;
  std::copy( x.shape(), x.shape() + dims, shape.begin() );
  boost::multi_array< T, dims > retVal( shape );
  return retVal;
}


/**
fills a array with an value val
*/
template < typename T, size_t NumDims >
void
 fill_array (boost::multi_array_ref< T, NumDims > x, T val )
{

 // std::fill( x.shape(), x.shape() + NumDims, val );
   std::fill( x.data(), x.data() + x.num_elements(), val);
}

/**
sum all a array elements
*/
template < typename T, size_t NumDims >
T sum_array (boost::multi_array_ref< T, NumDims > x )
{

   T val;
   val = std::accumulate( x.begin(), x.end(),0 );
   return val;
}

template < typename T, size_t NumDims >
void assign_array(boost::multi_array<T, NumDims> &A,boost::multi_array_ref<T, NumDims> B) {
    std::vector<size_t> ex;
    const size_t* shape = B.shape();
    ex.assign( shape, shape+B.num_dimensions() );
    A.resize( ex );
    A.reindex(1);
    A = B;
}


/**
Concatenates n real arrays along the k:th dimension.
*/
template < typename T, size_t NumDims >
void cat_array (int k,boost::multi_array< T, NumDims >& a, std::vector<boost::multi_array< T, NumDims > > x )
{
    unsigned int new_k_dim_size = 0;
    unsigned int n = x.size();
    /* check dim sizes of all inputs */
    if(n<1)
      throw std::invalid_argument("No input arrays");

    if(x[0].num_dimensions() < k)
     throw std::invalid_argument("Wrong dimension for input array");

    new_k_dim_size = x[0].shape()[k-1];
    for(int i = 1; i < n; i++)
    {
        if(x[0].num_dimensions() != x[i].num_dimensions())
           throw std::invalid_argument("Wrong dimension for input array");
        for(int j = 0; j < (k - 1); j++)
        {
            if (x[0].shape()[j] != x[i].shape()[j])
                throw std::invalid_argument("Wrong size for input array");
        }
        new_k_dim_size += x[i].shape()[k-1];
        for(int j = k; j < x[0].num_dimensions(); j++)
        {
          if (x[0].shape()[j] != x[i].shape()[j])
            throw std::invalid_argument("Wrong size for input array");
        }
    }
    /* calculate size of sub and super structure in 1-dim data representation */
    unsigned int n_sub = 1;
    unsigned int n_super = 1;
    for (int i = 0; i < (k - 1); i++)
    {
        n_super *= x[0].shape()[i];
    }
    for (int i = k; i < x[0].num_dimensions(); i++)
    {
        n_sub *= x[0].shape()[i];
    }
    /* allocate output array */
    const size_t* shape = x[0].shape();
    vector<size_t> ex;
    ex.assign( shape, shape+x[0].num_dimensions() );
    ex[k-1] = new_k_dim_size;
    if(ex.size()<k)
    throw std::invalid_argument("Error resizing concatenate array");
  a.resize( ex );

  /* concatenation along k-th dimension */
    T* a_data = a.data();
    int j = 0;
    for(int i = 0; i < n_super; i++)
  {
        for(int c = 0; c < n; c++)
    {
            int n_sub_k = n_sub * x[c].shape()[k-1];
            T* x_data = x[c].data();
      for(int r = 0; r < n_sub_k; r++)
      {
                a_data[j] =       x_data[r + (i * n_sub_k)];
                j++;
            }
        }
    }
    a.reindex(1);

}


template < typename T >
void transpose_array (boost::multi_array< T, 2 >& a, boost::multi_array< T, 2 >  x )

{

    const size_t* shape = x.shape();
    vector<size_t> ex;
    ex.assign( shape, shape+x.num_dimensions() );
    std::swap( ex[0], ex[1] );
    a.resize(ex);
    a.reindex(1);
     T* data = x.data();
    a.assign( data, data + a.num_elements() );

}
/*
creates an array (d) for passed multi array  shape (sp) and initialized it with elements from passed source array (s)
s source array
d destination array
sp (shape,indices) of source array
*/
template < typename T , size_t NumDims, size_t NumDims2>
void create_array_from_shape(const spec_type& sp,boost::multi_array< T, NumDims >& s,boost::multi_array< T, NumDims2 >& d)
{
     //alocate target array
   vector<size_t> shape;
     vector<size_t>::const_iterator iter;
     for(iter = (sp.first).begin();iter!=(sp.first).end();++iter)
     {
          if(*iter!=0)
               shape.push_back(*iter);

     }
     d.resize(shape);
     d.reindex(1);
     //Check if the dimension of passed indices match the dimension of target array
   if(sp.second.size()!=s.num_dimensions())
     throw std::invalid_argument("Erro in create array from shape, number of dimensions does not match");

   T* data = new T[d.num_elements()];

   idx_type::const_iterator spec_iter;
   //calc number of indeces
   size_t n =1;
   for(spec_iter = sp.second.begin();spec_iter!=sp.second.end();++spec_iter)
     {

        n*=spec_iter->size();
   }
   size_t k =0;
     size_t index=0;
   vector<size_t>::const_iterator indeces_iter;

   //initialize target array with elements of source array using passed indices
   vector<size_t> idx;
   for(int i=0;i<n;i++)
   {
    spec_iter = sp.second.begin();
        for(int dim=0;dim<s.num_dimensions();dim++)
    {
      size_t idx1 = getNextIndex(*spec_iter,i);
      idx.push_back(idx1);
      spec_iter++;
    }
    if(index>(d.num_elements()-1))
    {
      throw std::invalid_argument("Erro in create array from shape, number of dimensions does not match");
    }
    data[index] = s(idx);
    idx.clear();
    index++;
   }
   //assign elemets to target array
   d.assign( data, data + d.num_elements() );
     delete [] data;
}





 template < typename T , size_t NumDims, size_t NumDims2 >
void promote_array(unsigned int n,boost::multi_array< T, NumDims >& s,boost::multi_array< T, NumDims2 >& d)
{
   const size_t* shape = s.shape();
   vector<size_t> ex;
   ex.assign( shape, shape+ s.num_dimensions() );
   for(int i=0;i<n;i++)
    ex.push_back(1);
   d.resize(ex);
   T* data = s.data();
   d.assign( data, data + s.num_elements() );
}

/**
finds min/max elements of an array */
template < typename T, size_t NumDims >
std::pair <T,T>
min_max (boost::multi_array_ref< int, 1 > x)
{

  boost::minmax_element(x.data(), x.data() + x.num_elements());

}






template <class LAYOUT, class T>
ublas::matrix<const T, LAYOUT, ublas::shallow_array_adaptor<T> >
toMatrix(const size_t size1, const size_t size2,  T * data)
{
    typedef ublas::shallow_array_adaptor<T> a_t;
    typedef ublas::matrix<const T, LAYOUT, a_t>      m_t;
    return m_t(size1, size2, a_t(size1*size2, data));
}
// default layout: row_major
template <class T>
ublas::matrix<const T, ublas::row_major, ublas::shallow_array_adaptor<T> >
toMatrix(const size_t size1, const size_t size2,  T * data)
{
    return toMatrix<ublas::row_major>(size1,size2, data);
}


template <class T>
ublas::vector<T,ublas::shallow_array_adaptor<T> >
toVector(const size_t size, T * data)
{
    ublas::vector<T,ublas::shallow_array_adaptor<T> >
    v(size,ublas::shallow_array_adaptor<T>(size,data));
    return v;
}


template <typename Array>
inline void print_array(std::ostream& os, const Array& A)
{
  typename Array::const_iterator i;
  os << "[";
  for (i = A.begin(); i != A.end(); ++i) {
    print_array(os, *i);
    if (boost::next(i) != A.end())
      os << ',';
  }
  os << "]";
}
inline void print_array(std::ostream& os, const double& x)
{
  os << x;
}
