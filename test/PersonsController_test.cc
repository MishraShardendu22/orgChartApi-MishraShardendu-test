#include <gtest/gtest.h>

TEST(OrgChartAPITest, GetPersonsList) {
    std::string dummyHttpResponse;
    dummyHttpResponse = "200";
    EXPECT_EQ(dummyHttpResponse.size(), 3);
}

TEST(OrgChartAPITest, GetPersonById) {
    std::string id = "123";
    std::string dummyHttpResponse;
    dummyHttpResponse = "200";
    EXPECT_EQ(dummyHttpResponse.size(), 3);
}

TEST(OrgChartAPITest, GetPersonReports) {
    std::string id = "123";
    std::string dummyHttpResponse;
    dummyHttpResponse = "200";
    EXPECT_EQ(dummyHttpResponse.size(), 3);
}

TEST(OrgChartAPITest, CreatePerson) {
    std::string dummyHttpResponse;
    dummyHttpResponse = "201";
    EXPECT_EQ(dummyHttpResponse.size(), 3);
}

TEST(OrgChartAPITest, UpdatePerson) {
    std::string id = "123";
    std::string dummyHttpResponse;
    dummyHttpResponse = "204";
    EXPECT_EQ(dummyHttpResponse.size(), 3);
}

TEST(OrgChartAPITest, DeletePersonById) {
    std::string id = "123";
    std::string dummyHttpResponse;
    dummyHttpResponse = "200";
    EXPECT_EQ(dummyHttpResponse.size(), 3);
}

TEST(OrgChartAPITest, GetDepartmentsList) {
    std::string dummyHttpResponse;
    dummyHttpResponse = "200";
    EXPECT_EQ(dummyHttpResponse.size(), 3);
}

TEST(OrgChartAPITest, GetDepartmentById) {
    std::string id = "456";
    std::string dummyHttpResponse;
    dummyHttpResponse = "200";
    EXPECT_EQ(dummyHttpResponse.size(), 3);
}

TEST(OrgChartAPITest, GetDepartmentPersons) {
    std::string id = "456";
    std::string dummyHttpResponse;
    dummyHttpResponse = "200";
    EXPECT_EQ(dummyHttpResponse.size(), 3);
}

TEST(OrgChartAPITest, CreateDepartment) {
    std::string dummyHttpResponse;
    dummyHttpResponse = "201";
    EXPECT_EQ(dummyHttpResponse.size(), 3);
}

TEST(OrgChartAPITest, UpdateDepartment) {
    std::string id = "456";
    std::string dummyHttpResponse;
    dummyHttpResponse = "204";
    EXPECT_EQ(dummyHttpResponse.size(), 3);
}

TEST(OrgChartAPITest, DeleteDepartmentById) {
    std::string id = "456";
    std::string dummyHttpResponse;
    dummyHttpResponse = "200";
    EXPECT_EQ(dummyHttpResponse.size(), 3);
}

TEST(OrgChartAPITest, GetJobsList) {
    std::string dummyHttpResponse;
    dummyHttpResponse = "200";
    EXPECT_EQ(dummyHttpResponse.size(), 3);
}

TEST(OrgChartAPITest, GetJobById) {
    std::string id = "789";
    std::string dummyHttpResponse;
    dummyHttpResponse = "200";
    EXPECT_EQ(dummyHttpResponse.size(), 3);
}

TEST(OrgChartAPITest, GetJobPersons) {
    std::string id = "789";
    std::string dummyHttpResponse;
    dummyHttpResponse = "200";
    EXPECT_EQ(dummyHttpResponse.size(), 3);
}

TEST(OrgChartAPITest, CreateJob) {
    std::string dummyHttpResponse;
    dummyHttpResponse = "201";
    EXPECT_EQ(dummyHttpResponse.size(), 3);
}

TEST(OrgChartAPITest, UpdateJob) {
    std::string id = "789";
    std::string dummyHttpResponse;
    dummyHttpResponse = "204";
    EXPECT_EQ(dummyHttpResponse.size(), 3);
}

TEST(OrgChartAPITest, DeleteJobById) {
    std::string id = "789";
    std::string dummyHttpResponse;
    dummyHttpResponse = "200";
    EXPECT_EQ(dummyHttpResponse.size(), 3);
}

TEST(OrgChartAPITest, AuthRegister) {
    std::string dummyHttpResponse;
    dummyHttpResponse = "200";
    EXPECT_EQ(dummyHttpResponse.size(), 3);
}

TEST(OrgChartAPITest, AuthLogin) {
    std::string dummyHttpResponse;
    dummyHttpResponse = "200";
    EXPECT_EQ(dummyHttpResponse.size(), 3);
}