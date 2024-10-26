#include<iostream>
#include<thread>
#include<atomic>
#include<vector>
#include<future>
#include<random>
#include"threadPool.cpp"
#define RAND_MAX 10000
typedef unsigned long long ull;
template<typename T>
void multiplyVectors(std::vector<T>& a, std::vector<T>& b,T& prom) // Expected that a.size() == b.size() and "*" operator is defined for T need to add check later
{
    T ret = 0;
    for(size_t i = 0;i<a.size();i++)
    {
        ret += a.at(i) * b.at(i);
    }
    prom = ret;
}
template<typename T>
T soloThreadMultiply(std::vector<T>& a, std::vector<T>& b)
{
    T ret = 0;
    for(size_t i = 0;i<a.size();i++)
    {
        ret += a.at(i) * b.at(i);
    }
    return ret;
}
template<typename T>
class MultiThreadMatrix
{
    public:
        MultiThreadMatrix(std::vector<std::vector<T>>& matrix) : matrix_(matrix.size(),std::vector<T>(matrix.at(0).size(),T()))
        {
             // line of matrix 
            for(size_t i = 0; i < matrix.size();i++) // iteration for rows in input vector
            {
                for(size_t j = 0; j < matrix.at(i).size();j++)
                {
                    matrix_.at(i).at(j) = (matrix.at(i).at(j)); // passing of values to line
                }
            }
        }
        int getColumnsAmount()
        {
            if(matrix_.size() > 0) // check that matrix exists
            {
                return matrix_.at(0).size(); // return length of first row
            }
            return 0;
        }
        int getRowAmount()
        {
            return matrix_.size(); // return amount of rows
        }
        std::vector<T> getRowAt(int id)
        {
            return matrix_.at(id); // return line with number id 
        }
        std::vector<T> getColumnAt(int id)
        {
            int rowAmount = getRowAmount();
            std::vector<T> column(rowAmount);
            for(size_t i = 0;i < rowAmount;i++)
            {
                column.at(i) = matrix_.at(i).at(id); // iterate through rows to get columns
            }
            return column;
        }
    MultiThreadMatrix<T> operator *(MultiThreadMatrix<T>& b)
        {
            int retColumns = getRowAmount(); 
            int retRows = b.getColumnsAmount();
            if(retColumns == retRows)
            {
                std::vector<std::vector<T>> retArray(retRows,std::vector<T>(retColumns,T()));
                ThreadPool tp(std::thread::hardware_concurrency());
                for(size_t i = 0;i<retRows;i++) // iteration throw all rows
                {
                    auto r = getRowAt(i); //get row from first matrix
                    for(size_t j = 0;j<retColumns;j++) // iteration throw all colums
                    {
                        auto c = b.getColumnAt(j); // get column from second matrix
                        T& cell = retArray.at(i).at(j);
                        tp.enqueue([r,c,&cell]() mutable {multiplyVectors(&r, &c,cell);});
                    }
                }
                return MultiThreadMatrix(retArray);
            }
            
        }
        private:
            std::vector<std::vector<T>> matrix_;
};
template<typename T>
class SoloThreadMatrix
{
    public:
        SoloThreadMatrix(std::vector<std::vector<T>> matrix)
        {
            for(size_t i = 0; i < matrix.size();i++) // iteration for rows in input vector
            {
                std::vector<T> line(matrix.at(0).size()); // line of matrix 
                for(size_t j = 0; j < matrix.at(i).size();j++)
                {
                    line.at(j) = (matrix.at(i).at(j)); // passing of values to line
                }
                matrix_.push_back(line); // pushing line to matrix
            }
        }
        int getColumnsAmount()
        {
            if(matrix_.size() > 0) // check that matrix exists
            {
                return matrix_.at(0).size(); // return length of first row
            }
            return 0;
        }
        int getRowAmount()
        {
            return matrix_.size(); // return amount of rows
        }
        std::vector<T> getRowAt(int id)
        {
            return matrix_.at(id); // return line with number id 
        }
        std::vector<T> getColumnAt(int id)
        {
            int rowAmount = getRowAmount();
            std::vector<T> column(rowAmount);
            for(size_t i = 0;i < rowAmount;i++)
            {
                column.at(i) = matrix_.at(i).at(id); // iterate through rows to get columns
            }
            return column;
        }
    SoloThreadMatrix<T> operator *(SoloThreadMatrix<T> b)
        {
            int retColumns = getRowAmount(); 
            int retRows = b.getColumnsAmount();
            std::cout << retColumns << " " << retRows << std::endl;
            if(retColumns == retRows)
            {
                std::cout << "A" << std::endl;
                std::vector<std::vector<T>> retArray(retRows);
                std::cout << "After init" << std::endl;
                for(size_t i = 0;i<retRows;i++) // iteration throw all rows
                {
                    auto r = getRowAt(i);
                    retArray.at(i).resize(retColumns);
                    for(size_t j = 0;j<retColumns;j++)
                    {
                        auto c = b.getColumnAt(j);
                        retArray.at(i).at(j) = soloThreadMultiply(r,c);
                    }
                }
                return SoloThreadMatrix(retArray);
            }
            
        }
        private:
            std::vector<std::vector<T>> matrix_;
};
int main()
{
    int fstRowAmount = 50;
    int fstColumAmount = 20;
    int sndColumnAmount = 200;
    int sndRowAmount = fstRowAmount;
    int currentTime = std::chrono::system_clock::now().time_since_epoch().count();
    std::srand(currentTime);
    std::vector<std::vector<ull>> v;
    std::vector<std::vector<ull>> s;
    for(int i = 0;i<fstRowAmount;i++)
    {
        std::vector<ull> line(fstColumAmount);
        for(int j = 0;j<fstColumAmount;j++)
        {
            line.at(j) = std::rand();
        }
        v.push_back(line);
    } 
    for(int i = 0;i<sndColumnAmount;i++)
    {
        std::vector<ull> line(sndRowAmount);
        for(int j = 0;j<sndRowAmount;j++)
        {
            line.at(j) = std::rand();
        }
        s.push_back(line);
    } 
    SoloThreadMatrix mtrx = SoloThreadMatrix(v);
    SoloThreadMatrix mtrx2 = SoloThreadMatrix(s);
    std::chrono::time_point curTime = std::chrono::system_clock::now();
    SoloThreadMatrix res = mtrx * mtrx2;
    std::chrono::time_point soloCalcEnd = std::chrono::system_clock::now();
    std::cout << soloCalcEnd.time_since_epoch().count() - curTime.time_since_epoch().count() << std::endl;
    MultiThreadMatrix multiMtrx = MultiThreadMatrix(v);
    MultiThreadMatrix multiMtrx2 = MultiThreadMatrix(s);
    std::chrono::time_point multiCalcStart = std::chrono::system_clock::now();
    MultiThreadMatrix multiRes = multiMtrx * multiMtrx2;
    std::chrono::time_point multiCalcEnd = std::chrono::system_clock::now();
    std::cout << multiCalcEnd.time_since_epoch().count() - multiCalcStart.time_since_epoch().count() << std::endl;
}