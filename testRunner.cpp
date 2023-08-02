#include <iostream>
#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <map>

#include "IDatabaseConnection.hpp"
#include "Employee.hpp"
#include "EmployeeManager.hpp"

using namespace testing;

class ConnectionFixture : public testing::Test
{   
    public:
        void SetUp() override;
        void TearDown() override;
        MockDatabaseConnection dbConnection;
};

void ConnectionFixture::SetUp()
{
    MockDatabaseConnection dbConnection("DummyAddress");
    EXPECT_CALL(dbConnection, connect()).WillOnce(Throw(std::runtime_error("Dummy error")));
}

void ConnectionFixture::TearDown()
{
    //EXPECT_CALL(dbConnection, disconnect());
}
   
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
};

MockDatabaseConnection::MockDatabaseConnection(std::string serverAddress) : IDatabaseConnection(serverAddress)
{

}

ACTION(myThrow)
{
    std::cout << "This is error action\n";
    throw std::runtime_error("Dummy error");
}

TEST_F(ConnectionFixture, TestConnectionError)
{
    ASSERT_THROW(EmployeeManager employeeManager(ConnectionFixture::dbConnection), std::runtime_error);
}

TEST_F(ConnectionFixture, TestConnectionErrorWithAction)
{
    ASSERT_THROW(EmployeeManager employeeManager(ConnectionFixture::dbConnection), std::runtime_error);
}

TEST_F(ConnectionFixture,TestConnectionErrorLambdaInvoke)
{
    ASSERT_THROW(EmployeeManager employeeManager(ConnectionFixture::dbConnection), std::runtime_error);
}

TEST_F(ConnectionFixture, TestConnectionErrorMemberFunctionInvoke)
{
    ASSERT_THROW(EmployeeManager employeeManager(ConnectionFixture::dbConnection), std::runtime_error);
}

int main(int argc, char **argv)
{
    InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
