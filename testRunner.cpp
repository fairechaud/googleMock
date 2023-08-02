#include <iostream>
#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <map>

#include "IDatabaseConnection.hpp"
#include "Employee.hpp"
#include "EmployeeManager.hpp"

using namespace testing;

class MockDatabaseConnection : public IDatabaseConnection
{
public:
    MockDatabaseConnection(std::string serverAddress);

    //MOCK_METHODn n=0,10
    MOCK_METHOD0(connect, void());
    MOCK_METHOD0(disconnect, void());

    MOCK_CONST_METHOD1(getSalary, float(int));
    MOCK_METHOD2(updateSalary, void(int, float) );

    MOCK_CONST_METHOD1(getSalariesRange, std::vector<Employee>(float));
    MOCK_CONST_METHOD2(getSalariesRange, std::vector<Employee>(float, float));

    void memberMethodForConnectionError()
    {
        std::cout << "This is member function\n";
        throw std::runtime_error("Dummy error");
    }
};

MockDatabaseConnection::MockDatabaseConnection(std::string serverAddress) : IDatabaseConnection(serverAddress)
{

}
/*
    MATCHERS are used to verify that a method was called WITH A PARTICULAR set of parameters

    Examples:
    * Exact value
    EXPECT_CALL(someObject, someMethod(5,"Hello"));
    * Argument </>
    EXPECT_CALL(someObject, someMethod(Gt(5)));
    * Other matchers : 
        Similarity : Ge,Lt,Le,Eq
        Anything : _
        IsNull/IsNotNull
        String: HasSubstr("substring")

    They can be chained together:
    EXPECT_CALL(someObject, someMethod(AllOf(Gt(5),Le(100),Not(7))));
    The evaluation above expects something >5, <= 100 and != 7 all at the same time
    AnyOf() can be used to meet at least one matchers
    Matchers can also be used with ASSERT_THAT
*/
TEST(TestEmployeeManager, TestGetSalaryInRange)
{
    const int low = 5000, high = 8000;
    // Create a dummy vector of 3 employees are input to method
    std::vector<Employee> returnedVector{Employee{1, 5600, "John"},
                                    Employee{2, 7000, "Jane"},
                                    Employee{3, 6600, "Alex"}};

    MockDatabaseConnection dbConnection("dummyConnection");
    //Standard constructor/destructor calls
    EXPECT_CALL(dbConnection, connect());
    EXPECT_CALL(dbConnection, disconnect());
    // Method called with dummy vector
    EXPECT_CALL(dbConnection, getSalariesRange(low, high)).WillOnce(Return(returnedVector));

    EmployeeManager employeeManager(&dbConnection);

    std::map<std::string, float> returnedMap = employeeManager.getSalariesBetween(low, high);

    for(auto it=returnedMap.begin(); it!=returnedMap.end(); ++it)
    {
        std::cout << it->first << " " << it->second << '\n';
        ASSERT_THAT(it->second, AnyOf(Gt(low), Lt(high-3000)));
    }
}

int main(int argc, char **argv)
{
    InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
