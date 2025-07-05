#include <gtest/gtest.h>
#include <drogon/HttpController.h>
#include <drogon/HttpRequest.h>
#include <drogon/HttpResponse.h>
#include <drogon/orm/DrogonDbException.h>
#include <drogon/orm/EntityMapper.h>
#include <drogon/orm/Job.h>
#include <drogon/model/FakeModel.h>
#include <string>
#include <vector>

using namespace drogon;
using namespace drogon::orm;
using namespace drogon_model::org_chart;
using namespace testing;

TEST(JobsControllerTest, GetJobs) {
    auto req = HttpRequest::newHttpRequest();
    auto callback = [](const HttpResponsePtr &resp) {};
    auto dbClient = TestUtils::createMockDbClient();
    
    Mapper<Job> mp(dbClient);
    EXPECT_CALL(mp, 
        orderBy(::testing::Eq("id"), ::testing::Eq(SortOrder::ASC))
        .Times(1);
    EXPECT_CALL(mp, offset(::testing::Eq(0))).Times(1);
    EXPECT_CALL(mp, limit(::testing::Eq(25))).Times(1);
    EXPECT_CALL(mp, findAll(::testing::_, ::testing::_, ::testing::Return(true)))
        .WillOnce(::testing::Return(std::vector<Job>{
            Job{{"id", 1}, {"title", "Title 1"}},
            Job{{"id", 2}, {"title", "Title 2"}}
        }));
    
    controller.get(req, callback);
    TestUtils::verifyNoFailedExpectations();
}

TEST(JobsControllerTest, GetOneJob) {
    auto req = HttpRequest::newHttpRequest();
    req->setPath("/jobs/1");
    auto callback = [](const HttpResponsePtr &resp) {};
    auto dbClient = TestUtils::createMockDbClient();
    
    Mapper<Job> mp(dbClient);
    Criteria crit(Job::Cols::_id, CompareOperator::EQ, 1);
    EXPECT_CALL(mp, findByPrimaryKey(1, ::testing::_, ::testing::_, ::testing::Return(true)))
        .WillOnce(::testing::Return(Job{{"id", 1}, {"title", "Test Job"}}));
    
    controller.getOne(req, callback, 1);
    TestUtils::verifyNoFailedExpectations();
}

TEST(JobsControllerTest, CreateJob) {
    auto req = HttpRequest::newHttpRequest();
    Job job{{"title", "New Job"}};
    auto callback = [](const HttpResponsePtr&) {};
    auto dbClient = TestUtils::createMockDbClient();
    
    Mapper<Job> mp(dbClient);
    EXPECT_CALL(mp, insert(job))
        .WillOnce(::testing::Return(Job{{"id", 1}, {"title", "New Job"}}));
    
    controller.createOne(req, callback, std::move(job));
    TestUtils::verifyNoFailedExpectations();
}

TEST(JobsControllerTest, UpdateJob) {
    auto req = HttpRequest::newHttpRequest();
    Job job{{"id", 1}, {"title", "Updated Job"}};
    auto callback = [](const HttpResponsePtr&) {};
    auto dbClient = TestUtils::createMockDbClient();
    
    Mapper<Job> mp(dbClient);
    Criteria crit(Job::Cols::_id, CompareOperator::EQ, 1);
    EXPECT_CALL(mp, findFutureByPrimaryKey(1))
        .WillOnce(::testing::Return(Job{{"id", 1}, {"title", "Old Title"}})));
    EXPECT_CALL(mp, update(Job{{"id", 1}, {"title", "Updated Job"}}, ::testing::_, ::testing::Return(true)))
        .WillOnce(::testing::Return(std::size_t(1)));
    
    controller.updateOne(req, callback, 1, std::move(job));
    TestUtils::verifyNoFailedExpectations();
}

TEST(JobsControllerTest, DeleteJob) {
    auto req = HttpRequest::newHttpRequest();
    auto callback = [](const HttpResponsePtr&) {};
    auto dbClient = TestUtils::createMockDbClient();
    
    Mapper<Job> mp(dbClient);
    Criteria crit(Job::Cols::_id, CompareOperator::EQ, 1);
    EXPECT_CALL(mp, deleteBy(crit, ::testing::_, ::testing::Return(true)))
        .WillOnce(::testing::Return(std::size_t(1)));
    
    controller.deleteOne(req, callback, 1);
    TestUtils::verifyNoFailedExpectations();
}

TEST(JobsControllerTest, GetJobPersons) {
    auto req = HttpRequest::newHttpRequest();
    auto callback = [](const HttpResponsePtr&) {};
    auto dbClient = TestUtils::createMockDbClient();
    
    Mapper<Job> mp(dbClient);
    Criteria crit(Job::Cols::_id, CompareOperator::EQ, 1);
    EXPECT_CALL(mp, findFutureByPrimaryKey(1))
        .WillOnce(::testing::Return(Job{{"id", 1}, {"persons", std::vector<Person>{{"first_name", "Person"}}}})));
    
    controller.getJobPersons(req, callback, 1);
    TestUtils::verifyNoFailedExpectations();
}

TEST(JobsControllerTest, CreateJobWithJobId) {
    auto req = HttpRequest::newHttpRequest();
    Job job{{"title", ""}};
    auto callback = [](const HttpResponsePtr&) {};
    auto dbClient = TestUtils::createMockDbClient();
    
    Mapper<Job> mp(dbClient);
    EXPECT_THROW(controller.createOne(req, callback, std::move(job)), std::invalid_argument);
    TestUtils::verifyNoFailedExpectations();
}

TEST(JobsControllerTest, UpdateJobNotFound) {
    auto req = HttpRequest::newHttpRequest();
    Job job{{"id", 1}, {"title", "Updated Job"}};
    auto callback = [](const HttpResponsePtr&) {};
    auto dbClient = TestUtils::createMockDbClient();
    
    Mapper<Job> mp(dbClient);
    EXPECT_CALL(mp, findFutureByPrimaryKey(1))
        .WillOnce(::testing::Throw(DrogonDbException{DrogonDbError::kUnexpectedRows, "1 row not found"}));
    
    controller.updateOne(req, callback, 1, std::move(job));
    TestUtils::verifyNoFailedExpectations();
}

TEST(JobsControllerTest, DeleteJobNotFound) {
    auto req = HttpRequest::newHttpRequest();
    auto callback = [](const HttpResponsePtr&) {};
    auto dbClient = TestUtils::createMockDbClient();
    
    Mapper<Job> mp(dbClient);
    Criteria crit(Job::Cols::_id, CompareOperator::EQ, 1);
    EXPECT_CALL(mp, deleteBy(crit, ::testing::_, ::testing::Throw(DrogonDbException{DrogonDbError::kDeadlock, "Deadlock"})))
        .WillOnce(::testing::Return(std::size_t(0)));
    
    controller.deleteOne(req, callback, 1);
    TestUtils::verifyNoFailedExpectations();
}

class TestUtils {
public:
    static std::shared_ptr<DbClient> createMockDbClient() {
        auto client = std::make_shared<NiceMock<DbClient>>();
        auto db = std::make_shared<MockDb>();
        ON_CALL(*db, getCurrentConnection())
            .WillByDefault([conn = std::make_shared<MockConnection>()] { return conn; });
        ON_CALL(*client, getConnection())
            .WillByDefault([db] { return db; });
        return client;
    }
};