#include <cmath>
#include <vector>
#include <iostream>
#include <fstream>

enum class LatticeForm {
    square,
    kagome,
    triangular,
    circular,
    cubic,
    spherical,
};

template <class T, LatticeForm format> 
// or template <class T, LatticeForm format = LatticeForm::square>
class lattice
{
    public: 
        std::vector<T> val;
        std::vector<std::vector<double>> pos;
        int dimension; //lattice dimension
        int xsize, ysize, zsize; //size of each dimension
        //string format; //lattice format: square, kagome, triangular

        lattice(); //Constructor
        lattice(int); 
        lattice(int, int); 
        lattice(int, int, int); //explicit constructor
       
        ~lattice(); //Destructor
       
        void set_value(int x, T value);
        void set_value(int x, int y, T value);
        void set_value(int x, int y, int z, T value);
        void map(std::string title, int offset);
        std::vector<T> NN(int x, int y); //Retrieve the values of all nearest neighbours
        std::vector<int> NNindex(int x, int y); //Retrieve the indices of all nearest neighbours
        std::vector<int> NNindex(int x, int y, int z); //Retrieve the indices of all nearest neighbours       
        std::vector<T> NN(int x, int y, int z);
        std::vector<T> NNN(int x, int y); //call on all next nearest neighbours
        std::vector<T> NNN(int x, int y, int z);
        std::vector<int> NNNindex(int x, int y, int z); //Retrieve the indices of all nearest neighbours

        T& operator()(int x)
        {
            return val[x];
        } //calling for the value in the array
        
        T& operator()(int x, int y)
        {
            return val[index(x,y)];
        } //calling for the value in the array
        
        T& operator()(int x, int y, int z)
        {
            return val[index(x,y,z)];
        } //calling for the value in the array

        lattice& operator=(const lattice &input)
        {
            for (size_t i = 0; i < input.val.size(); ++i)
            {
                val[i] = input.val[i];
            }
            return *this;
        }

        template <class Type>
        inline auto index(Type x, Type y) //return vector index from input coordinates
        {
            switch (format)
            {
            case LatticeForm::square:
                return x + y * xsize;
            case LatticeForm::triangular:
                return x + y * xsize; 
            case LatticeForm::kagome: //kagome implements the same lattice structure as triangular except for, in practice in the code, skips every the other site in every the other row.
                return x + y * xsize;
            case LatticeForm::circular:
                    return int(x * std::cos(y)) + int(x * std::sin(y)) * xsize; //x as r, and y as theta
            default:  //default to square lattice
                return x + y * xsize;
            }
        }

        template <class Type>
        inline auto index(Type x, Type y, Type z) //return vector index from input coordinates
        {
            switch (format)
            {
            case LatticeForm::cubic:
                return x + y * xsize + z * xsize * ysize;
            case LatticeForm::spherical:
                return int(x * std::cos(z) * std::cos(y)) + int(x * std::cos(z) * std::sin(y)) * xsize + int(x * std::sin(z)) * xsize * ysize;
            default:
                return x + y * xsize + z * xsize * ysize; //default to cubic lattice
            }
        }
};

template<class T, LatticeForm format>
lattice<T, format>::lattice():val(0) //default constructor
{
    std::printf("Default lattice of zero dimension initialized.\n");
}

template<class T, LatticeForm format>
lattice<T, format>::lattice(int x) : lattice(x, 1, 1)
{ }

template<class T, LatticeForm format>
lattice<T, format>::lattice(int x, int y) : lattice(x, y, 1)
{ }

template<class T, LatticeForm format>
lattice<T, format>::lattice(int x, int y, int z) :
    val(x * y * z, T()), xsize(std::move(x)),
    ysize(std::move(y)), zsize(std::move(z))
{
    if (x == 0 || y == 0 || z == 0)
    {
        throw std::runtime_error("For lower dimension, please define it explicitly!");
    } //avoid using "0" as argument to lower the dimension
    std::printf("Lattice of %d x %d x %d initialized.\n", xsize, ysize, zsize);
}

template<class T, LatticeForm format>
lattice<T, format>::~lattice() = default;

template<class T, LatticeForm format>
void lattice<T, format>::set_value(int x, T value)
{
    val[x] = value;
}

template<class T, LatticeForm format>
void lattice<T, format>::set_value(int x, int y, T value)
{
    val[index(x,y)] = value;
}

template<class T, LatticeForm format>
void lattice<T, format>::set_value(int x, int y, int z, T value)
{
    val[index(x,y,z)] = value;
}

template <class T, LatticeForm format>
std::vector<int> lattice<T, format>::NNindex(int x, int y) //return the indices of the nearest neighbours of the input coordinate
{
    switch (format)
    {
    case LatticeForm::square:
        return {index(x + 1, y), index(x - 1, y), index(x, y + 1), index(x, y - 1)};
    case LatticeForm::triangular:
        return {index(x + 1, y), index(x - 1, y), index(x, y + 1), index(x, y - 1), index(x + 1, y + 1), index(x - 1, y - 1)};
    case LatticeForm::kagome: //kagome implements the same lattice structure as triangular except for, in practice in the code, skips every the other site in every the other row.
        return {index(x + 1, y), index(x - 1, y), index(x, y + 1), index(x, y - 1), index(x + 1, y + 1), index(x - 1, y - 1)};
    case LatticeForm::circular:
        return {index(x + 1, y), index(x -1, y)}; //To be defined
    default:  //default to square lattice
        return {index(x + 1, y), index(x - 1, y), index(x, y + 1), index(x, y - 1)};
    }
}

