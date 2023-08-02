#include <iostream>
#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <map>

#include "IDatabaseConnection.hpp"
#include "Employee.hpp"
#include "EmployeeManager.hpp"

using namespace testing;

/*
    Mocks can be used to isolate the test.

    Mocked methods have empty implementations.

    They can be used to control the behaviour of certain methods, like: returning a certain result, calling another method, throwing exceptions.

    Mocks can be used for collaboration tests. That means you can test that method A called method B, with what parameters, and so on

    Matchers can be used for matching parameters. Special matchers are "_" (anything) or the exact value ("Exactly").

    Other matchers usually found: Gt( greater than), Ge(greater or equal), Lt(lower than), Le(lower or equal).

    There are special matchers for strings.

    Matchers can be used in assertions on vectors.
*/

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

ACTION(myThrow)
{
    std::cout << "This is error action\n";
    throw std::runtime_error("Dummy error");
}

TEST(TestEmployeeManager, TestConnectionError)
{
    MockDatabaseConnection dbConnection("DummyAddress");
    EXPECT_CALL(dbConnection, connect()).WillOnce(Throw(std::runtime_error("Dummy error")));
    //EXPECT_CALL(dbConnection, disconnect());

    ASSERT_THROW(EmployeeManager employeeManager(&dbConnection), std::runtime_error);
}

TEST(TestEmployeeManager, TestConnectionErrorWithAction)
{
    MockDatabaseConnection dbConnection("DummyAddress");
    EXPECT_CALL(dbConnection, connect()).WillOnce(myThrow());

    ASSERT_THROW(EmployeeManager employeeManager(&dbConnection), std::runtime_error);
}

TEST(TestEmployeeManager, TestConnectionErrorLambdaInvoke)
{
    MockDatabaseConnection dbConnection("DummyAddress");
    EXPECT_CALL(dbConnection, connect()).WillOnce(Invoke(
        [](){
            std::cout << "This is lambda error invoke\n";
            throw std::runtime_error("Dummy error");
        }
    ));

    ASSERT_THROW(EmployeeManager employeeManager(&dbConnection), std::runtime_error);
}

TEST(TestEmployeeManager, TestConnectionErrorMemberFunctionInvoke)
{
    MockDatabaseConnection dbConnection("DummyAddress");
    auto boundMethod = std::bind(&MockDatabaseConnection::memberMethodForConnectionError, &dbConnection);
    EXPECT_CALL(dbConnection, connect()).WillOnce(InvokeWithoutArgs(
        boundMethod
    ));

    ASSERT_THROW(EmployeeManager employeeManager(&dbConnection), std::runtime_error);
}


TEST(TestEmployeeManager, TestConnection)
{
    MockDatabaseConnection dbConnection("dummyConnection");
    EXPECT_CALL(dbConnection, connect());
    EXPECT_CALL(dbConnection, disconnect());

    EmployeeManager employeeManager(&dbConnection);
}

TEST(TestEmployeeManager, TestUpdateSalary)
{
    MockDatabaseConnection dbConnection("dummyConnection");
    EXPECT_CALL(dbConnection, connect());
    EXPECT_CALL(dbConnection, disconnect());
    EXPECT_CALL(dbConnection, updateSalary(_,_)).Times(1);   

    EmployeeManager employeeManager(&dbConnection);

    employeeManager.setSalary(50, 6000);
}

TEST(TestEmployeeManager, TestGetSalary)
{
    const int employeeId = 50;
    const float salary = 6100.0;
    MockDatabaseConnection dbConnection("dummyConnection");
    EXPECT_CALL(dbConnection, connect());
    EXPECT_CALL(dbConnection, disconnect());
    EXPECT_CALL(dbConnection, getSalary(_)).Times(1).WillOnce(Return(salary));

    EmployeeManager employeeManager(&dbConnection);

    float returnedSalary = employeeManager.getSalary(employeeId);

    ASSERT_EQ(salary, returnedSalary);
}

TEST(TestEmployeeManager, TestGetSalaryInRange)
{
    const int low = 5000, high = 8000;
    std::vector<Employee> returnedVector{Employee{1, 5600, "John"},
                                    Employee{2, 7000, "Jane"},
                                    Employee{3, 6600, "Alex"}};

    MockDatabaseConnection dbConnection("dummyConnection");
    EXPECT_CALL(dbConnection, connect());
    EXPECT_CALL(dbConnection, disconnect());
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
