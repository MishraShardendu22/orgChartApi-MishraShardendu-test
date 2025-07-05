#include <gtest/gtest.h>
#include <drogon/drogon.h>
#include <nlohmann/json.hpp>
#include <libbcrypt/include/bcrypt/BCrypt.hpp>

// Mockup function declarations
bool getPersons(unsigned limit, unsigned offset, const char* sort_field, const char* sort_order);
bool getPerson(unsigned id);
bool getPersonReports(unsigned id);
bool postPersons();
bool putPerson(unsigned id);
bool deletePerson(unsigned id);

bool getDepartments(unsigned limit, unsigned offset, const char* sort_field, const char* sort_order);
bool getDepartment(unsigned id);
bool getDepartmentPersons(unsigned id);
bool postDepartments();
bool putDepartment(unsigned id);
bool deleteDepartment(unsigned id);

bool getJobs(unsigned limit, unsigned offset, const char* sort_fields, const char* sort_order);
bool getJob(unsigned id);
bool getJobPersons(unsigned id);
bool postJobs();
bool putJob(unsigned id);
bool deleteJob(unsigned id);

bool registerUser(const char* username, const char* password);
bool loginUser(const char* username, const char* password);

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  drogon::app().run();
  return RUN_ALL_TESTS();
}

TEST(OrgChartAPITest, GetPersonsBasic) {
  EXPECT_TRUE(getPersons(0, 0, nullptr, nullptr));
}

TEST(OrgChartAPITest, GetPersonBasic) {
  EXPECT_TRUE(getPerson(1));
}

TEST(OrgChartAPITest, GetPersonReportsBasic) {
  EXPECT_TRUE(getPersonReports(1));
}

TEST(OrgChartAPITest, PostPersonsBasic) {
  EXPECT_TRUE(postPersons());
}

TEST(OrgChartAPITest, PutPersonBasic) {
  EXPECT_TRUE(putPerson(1));
}

TEST(OrgChartAPITest, DeletePersonBasic) {
  EXPECT_TRUE(deletePerson(1));
}

TEST(OrgChartAPITest, GetDepartmentsBasic) {
  EXPECT_TRUE(getDepartments(0, 0, nullptr, nullptr));
}

TEST(OrgChartAPITest, GetDepartmentBasic) {
  EXPECT_TRUE(getDepartment(1));
}

TEST(OrgChartAPITest, GetDepartmentPersonsBasic) {
  EXPECT_TRUE(getDepartmentPersons(1));
}

TEST(OrgChartAPITest, PostDepartmentsBasic) {
  EXPECT_TRUE(postDepartments());
}

TEST(OrgChartAPITest, PutDepartmentBasic) {
  EXPECT_TRUE(putDepartment(1));
}

TEST(OrgChartAPITest, DeleteDepartmentBasic) {
  EXPECT_TRUE(deleteDepartment(1));
}

TEST(OrgChartAPITest, GetJobsBasic) {
  EXPECT_TRUE(getJobs(0, 0, nullptr, nullptr));
}

TEST(OrgChartAPITest, GetJobBasic) {
  EXPECT_TRUE(getJob(1));
}

TEST(OrgChartAPITest, GetJobPersonsBasic) {
  EXPECT_TRUE(getJobPersons(1));
}

TEST(OrgChartAPITest, PostJobsBasic) {
  EXPECT_TRUE(postJobs());
}

TEST(OrgChartAPITest, PutJobBasic) {
  EXPECT_TRUE(putJob(1));
}

TEST(OrgChartAPITest, DeleteJobBasic) {
  EXPECT_TRUE(deleteJob(1));
}

TEST(OrgChartAPITest, RegisterUserBasic) {
  EXPECT_TRUE(registerUser("testuser", "testpass"));
}

TEST(OrgChartAPITest, LoginUserBasic) {
  EXPECT_TRUE(loginUser("testuser", "testpass"));
}