template <class T, LatticeForm format>
std::vector<T> lattice<T, format>::NN(int x, int y) ////return the values of the nearest neighbours of the input coordinate
{
    switch (format)
    {
    case LatticeForm::square:
        return {val[index(x + 1, y)], val[index(x - 1, y)], val[index(x, y + 1)], val[index(x, y - 1)]};
    case LatticeForm::triangular:
        return {val[index(x + 1, y)], val[index(x - 1, y)], val[index(x, y + 1)], val[index(x, y - 1)], val[index(x + 1, y + 1)], val[index(x - 1, y - 1)]};
    case LatticeForm::kagome: //kagome implements the same lattice structure as triangular except for, in practice in the code, skips every the other site in every the other row.
        return {val[index(x + 1, y)], val[index(x - 1, y)], val[index(x, y + 1)], val[index(x, y - 1)], val[index(x + 1, y + 1)], val[index(x - 1, y - 1)]};
    case LatticeForm::circular:
        return {val[index(x + 1, y)], val[index(x -1, y)]}; //To be defined
    default:  //default to square lattice
        return {val[index(x + 1, y)], val[index(x - 1, y)], val[index(x, y + 1)], val[index(x, y - 1)]};
    }
}

template <class T, LatticeForm format>
std::vector<T> lattice<T, format>::NN(int x, int y, int z) //return the values of the nearest neighbours of the input coordinate
{
    switch (format)
    {
    case LatticeForm::cubic:
        return {val[index(x + 1, y, z)], val[index(x - 1, y, z)], val[index(x, y + 1, z)], val[index(x, y - 1, z)], val[index(x, y, z + 1)], val[index(x, y, z - 1)]};
    default: //default to cubic lattice
        return {val[index(x + 1, y, z)], val[index(x - 1, y, z)], val[index(x, y + 1, z)], val[index(x, y - 1, z)], val[index(x, y, z + 1)], val[index(x, y, z - 1)]};
    }
}

template <class T, LatticeForm format>
std::vector<int> lattice<T, format>::NNindex(int x, int y, int z) //return the values of the nearest neighbours of the input coordinate
{
    switch (format)
    {
    case LatticeForm::cubic:
        return {index(x + 1, y, z), index(x - 1, y, z), index(x, y + 1, z), index(x, y - 1, z), index(x, y, z + 1), index(x, y, z - 1)};
    default: //default to cubic lattice
        return {index(x + 1, y, z), index(x - 1, y, z), index(x, y + 1, z), index(x, y - 1, z), index(x, y, z + 1), index(x, y, z - 1)};
    }
}

template <class T, LatticeForm format>
std::vector<T> lattice<T, format>::NNN(int x, int y) ////return the values of the nearest neighbours of the input coordinate
{
    switch (format)
    {
    case LatticeForm::square:
        return {val[index(x + 1, y + 1)], val[index(x - 1, y -1)], val[index(x - 1, y + 1)], val[index(x + 1, y - 1)]};
    case LatticeForm::triangular:
        return {val[index(x + 1, y + 2)], val[index(x - 1, y - 2)]};
    case LatticeForm::kagome:
        return {val[index(x + 1, y + 2)], val[index(x - 1, y - 2)]};
    case LatticeForm::circular:
        return {val[index(x + 1, y)], val[index(x -1, y)]}; //To be defined
    default:  //default to square lattice
        return {val[index(x + 1, y + 1)], val[index(x - 1, y - 1)], val[index(x - 1, y + 1)], val[index(x + 1, y - 1)]};
    }
}

template <class T, LatticeForm format>
std::vector<int> lattice<T, format>::NNNindex(int x, int y, int z) ////return the values of the nearest neighbours of the input coordinate
{
    switch (format)
    {
    case LatticeForm::square:
        return {index(x + 1, y + 1), index(x - 1, y -1), index(x - 1, y + 1), index(x + 1, y - 1)};
    case LatticeForm::triangular:
        return {index(x + 1, y + 2), index(x - 1, y - 2)};
    case LatticeForm::kagome:
        return {index(x + 1, y + 2), index(x - 1, y - 2)};
    case LatticeForm::circular:
        return {index(x + 1, y), index(x -1, y)}; //To be defined
    default:  //default to square lattice
        return {index(x + 1, y + 1), index(x - 1, y - 1), index(x - 1, y + 1), index(x + 1, y - 1)};
    }
}

template<class T, LatticeForm format>
void lattice<T, format>::map(const std::string title, const int offset) //map the array to data file base on the lattice format
{
		std::ofstream fout;
		fout.precision();
		fout.open(title.c_str());
		for (int j = offset; j < (ysize - offset); ++j)
		{
			for (int i = offset; i < (xsize - offset); ++i)
			{
				fout << (int)val[index(i, j)] << "\t";
			}
			fout << std::endl;
		}
        fout.close();
}
