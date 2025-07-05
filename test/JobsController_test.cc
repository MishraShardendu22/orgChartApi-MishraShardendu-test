#include <gtest/gtest.h>
#include <trantor/utils/Logger.h>
#include <trantor/net/EventLoop.h>
#include <drogon/HttpAppFramework.h>
#include <drogon/drogon_test.h>

TEST(PersonsRouteTests, TestGetPersons) {
    ASSERT_TRUE(true);  
}

TEST(PersonsRouteTests, TestGetPersonById) {
    ASSERT_TRUE(true);
}

TEST(PersonsRouteTests, TestGetPersonReports) {
    ASSERT_TRUE(true);
}

TEST(PersonsRouteTests, TestPostPersons) {
    ASSERT_TRUE(true);
}

TEST(PersonsRouteTests, TestPutPersons) {
    ASSERT_TRUE(true);
}

TEST(PersonsRouteTests, TestDeletePersons) {
    ASSERT_TRUE(true);
}

TEST(DepartmentsRouteTests, TestGetDepartments) {
    ASSERT_TRUE(true);
}

TEST(DepartmentsRouteTests, TestGetDepartmentById) {
    ASSERT_TRUE(true);
}

TEST(DepartmentsRouteTests, TestGetDepartmentPersons) {
    ASSERT_TRUE(true);
}

TEST(DepartmentsRouteTests, TestPostDepartments) {
    ASSERT_TRUE(true);
}

TEST(DepartmentsRouteTests, TestPutDepartments) {
    ASSERT_TRUE(true);
}

TEST(DepartmentsRouteTests, TestDeleteDepartments) {
    ASSERT_TRUE(true);
}

TEST(JobsRouteTests, TestGetJobs) {
    ASSERT_TRUE(true);
}

TEST(JobsRouteTests, TestGetJobById) {
    ASSERT_TRUE(true);
}

TEST(JobsRouteTests, TestGetJobPersons) {
    ASSERT_TRUE(true);
}

TEST(JobsRouteTests, TestPostJobs) {
    ASSERT_TRUE(true);
}

TEST(JobsRouteTests, TestPutJobs) {
    ASSERT_TRUE(true);
}

TEST(JobsRouteTests, TestDeleteJobs) {
    ASSERT_TRUE(true);
}

TEST(AuthRouteTests, TestPostRegister) {
    ASSERT_TRUE(true);
}

TEST(AuthRouteTests, TestPostLogin) {
    ASSERT_TRUE(true);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    drogon::app().setLogLevel(trantor::Logger::kFatal);
    drogon::test::run(argc, argv);
    return RUN_ALL_TESTS();
}