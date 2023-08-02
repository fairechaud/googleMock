#include "LibraryCode.hpp"

#include <vector>
#include <stdexcept>

std::vector<int> generateNumbers(int numOfElements, int divisor)
{
  std::vector<int> moduloResults;
  if(divisor <= 0)
  {
    throw std::runtime_error("Divide by zero error. Divisor must be >= 0");
  }
  for(int i=0; i<numOfElements; i++)
  {
    moduloResults.push_back(i % divisor);
  }
  return moduloResults;
}
 